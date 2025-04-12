#include <QApplication>

#include "Application.h"
#include "Except.h"

int main(int argc, char** argv) {
  QApplication qt_runtime(argc, argv);
  try {
    BPT::Application app;
    app.Run();
    return qt_runtime.exec();
  } catch (...) {
    Except::React();
  }
  return 0;
}
