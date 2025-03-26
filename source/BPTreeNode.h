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
auto FindIterOfKey(const std::vector<KeyType> &kyes, KeyType key);
void Link(Node *left_node, Node *right_node);
void UpdateParent(const std::vector<std::unique_ptr<Node>> &children,
                  Node *new_parent);
bool IsNodeStateCorrect(Node *node);
bool IsLinkWithChildCorrect(Node *parent, Node *child);

Node *GetFistChild(Node *node);
Node *GetLastChild(Node *node);

template <typename T>
void AssignLeftHalf(std::vector<T> *from, std::vector<T> *to) {
  assert(to);
  to->assign(std::make_move_iterator(from->begin()),
             std::make_move_iterator(from->begin() + from->size() / 2));
}

template <typename T>
void AssignRightHalf(std::vector<T> *from, std::vector<T> *to) {
  assert(to);
  to->assign(std::make_move_iterator(from->begin() + from->size() / 2),
             std::make_move_iterator(from->end()));
}

}  // namespace BPT::BPTree::Detail
