#pragma once

#include <memory>

#include "BPTreeData.h"
#include "Observer.h"

namespace BPT::GeomModel {

class GeomBPlusTree;

class GeomModel {
  using DataFromBPTreeToGeomModel = BPTree::Detail::DataFromBPTree;

  using GeomObservable =
      NSLibrary::CObservable<std::shared_ptr<const GeomBPlusTree>,
                             NSLibrary::CByValue>;

  using GeomObserver = NSLibrary::CColdInput<DataFromBPTreeToGeomModel>;

 public:
  GeomModel(GeomObservable output_port);

  void ActionOnNotify(const BPT::BPTree::Detail::DataFromBPTree& data);

  std::shared_ptr<const GeomBPlusTree> SendData();

 private:
  GeomObserver input_port_;
  GeomObservable output_port_;
  std::shared_ptr<const GeomBPlusTree> temp_tree_;
};

}  // namespace BPT::GeomModel
