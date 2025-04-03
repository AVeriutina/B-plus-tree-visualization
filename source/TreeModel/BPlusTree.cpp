#include "BPlusTree.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>

#include "BPTreeData.h"
#include "GeomModel.h"
#include "IteratorBPTree.h"
#include "Observer.h"

namespace BPT::BPTree {

BPlusTree::BPlusTree() : port_([this]() { return GetData(); }) {}

void BPlusTree::SetDegree(int64_t max_degree) {
  assert(max_degree > 2);
  Reset();
  max_degree_ = max_degree;
}

bool BPlusTree::FindKey(KeyType key) {
  Node *node_with_key = FindLeafWithKey(key);
  assert(node_with_key != nullptr);
  statuses_.clear();
  bool is_key_in_node = IsKeyInNode(node_with_key, key);
  if (is_key_in_node) {
    statuses_[Iterator(node_with_key)] = Status::Found;
  } else {
    statuses_[Iterator(node_with_key)] = Status::NotFound;
  }
  port_.notify();
  statuses_.clear();
  port_.notify();
  return is_key_in_node;
}

bool BPlusTree::Insert(KeyType key) {
  if (root_ == nullptr) {
    root_ = std::make_unique<Node>();
    assert(root_ != nullptr);
    root_->keys.push_back(key);

    statuses_.clear();
    port_.notify();

    return true;
  }

  Node *leaf = FindLeafWithKey(key);
  assert(leaf != nullptr);
  bool cant_be_insert = IsKeyInNode(leaf, key);

  if (cant_be_insert) {
    statuses_[Iterator(leaf)] = Status::NotFound;
    port_.notify();

    statuses_.clear();
    port_.notify();

    return false;
  } else {
    statuses_[Iterator(leaf)] = Status::Found;
    port_.notify();
  }

  InsertKeyInNode(leaf, key);

  if (leaf->keys.size() >= max_degree_) {
    Split(leaf);
  }

  statuses_.clear();
  port_.notify();

  assert(IsStateCorrect(root_.get()));
  return true;
}

bool BPlusTree::Delete(KeyType key) {
  Node *leaf = FindLeafWithKey(key);
  if (!leaf || !IsKeyInNode(leaf, key)) {
    return false;
  }

  statuses_.clear();
  DeleteInNode(leaf, key);
  assert(IsStateCorrect(root_.get()));
  return true;
}

void BPlusTree::Reset() {
  if (root_) {
    root_ = nullptr;
  }
  statuses_.clear();
  port_.notify();
}

void BPlusTree::SubscribeGeomModel(GeomModel *geom_model_) {
  port_.subscribe(
      static_cast<NSLibrary::CObserver<Data, NSLibrary::CByValue> *>(
          geom_model_->GetObserverPort()));
}

void BPlusTree::InsertKeyInNode(Node *node, KeyType key) {
  assert(node);
  assert(!IsKeyInNode(node, key));
  auto iter = std::ranges::lower_bound(node->keys, key);
  node->keys.insert(iter, key);
}

void BPlusTree::Split(Node *old_node) {
  assert(old_node != nullptr);

  statuses_.clear();
  statuses_[Iterator(old_node)] = Status::IntermediateState;
  port_.notify();

  Node *parent = old_node->parent;
  auto new_left_node = std::make_unique<Node>();
  auto new_right_node = std::make_unique<Node>();

  if (old_node == root_.get()) {
    auto prev_root = std::move(root_);
    root_ = std::make_unique<Node>();
    prev_root->parent = root_.get();
    root_->children.push_back(std::move(prev_root));
    parent = root_.get();
  }

  size_t pos_of_central_key = old_node->keys.size() / 2;
  KeyType central_key = old_node->keys[pos_of_central_key];

  if (IsLeaf(old_node)) {
    LinkSiblings(old_node->left, new_left_node.get());
    LinkSiblings(new_left_node.get(), new_right_node.get());
    LinkSiblings(new_right_node.get(), old_node->right);

  } else {
    Detail::AssignLeftHalf(&old_node->children, &new_left_node->children);
    Detail::AssignRightHalf(&old_node->children, &new_right_node->children);

    UpdateParent(new_left_node->children, new_left_node.get());
    UpdateParent(new_right_node->children, new_right_node.get());

    old_node->keys.erase(old_node->keys.begin() +
                         static_cast<long>(pos_of_central_key));
  }
  InsertKeyInNode(parent, central_key);

  Detail::AssignLeftHalf(&old_node->keys, &new_left_node->keys);
  Detail::AssignRightHalf(&old_node->keys, &new_right_node->keys);

  auto iter_pos_in_children = std::ranges::find_if(
      parent->children, [old_node](const std::unique_ptr<Node> &lhs) {
        return lhs.get() == old_node;
      });

  new_left_node->parent = parent;
  new_right_node->parent = parent;

  iter_pos_in_children->reset();
  *iter_pos_in_children = std::move(new_left_node);
  // parent->children.insert(iter_pos_in_children + 1,
  // std::move(new_right_node));
  auto new_iter = parent->children.emplace(iter_pos_in_children + 1,
                                           std::move(new_right_node));

  statuses_.clear();
  statuses_[Iterator(std::prev(new_iter)->get())] = Status::IntermediateState;
  statuses_[Iterator(new_iter->get())] = Status::IntermediateState;
  statuses_[Iterator(parent)] = Status::IntermediateState;
  port_.notify();

  statuses_.clear();
  port_.notify();

  if (parent->keys.size() >= max_degree_) {
    Split(parent);
  }
}

Detail::Node *BPlusTree::FindLeafWithKey(KeyType key) {
  return FindLeafWithKeyFromNode(key, root_.get());
}

Detail::Node *BPlusTree::FindLeafWithKeyFromNode(KeyType key, Node *node) {
  if (root_ == nullptr) {
    return nullptr;
  }
  assert(node != nullptr);

  statuses_.clear();
  statuses_[Iterator(node)] = Status::Search;
  port_.notify();

  if (IsLeaf(node)) {
    return node;
  }

  auto iter = std::ranges::upper_bound(node->keys, key);
  size_t num_of_child_with_key = std::distance(node->keys.begin(), iter);
  Node *child_with_key = node->children[num_of_child_with_key].get();

  return FindLeafWithKeyFromNode(key, child_with_key);
}

void BPlusTree::DeleteInNode(Node *node, KeyType key) {
  assert(node != nullptr);
  auto iter = Detail::FindIterOfKey(*node, key);
  assert(iter != node->keys.end());
  node->keys.erase(iter);

  port_.notify();

  if (node == root_.get()) {
    return;
  }

  if (node->keys.size() < (max_degree_ / 2)) {
    statuses_[Iterator(node)] = Status::IntermediateState;

    if (LeftSibling(node) &&
        LeftSibling(node)->keys.size() > (max_degree_ / 2)) {
      statuses_[Iterator(LeftSibling(node))] = Status::IntermediateState;
      port_.notify();

      BorrowFromLeft(node, key);
    } else if (RightSibling(node) &&
               RightSibling(node)->keys.size() > (max_degree_ / 2)) {
      statuses_[Iterator(RightSibling(node))] = Status::IntermediateState;
      port_.notify();

      BorrowFromRight(node);
    } else {
      KeyType key_of_node_in_parent = key;
      if (!node->keys.empty()) {
        key_of_node_in_parent =
            std::min(key_of_node_in_parent, node->keys.front());
      }
      Merge(node, key_of_node_in_parent);
    }
  }

  statuses_.clear();
  port_.notify();
}

void BPlusTree::BorrowFromLeft(Node *node, KeyType prev_key) {
  assert(node != nullptr);
  Node *left_sibling = LeftSibling(node);

  assert(!left_sibling->keys.empty());
  node->keys.insert(node->keys.begin(), left_sibling->keys.back());
  left_sibling->keys.pop_back();

  assert(!node->keys.empty());
  KeyType new_key = node->keys.front();

  if (!IsLeaf(node)) {
    assert(!left_sibling->children.empty());
    node->children.insert(node->children.begin(),
                          std::move(left_sibling->children.back()));
    left_sibling->children.pop_back();
  }
  UpdateKeys(node, prev_key, new_key);
}

void BPlusTree::BorrowFromRight(Node *node) {
  assert(node != nullptr);
  Node *right_sibling = RightSibling(node);

  assert(!right_sibling->keys.empty());
  node->keys.push_back(right_sibling->keys.front());
  KeyType prev_key = right_sibling->keys.front();
  right_sibling->keys.erase(right_sibling->keys.begin());

  assert(!right_sibling->keys.empty());
  KeyType new_key = right_sibling->keys.front();

  if (!IsLeaf(node)) {
    assert(!right_sibling->children.empty());
    node->children.push_back(std::move(right_sibling->children.front()));
    right_sibling->children.erase(right_sibling->children.begin());
  }
  UpdateKeys(node, prev_key, new_key);
}

void BPlusTree::Merge(Node *node, KeyType key_of_node_in_parent) {
  assert(node != nullptr);
  Node *left_sibling = LeftSibling(node);
  Node *right_sibling = RightSibling(node);
  Node *parent = node->parent;

  auto iter = std::ranges::find_if(
      parent->children,
      [node](const std::unique_ptr<Node> &lhs) { return lhs.get() == node; });
  assert(iter != parent->children.end());
  std::unique_ptr<Node> deleted_node = std::move(*iter);
  parent->children.erase(iter);

  if (left_sibling) {
    statuses_[Iterator(left_sibling)] = Status::IntermediateState;
    port_.notify();

    if (IsLeaf(node)) {
      LinkSiblings(left_sibling, node);
    } else {
      left_sibling->keys.insert(left_sibling->keys.end(),
                                key_of_node_in_parent);

      UpdateParent(node->children, left_sibling);

      left_sibling->children.insert(
          left_sibling->children.end(),
          std::make_move_iterator(node->children.begin()),
          std::make_move_iterator(node->children.end()));
    }

    left_sibling->keys.insert(left_sibling->keys.end(), node->keys.begin(),
                              node->keys.end());

    DeleteInNode(parent, key_of_node_in_parent);
    if (parent == root_.get() && root_->keys.empty()) {
      assert(!parent->children.empty());
      root_ = std::move(parent->children.front());
      node->parent = nullptr;
    }
  } else if (right_sibling) {
    statuses_[Iterator(right_sibling)] = Status::IntermediateState;
    port_.notify();

    if (IsLeaf(node)) {
      LinkSiblings(node, right_sibling);
    } else {
      assert(!right_sibling->children.empty());
      assert(!right_sibling->children.front()->keys.empty());
      right_sibling->keys.insert(right_sibling->keys.begin(),
                                 right_sibling->children.front()->keys.front());

      UpdateParent(node->children, right_sibling);

      right_sibling->children.insert(
          right_sibling->children.begin(),
          std::make_move_iterator(node->children.begin()),
          std::make_move_iterator(node->children.end()));
    }
    right_sibling->keys.insert(right_sibling->keys.end(), node->keys.begin(),
                               node->keys.end());
    assert(!right_sibling->keys.empty());
    DeleteInNode(parent, right_sibling->keys.front());
    if (parent == root_.get() && root_->keys.empty()) {
      assert(!parent->children.empty());
      root_ = std::move(parent->children.front());
      node->parent = nullptr;
    }
  }
}

void BPlusTree::UpdateKeys(Node *node, KeyType prev_key, KeyType new_key) {
  assert(node != nullptr);
  if (node->parent == nullptr) {
    return;
  }

  node = node->parent;
  auto iter = std::ranges::lower_bound(node->keys, prev_key);
  while (iter != node->keys.end() && *iter == prev_key) {
    *iter = new_key;
    node = node->parent;
    if (node != nullptr) {
      iter = std::ranges::lower_bound(node->keys, prev_key);
    } else {
      return;
    }
  }
}

Detail::DataFromBPTree BPlusTree::GetData() const {
  return {.iter = Detail::Iterator(root_.get()), .statuses = statuses_};
}

bool BPlusTree::IsStateCorrect(Node *node) const {
  if (node == nullptr) {
    return true;
  }
  if (max_degree_ <= 1) {
    return false;
  }
  if (!IsNodeStateCorrect(node)) {
    return false;
  }
  for (const auto &child : node->children) {
    if (!IsParentForNode(node, child.get())) {
      return false;
    }
  }
  for (const auto &child : node->children) {
    if (!IsStateCorrect(child.get())) {
      return false;
    }
  }
  return true;
}

}  // namespace BPT::BPTree
