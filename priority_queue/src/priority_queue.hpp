#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include "exceptions.hpp"
#include <cstddef>
#include <functional>

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 * default: max heap
 */
template <typename T, class Compare = std::less<T>> class priority_queue {
private:
  Compare cmp; // compare function
  size_t siz;  // size
  struct Node {
    T key;
    Node *lc = nullptr, *rc = nullptr; // left and right child
    int h = 1; // height (distance between this node and the closest null node)
    Node(const T &k) : key(k) {}
  } *root;
  int h(Node *p) { return p ? p->h : 0; } // height
  Node *copy(Node *o) {
    if (!o) {
      return nullptr;
    }
    Node *p = new Node(o->key);
    p->lc = copy(o->lc);
    p->rc = copy(o->rc);
    p->h = o->h;
    return p;
  }
  void destroy(Node *o) {
    if (o) {
      destroy(o->lc);
      destroy(o->rc);
      delete o;
    }
  }
  /**
   * @brief merge two leftist trees rooted at x and y
   */
  Node *merge(Node *x, Node *y) {
    if (!x || !y) {
      return x ? x : y;
    }
    if (cmp(x->key, y->key)) {
      std::swap(x, y);
    }
    x->rc = merge(x->rc, y);
    if (h(x->rc) > h(x->lc)) {
      std::swap(x->rc, x->lc);
    }
    x->h = h(x->rc) + 1;
    return x;
  }

public:
  /**
   * constructors
   */
  priority_queue() : root(nullptr), siz(0) {}
  priority_queue(const priority_queue &other) : siz(other.siz) {
    root = copy(other.root);
  }
  /**
   * destructor
   */
  ~priority_queue() { destroy(root); }
  /**
   * Assignment operator
   */
  priority_queue &operator=(const priority_queue &other) {
    if (this == &other) {
      return *this;
    }
    siz = other.siz;
    destroy(root);
    root = copy(other.root);
    return *this;
  }
  /**
   * get the top of the queue.
   * @return a reference of the top element.
   * throw container_is_empty if empty() returns true;
   */
  const T &top() const {
    if (!siz) {
      throw container_is_empty();
    }
    return root->key;
  }
  /**
   * push new element to the priority queue.
   */
  void push(const T &e) {
    Node *p = new Node(e);
    try {
      root = merge(root, p);
      siz++;
    } catch (...) {
      delete p;
      throw;
    }
  }
  /**
   * delete the top element.
   * throw container_is_empty if empty() returns true;
   */
  void pop() {
    if (!siz) {
      throw container_is_empty();
    }
    Node *p = root;
    root = merge(root->lc, root->rc);
    delete p;
    siz--;
  }
  /**
   * return the number of the elements.
   */
  size_t size() const { return siz; }
  /**
   * check if the container has at least an element.
   * @return true if it is empty, false if it has at least an element.
   */
  bool empty() const { return !siz; }
  /**
   * merge two priority_queues with at least O(logn) complexity.
   * clear the other priority_queue.
   */
  void merge(priority_queue &other) {
    root = merge(root, other.root), other.root = nullptr;
    siz += other.siz, other.siz = 0;
  }
}; // class priority_queue

} // namespace sjtu

#endif
