#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>

#include <set>

#include <boost/filesystem.hpp>
#include "genie/dat/DatFile.h"
#include "genie/lang/LangFile.h"


namespace fs = boost::filesystem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
	int run();

private:

    std::string steamPath;
    fs::path HDPath;
    fs::path outPath;
    std::set<int> aocSlpFiles;
    std::map<int, fs::path> slpFiles;
    std::map<int, fs::path> wavFiles;
    std::map<std::string,fs::path> newTerrainFiles;
    std::vector<std::pair<int,std::string>> rmsCodeStrings;
    std::string version = "2.8";
    std::string hash1;
    std::string hash2;
    std::string patchNumber;
    std::map<int, std::tuple<std::string,std::string,int>> dataModList;
    std::string language = "en";
    std::map<std::string, std::string> translation;
    bool secondAttempt = false;
    bool allowRun = true;

    std::set<char> civLetters;
    QProgressBar* bar = NULL;
    int dlcLevel = 0;
    int patch = -1;
    std::string modName;
    std::ofstream logFile;

    fs::path nfzUpOutPath;
    fs::path nfzOutPath;
    /*
    fs::path modHkiOutPath;
    fs::path modHki2OutPath;
    fs::path upHkiOutPath;
    fs::path upHki2OutPath; */
    fs::path vooblyDir;
    fs::path upDir;
    std::string referenceDir = "WololoKingdoms FE";
    fs::path resourceDir;

	Ui::MainWindow *ui;
    void changeLanguage();
    void setButtonWhatsThis(QPushButton* button, std::string title);
	void changeModPatch();
	void updateUI();
    void copyHDMaps(fs::path inputDir, fs::path outputDir, bool replace = false);
	void terrainSwap(genie::DatFile *hdDat, genie::DatFile *aocDat, int tNew, int tOld, int slpID);
    void recCopy(fs::path const &src, fs::path const &dst, bool skip = false, bool force = false);
    void indexDrsFiles(fs::path const &src, bool expansionFiles = true, bool terrainFiles = false);
    void copyHistoryFiles(fs::path inputDir, fs::path outputDir);
    std::pair<int,std::string> getTextLine(std::string line);
	void convertLanguageFile(std::ifstream *in, std::ofstream *iniOut, genie::LangFile *dllOut, bool generateLangDll, std::map<int, std::string> *langReplacement);
	void makeDrs(std::ofstream *out);
    void uglyHudHack(fs::path);
    void copyCivIntroSounds(fs::path inputDir, fs::path outputDir);
    void copyWallFiles(fs::path inputDir);
	std::string tolower(std::string line);
	void createMusicPlaylist(std::string inputDir, std::string const outputDir);
	void transferHdDatElements(genie::DatFile *hdDat, genie::DatFile *aocDat);
    void adjustArchitectureFlags(genie::DatFile *aocDat, std::string flagFilename);
	void patchArchitectures(genie::DatFile *aocDat);
	bool checkGraphics(genie::DatFile *aocDat, short graphicID, std::vector<int> checkedGraphics);
    void replaceGraphic(genie::DatFile *aocDat, short* graphicID, short compareID, short c, std::map<short,short>& replacedGraphics, std::map<int,int> slpIdConversion = std::map<int,int>());
    short duplicateGraphic(genie::DatFile *aocDat, std::map<short,short>& replacedGraphics, std::vector<short> duplicatedGraphics, short graphicID, short compareID, short offset, bool manual = false, std::map<int,int> slpIdConversion = std::map<int,int>());
    bool identifyHotkeyFile(fs::path directory, fs::path& maxHki, fs::path& lastEditedHki);
    void copyHotkeyFile(fs::path maxHki, fs::path lastEditedHki, fs::path dst);
    void removeWkHotkeys();
	void hotkeySetup();
    void symlinkSetup(fs::path newDir, fs::path xmlIn, fs::path xmlOut, bool vooblySrc, bool vooblyDst, bool dataMod = false);
	bool copyData(QIODevice &inFile, QIODevice &outFile);
};

#endif // MAINWINDOW_H
