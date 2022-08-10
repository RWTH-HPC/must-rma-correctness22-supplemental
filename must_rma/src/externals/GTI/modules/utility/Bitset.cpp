#include <array>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
// source: http://c-faq.com/misc/bitsets.html

#include "Bitset.h"

// TODO: redo this with function templates
void AbstractBitset::set(const int* idx_set, size_type size) {
    for (size_type i = 0; i < size; i++)
        set(idx_set[i]);
}

void AbstractBitset::clear(const int* idx_set, size_type size) {
    for (size_type i = 0; i < size; i++)
        clear(idx_set[i]);
}

void AbstractBitset::flip(const int* idx_set, size_type size) {
    for (size_type i = 0; i < size; i++)
        flip(idx_set[i]);
}

void AbstractBitset::set() {
    std::memset(data(), 0xff, size());
    clearProtudingBits();
}

void AbstractBitset::clear() { std::memset(data(), 0, size()); }

void AbstractBitset::flip() {
    for (int i = 0; i < size(); i++)
        data()[i] = ~data()[i];
    clearProtudingBits();
}

std::string AbstractBitset::toStr(char del) const {
    if (!size())
        return std::string("EmptyBitset");
    std::string binaryStr((del != '\0') ? m_bit_size + size() - 1 : m_bit_size, '0');

    size_type i, chunks = 0;
    for (i = 0; i < m_bit_size; i++) {
        if (del != '\0' && i > 0 && i % CHAR_BIT == 0) {
            binaryStr[i + chunks] = del;
            chunks += 1;
        }
        if (test(i))
            binaryStr[i + chunks] = '1';
    }

    return binaryStr;
}

bool AbstractBitset::any() const {
    for (int i = 0; i < size(); i++)
        if (data()[i])
            return true;

    return false;
}

