#pragma once
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QMetaType>
#include "libwololokingdoms/wksettings.h"
#include "libwololokingdoms/wkconverter.h"
#include <fs.h>
#include <set>
#include <regex>
#include <memory>

namespace Ui {
class MainWindow;
}

Q_DECLARE_METATYPE(std::string)

class WKQConverter: public QObject, public WKConvertListener {
    Q_OBJECT
private:
    WKSettings settings;
    std::unique_ptr<WKConverter> converter;
public slots:
    void process();
signals:
    void finished() override;
    void log(std::string logMessage) override;
    void setInfo(std::string info) override;
    void createDialog(std::string info) override;
    void createDialog(std::string info, std::string title) override;
    void createDialog(std::string info, std::string toReplace, std::string replaceWith) override;
    void setProgress(int i) override;
    void increaseProgress(int i) override;
public:
    WKQConverter(WKSettings& settings);
    void error(std::exception const & err) override;
    void installUserPatch(fs::path exePath, std::vector<std::string> cliFlags) override;
};

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

    fs::path steamPath;
    fs::path hdPath;
    fs::path outPath;
    std::map<int, std::tuple<std::string,std::string, std::string, int, std::string>> dataModList;
    std::string language = "en";
    std::map<QString, QString> translation;
    bool allowRun = true;

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
    std::string baseModName = "WololoKingdoms";
    fs::path resourceDir;

	Ui::MainWindow *ui;

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
