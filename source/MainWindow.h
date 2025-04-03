#pragma once

#include <qslider.h>

#include <QApplication>
#include <QGraphicsView>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

namespace BPT {
class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow();
  MainWindow(MainWindow&&) = delete;
  MainWindow(const MainWindow&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;
  MainWindow& operator=(MainWindow&&) = delete;
  ~MainWindow() = default;

  QLineEdit* GetKeyEdit();
  QPushButton* GetButtonInsert();
  QPushButton* GetButtonFind();
  QPushButton* GetButtonDelete();
  QSpinBox* GetSpinBoxDegree();
  QSlider* GetSliderSpeed();
  QPushButton* GetButtonClear();
  void SetSceneForView(QGraphicsScene* scene);

 private:
  QGraphicsView* view_;

  QLineEdit* key_edit_;
  QPushButton* button_insert_;
  QPushButton* button_find_;
  QPushButton* button_delete_;
  QSpinBox* spin_box_degree_;
  QSlider* slider_degree_;
  QPushButton* button_clear_;
};
}  // namespace BPT
