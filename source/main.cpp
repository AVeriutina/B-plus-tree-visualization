#include <QApplication>

#include "Application.h"
// #include "Except.h"

int main(int argc, char** argv) {
  try {
    QApplication qt_runtime(argc, argv);
    BPT::Application app;
    app.Run();
    return qt_runtime.exec();
  } catch (...) {
    // except::react();  // тут обрабатываются исключения
  }
}
