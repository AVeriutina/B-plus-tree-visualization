#include "GeomModel.h"

#include <cassert>
#include <utility>

#include "Animator.h"
#include "GeomTree.h"
#include "Observer.h"

namespace BPT {

GeomModel::GeomModel()
    : output_port_([this]() { return temp_tree_; }),
      input_port_(
          [this](const DataFromBPTree& data) { ActionOnNotify(data); }) {}

GeomModel::GeomObserver* GeomModel::GetObserverPort() { return &input_port_; }

void GeomModel::SubscribeAnimator(Animator* animator) {
  output_port_.subscribe(animator->GetObserverPort());
}

void GeomModel::ActionOnNotify(const DataFromBPTree& data) {
  GeomBPlusTree geom_tree(data);
  temp_tree_ = ConstGeomBPTree(std::move(geom_tree));
  output_port_.notify();
}

}  // namespace BPT
