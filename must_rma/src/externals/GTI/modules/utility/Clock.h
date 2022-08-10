#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <utility>

#include "Bitset.h"

#ifndef CLOCK_H
#define CLOCK_H

typedef unsigned long long ClockEntry;
typedef int AppId;
typedef int GtiId;

class AbstractClock {
  protected:
    GtiId m_ownerId;

  public:
    AbstractClock(GtiId id) : m_ownerId(id){};
    ClockEntry operator[](size_type idx) const { return data()[idx]; }
    ClockEntry& operator[](size_type idx) { return data()[idx]; }

    void merge(const AbstractClock& other, const AbstractBitset& bs);
    void merge(const AbstractClock& other);
    /* Merge two intermediate clocks when performing a binomial tree reduction */
    void merge(const AbstractClock& other, const AbstractBitset& bs, size_type groupSize,
               size_type numTotalProcs, AppId localId, AppId remoteLocalId, AppId localRoot);
    /* Final merge of binomial tree reduction performed by root */
    void reduce(const AbstractClock& other, const AbstractBitset& bs, AppId localRootId,
                size_type groupSize);

    virtual size_type size() const = 0;
    virtual ClockEntry* data() const = 0;

    std::string toStr() const;
    GtiId getOwnerId() const { return m_ownerId; }
    void setOwnerId(GtiId id) { m_ownerId = id; }
};

class Clock : public AbstractClock {
  protected:
    std::vector<ClockEntry> m_data;

  public:
    Clock() : m_data(), AbstractClock(-1){};
    Clock(GtiId ownerId) : m_data(), AbstractClock(ownerId){};
    Clock(const ClockEntry* ptr, size_type size, GtiId ownerId)
        : m_data(ptr, ptr + size), AbstractClock(ownerId) {}

    Clock(size_type size, GtiId ownerId) : m_data(size), AbstractClock(ownerId){};
    Clock(size_type size, ClockEntry initVal, GtiId ownerId)
        : m_data(size, initVal), AbstractClock(ownerId){};
    Clock(const AbstractClock& c, AppId* groupIds, size_type groupSize, size_type numTotalProcs);
    /* Create complement clock from full clock */
    Clock(const AbstractClock& c, size_type groupSize, const AbstractBitset& bs);
    /* Expand a complement clock to a full clock */
    Clock(const ClockEntry* dataPtr, char* bsPtr, size_type bsSize, GtiId remoteId, GtiId ownerId);
    Clock(const AbstractClock& c, const AbstractBitset& bs, GtiId remoteId)
        : Clock(c.data(), bs.data(), bs.bit_size(), remoteId, c.getOwnerId()) {}

    Clock(const Clock& other) = default;
    Clock(Clock&& other) = default;
    Clock& operator=(const Clock& other) = default;
    Clock& operator=(Clock&& other) = default;

    // TODO: maybe split this to another clock class (ReductionClock or
    // BinomialReductionClock)
    /* In case c.size() == numTotalProcs: Create a reduction clock from a full
     * clock. (For blocking communication.) */
    /* Else: Create a reduction clock from a complement clock.
     * (For non-blocking communication. c belongs to same process the created
     * clock belongs to.) */
    Clock(const AbstractClock& c, const AbstractBitset& bs, size_type numTotalProcs, size_type groupSize,
          AppId localId, AppId localRoot);
    /* Reduction clock from complement clock for the case localGtiId hasn't
     * created its reduction clock yet at receival. (c belongs to the process
     * associated with remoteLocalId. The created clock belongs to localGtiId.) */
    Clock(const AbstractClock& c, AppId localId, AppId remoteLocalId, AppId localRoot,
          size_type groupSize, size_type numTotalProcs);

    void createBTRClockFromOwn(const AbstractClock& other, const AbstractBitset&,
                               size_type numTotalProcs, size_type groupSize, AppId localId,
                               AppId localRoot);
    void createBTRClockFromOther(const AbstractClock& other, size_type numTotalProcs, size_type groupSize,
                                 AppId localId, AppId localRoot);
    size_type size() const override { return m_data.size(); };
    ClockEntry* data() const override { return const_cast<ClockEntry*>(m_data.data()); };
    auto resize(size_type s) -> decltype(m_data.resize(s)) { return m_data.resize(s); };
    void fill(ClockEntry val) { std::fill(m_data.begin(), m_data.end(), val); }
};

class GtiClock : public GtiObject<ClockEntry>, public AbstractClock {
  public:
    size_type size() const override { return GtiObject<ClockEntry>::size(); }
    ClockEntry* data() const override { return GtiObject<ClockEntry>::data(); };
    GtiClock(ClockEntry* ptr, size_type size, GtiId ownerId)
        : GtiObject(ptr, size), AbstractClock(ownerId){};
};

#endif
