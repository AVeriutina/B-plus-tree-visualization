#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPoint>

#include "ConstValue.h"
#include "GeomTree.h"
#include "TreeModel/BPTreeNode.h"

namespace BPT {

namespace ViewDetail {

using KeyType = BPTree::Detail::KeyType;

struct DrawNode {
  QGraphicsRectItem background_node_;
  std::vector<KeyType> keys;
  std::vector<QGraphicsRectItem> keys_size;
};

}  // namespace ViewDetail

class View {
  using ConstGeomBPTree = Detail::ConstValue<GeomBPlusTree>;
  using ViewObserver =
      NSLibrary::CColdInput<ConstGeomBPTree, NSLibrary::CByValue>;
  using GeomNode = GeomTreeDetail::GeomNode;
  using DrawNode = ViewDetail::DrawNode;
  using GeomTreePoint = GeomTreeDetail::Point;
  using GeomTreeRectangle = GeomTreeDetail::Rectangle;

 public:
  View();
  ViewObserver* GetObserverPort();
  QGraphicsScene* GetScene();

 private:
  QPointF ConvertMyPointToQPoint(const GeomTreePoint& point);
  QRectF ConvertMyRectangleToQRect(const GeomTreeRectangle& rect);
  void CalcOffset(const GeomTreePoint& point);
  void DrawOneNode(const GeomNode& node);
  void DrawTree(ConstGeomBPTree data);
  void DrawSubTree(const GeomNode& node);

  static constexpr int width_of_scene_ = 1000;
  static constexpr int height_of_scene_ = 800;
  static constexpr QPointF center_of_scene_ = {width_of_scene_ / 2,
                                               height_of_scene_ / 2};

  ViewObserver input_port_;
  QGraphicsScene scene_;
  QPointF offset_for_centering_;
};
}  // namespace BPT
