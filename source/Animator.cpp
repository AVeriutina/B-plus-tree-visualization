#include "Animator.h"

#include "View.h"

namespace BPT {

Animator::Animator()
    : input_port_([this](ConstGeomBPTree data) { ActionOnNotify(data); }),
      output_port_([this]() { return SendOneWaiting(); }) {
  QObject::connect(&timer_, &QTimer::timeout, this, &Animator::OnTimer);
  timer_.setInterval(interval_between_frames_);
  timer_.start();
}

Animator::AnimatorObserver* Animator::GetObserverPort() { return &input_port_; }

void Animator::SubscribeView(View* view) {
  output_port_.subscribe(
      static_cast<NSLibrary::CObserver<ConstGeomBPTree, NSLibrary::CByValue>*>(
          view->GetObserverPort()));
}

QTimer* Animator::GetTimer() { return &timer_; }

void Animator::OnTimer() {
  if (queue_.empty()) {
    timer_.stop();
    return;
  }
  output_port_.notify();
}

void Animator::ActionOnNotify(ConstGeomBPTree data) {
  queue_.push(std::move(data));
  timer_.start();
}

Animator::ConstGeomBPTree Animator::SendOneWaiting() {
  assert(!queue_.empty());
  ConstGeomBPTree waiting = std::move(queue_.front());
  queue_.pop();
  return waiting;
}

}  // namespace BPT
