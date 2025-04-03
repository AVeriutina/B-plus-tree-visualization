#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "BPTreeData.h"
#include "BPTreeNode.h"
#include "Observer.h"

namespace BPT {
class GeomModel;
}

namespace BPT::BPTree {

namespace Detail {
class Iterator;
}  // namespace Detail

class BPlusTree {
  using Node = Detail::Node;
  using KeyType = Detail::KeyType;
  using Data = Detail::DataFromBPTree;
  using Observable = NSLibrary::CObservable<Data, NSLibrary::CByValue>;
  using Iterator = Detail::Iterator;
  using Status = Detail::Status;
  static constexpr auto IsLeaf = Detail::IsLeaf;
  static constexpr auto LeftSibling = Detail::LeftSibling;
  static constexpr auto RightSibling = Detail::RightSibling;
  static constexpr auto IsKeyInNode = Detail::IsKeyInNode;
  static constexpr auto LinkSiblings = Detail::LinkSiblings;
  static constexpr auto UpdateParent = Detail::UpdateParent;
  static constexpr auto IsNodeStateCorrect = Detail::IsNodeStateCorrect;
  static constexpr auto IsParentForNode = Detail::IsParentForNode;

 public:
  BPlusTree();

  void SetDegree(int64_t max_degree);
  bool FindKey(KeyType key);
  bool Insert(KeyType key);
  bool Delete(KeyType key);
  void Reset();
  void SubscribeGeomModel(GeomModel *geom_model_);

 private:
  void InsertKeyInNode(Node *node, KeyType key);
  void Split(Node *old_node);
  Node *FindLeafWithKey(KeyType key);
  Node *FindLeafWithKeyFromNode(KeyType key, Node *start_node);
  void DeleteInNode(Node *node, KeyType key);
  void BorrowFromLeft(Node *node, KeyType prev_key);
  void BorrowFromRight(Node *node);
  void Merge(Node *node, KeyType key_of_node_in_parent);
  void UpdateKeys(Node *node, KeyType prev_key, KeyType new_key);
  Data GetData() const;

  bool IsStateCorrect(Node *node) const;

  size_t max_degree_ = 3;
  std::unique_ptr<Node> root_ = nullptr;
  std::unordered_map<Iterator, Detail::Status> statuses_;
  Observable port_;
};

}  // namespace BPT::BPTree
