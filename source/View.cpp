#include "View.h"

#include <QColor>
#include <cassert>

#include "GeomTree.h"

namespace BPT {

namespace {

using MyRectangle = GeomTreeDetail::Rectangle;
using MyColor = GeomTreeDetail::Color;
using MyPoint = GeomTreeDetail::Point;

QColor ConvertMyColorToQColor(MyColor clr) {
  switch (clr) {
    using enum MyColor;
    case Blue:
      return QColor(173, 216, 230);
    case Green:
      return QColor(152, 251, 152);
    case Red:
      return QColor(255, 182, 193);
    case Orange:
      return QColor(255, 204, 153);
    case White:
      return Qt::white;
    default:
      return Qt::black;
  }
}

}  // namespace

View::View()
    : input_port_([this](ConstGeomBPTree data) { DrawTree(data); }), scene_() {
  scene_.setSceneRect(0, 0, width_of_scene_, height_of_scene_);
}

View::ViewObserver* View::GetObserverPort() { return &input_port_; }
QGraphicsScene* View::GetScene() { return &scene_; }

QPointF View::ConvertMyPointToQPoint(const GeomTreePoint& point) {
  return QPointF(point.x, point.y) + offset_for_centering_;
}

QRectF View::ConvertMyRectangleToQRect(const MyRectangle& rect) {
  return {QPointF(rect.upper_left_point.x, rect.upper_left_point.y) +
              offset_for_centering_,
          QPointF(rect.lower_right_point.x, rect.lower_right_point.y) +
              offset_for_centering_};
}

void View::CalcOffset(const GeomTreePoint& point) {
  QPointF center_of_geom_tree = QPointF(point.x, point.y) / 2;
  offset_for_centering_ = center_of_scene_ - center_of_geom_tree;
}

void View::DrawOneNode(const GeomNode& geom_node) {
  auto node_rect =
      scene_.addRect(ConvertMyRectangleToQRect(geom_node.node_size), QPen(),
                     QBrush(ConvertMyColorToQColor(geom_node.clr)));

  for (size_t key_index = 0; key_index < geom_node.keys.size(); ++key_index) {
    auto key_rect = scene_.addRect(
        ConvertMyRectangleToQRect(geom_node.keys_size[key_index]),
        QPen(Qt::black), QBrush(Qt::white));

    auto key_text = scene_.addText(QString::number(geom_node.keys[key_index]));

    key_text->setPos(key_rect->mapToScene(key_rect->boundingRect().center()) +
                     (key_text->boundingRect().topLeft() -
                      key_text->boundingRect().center()));
  }
}

void View::DrawTree(ConstGeomBPTree data) {
  scene_.clear();
  if (!data->GetDataOfTree().root) {
    return;
  }
  CalcOffset(data->GetDataOfTree().lower_right_point_of_tree_);
  DrawSubTree(*data->GetDataOfTree().root);
}

void View::DrawSubTree(const GeomNode& node) {
  DrawOneNode(node);
  for (const auto& child : node.children) {
    scene_.addLine(QLineF(ConvertMyPointToQPoint(child->upper_mid_point),
                          ConvertMyPointToQPoint(node.lower_mid_point)),
                   QPen(Qt::black));
    DrawSubTree(*child.get());
  }
}

}  // namespace BPT
