#include "IteratorBPTree.h"

#include <cassert>

#include "BPTreeNode.h"

std::size_t std::hash<BPT::BPTree::Detail::Iterator>::operator()(
    const BPT::BPTree::Detail::Iterator &lhs) const {
  return std::hash<BPT::BPTree::Detail::Node *>{}(lhs.node_);
}

namespace BPT::BPTree::Detail {

Iterator::Iterator(Node *node) : node_(node) {}

bool Iterator::HasParent() const {
  if (!IsValid()) {
    return false;
  }
  return node_->parent != nullptr;
}

bool Iterator::IsLeaf() const {
  if (!IsValid()) {
    return false;
  }
  return BPTree::Detail::IsLeaf(node_);
}

void Iterator::MoveToParent() {
  assert(IsValid());
  node_ = node_->parent;
}

bool Iterator::HasRightSibling() const {
  if (!IsValid()) {
    return false;
  }
  return BPTree::Detail::RightSibling(node_) != nullptr;
}

void Iterator::MoveToRightSibling() {
  assert(IsValid());
  Node *right_sibling = BPTree::Detail::RightSibling(node_);
  assert(right_sibling);
  node_ = right_sibling;
}

bool Iterator::HasLeftSibling() const {
  if (!IsValid()) {
    return false;
  }
  return BPTree::Detail::LeftSibling(node_) != nullptr;
}

void Iterator::MoveToLeftSibling() {
  assert(IsValid());
  Node *left_sibling = BPTree::Detail::LeftSibling(node_);
  assert(left_sibling);
  node_ = left_sibling;
}

ChildrenView Iterator::Children() {
  assert(!IsLeaf());
  return {node_};
}

const std::vector<KeyType> &Iterator::GetKeys() {
  assert(IsValid());
  return node_->keys;
}

bool Iterator::IsValid() const { return node_ != nullptr; }

ChildrenView::ChildrenView(Node *node) : node_(node) {}

IteratorChild ChildrenView::begin() const {
  return {Iterator(GetFirstChild(node_))};
}

IteratorChild ChildrenView::end() const {
  return {Iterator(GetLastChild(node_)), true};
}

bool IteratorChild::operator==(const IteratorChild &rhs) {
  return iter_ == rhs.iter_ && is_end_ == rhs.is_end_;
}

bool IteratorChild::operator!=(const IteratorChild &rhs) {
  return iter_ != rhs.iter_ || is_end_ != rhs.is_end_;
}

IteratorChild &IteratorChild::operator++() {
  assert(!is_end_);
  if (!iter_.HasRightSibling()) {
    is_end_ = true;
    return *this;
  };
  iter_.MoveToRightSibling();
  return *this;
}

IteratorChild &IteratorChild::operator--() {
  assert(iter_.HasLeftSibling());

  if (is_end_) {
    is_end_ = false;
  };
  iter_.MoveToLeftSibling();
  return *this;
}

Iterator &IteratorChild::operator*() { return iter_; }

}  // namespace BPT::BPTree::Detail
