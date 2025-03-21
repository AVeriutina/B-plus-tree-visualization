#pragma once

#include <memory>
#include <vector>

#include "BPlusTree.h"
#include "Iterator.h"

namespace BPTree::GeomModel {

namespace Detail {
using KeyType = BPTree::Detail::KeyType;

struct Point {
  double x = 0;
  double y = 0;
};

struct Rectangle {
  Point upper_left_dot;
  Point lower_right_dot;
};

struct GeomNode {
  Rectangle node_size;
  std::vector<KeyType> keys;
  std::vector<Rectangle> keys_size;
  std::vector<std::unique_ptr<GeomNode>> children;
  GeomNode* parent;
};

double GetLowerY(GeomNode* node);
double CalcWidthNode(GeomNode* node);
std::pair<double, double> GetLeftAndRightChildrenBoard(GeomNode* node);

void CalcChildrenSize(GeomNode* node);
void SetXWithLeftBoard(GeomNode* node, double left_board);
void SetXWithCenter(GeomNode* node, double center);
void SetY(GeomNode* node, double upper_board);
void SetKeys(BPTree::Detail::Iterator iter, GeomNode* node);

}  // namespace Detail

class GeomBPlusTree {
  using Point = Detail::Point;
  using Rectangle = Detail::Rectangle;
  using GeomNode = Detail::GeomNode;
  using Iterator = BPTree::Detail::Iterator;

  static constexpr auto GetLowerY = Detail::GetLowerY;
  static constexpr auto CalcWidthNode = Detail::CalcWidthNode;
  static constexpr auto GetLeftAndRightChildrenBoard =
      Detail::GetLeftAndRightChildrenBoard;
  static constexpr auto CalcChildrenSize = Detail::CalcChildrenSize;
  static constexpr auto SetXWithLeftBoard = Detail::SetXWithLeftBoard;
  static constexpr auto SetXWithCenter = Detail::SetXWithCenter;
  static constexpr auto SetY = Detail::SetY;
  static constexpr auto SetKeys = Detail::SetKeys;

 public:
  void BuildTree(Iterator iter_node);
  std::unique_ptr<GeomNode> BuildSubtree(Iterator iter_node,
                                         double start_height_for_node,
                                         double& left_board);

 private:
  std::unique_ptr<GeomNode> root_;
};

}  // namespace BPTree::GeomModel
