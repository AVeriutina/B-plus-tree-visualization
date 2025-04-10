#include "Application.h"

#include "Controllers.h"

namespace BPT {

Application::Application()
    : model_(),
      geom_model_(),
      animator_(),
      view_(),
      window_(),
      controller_work_with_key_(&model_, window_.GetKeyEdit()),
      controller_for_clean_(&model_, window_.GetSpinBoxDegree()),
      controller_timer_(animator_.GetTimer()) {
  model_.SubscribeGeomModel(&geom_model_);
  geom_model_.SubscribeAnimator(&animator_);
  animator_.SubscribeView(&view_);

  window_.SetSceneForView(view_.GetScene());

  QObject::connect(window_.GetButtonInsert(), &QPushButton::clicked,
                   &controller_work_with_key_,
                   &ControllerInsertFindDelete::OnPushButtonInsertClicked);
  QObject::connect(window_.GetButtonFind(), &QPushButton::clicked,
                   &controller_work_with_key_,
                   &ControllerInsertFindDelete::OnPushButtonFindClicked);
  QObject::connect(window_.GetButtonDelete(), &QPushButton::clicked,
                   &controller_work_with_key_,
                   &ControllerInsertFindDelete::OnPushButtonDeleteClicked);
  QObject::connect(window_.GetButtonClear(), &QPushButton::clicked,
                   &controller_for_clean_,
                   &ControllerClearAndDegree::OnPushButtonClearClicked);
  QObject::connect(
      window_.GetSpinBoxDegree(), QOverload<int>::of(&QSpinBox::valueChanged),
      &controller_for_clean_, &ControllerClearAndDegree::OnSpinBoxValueChanged);
  QObject::connect(window_.GetSliderSpeed(), &QSlider::valueChanged,
                   &controller_timer_, &ControllerTimer::SetInterval);
}

void Application::Run() { window_.show(); }

}  // namespace BPT
