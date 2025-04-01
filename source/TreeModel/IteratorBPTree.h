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
struct IteratorChild;

class Iterator {
  friend std::hash<Iterator>;

 public:
  Iterator(Node *node);

  bool HasParent() const;
  bool IsLeaf() const;
  void MoveToParent();
  bool HasRightSibling() const;
  void MoveToRightSibling();
  bool HasLeftSibling() const;
  void MoveToLeftSibling();

  IteratorChild ChildrenBegin();
  IteratorChild ChildrenEnd();
  bool operator==(const Iterator &rhs) const = default;
  const std::vector<KeyType> &GetKeys();

  bool IsValid() const;

 private:
  Node *node_;
};

IteratorChild begin(Iterator &node);
IteratorChild end(Iterator &node);

struct IteratorChild {
  explicit IteratorChild(Iterator iter_node, bool is_end = false)
      : value(iter_node), is_end(is_end) {}

  Iterator value;
  bool is_end;
};

bool operator==(const IteratorChild &lhs, const IteratorChild &rhs);
IteratorChild &operator++(IteratorChild &iter);
IteratorChild &operator--(IteratorChild &iter);
Iterator &operator*(IteratorChild &iter);

}  // namespace BPT::BPTree::Detail
