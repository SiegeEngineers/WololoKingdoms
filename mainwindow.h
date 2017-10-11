#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <boost/filesystem.hpp>
#include "genie/dat/DatFile.h"
#include "genie/lang/LangFile.h"

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
	Ui::MainWindow *ui;
    void changeLanguage();
	void changeModPatch();
	void updateUI();
	void copyHDMaps(boost::filesystem::path inputDir, boost::filesystem::path outputDir, bool replace = false);
	void terrainSwap(genie::DatFile *hdDat, genie::DatFile *aocDat, int tNew, int tOld, int slpID);
	void recCopy(boost::filesystem::path const &src, boost::filesystem::path const &dst, bool skip = false, bool force = false);
    void indexDrsFiles(boost::filesystem::path const &src, bool expansionFiles = true);
	void indexTerrainFiles(boost::filesystem::path const &src);
	void convertLanguageFile(std::ifstream *in, std::ofstream *iniOut, genie::LangFile *dllOut, bool generateLangDll, std::map<int, std::string> *langReplacement);
	void makeDrs(std::ofstream *out);
	void uglyHudHack(boost::filesystem::path);
	void copyCivIntroSounds(boost::filesystem::path inputDir, boost::filesystem::path outputDir);
	void copyWallFiles(boost::filesystem::path inputDir);
	std::string tolower(std::string line);
	void createMusicPlaylist(std::string inputDir, std::string const outputDir);
	void transferHdDatElements(genie::DatFile *hdDat, genie::DatFile *aocDat);
	void patchArchitectures(genie::DatFile *aocDat);
	bool checkGraphics(genie::DatFile *aocDat, short graphicID, std::vector<int> checkedGraphics);
    void replaceGraphic(genie::DatFile *aocDat, short* graphicID, short compareID, short c, std::map<short,short>& replacedGraphics, std::map<int,int> slpIdConversion = std::map<int,int>());
    short duplicateGraphic(genie::DatFile *aocDat, std::vector<int> duplicatedGraphics, short graphicID, short compareID, short offset, bool manual = false, std::map<int,int> slpIdConversion = std::map<int,int>());
	void hotkeySetup();
    void symlinkSetup(boost::filesystem::path newDir, boost::filesystem::path xmlIn, boost::filesystem::path xmlOut, bool voobly, bool datalink, bool copyLanguage = true);
	bool copyData(QIODevice &inFile, QIODevice &outFile);
};

#endif // MAINWINDOW_H
