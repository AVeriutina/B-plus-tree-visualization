#pragma once

#include "BPlusTree.h"

namespace BPTree::Detail {
struct IteratorChild;

class Iterator {
 public:
  Iterator(Node *node) : node_(node) {}

  bool IsValid() const;
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

}  // namespace BPTree::Detail
