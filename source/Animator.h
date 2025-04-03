#pragma once

#include <QObject>
#include <QTimer>
#include <queue>

#include "GeomTree.h"
#include "Observer.h"

namespace BPT {

class View;

class Animator : public QObject {
  Q_OBJECT
  using ConstGeomBPTree = GeomTreeDetail::ConstValue<GeomBPlusTree>;

  using AnimatorObservable =
      NSLibrary::CObservable<ConstGeomBPTree, NSLibrary::CByValue>;

  using AnimatorObserver =
      NSLibrary::CColdInput<ConstGeomBPTree, NSLibrary::CByValue>;

  static constexpr int standart_interval_between_frames_ = 400;

 public:
  Animator();
  AnimatorObserver* GetObserverPort();
  void SubscribeView(View* view);
  QTimer* GetTimer();

 private slots:
  void OnTimer();

 private:
  void ActionOnNotify(ConstGeomBPTree data);
  ConstGeomBPTree SendOneWaiting();
  void AnimateQueries() {}

  AnimatorObserver input_port_;
  AnimatorObservable output_port_;

  int interval_between_frames_ = standart_interval_between_frames_;
  QTimer timer_;
  std::queue<ConstGeomBPTree> queue_;
};

}  // namespace BPT
