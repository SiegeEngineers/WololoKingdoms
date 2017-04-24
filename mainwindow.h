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
	void check(int index);

private:
	Ui::MainWindow *ui;
	void recCopy(boost::filesystem::path const &src, boost::filesystem::path const &dst);
	void listAssetFiles(std::string const path, std::vector<std::string> *listOfSlpFiles, std::vector<std::string> *listOfWavFiles);
	void convertLanguageFile(std::ifstream *in, std::ofstream *iniOut, genie::LangFile *dllOut, bool generateLangDll, std::map<int, std::string> *langReplacement);
	void makeDrs(std::string const inputDir ,std::ofstream *out);
	void uglyHudHack(std::string const inputDir);
	void cleanTheUglyHudHack(std::string const inputDir);
	void copyCivIntroSounds(std::string const inputDir, std::string const outputDir);
	std::string tolower(std::string line);
	void createMusicPlaylist(std::string inputDir, std::string const outputDir);
	void transferHdDatElements(genie::DatFile *hdDat, genie::DatFile *aocDat);
	void hotkeySetup();
};

#endif // MAINWINDOW_H
