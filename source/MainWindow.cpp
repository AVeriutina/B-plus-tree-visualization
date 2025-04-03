#include "MainWindow.h"

namespace BPT {

MainWindow::MainWindow() : QMainWindow(), view_(new QGraphicsView(this)) {
  setGeometry(0, 0, 1220, 830);
  setWindowTitle("B-Plus Tree Visualization");

  auto* centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  view_->setParent(centralWidget);
  view_->setGeometry(10, 10, 1010, 810);

  key_edit_ = new QLineEdit(centralWidget);
  key_edit_->setGeometry(1030, 10, 180, 40);

  button_insert_ = new QPushButton("Insert", centralWidget);
  button_insert_->setGeometry(1030, 70, 180, 40);

  button_find_ = new QPushButton("Find", centralWidget);
  button_find_->setGeometry(1030, 130, 180, 40);

  button_delete_ = new QPushButton("Delete", centralWidget);
  button_delete_->setGeometry(1030, 190, 180, 40);

  button_clear_ = new QPushButton("Clear", centralWidget);
  button_clear_->setGeometry(1030, 780, 180, 40);

  auto* labelDegree = new QLabel("Degree", centralWidget);
  labelDegree->setGeometry(1030, 500, 180, 40);
  labelDegree->setAlignment(Qt::AlignCenter);

  spin_box_degree_ = new QSpinBox(centralWidget);
  spin_box_degree_->setGeometry(1030, 550, 180, 40);
  spin_box_degree_->setMinimum(3);
  spin_box_degree_->setMaximum(10);

  auto* labelSpeed = new QLabel("Speed", centralWidget);
  labelSpeed->setGeometry(1030, 640, 180, 40);
  labelSpeed->setAlignment(Qt::AlignCenter);

  slider_degree_ = new QSlider(Qt::Horizontal, centralWidget);
  slider_degree_->setGeometry(1030, 680, 180, 40);
  slider_degree_->setRange(100, 2000);
  slider_degree_->setValue(1000);
}

QLineEdit* MainWindow::GetKeyEdit() { return key_edit_; }

QPushButton* MainWindow::GetButtonInsert() { return button_insert_; }

QPushButton* MainWindow::GetButtonFind() { return button_find_; }

QPushButton* MainWindow::GetButtonDelete() { return button_delete_; }

QSpinBox* MainWindow::GetSpinBoxDegree() { return spin_box_degree_; }

QSlider* MainWindow::GetSliderSpeed() { return slider_degree_; }

QPushButton* MainWindow::GetButtonClear() { return button_clear_; }

void MainWindow::SetSceneForView(QGraphicsScene* scene) {
  view_->setScene(scene);
}

}  // namespace BPT
