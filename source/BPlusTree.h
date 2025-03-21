#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <vector>

namespace BPTree {

namespace Detail {

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
}  // namespace Detail

class BPlusTree {
  using Node = Detail::Node;
  using KeyType = Detail::KeyType;
  static constexpr auto IsLeaf = Detail::IsLeaf;
  static constexpr auto LeftSibling = Detail::LeftSibling;
  static constexpr auto RightSibling = Detail::RightSibling;
  static constexpr auto IsKeyInNode = Detail::IsKeyInNode;
  static constexpr auto Link = Detail::Link;
  static constexpr auto UpdateParent = Detail::UpdateParent;
  static constexpr auto IsNodeStateCorrect = Detail::IsNodeStateCorrect;
  static constexpr auto IsLinkWithChildCorrect = Detail::IsLinkWithChildCorrect;

 public:
  explicit BPlusTree(int32_t max_degree);

  bool FindKey(KeyType key);
  bool Insert(KeyType key);
  bool Delete(KeyType key);

 private:
  static bool IsStateCorrect(Node *ptr);
  void InsertKeyInNode(Node *node, KeyType key);
  void Split(Node *old_node);
  Node *FindLeafWithKey(KeyType key);
  Node *FindLeafWithKeyFromNode(KeyType key, Node *start_node);
  void DeleteInNode(Node *node, KeyType key);
  void BorrowFromLeft(Node *node, KeyType prev_key);
  void BorrowFromRight(Node *node);
  void Merge(Node *node, KeyType key_of_node_in_parent);
  void UpdateKeys(Node *node, KeyType prev_key, KeyType new_key);

  size_t max_degree_;

 public:
  std::unique_ptr<Node> root_ = nullptr;
};

}  // namespace BPTree
