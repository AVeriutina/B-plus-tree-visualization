#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "TreeModel/BPTreeData.h"
#include "TreeModel/BPlusTree.h"

namespace BPT {

namespace GeomTreeDetail {
using KeyType = BPTree::Detail::KeyType;
using Status = BPTree::Detail::Status;
using BPTreeIterator = BPTree::Detail::Iterator;

namespace Settings {
inline constexpr double HeightOfKey = 40.;
inline constexpr double WidthOfKey = 65.;
inline constexpr double DistBetweenKeys = 8.;
inline constexpr double DistKeyBorder = 7.5;
inline constexpr double HeightOfNode = 2 * DistKeyBorder + HeightOfKey;
inline constexpr double HeightBetweenNodes = 15.;
inline constexpr double WidthBetweenNodes = 15.;
}  // namespace Settings

enum class Color {
  Blue,
  Green,
  Red,
  Orange,
  White,
};

Color ConvertStatusToColor(Status status);

struct Point {
  double x = 0;
  double y = 0;
};

struct Rectangle {
  Point upper_left_point;
  Point lower_right_point;
};

struct GeomNode {
  Rectangle node_size;
  std::vector<KeyType> keys;
  std::vector<Rectangle> keys_size;
  Point upper_mid_point;
  Point lower_mid_point;
  std::vector<std::unique_ptr<GeomNode>> children;
  GeomNode* parent;
  Color clr = Color::White;
};

double GetLowerY(GeomNode* node);
double CalcWidthOfNode(GeomNode* node);
std::pair<double, double> GetLeftAndRightChildrenBorder(GeomNode* node);

void CalcChildrenSize(GeomNode* node);
void SetXWithLeftBorder(GeomNode* node, double left_border);
void SetXWithCenter(GeomNode* node, double center);
void SetY(GeomNode* node, double upper_border);
void SetKeys(BPTreeIterator iter, GeomNode* node);

struct DataFromGeomTree {
  const GeomNode* root = nullptr;
  Point lower_right_point_of_tree_;
};

}  // namespace GeomTreeDetail

class GeomBPlusTree {
  using Point = GeomTreeDetail::Point;
  using Rectangle = GeomTreeDetail::Rectangle;
  using GeomNode = GeomTreeDetail::GeomNode;
  using Iterator = BPTree::Detail::Iterator;
  using DataFromBPTree = BPTree::Detail::DataFromBPTree;
  using DataFromGeomTree = GeomTreeDetail::DataFromGeomTree;
  using Statuses = std::unordered_map<Iterator, BPTree::Detail::Status>;

  static constexpr auto ConvertStatusToColor =
      GeomTreeDetail::ConvertStatusToColor;
  static constexpr auto GetLowerY = GeomTreeDetail::GetLowerY;
  static constexpr auto CalcWidthOfNode = GeomTreeDetail::CalcWidthOfNode;
  static constexpr auto GetLeftAndRightChildrenBorder =
      GeomTreeDetail::GetLeftAndRightChildrenBorder;
  static constexpr auto CalcChildrenSize = GeomTreeDetail::CalcChildrenSize;
  static constexpr auto SetXWithLeftBorder = GeomTreeDetail::SetXWithLeftBorder;
  static constexpr auto SetXWithCenter = GeomTreeDetail::SetXWithCenter;
  static constexpr auto SetY = GeomTreeDetail::SetY;
  static constexpr auto SetKeys = GeomTreeDetail::SetKeys;

 public:
  GeomBPlusTree(const DataFromBPTree& data);
  DataFromGeomTree GetDataOfTree() const;

 private:
  std::unique_ptr<GeomNode> BuildSubtree(Iterator iter_node,
                                         const Statuses& statuses,
                                         const double start_height_for_node,
                                         double* left_border);
  std::unique_ptr<GeomNode> root_;
  Point lower_right_point_of_tree_;
};

}  // namespace BPT
