#include <stddef.h>
#include <string>

#include "GtiObject.h"
#include <limits.h> /* for CHAR_BIT */

#ifndef BITSET_H
#define BITSET_H

class AbstractBitset {
  protected:
    size_type m_bit_size;

  public:
    AbstractBitset(size_type numBits) : m_bit_size(numBits){};
    size_type bit_size() const { return m_bit_size; };

    bool operator[](size_type idx) const { return test(idx); }
    void set(size_type idx) { data()[bitslot(idx)] |= bitmask(idx); }
    void clear(size_type idx) { data()[bitslot(idx)] &= ~bitmask(idx); }
    void flip(size_type idx) { data()[bitslot(idx)] ^= bitmask(idx); }

    void set(const int* idx_set, size_type size);
    void clear(const int* idx_set, size_type size);
    void flip(const int* idx_set, size_type size);

    void set();
    void clear();
    void flip();

    bool test(size_type idx) const {
        return static_cast<bool>(data()[bitslot(idx)] & bitmask(idx));
    }

    bool any() const;

    std::string toStr(char del = '\0') const;

    bool operator==(const AbstractBitset& rhs) const;

    bool cmp(const char* ptr) const { return std::memcmp(data(), ptr, size()) == 0; }

    size_type bitNSlots(size_type numBits) const { return (numBits + CHAR_BIT - 1) / CHAR_BIT; }

    char bitmask(size_type idx) const { return 1 << ((idx) % CHAR_BIT); }

    size_type bitslot(size_type idx) const { return idx / CHAR_BIT; }

    /* Set protruding bits always to 0 for easier comparison of two bit sets */
    void clearProtudingBits() {
        if (m_bit_size % CHAR_BIT)
            data()[size() - 1] &= ~(0xff << (m_bit_size % CHAR_BIT));
    }

    void set_num_bits(size_type numBits) { m_bit_size = numBits; };
    virtual size_type size() const = 0;
    virtual char* data() const = 0;

}; /*class AbstractBitset*/

class Bitset : public AbstractBitset {
  protected:
    std::vector<char> m_data;

  public:
    Bitset() : m_data(0), AbstractBitset(0){};
    Bitset(size_type numBits) : m_data(bitNSlots(numBits), 0), AbstractBitset(numBits){};
    Bitset(bool initVal, size_type numBits)
        : m_data(bitNSlots(numBits), initVal), AbstractBitset(0) {
        if (initVal)
            clearProtudingBits();
    };
    Bitset(const char* ptr, size_type numBits)
        : m_data(ptr, ptr + bitNSlots(numBits)), AbstractBitset(numBits){};

    Bitset(const int* groupIdx, size_type groupSize, size_type numBits) : Bitset(false, numBits) {
        set(groupIdx, groupSize);
    }

    Bitset(const std::vector<int>& groupIdx, size_type numBits)
        : Bitset(groupIdx.data(), groupIdx.size(), numBits){};

    Bitset(const Bitset& other) = default;
    Bitset(Bitset&& other) = default;
    Bitset& operator=(const Bitset& other) = default;
    Bitset& operator=(Bitset&& other) = default;

    bool operator==(const AbstractBitset& rhs) const;

    size_type size() const override { return m_data.size(); };
    char* data() const override { return const_cast<char*>(m_data.data()); };

}; /*class Bitset*/

class GtiBitset : public GtiObject<char>, public AbstractBitset {
  public:
    GtiBitset(char* ptr, size_type numBits)
        : GtiObject(ptr, bitNSlots(numBits)), AbstractBitset(numBits) {
        set_num_bits(numBits);
    }

    size_type size() const override { return GtiObject<char>::size(); };
    char* data() const override { return GtiObject<char>::data(); };

}; /*class GtiBitset*/

#endif
