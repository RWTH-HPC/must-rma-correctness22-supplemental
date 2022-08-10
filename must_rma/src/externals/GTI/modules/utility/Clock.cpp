#include <array>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "BinomialTree.h"
#include "Clock.h"

Clock::Clock(const AbstractClock& c, AppId* groupIds, size_type groupSize, size_type numTotalProcs)
    : m_data(c.size() + 1 - groupSize), AbstractClock(c.getOwnerId()) {
    data()[0] = c[c.getOwnerId()];
    if (groupSize < numTotalProcs) {
        Bitset bs(groupIds, groupSize, numTotalProcs);

        for (int i = 0, j = 1; i < numTotalProcs; i++)
            if (!bs[i])
                data()[j++] = c[i];
    }
}

Clock::Clock(const AbstractClock& c, size_type groupSize, const AbstractBitset& bs)
    : m_data(bs.bit_size() + 1 - groupSize), AbstractClock(c.getOwnerId()) {
    m_data[0] = c[c.getOwnerId()];
    if (groupSize < bs.bit_size() && bs.size()) {
        for (size_type i = 0, j = 1; i < bs.bit_size(); i++)
            if (!bs[i])
                m_data[j++] = c[i];
    }
}


Clock::Clock(const ClockEntry* dataPtr, char* bsPtr, size_type bsSize, GtiId remoteId, GtiId ownerId)
    : m_data(bsSize, 0), AbstractClock(ownerId) {
    setOwnerId(ownerId);
    if (bsPtr) {
        GtiBitset bs(bsPtr, bsSize);
        for (size_type i = 0, j = 1; i < bsSize; i++)
            data()[i] = !bs[i] ? dataPtr[j++] : 0;
    }
    data()[remoteId] = dataPtr[0];
}

void Clock::createBTRClockFromOwn(const AbstractClock& c, const AbstractBitset& bs,
                                  size_type numTotalProcs, size_type groupSize, AppId localId,
                                  AppId localRoot) {
    resize(BinomialTree::nodeClockSize(numTotalProcs, groupSize, localId, localRoot));
    setOwnerId(c.getOwnerId());
    /* Create reduction clock from complement clock (non-blocking communication) */
    if (c.size() < numTotalProcs) {
        data()[0] = c[0];
        size_type compSize = numTotalProcs - groupSize;
        std::memcpy(&(data()[size() - compSize]), &(c.data()[1]), sizeof(ClockEntry) * (compSize));
    }
    /* Create reduction clock from full clock (blocking communication) */
    else {
        data()[0] = c[c.getOwnerId()];
        if (bs.size())
            for (size_type i = 0, j = size() - (numTotalProcs - groupSize); i < numTotalProcs; i++)
                if (!bs[i])
                    data()[j++] = c[i];
    }
}

void Clock::createBTRClockFromOther(const AbstractClock& c, size_type numTotalProcs, size_type groupSize,
                                    AppId localId, AppId localRoot) {
    resize(BinomialTree::nodeClockSize(numTotalProcs, groupSize, localId, localRoot));
    setOwnerId(c.getOwnerId());
    /* Create reduction clock from complement clock (non-blocking communication) */
    size_type compSize = numTotalProcs - groupSize;
    std::memcpy(&(data()[size() - compSize]), c.data(), sizeof(ClockEntry) * (compSize));
    std::memcpy(&(data()[size() - compSize + 1]), &(c.data()[c.size() - compSize]),
                sizeof(ClockEntry) * (compSize));
}

Clock::Clock(const AbstractClock& c, const AbstractBitset& bs, size_type numTotalProcs,
             size_type groupSize, AppId localId, AppId localRoot)
    : m_data(BinomialTree::nodeClockSize(numTotalProcs, groupSize, localId, localRoot), 0),
      AbstractClock(c.getOwnerId()) {
    /* Create reduction clock from complement clock (non-blocking communication) */
    if (c.size() < numTotalProcs) {
        data()[0] = c[0];
        size_type compSize = numTotalProcs - groupSize;
        std::memcpy(&(data()[size() - compSize]), &(c.data()[1]), sizeof(ClockEntry) * (compSize));
    }
    /* Create reduction clock from full clock (blocking communication) */
    else {
        data()[0] = c[c.getOwnerId()];
        if (bs.size())
            for (size_type i = 0, j = size() - (numTotalProcs - groupSize); i < numTotalProcs; i++)
                if (!bs[i])
                    data()[j++] = c[i];
    }
}

Clock::Clock(const AbstractClock& c, AppId localId, AppId remoteLocalId, AppId localRoot,
             size_type groupSize, size_type numTotalProcs)
    : m_data(BinomialTree::nodeClockSize(numTotalProcs, groupSize, localId, localRoot), 0),
      AbstractClock(c.getOwnerId()) {
    size_type compSize = numTotalProcs - groupSize;
    /* Copy clock entries for group members */
    std::memcpy(
        &(data()[BinomialTree::posForClockSize(groupSize, localId, remoteLocalId, localRoot)]),
        c.data(), sizeof(ClockEntry) * (c.size() - compSize));
    /* Copy clock entries of complement group */
    std::memcpy(&(data()[size() - compSize]), &(c.data()[c.size() - compSize]),
                sizeof(ClockEntry) * (compSize));
}

void AbstractClock::merge(const AbstractClock& other, const AbstractBitset& bs, size_type groupSize,
                          size_type numTotalProcs, AppId localId, AppId remoteLocalId,
                          AppId localRoot) {
    if (other.getOwnerId() != getOwnerId()) {
        size_type compSize = numTotalProcs - groupSize;
        /* Copy clock entries for group members */
        std::memcpy(
            &(data()[BinomialTree::posForClockSize(groupSize, localId, remoteLocalId, localRoot)]),
            other.data(), sizeof(ClockEntry) * (other.size() - compSize));
        /* Merge clock entries of complement group */
        for (size_type i = size() - compSize, j = other.size() - compSize; i < size(); i++, j++)
            if (data()[i] < other[j])
                data()[i] = other[j];
    } else {
        if (other.size() < numTotalProcs) {
            data()[0] = other[0];
            if (bs.size())
                for (int i = 0, j = 1; i < size(); i++)
                    if (data()[i] < other[j])
                        data()[i] = other[j++];
        } else {
            data()[0] = other[other.getOwnerId()];
            if (bs.size())
                for (size_type i = 0, j = size() - (numTotalProcs - groupSize); i < numTotalProcs; i++)
                    if (!bs[i] && data()[j] < other[i])
                        data()[j++] = other[i];
        }
    }
}

void AbstractClock::reduce(const AbstractClock& other, const AbstractBitset& bs, AppId localRootId,
                           size_type groupSize) {}

void AbstractClock::merge(const AbstractClock& other, const AbstractBitset& bs) {
    /* Merge full clock with single entry */
    if (other.size() == 1)
        data()[other.getOwnerId()] = other[0];
    /* Merge over a bitset of complement group */
    else if (other.size() < size() && bs.size()) {
        data()[other.getOwnerId()] = other[0];
        for (int i = 0, j = 1; i < size(); i++)
            if (!bs[i] && data()[i] < other[j])
                data()[i] = other[j++];
    }
    /* Merge two full clocks */
    else if (other.size() == size()) {
        merge(other);
    }
}

void AbstractClock::merge(const AbstractClock& other) {
    /* Merge full clock with single entry */
    if (other.size() == 1) {
        data()[other.getOwnerId()] = other[0];
        return;
    }
    for (int i = 0; i < size(); i++)
        if (data()[i] < other[i])
            data()[i] = other[i];
}

std::string AbstractClock::toStr() const {
    std::stringstream out;
    out << "(";
    for (int i = 0; i < size(); i++)
        out << data()[i] << (i < size() - 1 ? ", " : "");
    out << ")";

    return out.str();
}
