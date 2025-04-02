#include "Application.h"

#include <qobject.h>

#include "Controllers.h"

namespace BPT {

Application::Application()
    : model_(),
      geom_model_(),
      animator_(),
      view_(),
      window_(),
      controller1_(&model_, window_.GetKeyEdit()),
      controller2_(&model_, window_.GetSpinBoxDegree(),
                   view_.FunctionClearScene()),
      controller3_(animator_.GetTimer()) {
  model_.SubscribeGeomModel(&geom_model_);
  geom_model_.SubscribeAnimator(&animator_);
  animator_.SubscribeView(&view_);

  window_.SetSceneForView(view_.GetScene());

  QObject::connect(window_.GetButtonInsert(), &QPushButton::clicked,
                   &controller1_,
                   &ControllerInsertFindDelete::OnPushButtonInsertClicked);
  QObject::connect(window_.GetButtonFind(), &QPushButton::clicked,
                   &controller1_,
                   &ControllerInsertFindDelete::OnPushButtonFindClicked);
  QObject::connect(window_.GetButtonDelete(), &QPushButton::clicked,
                   &controller1_,
                   &ControllerInsertFindDelete::OnPushButtonDeleteClicked);
  QObject::connect(window_.GetButtonClear(), &QPushButton::clicked,
                   &controller2_,
                   &ControllerClearAndDegree::OnPushButtonClearClicked);
  QObject::connect(window_.GetSpinBoxDegree(),
                   QOverload<int>::of(&QSpinBox::valueChanged), &controller2_,
                   &ControllerClearAndDegree::OnSpinBoxValueChanged);
  QObject::connect(window_.GetSliderSpeed(), &QSlider::valueChanged,
                   &controller3_, &ControllerTimer::SetInterval);
}

void Application::Run() { window_.show(); }

}  // namespace BPT
