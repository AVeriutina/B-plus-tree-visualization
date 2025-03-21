#include "GeomTree.h"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <memory>

#include "GeomModel.h"

namespace BPTree::GeomModel {

namespace Detail {

double GetLowerY(GeomNode *node) {
  assert(node);
  return node->node_size.lower_right_dot.y + Settings::HeightBetweenNodes;
}

double CalcWidthNode(GeomNode *node) {
  assert(node);
  auto num_keys = static_cast<double>(node->keys.size());
  return num_keys * Settings::WidthKey +
         (num_keys - 1) * Settings::DistBetweenKeys +
         2 * Settings::DistKeyBoard;
}

std::pair<double, double> GetLeftAndRightChildrenBoard(GeomNode *node) {
  assert(node);
  assert(!node->children.empty());
  return std::make_pair(node->children.front()->node_size.upper_left_dot.x,
                        node->children.back()->node_size.lower_right_dot.x);
}

void CalcChildrenSize(GeomNode *node) {
  assert(node);
  double left_board = node->node_size.upper_left_dot.x;
  left_board += Settings::DistKeyBoard;

  double upper_y = node->node_size.upper_left_dot.y + Settings::DistKeyBoard;
  double lower_y = node->node_size.lower_right_dot.y - Settings::DistKeyBoard;
  for (size_t i = 0; i < node->keys.size(); ++i) {
    node->keys_size[i] =
        Rectangle(Point(left_board, upper_y),
                  Point(left_board + Settings::WidthKey, lower_y));
    left_board += Settings::WidthKey;
    left_board += Settings::DistBetweenKeys;
  }
}

void SetXWithLeftBoard(GeomNode *node, double left_board) {
  assert(node);
  node->node_size.upper_left_dot.x = left_board;
  node->node_size.lower_right_dot.x = left_board + CalcWidthNode(node);
}

void SetXWithCenter(GeomNode *node, double center) {
  assert(node);
  double width_node = CalcWidthNode(node);
  node->node_size.upper_left_dot.x = center - width_node / 2;
  node->node_size.lower_right_dot.x = center + width_node / 2;
}

void SetY(GeomNode *node, double upper_board) {
  node->node_size.upper_left_dot.y = upper_board;
  node->node_size.lower_right_dot.y = upper_board + Settings::HeightNode;
}

void SetKeys(BPTree::Detail::Iterator iter, GeomNode *node) {
  node->keys = iter.GetKeys();
}

} // namespace Detail

void GeomBPlusTree::BuildTree(Iterator iter_node) {
  double left_board = 0;
  root_ = BuildSubtree(iter_node, 0, left_board);
}

std::unique_ptr<Detail::GeomNode>
GeomBPlusTree::BuildSubtree(Iterator iter_node, double start_height_for_node,
                            double &left_board) {
  assert(iter_node.IsValid());

  std::unique_ptr<GeomNode> new_node = std::make_unique<GeomNode>();
  SetY(new_node.get(), start_height_for_node);
  SetKeys(iter_node, new_node.get());

  if (iter_node.IsLeaf()) {
    SetXWithLeftBoard(new_node.get(), left_board);
    left_board += CalcWidthNode(new_node.get()) + Settings::WidthBetweenNodes;
  } else {
    for (auto child : iter_node) {
      new_node->children.push_back(BuildSubtree(
          child, GetLowerY(new_node.get()) + Settings::HeightBetweenNodes,
          left_board));
    }
    auto [left, right] = GetLeftAndRightChildrenBoard(new_node.get());
    SetXWithCenter(new_node.get(), (left + right) / 2);
  }

  CalcChildrenSize(new_node.get());
  return new_node;
}

} // namespace BPTree::GeomModel
