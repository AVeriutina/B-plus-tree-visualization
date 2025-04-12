#pragma once

#include "ConstValue.h"
#include "GeomTree.h"
#include "Observer.h"

namespace BPT {

class Animator;

class GeomModel {
  using DataFromBPTree = BPTree::Detail::DataFromBPTree;
  using ConstGeomBPTree = Detail::ConstValue<GeomBPlusTree>;
  using GeomObservable =
      NSLibrary::CObservable<ConstGeomBPTree, NSLibrary::CByValue>;

  using GeomObserver =
      NSLibrary::CColdInput<DataFromBPTree, NSLibrary::CByValue>;

 public:
  GeomModel();
  GeomObserver* GetObserverPort();
  void SubscribeAnimator(Animator* animator);

 private:
  void ActionOnNotify(const DataFromBPTree& data);

  GeomObserver input_port_;
  GeomObservable output_port_;
  ConstGeomBPTree temp_tree_;
};

}  // namespace BPT
