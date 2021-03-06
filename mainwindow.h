#pragma once
#include <QMainWindow>
#include <QMetaType>
#include <QProgressBar>
#include <QPushButton>
#include <fs.h>

namespace Ui {
class MainWindow;
}

Q_DECLARE_METATYPE(std::string)

class MainWindow : public QMainWindow {
  Q_OBJECT

public slots:
  void log(std::string logMessage);
  void setInfo(std::string info);
  void finished();
  void createDialog(std::string info);
  void createDialog(std::string info, std::string title);
  void createDialog(std::string info, std::string toReplace,
                    std::string replaceWith);
  void setProgress(int i);
  void increaseProgress(int i);
  QString translate(QString line);

public:
  explicit MainWindow(QWidget* parent = 0, bool skipUpdater = false);
  ~MainWindow();

private:
  fs::path steamPath;
  fs::path hdPath;
  fs::path outPath;
  std::map<int,
           std::tuple<std::string, std::string, std::string, int, std::string>>
      dataModList;
  std::string language = "en";
  std::map<QString, QString> translation;
  bool allowRun = true;
  bool skipUpdater = false;

  QProgressBar* bar = nullptr;
  int dlcLevel;
  int patch = -1;
  std::string modName;
  std::ofstream logFile;

  fs::path vooblyDir;
  fs::path upDir;
  fs::path installDir;
  fs::path nfzUpOutPath;
  fs::path nfzVooblyOutPath;
  std::string baseModName = "WololoKingdoms Beta";
  fs::path resourceDir;

  Ui::MainWindow* ui;

  void runConverter();

  int initialize();
  void setInstallDirectory(std::string directory);
  void changeLanguage();
  void setButtonWhatsThis(QPushButton* button, QString title);
  void readDataModList();
  bool checkSteamApi();
  void readSettings();
  void writeSettings();
  void changeModPatch();
  void updateUI();
};
