#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#ifndef _WIN32
#include <stdlib.h>
#endif

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;

#ifndef _WIN32
  // Make sure that Wine exists on Linux
  int exitcode = system("wine hostname");
  if (exitcode != 0) {
    QMessageBox::warning(&w, "Wine not found", "Wine could not be found. You will have to pick the path to your Age of Empires 2 directory manually.", QMessageBox::Ok);
  }
#endif

  w.show();

  return a.exec();
}
