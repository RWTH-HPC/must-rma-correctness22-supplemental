#include <algorithm>
#include <cstring>
#include <memory>
#include <sstream>
#include <stddef.h>
#include <vector>

#ifndef COMMON_H
#define COMMON_H

using size_type = std::size_t;

/* Data not owned by this class but by GTI */
template <class T> class GtiObject {
  protected:
    T* m_data;
    size_type m_size;

  public:
    ~GtiObject() = default;
    GtiObject() : m_data(nullptr), m_size(0){};
    GtiObject(T* ptr, size_type size) : m_data(ptr), m_size(size){};

    size_type size() const { return m_size; };
    T* data() const { return m_data; };
};

#endif
