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
  ControllerInsertFindDelete controller1_;
  ControllerClearAndDegree controller2_;
  ControllerTimer controller3_;
};

}  // namespace BPT
