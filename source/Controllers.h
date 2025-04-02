#pragma once

#include <qlineedit.h>
#include <qobjectdefs.h>
#include <qslider.h>

#include <QLineEdit>
#include <QObject>
#include <QSpinBox>
#include <functional>

namespace BPT {

namespace BPTree {
class BPlusTree;
}

class ControllerInsertFindDelete : public QObject {
  Q_OBJECT
 public:
  ControllerInsertFindDelete(BPTree::BPlusTree* bp_tree, QLineEdit* key_edit);
  ControllerInsertFindDelete(const ControllerInsertFindDelete&) = delete;
  ControllerInsertFindDelete(ControllerInsertFindDelete&&) = delete;
  ControllerInsertFindDelete& operator=(const ControllerInsertFindDelete&) =
      delete;
  ControllerInsertFindDelete& operator=(ControllerInsertFindDelete&&) = delete;
  ~ControllerInsertFindDelete() = default;

 public slots:
  void OnPushButtonInsertClicked();
  void OnPushButtonFindClicked();
  void OnPushButtonDeleteClicked();

 private:
  void PushButtonClicked(const std::function<void(int)>& operation);
  BPTree::BPlusTree* bp_tree_;
  QLineEdit* key_edit_;
};

class ControllerClearAndDegree : public QObject {
  Q_OBJECT
 public:
  ControllerClearAndDegree(BPTree::BPlusTree* bp_tree, QSpinBox* box_degree,
                           std::function<void(void)> clear_scene);
  ControllerClearAndDegree(const ControllerClearAndDegree&) = delete;
  ControllerClearAndDegree(ControllerClearAndDegree&&) = delete;
  ControllerClearAndDegree& operator=(const ControllerClearAndDegree&) = delete;
  ControllerClearAndDegree& operator=(ControllerClearAndDegree&&) = delete;
  ~ControllerClearAndDegree() = default;

 public slots:
  void OnPushButtonClearClicked();
  void OnSpinBoxValueChanged(int new_degree);

 private:
  BPTree::BPlusTree* bp_tree_;
  std::function<void(void)> clear_scene_;
  QSpinBox* box_degree_;
  int previous_degree_ = 3;
};

class ControllerTimer : public QObject {
  Q_OBJECT
 public:
  ControllerTimer(QTimer* timer);
  ControllerTimer(const ControllerTimer&) = delete;
  ControllerTimer(ControllerTimer&&) = delete;
  ControllerTimer& operator=(const ControllerTimer&) = delete;
  ControllerTimer& operator=(ControllerTimer&&) = delete;
  ~ControllerTimer() = default;

 public slots:
  void SetInterval(int value);

 private:
  QTimer* timer_;
};

}  // namespace BPT
