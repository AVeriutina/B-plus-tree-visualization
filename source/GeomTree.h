#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "BPTreeData.h"

namespace BPT::GeomModel {

namespace Detail {

namespace Settings {
constexpr double WidthOfKey = 8.;
constexpr double DistBetweenKeys = 1.;
constexpr double DistKeyBorder = 1.;
constexpr double HeightOfNode = 2 * DistKeyBorder + WidthOfKey;
constexpr double HeightBetweenNodes = 3.;
constexpr double WidthBetweenNodes = 3.;

}  // namespace Settings

enum class Color {
  Blue,
  Green,
  Red,
  Orange,
};

Color ConvertStatusToColor(BPTree::Detail::Status status);

using KeyType = BPTree::Detail::KeyType;

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
  std::vector<std::unique_ptr<GeomNode>> children;
  GeomNode* parent;
  Color clr = Color::Green;
};

double GetLowerY(GeomNode* node);
double CalcWidthOfNode(GeomNode* node);
std::pair<double, double> GetLeftAndRightChildrenBorder(GeomNode* node);

void CalcChildrenSize(GeomNode* node);
void SetXWithLeftBorder(GeomNode* node, double left_border);
void SetXWithCenter(GeomNode* node, double center);
void SetY(GeomNode* node, double upper_border);
void SetKeys(BPTree::Detail::Iterator iter, GeomNode* node);

}  // namespace Detail

class GeomBPlusTree {
  using Point = Detail::Point;
  using Rectangle = Detail::Rectangle;
  using GeomNode = Detail::GeomNode;
  using Iterator = BPTree::Detail::Iterator;
  using DataFromBPTreeToGeomModel = BPTree::Detail::DataFromBPTree;
  using Statuses = std::unordered_map<Iterator, BPTree::Detail::Status>;

  static constexpr auto ConvertStatusToColor = Detail::ConvertStatusToColor;
  static constexpr auto GetLowerY = Detail::GetLowerY;
  static constexpr auto CalcWidthOfNode = Detail::CalcWidthOfNode;
  static constexpr auto GetLeftAndRightChildrenBorder =
      Detail::GetLeftAndRightChildrenBorder;
  static constexpr auto CalcChildrenSize = Detail::CalcChildrenSize;
  static constexpr auto SetXWithLeftBorder = Detail::SetXWithLeftBorder;
  static constexpr auto SetXWithCenter = Detail::SetXWithCenter;
  static constexpr auto SetY = Detail::SetY;
  static constexpr auto SetKeys = Detail::SetKeys;

 public:
  void BuildTree(const DataFromBPTreeToGeomModel& data);

 private:
  std::unique_ptr<Detail::GeomNode> BuildSubtree(Iterator iter_node,
                                                 Statuses statuses,
                                                 double start_height_for_node,
                                                 double& left_border);
  std::unique_ptr<GeomNode> root_;
};

}  // namespace BPT::GeomModel
