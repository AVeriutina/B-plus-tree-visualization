#pragma once

#include "Animator.h"
#include "Controllers.h"
#include "GeomModel.h"
#include "MainWindow.h"
#include "TreeModel/BPlusTree.h"
#include "View.h"

namespace BPT {

class Application {
 public:
  Application();
  Application(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(const Application&) = delete;
  Application& operator=(Application&&) = delete;
  ~Application() = default;

  void Run();

 private:
  BPTree::BPlusTree model_;
  GeomModel geom_model_;
  Animator animator_;
  View view_;
  MainWindow window_;
  ControllerInsertFindDelete controller_work_with_key_;
  ControllerClearAndDegree controller_for_clean_;
  ControllerTimer controller_timer_;
};

}  // namespace BPT
