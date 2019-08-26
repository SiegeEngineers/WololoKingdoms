#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>
#ifndef _WIN32
#include <stdlib.h>
#endif

bool can_run_windows_programs() {
#ifndef _WIN32
  // Make sure that Wine exists on Linux
  int exitcode = system("wine --version");
  return exitcode == 0;
#else
  return true;
#endif
}

int main(int argc, char* argv[]) {
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication application(argc, argv);
  application.setApplicationName("WololoKingdoms");
  application.setApplicationVersion("5.8.1.6");

  QCommandLineParser cli;
  cli.setApplicationDescription(
      "Installer for the WololoKingdoms mod, bringing Age of Empires II: HD "
      "Edition expansions to Age of Empires II: The Conquerors");
  cli.addHelpOption();
  cli.addVersionOption();
  QCommandLineOption skipUpdaterOption(
      "s",
      QCoreApplication::translate("main", "Do not call the auto-updater."));
  cli.addOption(skipUpdaterOption);

  cli.process(application);

  bool skipUpdater = cli.isSet(skipUpdaterOption);
  MainWindow window(nullptr, skipUpdater);

  if (!can_run_windows_programs()) {
    QMessageBox::warning(&window, "Wine not found",
                         "Wine could not be found. You will have to pick the "
                         "path to your Age of Empires 2 directory manually.",
                         QMessageBox::Ok);
  }

  window.show();

  return application.exec();
}
