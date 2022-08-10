/* This file is part of GTI (Generic Tool Infrastructure)
 *
 * Copyright (C)
 *  2008-2019 ZIH, Technische Universitaet Dresden, Federal Republic of Germany
 *  2008-2019 Lawrence Livermore National Laboratories, United States of America
 *  2013-2019 RWTH Aachen University, Federal Republic of Germany
 *
 * See the LICENSE file in the package base directory for details
 */

/**
 * @file CProtSharedMemory.h
 *       A shared memory implementation of the communication protocol interface.
 *
 *  This implementation may be used to communicate between processes that share
 *  memory.
 *
 * @author Joachim Protze
 * @date 15.03.2012
 *
 */

#ifndef C_PROT_SHARED_MEMORY_H
#define C_PROT_SHARED_MEMORY_H

#include "I_CommProtocol.h"
#include "I_Module.h"
#include "ModuleBase.h"

#include <vector>
//#include <list>
//#include <map>
#include <queue>
#include <stack>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <sys/types.h>


extern "C" {
void __attribute__((weak)) AnnotateHappensAfter(const char *file, int line, const volatile void *cv){}
void __attribute__((weak)) AnnotateHappensBefore(const char *file, int line, const volatile void *cv){}
void __attribute__((weak)) AnnotateNewMemory(const char *file, int line, const volatile void *cv, size_t size){}
}
// This marker is used to define a happens-before arc. The race detector will
// infer an arc from the begin to the end when they share the same pointer
// argument.
# define TsanHappensBefore(cv) AnnotateHappensBefore(__FILE__, __LINE__, cv)
// This marker defines the destination of a happens-before arc.
# define TsanHappensAfter(cv) AnnotateHappensAfter(__FILE__, __LINE__, cv)
# define TsanNewMemory(addr, size) AnnotateNewMemory(__FILE__, __LINE__, addr, size)

namespace gti
{

static int pagesize{512};

// Data structure to provide a threadsafe pool of reusable objects.
// DataPool<Type of objects, Size of blockalloc>
template <typename T, int N = 1, bool Ring = false> struct DataPool {
  std::mutex DPMutex;
  std::vector<T *> DataPointer;
  std::vector<T *> ReturnDataPointer;
  std::list<void *> memory;
  std::atomic<int> remote{0};
  int total{0};

  struct pooldata {
    DataPool<T, N, Ring> *dp;
    T data;
  };
  void newDatas() {
    if (remote > 0) {
      DPMutex.lock();
      // remoteReturn++;
      DataPointer.swap(ReturnDataPointer);
      remote = 0;
      DPMutex.unlock();
      if (Ring)
        std::reverse(DataPointer.begin(), DataPointer.end());
      return;
    }
    // calculate size of an object including padding to cacheline size
    size_t elemSize = sizeof(T);
    size_t paddedSize = (((elemSize - 1) / 64) + 1) * 64;
    // number of padded elements to allocate
    int ndatas = pagesize * N / paddedSize;
    char *datas = (char *)malloc(ndatas * paddedSize);
    memory.push_back(datas);
    for (int i = 0; i < ndatas; i++) {
      DataPointer.push_back(new (datas + i * paddedSize) T(this));
    }
    total += ndatas;
  }

  T *getData() {
    T *ret;
    if (DataPointer.empty())
      newDatas();
    ret = DataPointer.back();
    DataPointer.pop_back();
    return ret;
  }

  void returnData(T *data) {
    DPMutex.lock();
    ReturnDataPointer.emplace_back(data);
    remote++;
    DPMutex.unlock();
  }

  DataPool() : DataPointer(), DPMutex(), total(0) {}
  ~DataPool() {
    for (auto i : memory)
      if (i)
        free(i);
  }
};

template <typename T> struct DataPoolEntry {
  DataPool<T> *owner;

  static T *New() { return DataPool<T>::ThreadDataPool->getData(); }

  void Delete() {
    static_cast<T *>(this)->Reset();
    owner->returnData(static_cast<T *>(this));
  }

  DataPoolEntry(DataPool<T> *dp) : owner(dp) {}
};

struct SMRequest;
typedef DataPool<SMRequest> SMRequestDataPool;

struct SMRequest final : DataPoolEntry<SMRequest> {
  uint64_t size;         // size for send/ recv
  uint64_t inSize;       // actual received size
  unsigned int request;  // request for non-blocking
  uint64_t channel;      // channel for recv
  std::mutex SMRMutex{}; // mutex for request, used for signaling
  std::condition_variable
      SMRCondVar{};           // conditional variable for waiting and signaling
  std::atomic<bool> finished; // send is finished
  bool send;                  // this is a send request
  void *data;                 // pointer to data
  // send
  SMRequest *Init(void *_buf, uint64_t _size, int _request) {
    size = _size;
    request = _request;
    channel = 0;
    finished = false;
    send = true;
    data = _buf;
    return this;
  }
  // recv
  SMRequest *Init(void *_buf, uint64_t _size, uint64_t _channel, int _request) {
    size = _size;
    request = _request;
    channel = _channel;
    finished = false;
    send = false;
    data = _buf;
    return this;
  }

  void Reset() {}
  SMRequest(DataPool<SMRequest> *dp) : DataPoolEntry<SMRequest>(dp) {}
};

  struct SMQueue {
    std::atomic<int> size;
    std::queue<SMRequest*> MessageQueue;
    std::mutex SMQMutex;
    std::condition_variable SMQCondVar;
    uint64_t outChannel;
    uint64_t inChannel;
    SMQueue() : size(0), MessageQueue(), SMQMutex() {}
    bool empty(){return size==0;}
    SMRequest* try_pop(){
	    std::unique_lock<std::mutex> lock(SMQMutex);
	    if (MessageQueue.empty())
	      return nullptr;

	    SMRequest * ret=MessageQueue.front();
	    MessageQueue.pop();
	    size = MessageQueue.size();
	    return ret;
    }
    SMRequest* wait_pop(){
	    std::unique_lock<std::mutex> lock(SMQMutex);
	    while (MessageQueue.empty())
	      SMQCondVar.wait(lock);

	    SMRequest * ret=MessageQueue.front();
	    MessageQueue.pop();
	    size = MessageQueue.size();
	    return ret;
    }
    void push(SMRequest* p){
	    std::unique_lock<std::mutex> lock(SMQMutex);
	    MessageQueue.push(p);
	    size = MessageQueue.size();
	    SMQCondVar.notify_one();
    }
  };


  /* SMSyncPoint enforces P2P synchronization. Whoever reaches first blocks and waits for the partner to arrive */
  struct SMSyncPoint {
    bool isActive = false;
    std::mutex SMSPMutex;
    std::condition_variable SMSPCondVar;
    void visit(){
	    std::unique_lock<std::mutex> lock(SMSPMutex);
      if (isActive)
      {
        isActive = false;
        TsanHappensAfter(&isActive);
        TsanHappensBefore(&isActive);
        SMSPCondVar.notify_one();
        return;
      }
      else
      {
        isActive = true;
        TsanHappensBefore(&isActive);
        while (isActive)
          SMSPCondVar.wait(lock);
        TsanHappensAfter(&isActive);
        return;
      }
    }
  };

    /**
     * Class that describes the communication protocol interface.
     */
    class CommProtSharedMemory : public ModuleBase<CommProtSharedMemory, I_CommProtocol>
    {
    protected:
        bool initialized;
        bool finalized;
        bool isTop;
        bool myIsIntra;

        std::vector<SMQueue*> outQueues;    // out-going messages

        std::vector<SMQueue*> inQueues;     // in-comming messages
        std::vector<std::queue<SMRequest*>*> recvQueues;   // open non-blocking recv requests (accessed only locally)
        static SMSyncPoint entrySyncPoint;
        static SMSyncPoint exitSyncPoint;
        //std::queue<SMRequest*>* recvAnyQueue;

        static SMQueue* helloQueue;

        int numPartners;
        int maxNumPartners;

        uint64_t gtiOwnLevel;
        uint64_t remoteTierSize;
        uint64_t tierSize;
        int commId;
        GtiTbonNodeInLayerId myPlaceId;
        char commSide;
        int next_channel=0;

        std::map<int, SMRequest*> requestMap;
        unsigned int requestId;

        static void dummy(void){}
        static void initModule(); // called once
        void(*newClientCallback)(void) = dummy;

        /**
         * initialize connections
         */
        void connect();
        void reconnect();
        ssize_t recv_wrapper ( void* out_buf, size_t num_bytes, uint64_t channel, uint64_t* out_channel, int msgflg=0 );
        void handle_test( unsigned int request, int* out_completed, uint64_t* out_receive_length, uint64_t* out_channel, bool test);
        SMRequestDataPool smrdp;

      public:

        /**
         * Constructor.
         * @ref ModConf - The module configuration syntax
         * @param intanceName name of the module instance.
         */
        CommProtSharedMemory (const char* instanceName);

        /**
         * Destructor.
         */
        ~CommProtSharedMemory (void);

        /**
         * @see gti::I_CommProtocol::isConnected
         */
        bool isConnected (void);

        /**
         * @see gti::I_CommProtocol::isInitialized
         */
        bool isInitialized (void);

        /**
         * @see gti::I_CommProtocol::isFinalized
         */
        bool isFinalized (void);

        /**
         * @see gti::I_CommProtocol::getNumChannels
         */
        GTI_RETURN getNumChannels (uint64_t* out_numChannels);

        /**
         * @see gti::I_CommProtocol::getNumClients
         */
        GTI_RETURN getNumClients (uint64_t* out_numClients);

        /**
         * @see gti::I_CommProtocol::getPlaceId
         */
        GTI_RETURN getPlaceId (uint64_t* outPlaceId);

        /**
         * @see gti::I_CommProtocol::shutdown
         */
        GTI_RETURN shutdown (void);

        /**
         * @see gti::I_CommProtocol::removeOutstandingRequests
         */
        GTI_RETURN removeOutstandingRequests (void);

        GTI_RETURN registerNewClientCallback (
                void(*fun)(void),
                bool& isUsed)
        {isUsed=true; newClientCallback = fun; return GTI_SUCCESS;}
        /**
         * @see gti::I_CommProtocol::ssend
         */
        GTI_RETURN ssend (
                void* buf,
                uint64_t num_bytes,
                uint64_t channel
                );

        /**
         * @see gti::I_CommProtocol::isend
         */
        GTI_RETURN isend (
                void* buf,
                uint64_t num_bytes,
                unsigned int *out_request,
                uint64_t channel
                );

        /**
         * @see gti::I_CommProtocol::recv
         */
        GTI_RETURN recv (
                void* out_buf,
                uint64_t num_bytes,
                uint64_t* out_length,
                uint64_t channel,
                uint64_t *out_channel
                );

        /**
         * @see gti::I_CommProtocol::irecv
         */
        GTI_RETURN irecv (
                void* out_buf,
                uint64_t num_bytes,
                unsigned int *out_request,
                uint64_t channel
                );

        /**
         * @see gti::I_CommProtocol::test_msg
         */
        GTI_RETURN test_msg (
                unsigned int request,
                int* out_completed,
                uint64_t* out_receive_length,
                uint64_t* out_channel
                );

        /**
         * @see gti::I_CommProtocol::wait_msg
         */
        GTI_RETURN wait_msg (
                unsigned int request,
                uint64_t* out_receive_length,
                uint64_t* out_channel
                );
    }; /*class CommProtSharedMemory*/
} /*namespace gti*/

#endif /* C_PROT_SHARED_MEMORY_H */
