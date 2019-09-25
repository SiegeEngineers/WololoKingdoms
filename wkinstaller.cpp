#include "wkinstaller.h"
#include <QProcess>
#include <QString>

WKInstaller::WKInstaller(WKSettings& settings) : settings(settings) {}

void WKInstaller::process() {
  converter = std::make_unique<WKConverter>(settings, this);
  try {
    converter->run();
  } catch (const std::exception& e) {
    error(e);
    emit setInfo("error");
    // Retry once…
    try {
      converter->retryInstall();
    } catch (const std::exception& e) {
      error(e, true);
      emit finished();
      emit setInfo("error");
    }
  }
}

void WKInstaller::error(std::exception const& err, bool showDialog) { 
  std::string errorMessage = err.what();
  if (showDialog)
	emit createDialog("dialogError$" + errorMessage, "Error");
  emit log(errorMessage); 
}

void WKInstaller::installUserPatch(fs::path exePath,
                                   std::vector<std::string> cliFlags) {
  QProcess process;
  QStringList args;
  QString name = QString::fromStdString(exePath.string());

#ifndef _WIN32
  // Use wine on non-windows
  args << name;
  name = QString("wine");
#endif

  for (auto arg : cliFlags) {
    args << QString::fromStdString(arg);
  }

  process.start(name, args);
  process.waitForFinished(180000);
}
