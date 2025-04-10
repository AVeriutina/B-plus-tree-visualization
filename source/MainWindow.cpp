#include "MainWindow.h"

#include "MainWindowDetail.h"

namespace BPT {

MainWindow::MainWindow() : QMainWindow(), view_(new QGraphicsView(this)) {
  using namespace MainWindowDetails;

  setGeometry(0, 0, kTotalWidth, kTotalHeight);
  setWindowTitle("B-Plus Tree Visualization");

  auto* centralWidget = new QWidget(this);

  setCentralWidget(centralWidget);

  view_->setParent(centralWidget);
  view_->setGeometry(kDistBetweenItems, kDistBetweenItems, kTotalWidthScene,
                     kTotalHeightScene);

  key_edit_ = new QLineEdit(centralWidget);
  key_edit_->setGeometry(kStartXOfButton, kDistBetweenItems, kWidthButton,
                         kHeightButton);

  button_insert_ = new QPushButton("Insert", centralWidget);
  button_insert_->setGeometry(
      kStartXOfButton, kDistBetweenItems + kHeightButton + kDistBetweenButtons,
      kWidthButton, kHeightButton);

  button_find_ = new QPushButton("Find", centralWidget);
  button_find_->setGeometry(
      kStartXOfButton,
      kDistBetweenItems + 2 * kHeightButton + 2 * kDistBetweenButtons,
      kWidthButton, kHeightButton);

  button_delete_ = new QPushButton("Delete", centralWidget);
  button_delete_->setGeometry(
      kStartXOfButton,
      kDistBetweenItems + 3 * kHeightButton + 3 * kDistBetweenButtons,
      kWidthButton, kHeightButton);

  button_clear_ = new QPushButton("Clear", centralWidget);
  button_clear_->setGeometry(kStartXOfButton,
                             kTotalHeight - kDistBetweenItems - kHeightButton,
                             kWidthButton, kHeightButton);

  auto* labelDegree = new QLabel("Degree", centralWidget);
  labelDegree->setGeometry(kStartXOfButton, kStartHeightOfDegreeAndSpeed,
                           kWidthButton, kHeightButton);
  labelDegree->setAlignment(Qt::AlignCenter);

  spin_box_degree_ = new QSpinBox(centralWidget);
  spin_box_degree_->setGeometry(kStartXOfButton,
                                kStartHeightOfDegreeAndSpeed + kHeightButton,
                                kWidthButton, kHeightButton);
  spin_box_degree_->setMinimum(MinDegreeOfTree);
  spin_box_degree_->setMaximum(MaxDegreeOfTree);

  auto* labelSpeed = new QLabel("Speed", centralWidget);
  labelSpeed->setGeometry(kStartXOfButton,
                          kStartHeightOfDegreeAndSpeed + 2 * kHeightButton +
                              2 * kDistBetweenButtons,
                          kWidthButton, kHeightButton);
  labelSpeed->setAlignment(Qt::AlignCenter);

  slider_degree_ = new QSlider(Qt::Horizontal, centralWidget);
  slider_degree_->setGeometry(
      kStartXOfButton,
      kStartHeightOfDegreeAndSpeed + 3 * kHeightButton + kDistBetweenButtons,
      kWidthButton, kHeightButton);
  slider_degree_->setRange(MinAnimationSpeed, MaxAnimationSpeed);
  slider_degree_->setValue(StartAnimationSpeed);
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
