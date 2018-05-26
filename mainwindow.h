#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>

#include <set>
#include <regex>
#include "wkgui.h"
#include "wksettings.h"
#include "wkconverter.h"

#include <boost/filesystem.hpp>
#include "genie/dat/DatFile.h"
#include "genie/lang/LangFile.h"

#define rt_getSLPName() std::get<0>(*repIt)
#define rt_getPattern() std::get<1>(*repIt)
#define rt_getReplaceName() std::get<2>(*repIt)
#define rt_getOldId() std::get<3>(*repIt)
#define rt_getNewId() std::get<4>(*repIt)
#define rt_getTerrainType() std::get<5>(*repIt)


namespace fs = boost::filesystem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public WKGui
{
    Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    std::string steamPath;
    fs::path HDPath;
    fs::path outPath;
    std::string version = "5.7.2";
    std::map<int, std::tuple<std::string,std::string, std::string, int, std::string>> dataModList;
    std::string language = "en";
    std::map<std::string, std::string> translation;
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

    void log(std::string logMessage);
    void setInfo(std::string info);
    void createDialog(std::string info);
    void createDialog(std::string info, std::string title);
    void setProgress(int i);
    void increaseProgress(int i);
    std::string translate(std::string line);

    void runConverter();

    int initialize();
    void setInstallDirectory(std::string directory);
    void changeLanguage();
    void setButtonWhatsThis(QPushButton* button, std::string title);
    void readDataModList();
    void checkSteamApi();
    void readSettings();
    void writeSettings();
	void changeModPatch();
    void callExternalExe(std::wstring exe);
    void updateUI();
};

#endif // MAINWINDOW_H
