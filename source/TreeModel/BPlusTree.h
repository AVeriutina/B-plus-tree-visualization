#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "BPTreeData.h"
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
  static constexpr auto GetKeyInParent = Detail::GetKeyInParent;
  static constexpr auto GetIterOnKeyOfNodeInParent =
      Detail::GetIterOnKeyOfNodeInParent;

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
  void BorrowFromSibling(Node *node, bool for_left_sibling, KeyType prev_key,
                         KeyType new_key_instead_of_prev,
                         bool is_begin_of_keys);
  void BorrowFromLeft(Node *node);
  void BorrowFromRight(Node *node);
  void Merge(Node *node, KeyType key_of_node_in_parent,
             std::optional<KeyType> need_update_of_keys);
  void UpdateKeys(Node *node, KeyType prev_key, KeyType new_key);
  Data GetData() const;

  bool IsStateCorrect(Node *node) const;

  size_t max_degree_ = 3;
  std::unique_ptr<Node> root_ = nullptr;
  std::unordered_map<Iterator, Detail::Status> statuses_;
  Observable port_;
};

}  // namespace BPT::BPTree
