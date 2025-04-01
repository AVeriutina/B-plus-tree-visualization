#include "Animator.h"

#include "Observer.h"
#include "View.h"

namespace BPT {

Animator::Animator()
    : input_port_([this](ConstGeomBPTree data) { ActionOnNotify(data); }),
      output_port_([this]() { return SendOneWaiting(); }) {}

Animator::AnimatorObserver* Animator::GetObserverPort() { return &input_port_; }

void Animator::SubscribeView(View* view) {
  output_port_.subscribe(
      static_cast<NSLibrary::CObserver<ConstGeomBPTree, NSLibrary::CByValue>*>(
          view->GetObserverPort()));
}

void Animator::ActionOnNotify(ConstGeomBPTree data) {
  queue_.push(std::move(data));
  output_port_.notify();
}

Animator::ConstGeomBPTree Animator::SendOneWaiting() {
  assert(!queue_.empty());
  ConstGeomBPTree waiting = std::move(queue_.front());
  queue_.pop();
  // timer_.start(interval_between_frames_);
  return std::move(waiting);
}

}  // namespace BPT
