#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <iostream>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template <typename T> class vector {
  T *arr;     // array
  size_t siz; // size
  size_t cap; // capacity

  /**
   * @brief expand to fit more elements
   */
  void expand() {
    if (siz >= cap) {
      cap = siz * 2 + 2;
      T *p = (T *)malloc(sizeof(T) * cap);
      // copy
      for (int i = 0; i < siz; i++) {
        new (p + i) T(arr[i]);
      }
      // std::cerr << sizeof(T) << ' ' << siz << ' ' << cap << std::endl;
      if (arr != nullptr) {
        for (int i = 0; i < siz; i++) {
          arr[i].~T();
        }
        free(arr);
      }
      arr = p;
    }
  }

public:
  /**
   * TODO
   * a type for actions of the elements of a vector, and you should write
   *   a class named const_iterator with same interfaces.
   */
  /**
   * you can see RandomAccessIterator at CppReference for help.
   */
  class const_iterator;
  class iterator {
    // The following code is written for the C++ type_traits library.
    // Type traits is a C++ feature for describing certain properties of a type.
    // For instance, for an iterator, iterator::value_type is the type that the
    // iterator points to.
    // STL algorithms and containers may use these type_traits (e.g. the
    // following typedef) to work properly. In particular, without the following
    // code,
    // @code{std::sort(iter, iter1);} would not compile.
    // See these websites for more information:
    // https://en.cppreference.com/w/cpp/header/type_traits
    // About value_type:
    // https://blog.csdn.net/u014299153/article/details/72419713 About
    // iterator_category: https://en.cppreference.com/w/cpp/iterator
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;

  private:
    /**
     * TODO add data members
     *   just add whatever you want.
     */
    T *beg, *ptr;
    // beg is for judging whether two iterators point to the same vector

  public:
    iterator(T *b, T *p) : beg(b), ptr(p) {} //! How to get rid of this?
    iterator() : beg(nullptr), ptr(nullptr) {}
    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */
    iterator operator+(const int &n) const {
      // TODO
      return iterator(beg, ptr + n);
    }
    iterator operator-(const int &n) const {
      // TODO
      return iterator(beg, ptr - n);
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw
    // invaild_iterator.
    int operator-(const iterator &rhs) const {
      // TODO
      if (beg != rhs.beg) {
        throw invalid_iterator();
      }
      return ptr - rhs.ptr;
    }
    iterator &operator+=(const int &n) {
      // TODO
      ptr += n;
      return *this;
    }
    iterator &operator-=(const int &n) {
      // TODO
      ptr -= n;
      return *this;
    }
    /**
     * TODO iter++
     */
    iterator operator++(int) {
      iterator tmp = *this;
      *this += 1;
      return tmp;
    }
    /**
     * TODO ++iter
     */
    iterator &operator++() { return *this += 1; }
    /**
     * TODO iter--
     */
    iterator operator--(int) {
      iterator tmp = *this;
      *this -= 1;
      return tmp;
    }
    /**
     * TODO --iter
     */
    iterator &operator--() { return *this -= 1; }
    /**
     * TODO *it
     */
    T &operator*() const { return *ptr; }
    /**
     * an operator for checking whether two iterators are the same (pointing to
     * the same memory address).
     */
    bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
    /**
     * some other operators for iterator.
     */
    bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  }; // class iterator
  /**
   * TODO
   * has same function as iterator, just for a const object.
   */
  class const_iterator {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;

  private:
    /**
     * TODO add data members
     *   just add whatever you want.
     */
    T *beg, *ptr;
    // beg is for judging whether two iterators point to the same vector

  public:
    const_iterator(T *b, T *p) : beg(b), ptr(p) {} //! How to get rid of this?
    const_iterator() : beg(nullptr), ptr(nullptr) {}
    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */
    const_iterator operator+(const int &n) const {
      // TODO
      return const_iterator(beg, ptr + n);
    }
    const_iterator operator-(const int &n) const {
      // TODO
      return const_iterator(beg, ptr - n);
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw
    // invaild_iterator.
    int operator-(const const_iterator &rhs) const {
      // TODO
      if (beg != rhs.beg) {
        throw invalid_iterator();
      }
      return ptr - rhs.ptr;
    }
    const_iterator &operator+=(const int &n) {
      // TODO
      ptr += n;
      return *this;
    }
    const_iterator &operator-=(const int &n) {
      // TODO
      ptr -= n;
      return *this;
    }
    /**
     * TODO iter++
     */
    const_iterator operator++(int) {
      const_iterator tmp = *this;
      *this += 1;
      return tmp;
    }
    /**
     * TODO ++iter
     */
    const_iterator &operator++() { return *this += 1; }
    /**
     * TODO iter--
     */
    const_iterator operator--(int) {
      const_iterator tmp = *this;
      *this -= 1;
      return tmp;
    }
    /**
     * TODO --iter
     */
    const_iterator &operator--() { return *this -= 1; }
    /**
     * TODO *it
     */
    const T &operator*() const { return *ptr; } //! invalid address???
    /**
     * a operator to check whether two iterators are same (pointing to the same
     * memory address).
     */
    bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  }; // class const_iterator
  /**
   * TODO Constructs
   * At least two: default constructor, copy constructor
   */
  vector() : arr(nullptr), siz(0), cap(0) {}
  vector(const vector &other) : siz(other.siz), cap(other.cap) {
    arr = (T *)malloc(sizeof(T) * cap);
    // copy
    for (int i = 0; i < siz; i++) {
      new (arr + i) T(other.arr[i]);
    }
  }
  /**
   * TODO Destructor
   */
  ~vector() {
    for (int i = 0; i < siz; i++) {
      arr[i].~T();
    }
    free(arr);
    arr = nullptr;
    siz = cap = 0;
  }
  /**
   * TODO Assignment operator
   */
  vector &operator=(const vector &other) {
    if (this == &other) {
      return *this;
    }
    for (int i = 0; i < siz; i++) {
      arr[i].~T();
    }
    free(arr);
    siz = other.siz, cap = other.cap;
    arr = (T *)malloc(sizeof(T) * cap);
    // copy
    for (int i = 0; i < siz; i++) {
      new (arr + i) T(other.arr[i]);
    }
    return *this;
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   */
  T &at(const size_t &pos) {
    if (pos >= siz) {
      throw index_out_of_bound();
    }
    return arr[pos];
  }
  const T &at(const size_t &pos) const {
    if (pos >= siz) {
      throw index_out_of_bound();
    }
    return arr[pos];
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   * !!! Pay attention
   *   In STL this operator does not check the boundary but I want you to do so.
   */
  T &operator[](const size_t &pos) {
    if (pos >= siz) {
      throw index_out_of_bound();
    }
    return arr[pos];
  }
  const T &operator[](const size_t &pos) const {
    if (pos >= siz) {
      throw index_out_of_bound();
    }
    return arr[pos];
  }
  /**
   * access the first element.
   * throw container_is_empty if size == 0
   */
  const T &front() const {
    if (!siz) {
      throw container_is_empty();
    }
    return arr[0];
  }
  /**
   * access the last element.
   * throw container_is_empty if size == 0
   */
  const T &back() const {
    if (!siz) {
      throw container_is_empty();
    }
    return arr[siz - 1];
  }
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() { return iterator(arr, arr); }
  const_iterator cbegin() const { return const_iterator(arr, arr); }
  /**
   * returns an iterator to the end.
   */
  iterator end() { return iterator(arr, arr + siz); }
  const_iterator cend() const { return const_iterator(arr, arr + siz); }
  /**
   * checks whether the container is empty
   */
  bool empty() const { return !siz; }
  /**
   * returns the number of elements
   */
  size_t size() const { return siz; }
  /**
   * clears the contents
   */
  void clear() {
    for (int i = 0; i < siz; i++) {
      arr[i].~T();
    }
    free(arr);
    arr = nullptr;
    siz = cap = 0;
  }
  /**
   * inserts value before pos
   * returns an iterator pointing to the inserted value.
   */
  iterator insert(iterator pos, const T &value) {
    int ind = pos - begin();
    if (ind < 0 || ind > siz) {
      throw invalid_iterator(); //? what error?
    };
    return insert(ind, value);
  }
  /**
   * inserts value at index ind.
   * after inserting, this->at(ind) == value
   * returns an iterator pointing to the inserted value.
   * throw index_out_of_bound if ind > size (in this situation ind can be size
   * because after inserting the size will increase 1.)
   */
  iterator insert(const size_t &ind, const T &value) {
    if (ind > siz) {
      throw index_out_of_bound();
    }
    expand();
    for (size_t i = siz++; i > ind; i--) {
      new (arr + i) T(arr[i - 1]);
    }
    new (arr + ind) T(value);
    return begin() + ind;
  }
  /**
   * removes the element at pos.
   * return an iterator pointing to the following element.
   * If the iterator pos refers the last element, the end() iterator is
   * returned.
   */
  iterator erase(iterator pos) {
    int ind = pos - begin();
    if (ind < 0 || ind >= siz) {
      throw invalid_iterator(); //? what error?
    };
    return erase(ind);
  }
  /**
   * removes the element with index ind.
   * return an iterator pointing to the following element.
   * throw index_out_of_bound if ind >= size
   */
  iterator erase(const size_t &ind) {
    if (ind > siz) {
      throw index_out_of_bound();
    }
    siz--;
    for (size_t i = ind; i < siz; i++) {
      arr[i] = arr[i + 1];
    }
    arr[siz].~T();
    return begin() + ind;
  }
  /**
   * adds an element to the end.
   */
  void push_back(const T &value) {
    expand();
    new (arr + siz++) T(value); // placement new
  }
  /**
   * remove the last element from the end.
   * throw container_is_empty if size() == 0
   */
  void pop_back() {
    if (!siz) {
      throw container_is_empty();
    }
    arr[--siz].~T();
  }
}; // class vector

} // namespace sjtu

#endif
