#include "BPlusTree.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <optional>

#include "GeomModel.h"
#include "IteratorBPTree.h"

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

    assert(IsStateCorrect(root_.get()));
    return true;
  }

  Node *leaf = FindLeafWithKey(key);
  assert(leaf != nullptr);
  bool cant_insert = IsKeyInNode(leaf, key);

  if (cant_insert) {
    statuses_[Iterator(leaf)] = Status::NotFound;
    port_.notify();

    statuses_.clear();
    port_.notify();

    return false;
  }

  statuses_[Iterator(leaf)] = Status::Found;
  port_.notify();

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
    statuses_[Iterator(leaf)] = Status::NotFound;
    port_.notify();

    statuses_.clear();
    port_.notify();

    return false;
  }

  statuses_.clear();
  DeleteInNode(leaf, key);

  statuses_.clear();
  port_.notify();

  assert(IsStateCorrect(root_.get()));
  return true;
}

void BPlusTree::Reset() {
  if (root_) {
    root_.reset();
  }
  statuses_.clear();
  port_.notify();
}

void BPlusTree::SubscribeGeomModel(GeomModel *geom_model_) {
  assert(geom_model_);
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

  if (old_node == root_.get()) {
    auto prev_root = std::move(root_);
    root_ = std::make_unique<Node>();
    prev_root->parent = root_.get();
    root_->children.push_back(std::move(prev_root));
    parent = root_.get();
  }

  auto new_left_node = std::make_unique<Node>();
  auto new_right_node = std::make_unique<Node>();

  size_t pos_of_central_key = old_node->keys.size() / 2;
  KeyType central_key = old_node->keys[pos_of_central_key];

  if (IsLeaf(old_node)) {
    LinkSiblings(old_node->left, new_left_node.get());
    LinkSiblings(new_left_node.get(), new_right_node.get());
    LinkSiblings(new_right_node.get(), old_node->right);

  } else {
    Detail::AssignLeftHalf(&old_node->children, &new_left_node->children,
                           !IsLeaf(old_node));
    Detail::AssignRightHalf(&old_node->children, &new_right_node->children,
                            !IsLeaf(old_node));

    UpdateParent(new_left_node->children, new_left_node.get());
    UpdateParent(new_right_node->children, new_right_node.get());

    old_node->keys.erase(std::next(old_node->keys.begin(), pos_of_central_key));
  }

  InsertKeyInNode(parent, central_key);
  Detail::AssignLeftHalf(&old_node->keys, &new_left_node->keys,
                         !IsLeaf(old_node));
  Detail::AssignRightHalf(&old_node->keys, &new_right_node->keys,
                          !IsLeaf(old_node));

  auto iter_pos_in_children = std::ranges::find_if(
      parent->children, [old_node](const std::unique_ptr<Node> &lhs) {
        return lhs.get() == old_node;
      });

  new_left_node->parent = parent;
  new_right_node->parent = parent;

  *iter_pos_in_children = std::move(new_left_node);
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

  KeyType new_key_instead_of_prev = 0;
  if (iter == node->keys.begin() && node->keys.size() > 1) {
    new_key_instead_of_prev = *std::next(iter);
  }
  bool is_begin_of_keys = iter == node->keys.begin();

  statuses_.clear();
  statuses_[Iterator(node)] = Status::IntermediateState;
  port_.notify();

  node->keys.erase(iter);

  port_.notify();

  if (node == root_.get() && root_->keys.size() == 0 &&
      root_->children.size() == 0) {
    root_.reset();
    statuses_.clear();
    port_.notify();
    return;
  }

  if (node == root_.get()) {
    return;
  }

  if (node->keys.size() < ((max_degree_ + 1) / 2 - 1)) {
    if (LeftSibling(node) &&
        LeftSibling(node)->keys.size() >= ((max_degree_ + 1) / 2)) {
      BorrowFromSibling(node, true, key, new_key_instead_of_prev,
                        is_begin_of_keys);

    } else if (RightSibling(node) &&
               RightSibling(node)->keys.size() >= ((max_degree_ + 1) / 2)) {
      BorrowFromSibling(node, false, key, new_key_instead_of_prev,
                        is_begin_of_keys);
    } else {
      std::optional<KeyType> prev_key;
      if (IsLeaf(node) && is_begin_of_keys) {
        prev_key = key;
      }
      Merge(node, GetKeyInParent(node), prev_key);
    }
  } else {
    if (IsLeaf(node) && is_begin_of_keys) {
      UpdateKeys(node, key, new_key_instead_of_prev);
    }
  }

  statuses_.clear();
  port_.notify();
}

void BPlusTree::BorrowFromSibling(Node *node, bool for_left_sibling,
                                  KeyType prev_key,
                                  KeyType new_key_instead_of_prev,
                                  bool is_begin_of_keys) {
  if (for_left_sibling) {
    statuses_[Iterator(LeftSibling(node))] = Status::IntermediateState;
    port_.notify();
    BorrowFromLeft(node);

  } else {
    statuses_[Iterator(RightSibling(node))] = Status::IntermediateState;
    port_.notify();
    BorrowFromRight(node);
  }
  port_.notify();
  assert(!node->keys.empty());
  new_key_instead_of_prev = node->keys.front();
  if (IsLeaf(node) && is_begin_of_keys) {
    UpdateKeys(node, prev_key, new_key_instead_of_prev);
  }
}

void BPlusTree::BorrowFromLeft(Node *node) {
  assert(node != nullptr);
  Node *left_sibling = LeftSibling(node);

  assert(node != root_.get());

  auto iter_of_node = GetIterOnKeyOfNodeInParent(node);
  KeyType key_of_node_in_parent = *iter_of_node;

  assert(!left_sibling->keys.empty());
  KeyType key_for_parent = left_sibling->keys.back();
  *iter_of_node = key_for_parent;
  left_sibling->keys.pop_back();

  if (!IsLeaf(node)) {
    node->keys.insert(node->keys.begin(), key_of_node_in_parent);
  } else {
    node->keys.insert(node->keys.begin(), key_for_parent);
  }

  if (!IsLeaf(node)) {
    assert(!left_sibling->children.empty());
    node->children.insert(node->children.begin(),
                          std::move(left_sibling->children.back()));
    left_sibling->children.pop_back();
    node->children.front()->parent = node;
  }
}

void BPlusTree::BorrowFromRight(Node *node) {
  assert(node != nullptr);
  Node *right_sibling = RightSibling(node);

  assert(node != root_.get());

  auto iter_on_key_of_right_in_parent =
      GetIterOnKeyOfNodeInParent(right_sibling);
  KeyType key_of_right_in_parent = *iter_on_key_of_right_in_parent;

  node->keys.push_back(key_of_right_in_parent);

  if (IsLeaf(node)) {
    assert(!right_sibling->keys.empty());
    right_sibling->keys.erase(right_sibling->keys.begin());
  }

  assert(!right_sibling->keys.empty());
  KeyType key_for_parent = right_sibling->keys.front();
  *iter_on_key_of_right_in_parent = key_for_parent;

  if (!IsLeaf(node)) {
    right_sibling->keys.erase(right_sibling->keys.begin());
  }

  if (!IsLeaf(node)) {
    assert(!right_sibling->children.empty());
    node->children.push_back(std::move(right_sibling->children.front()));
    right_sibling->children.erase(right_sibling->children.begin());
    node->children.back()->parent = node;
  }
}

void BPlusTree::Merge(Node *node, KeyType key_of_node_in_parent,
                      std::optional<KeyType> need_update_of_keys) {
  assert(node != nullptr);
  Node *left_sibling = LeftSibling(node);
  Node *right_sibling = RightSibling(node);
  Node *parent = node->parent;

  statuses_.clear();
  statuses_[Iterator(node)] = Detail::Status::IntermediateState;
  port_.notify();

  if (left_sibling) {
    statuses_[Iterator(left_sibling)] = Status::IntermediateState;
  } else if (right_sibling) {
    statuses_[Iterator(right_sibling)] = Status::IntermediateState;
  }
  port_.notify();

  auto iter = std::ranges::find_if(
      parent->children,
      [node](const std::unique_ptr<Node> &lhs) { return lhs.get() == node; });
  assert(iter != parent->children.end());
  std::unique_ptr<Node> deleted_node = std::move(*iter);
  parent->children.erase(iter);

  KeyType new_key_instead_of_prev = 0;

  if (left_sibling) {
    if (IsLeaf(deleted_node.get())) {
      LinkSiblings(left_sibling, deleted_node.get());
    } else {
      left_sibling->keys.insert(left_sibling->keys.end(),
                                key_of_node_in_parent);

      UpdateParent(deleted_node->children, left_sibling);

      left_sibling->children.insert(
          left_sibling->children.end(),
          std::make_move_iterator(deleted_node->children.begin()),
          std::make_move_iterator(deleted_node->children.end()));
    }

    left_sibling->keys.insert(left_sibling->keys.end(),
                              deleted_node->keys.begin(),
                              deleted_node->keys.end());

    need_update_of_keys = false;

  } else if (right_sibling) {
    if (IsLeaf(deleted_node.get())) {
      LinkSiblings(deleted_node.get(), right_sibling);
    } else {
      assert(!right_sibling->children.empty());
      assert(!right_sibling->children.front()->keys.empty());
      right_sibling->keys.insert(right_sibling->keys.begin(),
                                 right_sibling->children.front()->keys.front());

      UpdateParent(deleted_node->children, right_sibling);

      right_sibling->children.insert(
          right_sibling->children.begin(),
          std::make_move_iterator(deleted_node->children.begin()),
          std::make_move_iterator(deleted_node->children.end()));
    }
    right_sibling->keys.insert(right_sibling->keys.begin(),
                               deleted_node->keys.begin(),
                               deleted_node->keys.end());

    new_key_instead_of_prev = right_sibling->keys.front();
  }

  port_.notify();

  if (need_update_of_keys) {
    UpdateKeys(parent, need_update_of_keys.value(), new_key_instead_of_prev);

    port_.notify();
  }
  DeleteInNode(parent, key_of_node_in_parent);
  if (parent == root_.get() && root_->keys.empty()) {
    assert(!parent->children.empty());
    root_ = std::move(parent->children.front());
    root_->parent = nullptr;
  }
}

void BPlusTree::UpdateKeys(Node *node, KeyType prev_key, KeyType new_key) {
  while (node != nullptr) {
    auto iter = std::ranges::lower_bound(node->keys, prev_key);
    if (iter != node->keys.end() && *iter == prev_key) {
      statuses_[Iterator(node)] = Status::IntermediateState;
      port_.notify();

      *iter = new_key;

      port_.notify();

      statuses_.erase(Iterator(node));
      port_.notify();
    }
    node = node->parent;
  }
}

Detail::DataFromBPTree BPlusTree::GetData() const {
  return {.iter = Detail::Iterator(root_.get()), .statuses = statuses_};
}

bool BPlusTree::IsStateCorrect(Node *node) const {
  if (node == nullptr) {
    return true;
  }
  if (max_degree_ <= 2) {
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
