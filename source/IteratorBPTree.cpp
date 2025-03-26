#include "IteratorBPTree.h"

#include <cassert>

#include "BPTreeNode.h"

std::size_t std::hash<BPT::BPTree::Detail::Iterator>::operator()(
    const BPT::BPTree::Detail::Iterator &lhs) const {
  return std::hash<BPT::BPTree::Detail::Node *>{}(lhs.node_);
}

namespace BPT::BPTree::Detail {

Iterator::Iterator(Node *node) : node_(node) {}

bool Iterator::IsValid() const { return node_ != nullptr; }

bool Iterator::HasParent() const {
  assert(IsValid());
  return node_->parent != nullptr;
}

bool Iterator::IsLeaf() const {
  assert(IsValid());
  return BPTree::Detail::IsLeaf(node_);
}

void Iterator::MoveToParent() {
  assert(IsValid());
  node_ = node_->parent;
}

bool Iterator::HasRightSibling() const {
  assert(IsValid());
  return BPTree::Detail::RightSibling(node_) != nullptr;
}

void Iterator::MoveToRightSibling() {
  assert(IsValid());
  Node *right_sibling = BPTree::Detail::RightSibling(node_);
  assert(right_sibling);
  node_ = right_sibling;
}

bool Iterator::HasLeftSibling() const {
  assert(IsValid());
  return BPTree::Detail::LeftSibling(node_) != nullptr;
}

void Iterator::MoveToLeftSibling() {
  assert(IsValid());
  Node *left_sibling = BPTree::Detail::LeftSibling(node_);
  assert(left_sibling);
  node_ = left_sibling;
}

IteratorChild Iterator::ChildrenBegin() {
  assert(!IsLeaf());
  return IteratorChild(Iterator(GetFistChild(node_)));
}

IteratorChild Iterator::ChildrenEnd() {
  assert(!IsLeaf());
  return IteratorChild(Iterator(BPTree::Detail::GetLastChild(node_)), true);
}

const std::vector<KeyType> &Iterator::GetKeys() { return node_->keys; }

IteratorChild begin(Iterator &node) { return node.ChildrenBegin(); }

IteratorChild end(Iterator &node) { return node.ChildrenEnd(); }

bool operator==(const IteratorChild &lhs, const IteratorChild &rhs) {
  return lhs.value == rhs.value && lhs.is_end == rhs.is_end;
}

IteratorChild &operator++(IteratorChild &iter) {
  assert(iter.is_end);
  if (!iter.value.HasRightSibling()) {
    iter.is_end = true;
    return iter;
  };
  iter.value.MoveToRightSibling();
  return iter;
}

IteratorChild &operator--(IteratorChild &iter) {
  assert(iter.is_end);
  assert(iter.value.HasLeftSibling());

  if (iter.is_end) {
    iter.is_end = false;
  };
  iter.value.MoveToLeftSibling();
  return iter;
}

Iterator &operator*(IteratorChild &iter) { return iter.value; }

}  // namespace BPT::BPTree::Detail
