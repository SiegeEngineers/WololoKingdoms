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
	void changeLanguage(std::string);
	void copyHDMaps(boost::filesystem::path inputDir, boost::filesystem::path outputDir);
	void terrainSwap(genie::DatFile *hdDat, genie::DatFile *aocDat, int tNew, int tOld, int slpID);
	void recCopy(boost::filesystem::path const &src, boost::filesystem::path const &dst, bool skip = false, bool force = false);
	void listAssetFiles(boost::filesystem::path path, std::vector<std::string> *listOfSlpFiles, std::vector<std::string> *listOfWavFiles);
	void convertLanguageFile(std::ifstream *in, std::ofstream *iniOut, genie::LangFile *dllOut, bool generateLangDll, std::map<int, std::string> *langReplacement);
	void makeDrs(std::string const inputDir, std::string const moddedInputDir, std::ofstream *out);
	void uglyHudHack(std::string const inputDir, std::string const moddedDir);
	void copyCivIntroSounds(boost::filesystem::path inputDir, boost::filesystem::path outputDir);
	std::string tolower(std::string line);
	void createMusicPlaylist(std::string inputDir, std::string const outputDir);
	void transferHdDatElements(genie::DatFile *hdDat, genie::DatFile *aocDat);
	void patchArchitectures(genie::DatFile *aocDat, boost::filesystem::path HDPath, boost::filesystem::path moddedAssetsPath);
	bool checkGraphics(genie::DatFile *aocDat, short graphicID, std::vector<int> checkedGraphics);
	short duplicateGraphic(genie::DatFile *aocDat, std::vector<int> duplicatedGraphics, short graphicID, short compareID, short offset, boost::filesystem::path HDPath, boost::filesystem::path moddedAssetsPath);
	void hotkeySetup();
	bool copyData(QIODevice &inFile, QIODevice &outFile);
};

#endif // MAINWINDOW_H
