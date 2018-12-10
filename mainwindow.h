#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>

#include <set>
#include <regex>
#include <filesystem>
#include "wkgui.h"
#include "libwololokingdoms/wksettings.h"
#include "libwololokingdoms/wkconverter.h"

namespace fs = std::filesystem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void log(std::string logMessage);
    void setInfo(std::string info);
    void createDialog(std::string info);
    void createDialog(std::string info, std::string title);
    void createDialog(std::string info, std::string toReplace, std::string replaceWith);
    void setProgress(int i);
    void increaseProgress(int i);
    QString translate(QString line);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    std::string steamPath;
    fs::path HDPath;
    fs::path outPath;
    std::map<int, std::tuple<std::string,std::string, std::string, int, std::string>> dataModList;
    std::string language = "en";
    std::map<QString, QString> translation;
    bool allowRun = true;

    QProgressBar* bar = NULL;
    int dlcLevel;
    int patch = -1;
    std::string modName;
    std::ofstream logFile;

    fs::path vooblyDir;
    fs::path upDir;
    fs::path installDir;
    fs::path nfzUpOutPath;
    fs::path nfzVooblyOutPath;
    std::string baseModName = "WololoKingdoms";
    fs::path resourceDir;

	Ui::MainWindow *ui;

    /*
    void log(std::string logMessage);
    void setInfo(std::string info);
    void createDialog(std::string info);
    void createDialog(std::string info, std::string title);
    void setProgress(int i);
    void increaseProgress(int i);
    std::string translate(std::string line);
*/
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
    void callExternalExe(std::wstring exe);
    void updateUI();
};

#endif // MAINWINDOW_H
