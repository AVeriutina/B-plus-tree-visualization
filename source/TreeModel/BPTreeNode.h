#pragma once

#include <cassert>
#include <cstdint>
#include <iterator>
#include <memory>
#include <vector>

namespace BPT::BPTree::Detail {

using KeyType = int64_t;

struct Node {
  std::vector<std::unique_ptr<Node>> children;
  std::vector<KeyType> keys;
  Node *parent = nullptr;
  Node *left = nullptr;
  Node *right = nullptr;
};

bool IsLeaf(const Node *node);
Node *LeftSibling(Node *node);
Node *RightSibling(Node *node);
bool IsKeyInNode(const Node *node, KeyType key);
std::vector<KeyType>::const_iterator FindIterOfKey(const Node &node,
                                                   KeyType key);
void LinkSiblings(Node *left_node, Node *right_node);
void UpdateParent(const std::vector<std::unique_ptr<Node>> &children,
                  Node *new_parent);
bool IsNodeStateCorrect(Node *node);
bool IsParentForNode(Node *parent, Node *child);
KeyType GetKeyInParent(Node *node);
std::vector<KeyType>::iterator GetIterOnKeyOfNodeInParent(Node *node);

Node *GetFirstChild(Node *node);
Node *GetLastChild(Node *node);

template <typename T>
void AssignLeftHalf(std::vector<T> *from, std::vector<T> *to,
                    bool is_not_leaf) {
  assert(to);
  to->assign(std::make_move_iterator(from->begin()),
             std::make_move_iterator(from->begin() +
                                     (from->size() + is_not_leaf) / 2));
}

template <typename T>
void AssignRightHalf(std::vector<T> *from, std::vector<T> *to,
                     bool is_not_leaf) {
  assert(to);
  to->assign(
      std::make_move_iterator(from->begin() + (from->size() + is_not_leaf) / 2),
      std::make_move_iterator(from->end()));
}

}  // namespace BPT::BPTree::Detail
