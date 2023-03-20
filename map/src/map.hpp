/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include "exceptions.hpp"
#include "utility.hpp"
#include <cstddef>
#include <functional>

namespace sjtu {

template <class Key, class T, class Compare = std::less<Key>> class map {
private:
  size_t siz; // size
  Compare cmp;

  struct Node;
  typedef Node *NodePtr;
  static constexpr bool R = 1, B = 0; // red=1,black=0
  struct Node {
    pair<const Key, T> data; // default constructor for Key might be missing
    bool color = R;          // red-black tree
    NodePtr lc = nullptr, rc = nullptr; // left and right children
    NodePtr pa = nullptr;               // parent
    Node(const Key &k) : data(k, T()) {}
    NodePtr &ch(bool d) { return d ? rc : lc; }
    const Key &key() const { return data.first; }
    T &value() { return data.second; }
    T &value() const { return data.second; }
  };
  NodePtr root;

  /**
   * @brief copies a tree
   *
   * @param x current node
   * @param p parent (to be copied)
   * @return root of the copied tree
   */
  NodePtr copy(NodePtr x, NodePtr p = nullptr) {
    if (!x) {
      return nullptr;
    }
    NodePtr y = new Node(*x);
    y->lc = copy(x->lc, y);
    y->rc = copy(x->rc, y);
    y->pa = p;
    return y;
  }

  /**
   * @brief destroys a tree
   */
  void destroy(NodePtr x) {
    if (x) {
      destroy(x->lc);
      destroy(x->rc);
      delete x;
    }
  }

  /**
   * @brief check if two keys are equal
   */
  bool equal(const Key &a, const Key &b) const {
    return !cmp(a, b) && !cmp(b, a);
  }

  /**
   * @brief check if x is the right child of its parent
   */
  bool id(NodePtr x) const { return x == x->pa->rc; }

  /**
   * @brief checks if a (possibly null) node is black/red
   */
  bool black(NodePtr x) const { return !x || x->color == B; }
  bool red(NodePtr x) const { return x && x->color == R; }

  /**
   * @brief rotates a node upwards
   *
   * @param x the child node
   */
  void rotate(NodePtr x) {
    bool d = id(x);
    NodePtr p = x->pa, g = p->pa; // parent, grandparent
    x->pa = g;
    if (g) {
      g->ch(id(p)) = x;
    } else {
      root = x;
    }
    p->ch(d) = x->ch(!d), x->ch(!d) = p, p->pa = x;
    if (p->ch(d)) {
      p->ch(d)->pa = p;
    }
  }

  /**
   * @brief fix coloring after insertion
   *
   * @param x the newly inserted red node
   */
  void RB_insert_fixup(NodePtr x) {
    while (red(x->pa)) {
      NodePtr p = x->pa, g, u; // parent, grandparent, uncle
      g = p->pa, u = g->ch(!id(p));
      if (red(u)) { // #1
        p->color = u->color = B;
        g->color = R;
        x = g;
      } else {
        // uncle is black
        if (id(x) != id(p)) { // #2
          rotate(x), rotate(x);
          x->color = B;
        } else { // #3
          rotate(p);
          p->color = B;
        }
        g->color = R;
        break;
      }
    }
    root->color = B; // root is always black
  }

  /**
   * @brief finds a node with the given key
   *
   * @param key
   * @param mode 0:find(replace), 1:insert
   * @return the wanted node (possibly after insertion), null if error occurs
   */
  NodePtr RB_find(const Key &key, int mode = 0) {
    NodePtr y = nullptr, z = root;
    while (z && !equal(key, z->key())) { // finds the place for insertion
      y = z;
      z = z->ch(cmp(z->key(), key));
    }
    if (z || mode == 0) { // finds
      return z;
    }
    // inserts
    siz++;
    NodePtr x = new Node(key);
    x->pa = y;
    if (!y) {
      root = x;
    } else {
      y->ch(cmp(y->key(), key)) = x;
    }
    RB_insert_fixup(x);
    return x;
  }
  NodePtr RB_find(const Key &key) const {
    NodePtr z = root;
    while (z && !equal(key, z->key())) { // finds the place for insertion
      z = z->ch(!cmp(key, z->key()));
    }
    return z;
  }

  /**
   * @brief finds the minimum(0)/maximum(1) in the (nonempty) subtree rooted
   * at
   * x
   */
  NodePtr minmax(NodePtr x, bool d) const {
    if (!x) {
      return x;
    }
    while (x->ch(d)) {
      x = x->ch(d);
    }
    return x;
  }
  NodePtr min(NodePtr x) const { return minmax(x, 0); }
  NodePtr max(NodePtr x) const { return minmax(x, 1); }
  /**
   * @brief finds predecessor(0)/successor(1) of x
   */
  NodePtr predsucc(NodePtr x, bool d) const {
    if (x->ch(d)) {
      return minmax(x->ch(d), !d);
    }
    for (; x->pa; x = x->pa) {
      if (x->pa->ch(!d) == x) {
        return x->pa;
      }
    }
    return nullptr; // cannot find predecessor/successor
  }
  NodePtr pred(NodePtr x) const { return predsucc(x, 0); }
  NodePtr succ(NodePtr x) const { return predsucc(x, 1); }

  /**
   * @brief transplant v to u's position (u exists; only affects the parent)
   */
  void RB_transplant(NodePtr u, NodePtr v) {
    if (u == root) {
      root = v;
    } else {
      u->pa->ch(id(u)) = v;
    }
    if (v) {
      v->pa = u->pa;
    }
  }

  /**
   * @brief fix coloring after deletion
   *
   * @param x node whose black parent was removed (can be null!!!)
   * @param p new parent
   */
  void RB_delete_fixup(NodePtr x, NodePtr p) {
    while (x != root && black(x)) {
      bool d = (p->rc != x); // id(x)^1, x can be null
      NodePtr b = p->ch(d);  // brother (exists, and has 2 children if x exists)
      if (red(b)) {          // #1
        rotate(b);
        b->color = B, p->color = R;
        b = p->ch(d);
      }
      // now b is black
      if (black(b->lc) && black(b->rc)) { // #2
        b->color = R;
        x = p, p = x->pa;
      } else {
        if (black(b->ch(d))) { // #3
          rotate(b->ch(!d));
          b->color = R;
          b = b->pa;
          b->color = B;
        }
        // #4
        rotate(b);
        b->color = p->color;
        p->color = B;
        b->ch(d)->color = B; // exists
        x = root;            // break;
      }
    }
    if (x) {
      x->color = B;
    }
  }
  /**
   * @brief deletes a given node
   */
  void RB_delete(NodePtr z) {
    siz--;
    NodePtr y = z, x,
            p = z->pa;    // p: parent of x after deletion (x might be null)
    bool flag = black(y); // it is always OK to remove a red node
    if (!z->lc) {
      x = z->rc;
      RB_transplant(z, x);
    } else if (!z->rc) {
      x = z->lc;
      RB_transplant(z, x);
    } else {
      y = min(z->rc); // successor
      flag = y->color == B;
      x = y->rc;
      if (y->pa == z) {
        p = y;
      } else {
        p = y->pa;
        RB_transplant(y, x);
        y->rc = z->rc; // exists
        y->rc->pa = y;
      }
      RB_transplant(z, y);
      y->lc = z->lc;
      y->lc->pa = y; // exists
      y->color = z->color;
    }
    delete z;
    if (flag) {
      RB_delete_fixup(x, p);
    }
  }

public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  using key_type = Key;
  using mapped_type = T;
  using value_type = pair<const Key, T>;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;
  class iterator {
    friend class map;
    friend class const_iterator;

  private:
    /**
     * add data members
     *   just add whatever you want.
     */
    NodePtr ptr;
    map *tree; // for checking whether this iterator belongs to a given object
    iterator(NodePtr p, map *t) : ptr(p), tree(t) {}

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = map::value_type;
    using pointer = map::value_type *;
    using reference = map::value_type &;
    using difference_type = std::ptrdiff_t;

    iterator() : ptr(nullptr), tree(nullptr) {}
    iterator(const iterator &other) : ptr(other.ptr), tree(other.tree) {}
    iterator(const const_iterator &other) : ptr(other.ptr), tree(other.tree) {}
    /**
     * iter++
     */
    iterator operator++(int) {
      if (!ptr) {
        throw invalid_iterator();
      }
      iterator ret = *this;
      ptr = tree->succ(ptr);
      return ret;
    }
    /**
     * ++iter
     */
    iterator &operator++() {
      if (!ptr) {
        throw invalid_iterator();
      }
      ptr = tree->succ(ptr);
      return *this;
    }
    /**
     * iter--
     */
    iterator operator--(int) {
      iterator ret = *this;
      --*this;
      return ret;
    }
    /**
     * --iter
     */
    iterator operator--() {
      NodePtr p = ptr ? tree->pred(ptr) : tree->max(tree->root);
      if (!p) {
        throw invalid_iterator();
      }
      ptr = p;
      return *this;
    }
    /**
     * an operator for checking whether two iterators are the same (pointing to
     * the same memory).
     */
    value_type &operator*() const {
      if (!ptr) {
        throw invalid_iterator();
      }
      return ptr->data;
    }
    bool operator==(const iterator &rhs) const {
      return ptr == rhs.ptr && tree == rhs.tree;
    }
    bool operator==(const const_iterator &rhs) const {
      return ptr == rhs.ptr && tree == rhs.tree;
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
    bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }

    /**
     * for the support of it->first.
     * See
     * <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/>
     * for help.
     */
    value_type *operator->() const noexcept { return &(ptr->data); }
  }; // class iterator
  class const_iterator {
    friend class map;
    friend class const_iterator;

  private:
    /**
     * add data members
     *   just add whatever you want.
     */
    NodePtr ptr;
    const map
        *tree; // for checking whether this iterator belongs to a given object
               //! must have const !!! (error info: passing ... as ... discards
               //! qualifiers; invalid conversion)
    const_iterator(NodePtr p, const map *t) : ptr(p), tree(t) {}

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = map::value_type;
    using pointer = const map::value_type *;
    using reference = const map::value_type &;
    using difference_type = std::ptrdiff_t;

    const_iterator() : ptr(nullptr), tree(nullptr) {}
    const_iterator(const const_iterator &other)
        : ptr(other.ptr), tree(other.tree) {}
    const_iterator(const iterator &other) : ptr(other.ptr), tree(other.tree) {}
    /**
     * iter++
     */
    const_iterator operator++(int) {
      if (!ptr) {
        throw invalid_iterator();
      }
      const_iterator ret = *this;
      ptr = tree->succ(ptr);
      return ret;
    }
    /**
     * ++iter
     */
    const_iterator &operator++() {
      if (!ptr) {
        throw invalid_iterator();
      }
      ptr = tree->succ(ptr);
      return *this;
    }
    /**
     * iter--
     */
    const_iterator operator--(int) {
      const_iterator ret = *this;
      --*this;
      return ret;
    }
    /**
     * --iter
     */
    const_iterator operator--() {
      NodePtr p = ptr ? tree->pred(ptr) : tree->max(tree->root);
      if (!p) {
        throw invalid_iterator();
      }
      ptr = p;
      return *this;
    }
    /**
     * an operator for checking whether two iterators are the same (pointing to
     * the same memory).
     */
    const value_type &operator*() const {
      if (!ptr) {
        throw invalid_iterator();
      }
      return ptr->data;
    }
    bool operator==(const iterator &rhs) const {
      return ptr == rhs.ptr && tree == rhs.tree;
    }
    bool operator==(const const_iterator &rhs) const {
      return ptr == rhs.ptr && tree == rhs.tree;
    }
    /**
     * some other operators for iterator.
     */
    bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
    bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }

    /**
     * for the support of it->first.
     * See
     * <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/>
     * for help.
     */
    const value_type *operator->() const noexcept { return &(ptr->data); }
  }; // class const_iterator
  /**
   * two constructors
   */
  map() : siz(0), root(nullptr) {}
  map(const map &other) : siz(other.siz) { root = copy(other.root); }
  /**
   * assignment operator
   */
  map &operator=(const map &other) {
    if (this == &other) {
      return *this;
    }
    siz = other.siz;
    destroy(root);
    root = copy(other.root);
    return *this;
  }
  /**
   * Destructors
   */
  ~map() { destroy(root); }
  /**
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key
   * equivalent to key. If no such element exists, an exception of type
   * `index_out_of_bound'
   */
  T &at(const Key &key) {
    NodePtr p = RB_find(key);
    if (!p) {
      throw index_out_of_bound();
    }
    return p->value();
  }
  const T &at(const Key &key) const {
    NodePtr p = RB_find(key);
    if (!p) {
      throw index_out_of_bound();
    }
    return p->value();
  }
  /**
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to
   * key, performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) { return RB_find(key, 1)->value(); }
  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const { return at(key); }
  /**
   * return a iterator to the beginning
   */
  iterator begin() { return iterator(min(root), this); }
  const_iterator cbegin() const { return const_iterator(min(root), this); }
  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() { return iterator(nullptr, this); }
  const_iterator cend() const { return const_iterator(nullptr, this); }
  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const { return !siz; }
  /**
   * returns the number of elements.
   */
  size_t size() const { return siz; }
  /**
   * clears the contents
   */
  void clear() {
    destroy(root);
    root = nullptr;
    siz = 0;
  }
  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the
   * insertion), the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    size_t sz0 = siz;
    NodePtr p = RB_find(value.first, 1);
    if (siz == sz0) {
      return pair<iterator, bool>(iterator(p, this), 0);
    }
    p->value() = value.second;
    return pair<iterator, bool>(iterator(p, this), 1);
  }
  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points
   * an element out of this)
   */
  void erase(iterator pos) {
    if (!pos.ptr || pos.tree != this) { //! it's pos.___ , not pos->___
      throw invalid_iterator();
    }
    RB_delete(pos.ptr);
  }
  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b < a)
   */
  size_t count(const Key &key) const { return RB_find(key) != nullptr; }
  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is
   * returned.
   */
  iterator find(const Key &key) { return iterator(RB_find(key), this); }
  const_iterator find(const Key &key) const {
    return const_iterator(RB_find(key), this);
  }
  // int debug(NodePtr o, int dep = 0) const {
  //   if (o == nullptr)
  //     return 0;
  //   int bh1 = debug(o->lc, dep + 1);
  //   std::cerr << o << " value=" << o->value() <<  " color="<< "BR"[o->color]
  //             << " dep=" << dep << std::endl;
  //   int bh2 = debug(o->rc, dep + 1);
  //   assert(bh1 == bh2);
  //   return bh1 + (o->color == B);
  // }
  // void debug() const { debug(root); }
}; // class map
} // namespace sjtu

#endif
