#include "GeomModel.h"

#include <cassert>
#include <memory>
#include <utility>

#include "GeomTree.h"

namespace BPT::GeomModel {

GeomModel::GeomModel(GeomObservable output_port)
    : output_port_([this]() { return SendData(); }),
      input_port_([this](const DataFromBPTreeToGeomModel& data) {
        this->ActionOnNotify(data);
      }) {}

void GeomModel::ActionOnNotify(const DataFromBPTreeToGeomModel& data) {
  GeomBPlusTree geom_tree;
  geom_tree.BuildTree(data);

  temp_tree_ = std::make_shared<const GeomBPlusTree>(std::move(geom_tree));
  output_port_.notify();
}

std::shared_ptr<const GeomBPlusTree> GeomModel::SendData() {
  assert(temp_tree_);
  auto send_tree = temp_tree_;
  temp_tree_ = nullptr;
  return send_tree;
}

}  // namespace BPT::GeomModel
