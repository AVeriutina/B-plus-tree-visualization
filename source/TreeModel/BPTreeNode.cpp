#include "BPTreeNode.h"

#include <algorithm>
#include <vector>

namespace BPT::BPTree::Detail {

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
  return (*std::prev(iter_of_right)).get();
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

std::vector<KeyType>::const_iterator FindIterOfKey(const Node &node,
                                                   KeyType key) {
  auto iter = std::ranges::lower_bound(node.keys, key);
  assert(iter != node.keys.end() && *iter == key);
  return iter;
}

void LinkSiblings(Node *left_node, Node *right_node) {
  if (left_node) {
    left_node->right = right_node;
  }
  if (right_node) {
    right_node->left = left_node;
  }
}

void UpdateParent(const std::vector<std::unique_ptr<Node>> &children,
                  Node *new_parent) {
  for (const auto &child : children) {
    child->parent = new_parent;
  }
}

bool IsNodeStateCorrect(Node *node) {
  if (!IsLeaf(node)) {
    return node->keys.size() + 1 == node->children.size();
  }
  return true;
}

bool IsParentForNode(Node *parent, Node *child) {
  return child->parent == parent;
}

KeyType GetKeyInParent(Node *node) {
  assert(node->parent != nullptr);
  Node *parent = node->parent;
  auto iter_of_node = std::ranges::find_if(
      parent->children,
      [node](const std::unique_ptr<Node> &lhs) { return lhs.get() == node; });
  auto pos_of_node_in_parent =
      std::distance(parent->children.begin(), iter_of_node);
  if (pos_of_node_in_parent == 0) {
    return parent->keys.front();
  }
  return parent->keys[pos_of_node_in_parent - 1];
}

std::vector<KeyType>::iterator GetIterOnKeyOfNodeInParent(Node *node) {
  assert(node);
  Node *parent = node->parent;
  assert(parent);
  auto iter_of_node = std::ranges::find_if(
      parent->children,
      [node](const std::unique_ptr<Node> &lhs) { return lhs.get() == node; });
  assert(iter_of_node != parent->children.begin());
  long pos_of_node_in_parent =
      std::distance(parent->children.begin(), iter_of_node);
  return parent->keys.begin() + pos_of_node_in_parent - 1;
}

Node *GetFirstChild(Node *node) {
  assert(node);
  assert(!IsLeaf(node));
  return node->children.front().get();
}

Node *GetLastChild(Node *node) {
  assert(node);
  assert(!IsLeaf(node));
  return node->children.back().get();
}

}  // namespace BPT::BPTree::Detail
