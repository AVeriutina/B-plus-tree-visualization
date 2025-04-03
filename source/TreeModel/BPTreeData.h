#pragma once

#include "IteratorBPTree.h"

namespace BPT::BPTree::Detail {

struct Node;
class Iterator;

enum class Status {
  Search,
  Found,
  NotFound,
  IntermediateState,
};

struct DataFromBPTree {
  Iterator iter;
  std::unordered_map<Iterator, Status> statuses;
};
}  // namespace BPT::BPTree::Detail
