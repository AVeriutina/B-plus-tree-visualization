#pragma once
#include <cstddef>
#include <functional>

#include "BPTreeNode.h"

namespace BPT::BPTree::Detail {
class Iterator;
}

template <>
struct std::hash<BPT::BPTree::Detail::Iterator> {
  std::size_t operator()(const BPT::BPTree::Detail::Iterator &lhs) const;
};

namespace BPT::BPTree::Detail {
class IteratorChild;
class ChildrenView;

class Iterator {
  friend std::hash<Iterator>;

 public:
  Iterator(Node *node);

  [[nodiscard]] bool HasParent() const;
  [[nodiscard]] bool IsLeaf() const;
  void MoveToParent();
  [[nodiscard]] bool HasRightSibling() const;
  void MoveToRightSibling();
  [[nodiscard]] bool HasLeftSibling() const;
  void MoveToLeftSibling();
  bool operator==(const Iterator &rhs) const = default;
  const std::vector<KeyType> &GetKeys();
  ChildrenView Children();

  bool IsValid() const;

 private:
  Node *node_;
};

class ChildrenView {
 public:
  ChildrenView(Node *node);
  [[nodiscard]] IteratorChild begin() const;
  [[nodiscard]] IteratorChild end() const;

 private:
  Node *node_;
};

class IteratorChild {
 public:
  IteratorChild(Iterator iter_node, bool is_end = false)
      : iter_(iter_node), is_end_(is_end) {}

  bool operator==(const IteratorChild &rhs);
  bool operator!=(const IteratorChild &rhs);
  IteratorChild &operator++();
  IteratorChild &operator--();
  Iterator &operator*();

 private:
  Iterator iter_;
  bool is_end_;
};

}  // namespace BPT::BPTree::Detail
