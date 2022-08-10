/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file DataPool.h
 *
 *  @date 28.06.2017
 *  @author Joachim Protze, Simon Schwitanski
 */

#include "MustTypes.h"
#include <stack>
#include <mutex>

#ifndef DATAPOOL_H
#define DATAPOOL_H

namespace must {
    // Data structure to provide a threadsafe pool of reusable objects.
    // DataPool<Type of objects, Size of blockalloc>
    template <typename T, int N>
    class DataPool {
        public:
            DataPool() : DataPointer(), DPMutex(), total(0)
            {}

            T * getData() {
                T * ret;
                DPMutex.lock();
                if (DataPointer.empty())
                    newDatas();
                ret=DataPointer.top();
                DataPointer.pop();
                DPMutex.unlock();
                return ret;
            }

            void returnData(T * data) {
                DPMutex.lock();
                DataPointer.push(data);
                DPMutex.unlock();
            }

            void getDatas(int n, T** datas) {
                DPMutex.lock();
                for (int i=0; i<n; i++) {
                    if (DataPointer.empty())
                        newDatas();
                    datas[i]=DataPointer.top();
                    DataPointer.pop();
                }
                DPMutex.unlock();
            }

            void returnDatas(int n, T** datas) {
                DPMutex.lock();
                for (int i=0; i<n; i++) {
                    DataPointer.push(datas[i]);
                }
                DPMutex.unlock();
            }

        private:
            std::mutex DPMutex;
            std::stack<T *> DataPointer;
            int total;

            void newDatas() {
                // prefix the Data with a pointer to 'this', allows to return memory to 'this',
                // without explicitly knowing the source.
                //
                // To reduce lock contention, we use thread local DataPools, but Data objects move to other threads.
                // The strategy is to get objects from local pool. Only if the object moved to another
                // thread, we might see a penalty on release (returnData).
                // For "single producer" pattern, a single thread creates tasks, these are executed by other threads.
                // The master will have a high demand on TaskData, so return after use.
                struct pooldata {DataPool<T,N>* dp; T data;};
                // We alloc without initialize the memory. We cannot call constructors. Therefore use malloc!
                pooldata* datas = (pooldata*) malloc(sizeof(pooldata) * N);
                for (int i = 0; i<N; i++) {
                    datas[i].dp = this;
                    DataPointer.push(&(datas[i].data));
                }
                total+=N;
            }
    };

    // This function takes care to return the data to the originating DataPool
    // A pointer to the originating DataPool is stored just before the actual data.
    template <typename T, int N>
    static void retData(void * data) {
        ((DataPool<T,N>**)data)[-1]->returnData((T*)data);
    }

}

#endif
