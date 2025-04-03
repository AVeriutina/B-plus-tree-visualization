#include "GeomTree.h"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <memory>

namespace BPT {

namespace GeomTreeDetail {

Color ConvertStatusToColor(Status status) {
  switch (status) {
    case Status::Found:
      return Color::Green;
    case Status::NotFound:
      return Color::Red;
    case Status::Search:
      return Color::Blue;
    case Status::IntermediateState:
      return Color::Orange;
    default:
      assert(false);
  }
}

double GetLowerY(GeomNode *node) {
  assert(node);
  return node->node_size.lower_right_point.y + Settings::HeightBetweenNodes;
}

double CalcWidthOfNode(GeomNode *node) {
  assert(node);
  double num_keys = node->keys.size();
  return num_keys * Settings::WidthOfKey +
         (num_keys - 1) * Settings::DistBetweenKeys +
         2 * Settings::DistKeyBorder;
}

std::pair<double, double> GetLeftAndRightChildrenBorder(GeomNode *node) {
  assert(node);
  assert(!node->children.empty());
  return std::make_pair(node->children.front()->node_size.upper_left_point.x,
                        node->children.back()->node_size.lower_right_point.x);
}

void CalcChildrenSize(GeomNode *node) {
  assert(node);
  double left_border = node->node_size.upper_left_point.x;
  left_border += Settings::DistKeyBorder;

  double upper_y = node->node_size.upper_left_point.y + Settings::DistKeyBorder;
  double lower_y =
      node->node_size.lower_right_point.y - Settings::DistKeyBorder;
  for (size_t i = 0; i < node->keys.size(); ++i) {
    node->keys_size.emplace_back(
        Point(left_border, upper_y),
        Point(left_border + Settings::WidthOfKey, lower_y));
    left_border += Settings::WidthOfKey;
    left_border += Settings::DistBetweenKeys;
  }
}

void SetXWithLeftBorder(GeomNode *node, double left_border) {
  assert(node);
  node->node_size.upper_left_point.x = left_border;
  node->node_size.lower_right_point.x = left_border + CalcWidthOfNode(node);
}

void SetXWithCenter(GeomNode *node, double center) {
  assert(node);
  double width_node = CalcWidthOfNode(node);
  node->node_size.upper_left_point.x = center - width_node / 2;
  node->node_size.lower_right_point.x = center + width_node / 2;
}

void SetY(GeomNode *node, double upper_border) {
  node->node_size.upper_left_point.y = upper_border;
  node->node_size.lower_right_point.y = upper_border + Settings::HeightOfNode;
}

void SetKeys(BPTreeIterator iter, GeomNode *node) {
  node->keys = iter.GetKeys();
}

}  // namespace GeomTreeDetail

GeomBPlusTree::GeomBPlusTree(const DataFromBPTree &data) {
  Iterator iter_node = data.iter;
  if (!iter_node.IsValid()) {
    return;
  }
  double left_border = 0;
  root_ = BuildSubtree(iter_node, data.statuses, 0, &left_border);
}

std::unique_ptr<GeomTreeDetail::GeomNode> GeomBPlusTree::BuildSubtree(
    Iterator iter_node, const Statuses &statuses,
    const double start_height_for_node, double *left_border) {
  std::unique_ptr<GeomNode> new_node = std::make_unique<GeomNode>();
  SetY(new_node.get(), start_height_for_node);
  SetKeys(iter_node, new_node.get());
  if (statuses.contains(iter_node)) {
    new_node->clr = ConvertStatusToColor(statuses.at(iter_node));
  }

  if (iter_node.IsLeaf()) {
    SetXWithLeftBorder(new_node.get(), *left_border);
    *left_border += CalcWidthOfNode(new_node.get()) +
                    GeomTreeDetail::Settings::WidthBetweenNodes;
    lower_right_point_of_tree_ = new_node->node_size.lower_right_point;
  } else {
    for (Iterator child : iter_node.Children()) {
      new_node->children.push_back(
          BuildSubtree(child, statuses,
                       GetLowerY(new_node.get()) +
                           GeomTreeDetail::Settings::HeightBetweenNodes,
                       left_border));
    }
    auto [left, right] = GetLeftAndRightChildrenBorder(new_node.get());
    SetXWithCenter(new_node.get(), (left + right) / 2);
  }

  CalcChildrenSize(new_node.get());

  double mid_x_of_node = (new_node->node_size.lower_right_point.x +
                          new_node->node_size.upper_left_point.x) /
                         2;
  new_node->upper_mid_point = Point(mid_x_of_node, start_height_for_node);
  new_node->lower_mid_point =
      Point(mid_x_of_node,
            start_height_for_node + GeomTreeDetail::Settings::HeightOfNode);
  return new_node;
}

GeomBPlusTree::DataFromGeomTree GeomBPlusTree::GetDataOfTree() const {
  return DataFromGeomTree(root_.get(), lower_right_point_of_tree_);
}

}  // namespace BPT
