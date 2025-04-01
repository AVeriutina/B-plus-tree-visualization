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

ControllerClearAndDegree::ControllerClearAndDegree(
    BPTree::BPlusTree* bp_tree, QSpinBox* box_degree,
    std::function<void(void)> clear_scene)
    : bp_tree_(bp_tree), box_degree_(box_degree), clear_scene_(clear_scene) {}

void ControllerClearAndDegree::OnPushButtonClearClicked() {
  clear_scene_();
  bp_tree_->Reset();
}

void ControllerClearAndDegree::OnSpinBoxValueChanged(int new_degree) {
  clear_scene_();
  bp_tree_->SetDegree(new_degree);
}

}  // namespace BPT
