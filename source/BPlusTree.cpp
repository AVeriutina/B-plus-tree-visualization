#include "BPlusTree.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>

#include "IteratorBPTree.h"

namespace BPT::BPTree {

namespace Detail {

bool IsLeaf(const Node *node) {
  assert(node != nullptr);
  return node->children.empty();
}

Node *LeftSibling(Node *node) {
  assert(node);
  const Node *parent = node->parent;
  if (parent == nullptr) {
    return nullptr;
  }
  auto iter_of_right = std::ranges::find_if(
      parent->children,
      [node](const std::unique_ptr<Node> &lhs) { return lhs.get() == node; });
  assert(iter_of_right != parent->children.end());
  if (iter_of_right == parent->children.begin()) {
    return nullptr;
  }
  return (*(iter_of_right - 1)).get();
}

Node *RightSibling(Node *left_sibling) {
  assert(left_sibling);
  const Node *parent = left_sibling->parent;
  if (parent == nullptr) {
    return nullptr;
  }
  auto iter_of_left = std::ranges::find_if(
      parent->children, [left_sibling](const std::unique_ptr<Node> &lhs) {
        return lhs.get() == left_sibling;
      });
  assert(iter_of_left != parent->children.end());
  if (iter_of_left == parent->children.end() - 1) {
    return nullptr;
  }
  return (*(iter_of_left + 1)).get();
}

bool IsKeyInNode(const Node *node, KeyType key) {
  assert(node != nullptr);
  return std::ranges::binary_search(node->keys, key);
}

auto FindIterOfKey(const Node &node, KeyType key) {
  auto iter = std::ranges::lower_bound(node.keys, key);
  assert(iter != node.keys.end() && *iter == key);
  return iter;
}

void Link(Node *left_node, Node *right_node) {
  if (left_node) {
    left_node->right = right_node;
  }
  if (right_node) {
    right_node->left = left_node;
  }
}

void UpdateParent(const std::vector<std::unique_ptr<Node>> &children,
                  Node *new_parant) {
  for (const auto &child : children) {
    child->parent = new_parant;
  }
}

bool IsNodeStateCorrect(Node *node) {
  if (!IsLeaf(node)) {
    return node->keys.size() + 1 == node->children.size();
  }
  return true;
}

bool IsLinkWithChildCorrect(Node *parent, Node *child) {
  return child->parent == parent;
}

Node *GetFistChild(Node *node) {
  assert(node);
  assert(!IsLeaf(node));
  return node->children.front().get();
}

Node *GetLastChild(Node *node) {
  assert(node);
  assert(!IsLeaf(node));
  return node->children.back().get();
}

}  // namespace Detail

BPlusTree::BPlusTree() : port_([this]() { return GetData(); }) {}

void BPlusTree::SetDegree(int32_t max_degree) {
  assert(max_degree > 1);
  Clear();
  max_degree_ = max_degree;
}

void BPlusTree::Clear() {
  if (root_) {
    root_ = nullptr;
  }
  max_degree_ = 0;
}

bool BPlusTree::IsStateCorrect(Node *ptr) {
  if (ptr == nullptr) {
    return true;
  }
  if (max_degree_ <= 1) {
    return false;
  }
  if (!IsNodeStateCorrect(ptr)) {
    return false;
  }
  for (const auto &child : ptr->children) {
    if (!IsLinkWithChildCorrect(ptr, child.get())) {
      return false;
    }
  }
  for (const auto &child : ptr->children) {
    if (!IsStateCorrect(child.get())) {
      return false;
    }
  }
  return true;
}

bool BPlusTree::Insert(KeyType key) {
  if (root_ == nullptr) {
    root_ = std::make_unique<Node>();
    assert(root_ != nullptr);
    root_->keys.push_back(key);
    return true;
  }

  if (FindKey(key)) {
    return false;
  }

  Node *leaf = FindLeafWithKey(key);
  assert(leaf != nullptr);

  InsertKeyInNode(leaf, key);
  statuses_.clear();
  port_.notify();

  if (leaf->keys.size() >= max_degree_) {
    Split(leaf);
  }

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

void BPlusTree::Split(Node *old_node) {
  assert(old_node != nullptr);
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
    Link(old_node->left, new_left_node.get());
    Link(new_left_node.get(), new_right_node.get());
    Link(new_right_node.get(), old_node->right);

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
  parent->children.insert(iter_pos_in_children + 1, std::move(new_right_node));

  statuses_.clear();
  port_.notify();

  if (parent->keys.size() >= max_degree_) {
    Split(parent);
  }
}

bool BPlusTree::FindKey(KeyType key) {
  statuses_.clear();
  const Node *node_with_key = FindLeafWithKey(key);
  assert(node_with_key != nullptr);
  return IsKeyInNode(node_with_key, key);
}

void BPlusTree::InsertKeyInNode(Node *node, KeyType key) {
  assert(node);
  assert(!IsKeyInNode(node, key));
  auto iter = std::ranges::lower_bound(node->keys, key);
  node->keys.insert(iter, key);
}

Detail::Node *BPlusTree::FindLeafWithKey(KeyType key) {
  return FindLeafWithKeyFromNode(key, root_.get());
}

Detail::Node *BPlusTree::FindLeafWithKeyFromNode(KeyType key, Node *node) {
  if (root_ == nullptr) {
    return nullptr;
  }
  assert(node != nullptr);
  if (IsLeaf(node)) {
    statuses_.clear();
    statuses_[Iterator(node)] = Status::Found;
    port_.notify();
    return node;
  }

  statuses_.clear();
  statuses_[Iterator(node)] = Status::Found;

  auto iter = std::ranges::upper_bound(node->keys, key);
  size_t num_of_child_with_key = std::distance(node->keys.begin(), iter);
  Node *child_with_key = node->children[num_of_child_with_key].get();

  statuses_[Iterator(child_with_key)] = Status::Search;
  port_.notify();

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
    statuses_[Iterator(node)] = Status::Unity;

    if (LeftSibling(node) &&
        LeftSibling(node)->keys.size() > (max_degree_ / 2)) {
      statuses_[Iterator(LeftSibling(node))] = Status::Unity;
      port_.notify();

      BorrowFromLeft(node, key);
    } else if (RightSibling(node) &&
               RightSibling(node)->keys.size() > (max_degree_ / 2)) {
      statuses_[Iterator(RightSibling(node))] = Status::Unity;
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
    statuses_[Iterator(left_sibling)] = Status::Unity;
    port_.notify();

    if (IsLeaf(node)) {
      Link(left_sibling, node);
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
    statuses_[Iterator(right_sibling)] = Status::Unity;
    port_.notify();

    if (IsLeaf(node)) {
      Link(node, right_sibling);
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

Detail::DataFromBPTree BPlusTree::GetData() {
  return {.iter = Detail::Iterator(root_.get()), .statuses = statuses_};
}

}  // namespace BPT::BPTree
