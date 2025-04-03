#include "Controllers.h"

#include <QMessageBox>
#include <cassert>

#include "TreeModel/BPlusTree.h"

namespace BPT {

namespace ControllerDetail {

void ShowIncorrectInputMessage() {
  QMessageBox error_box;
  error_box.setWindowTitle("Input Error");
  error_box.setText("The entered key is not correct");
  error_box.setIcon(QMessageBox::Information);
  error_box.setStandardButtons(QMessageBox::Ok);
  error_box.exec();
}

bool ShowConfirmationOfAction() {
  QMessageBox confirmation_box;
  confirmation_box.setWindowTitle("Confirmation");
  confirmation_box.setText(
      "Changing the tree degree will clear the existing tree. Do you want to "
      "proceed?");
  confirmation_box.setIcon(QMessageBox::Warning);
  confirmation_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

  int result = confirmation_box.exec();
  return (result == QMessageBox::Ok);
}

}  // namespace ControllerDetail

ControllerInsertFindDelete::ControllerInsertFindDelete(
    BPTree::BPlusTree* bp_tree, QLineEdit* key_edit)
    : bp_tree_(bp_tree), key_edit_(key_edit) {}

void ControllerInsertFindDelete::OnPushButtonInsertClicked() {
  PushButtonClicked([this](int value) { bp_tree_->Insert(value); });
}

void ControllerInsertFindDelete::OnPushButtonFindClicked() {
  PushButtonClicked([this](int value) { bp_tree_->FindKey(value); });
}

void ControllerInsertFindDelete::OnPushButtonDeleteClicked() {
  PushButtonClicked([this](int value) { bp_tree_->Delete(value); });
}

void ControllerInsertFindDelete::PushButtonClicked(
    const std::function<void(int)>& operation) {
  assert(bp_tree_);
  bool convert_to_int = false;
  int value = key_edit_->text().toInt(&convert_to_int);
  key_edit_->clear();
  if (convert_to_int) {
    operation(value);
  } else {
    ControllerDetail::ShowIncorrectInputMessage();
  }
}

ControllerClearAndDegree::ControllerClearAndDegree(BPTree::BPlusTree* bp_tree,
                                                   QSpinBox* box_degree)
    : bp_tree_(bp_tree), box_degree_(box_degree) {}

void ControllerClearAndDegree::OnPushButtonClearClicked() { bp_tree_->Reset(); }

void ControllerClearAndDegree::OnSpinBoxValueChanged(int new_degree) {
  if (ControllerDetail::ShowConfirmationOfAction()) {
    bp_tree_->SetDegree(new_degree);
    previous_degree_ = new_degree;
  } else {
    box_degree_->blockSignals(true);
    box_degree_->setValue(previous_degree_);
    box_degree_->blockSignals(false);
  }
}

ControllerTimer::ControllerTimer(QTimer* timer) : timer_(timer) {}

void ControllerTimer::SetInterval(int msec) {
  timer_->setInterval(2100 - msec);
}

}  // namespace BPT
