#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <windows.h>
#include <ShellAPI.h>
#include <regex>

#include <chrono>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "genie/dat/DatFile.h"
#include "genie/lang/LangFile.h"
#include "paths.h"
#include "conversions.h"
#include "wololo/datPatch.h"
#include "wololo/Drs.h"
#include "fixes/berbersutfix.h"
#include "fixes/vietfix.h"
#include "fixes/demoshipfix.h"
#include "fixes/portuguesefix.h"
#include "fixes/malayfix.h"
#include "fixes/ethiopiansfreepikeupgradefix.h"
#include "fixes/maliansfreeminingupgradefix.h"
#include "fixes/ai900unitidfix.h"
#include "fixes/hotkeysfix.h"
#include "fixes/disablenonworkingunits.h"
#include "fixes/burmesefix.h"
#include "fixes/incafix.h"
#include "fixes/siegetowerfix.h"
#include "fixes/khmerfix.h"
#include "fixes/smallfixes.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include <QWhatsThis>
#include <QPoint>
#include <QProgressBar>
#include <QCryptographicHash>
#include "sdk/public/steam/steam_api.h"

#include "JlCompress.h"

namespace fs = boost::filesystem;

std::string steamPath;
fs::path HDPath;
fs::path outPath;
std::set<int> aocSlpFiles;
std::map<int, fs::path> slpFiles;
std::map<int, fs::path> wavFiles;
std::map<std::string,fs::path> newTerrainFiles;
std::vector<std::pair<int,std::string>> rmsCodeStrings;
std::string version = "2.6.";
std::string language;
std::map<std::string, std::string> translation;
bool secondAttempt = false;
QProgressBar* bar = NULL;
int dlcLevel = 0;
int patch = -1;
std::string modName;
std::ofstream logFile;

fs::path nfzUpOutPath;
fs::path nfzOutPath;
fs::path modHkiOutPath;
fs::path modHki2OutPath;
fs::path upHkiOutPath;
fs::path upHki2OutPath;
fs::path vooblyDir;
fs::path upDir;
std::string referenceDir = "WololoKingdoms FE";
fs::path resourceDir("resources/");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	language = "en";
	ui->setupUi(this);
    steamPath = getSteamPath();
	HDPath = getHDPath(steamPath);
	outPath = getOutPath(HDPath);

	vooblyDir = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms FE";
	upDir = outPath / "Games/WololoKingdoms FE";

	if(!fs::exists(vooblyDir)) {
		this->ui->usePatch->setDisabled(true);
	}

	nfzUpOutPath = upDir / "Player.nfz";
	nfzOutPath = vooblyDir / "Player.nfz";
	modHkiOutPath = vooblyDir / "player1.hki";
	modHki2OutPath = vooblyDir / "player2.hki";
	upHkiOutPath = upDir / "player1.hki";
	upHki2OutPath = upDir / "player2.hki";

    changeLanguage();

    QDialog* dialog;
    this->ui->label->setText(("WololoKingdoms version " + version).c_str());
    if(outPath == fs::path()) {
        this->ui->label->setText(translation["noAoC"].c_str());
        dialog = new Dialog(this,translation["noAoC"],translation["errorTitle"]);
        dialog->exec();
    }
    else if(HDPath == fs::path()) {
        this->ui->label->setText(translation["noSteamInstallation"].c_str());
        dialog = new Dialog(this,translation["noSteamInstallation"],translation["errorTitle"]);
        dialog->exec();
    }


    SteamAPI_Init();
    if(!SteamApps()) {
        // open steam
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &pi, sizeof(pi) );
        std::wstring wSteamPath = strtowstr(steamPath);
        CreateProcess( (wSteamPath + L"\\Steam.exe").c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
        SteamAPI_Init();
    }
    int tries = 0;
    while(!SteamApps()) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        SteamAPI_Init();
        tries++;
        if(tries>12)
            break;
    }
    if(!SteamApps()) {
		this->ui->label->setText(translation["noSteam"].c_str());
		dialog = new Dialog(this,translation["noSteam"],translation["errorTitle"]);
		dialog->exec();
	} else if(SteamApps()->BIsDlcInstalled(239550)) {
		if(SteamApps()->BIsDlcInstalled(355950)) {
			if(SteamApps()->BIsDlcInstalled(488060))
				dlcLevel = 3;
			else {
				dlcLevel = 2;
				dialog = new Dialog(this,translation["noRajas"]);
				dialog->exec();
			}
		} else {
			dlcLevel = 1;
			dialog = new Dialog(this,translation["noAK"]);
			dialog->exec();
		}
	} else {
		this->ui->label->setText(translation["noFE"].c_str());
		dialog = new Dialog(this,translation["noFE"],translation["errorTitle"]);
		dialog->exec();
	}
	SteamAPI_Shutdown();

    //Language selection dropdown
	QObject::connect( this->ui->languageChoice, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, [this]() {
		switch(this->ui->languageChoice->currentIndex()) {
			case 0: language = "br"; break;
			case 1: language = "de"; break;
			case 2: language = "en"; break;
			case 3: language = "es"; break;
			case 4: language = "fr"; break;
            case 5: language = "it"; break;
            case 6: language = "jp"; break;
            case 7: language = "ko"; break;
            case 8: language = "nl"; break;
            case 9: language = "ru"; break;
            case 10: language = "zh"; break;
			default: language = "en";
		}
        changeLanguage();
	} );

    //Patch selection dropdown.
	QObject::connect( this->ui->patchSelection, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, [this]() {
		changeModPatch();
	} );
	
	const char * questionIcon = "resources/question.png";
	//TODO do this in a loop

    //What's this for the hotkey selection dropdown
	this->ui->hotkeyTip->setIcon(QIcon(questionIcon));
	this->ui->hotkeyTip->setIconSize(QSize(16,16));
	this->ui->hotkeyTip->setWhatsThis(translation["hotkeyTip"].c_str());
	QObject::connect( this->ui->hotkeyTip, &QPushButton::clicked, this, [this]() {
			QWhatsThis::showText(this->ui->hotkeyTip->mapToGlobal(QPoint(0,0)),this->ui->hotkeyTip->whatsThis());
	} );
	if(fs::exists("player1.hki")) {
		this->ui->hotkeyChoice->setDisabled(true);
		this->ui->hotkeyChoice->setItemText(0,translation["customHotkeys"].c_str());
		this->ui->hotkeyTip->setDisabled(true);
	}

    //WhatsThis for the tooltip mod option
	this->ui->tooltipTip->setIcon(QIcon(questionIcon));
	this->ui->tooltipTip->setIconSize(QSize(16,16));
	this->ui->tooltipTip->setWhatsThis(translation["tooltipTip"].c_str());
	QObject::connect( this->ui->tooltipTip, &QPushButton::clicked, this, [this]() {
		QWhatsThis::showText(this->ui->tooltipTip->mapToGlobal(QPoint(0,0)),this->ui->tooltipTip->whatsThis());
	} );

    //WhatsThis for the patch selection option
	this->ui->patchSelectionTip->setIcon(QIcon(questionIcon));
	this->ui->patchSelectionTip->setIconSize(QSize(16,16));
	this->ui->patchSelectionTip->setWhatsThis(translation["patchSelectionTip"].c_str());
	QObject::connect( this->ui->patchSelectionTip, &QPushButton::clicked, this, [this]() {
		QWhatsThis::showText(this->ui->patchSelectionTip->mapToGlobal(QPoint(0,0)),this->ui->patchSelectionTip->whatsThis());
	} );

    //Checkbox en-/disabling the patch selection dropdown
	QObject::connect( this->ui->usePatch, &QCheckBox::clicked, this, [this]() {
		if(this->ui->usePatch->isChecked()) {
			this->ui->patchSelection->setDisabled(false);
			this->ui->hotkeyChoice->setDisabled(true);
		} else {
			this->ui->patchSelection->setDisabled(true);
			this->ui->hotkeyChoice->setDisabled(false);
		}
		changeModPatch();
	} );

    //What's this for the offline installation option
	this->ui->exeTip->setIcon(QIcon(questionIcon));
	this->ui->exeTip->setIconSize(QSize(16,16));
	std::string line = translation["exeTip"];
	boost::replace_all(line, "<folder>", outPath.string()+"\\age2_x1");
	this->ui->exeTip->setWhatsThis(line.c_str());
	QObject::connect( this->ui->exeTip, &QPushButton::clicked, this, [this]() {
		QWhatsThis::showText(this->ui->exeTip->mapToGlobal(QPoint(0,0)),this->ui->exeTip->whatsThis());
	} );

    //WhatsThis for the mods options (pussywood, small walls, grid)
	this->ui->modsTip->setIcon(QIcon(questionIcon));
	this->ui->modsTip->setIconSize(QSize(16,16));
	this->ui->modsTip->setWhatsThis(translation["modsTip"].c_str());
	QObject::connect( this->ui->modsTip, &QPushButton::clicked, this, [this]() {
			QWhatsThis::showText(this->ui->modsTip->mapToGlobal(QPoint(0,0)),this->ui->modsTip->whatsThis());
	} );

    //WhatsThis for the special maps option
	this->ui->mapsTip->setIcon(QIcon(questionIcon));
	this->ui->mapsTip->setIconSize(QSize(16,16));
	this->ui->mapsTip->setWhatsThis(translation["mapsTip"].c_str());
	QObject::connect( this->ui->mapsTip, &QPushButton::clicked, this, [this]() {
			QWhatsThis::showText(this->ui->mapsTip->mapToGlobal(QPoint(0,0)),this->ui->mapsTip->whatsThis());
	} );
	QObject::connect( this->ui->runButton, &QPushButton::clicked, this, &MainWindow::run);

    updateUI();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeModPatch() {
	modName = "WK ";
	patch = this->ui->usePatch->isChecked()?this->ui->patchSelection->currentIndex():-1;

    switch(patch) {
        case 0:     modName += "Balance Patch"; break;
        case 1: 	modName += dlcLevel == 3?"Patch 5.4":dlcLevel==2?"Patch 5.4 AK":"Patch 5.4 FE"; break;
	}
    if(patch == -1) {
		vooblyDir = vooblyDir.parent_path() / "WololoKingdoms FE";
		upDir = upDir.parent_path() / "WololoKingdoms FE";
	} else {
		vooblyDir = vooblyDir.parent_path() / modName;
		upDir = upDir.parent_path() / modName;
	}
	nfzUpOutPath = upDir / "Player.nfz";
	nfzOutPath = vooblyDir / "Player.nfz";
	modHkiOutPath = vooblyDir / "player1.hki";
	modHki2OutPath = vooblyDir / "player2.hki";
	upHkiOutPath = upDir / "player1.hki";
	upHki2OutPath = upDir / "player2.hki";
	updateUI();
}

void MainWindow::changeLanguage() {

	std::string line;
	std::ifstream translationFile("resources/"+language+".txt");
	while (std::getline(translationFile, line)) {
		boost::replace_all(line, "\\n", "\n");
		int index = line.find('=');
		translation[line.substr(0, index)] = line.substr(index+1, std::string::npos);
	}
    translationFile.close();
	this->ui->runButton->setText(translation["runButton"].c_str());
	this->ui->replaceTooltips->setText(translation["replaceTooltips"].c_str());
	this->ui->createExe->setText(translation["createExe"].c_str());
	this->ui->useGrid->setText(translation["useGrid"].c_str());
	this->ui->usePw->setText(translation["usePw"].c_str());
	this->ui->useWalls->setText(translation["useWalls"].c_str());
    this->ui->usePatch->setText(translation["usePatch"].c_str());
	this->ui->hotkeyChoice->setItemText(1,translation["hotkeys1"].c_str());
	this->ui->hotkeyChoice->setItemText(2,translation["hotkeys2"].c_str());
	this->ui->hotkeyChoice->setItemText(3,translation["hotkeys3"].c_str());
	this->ui->patchSelection->setItemText(0,translation["mod0"].c_str());
    this->ui->patchSelection->setItemText(1,translation["mod1"].c_str());
	updateUI();
}

void MainWindow::updateUI() {
    /*
     * First, we check if a hotkey file already exists. If it doesn't, the user has to select an option or won't be able to run
     * In either case, we also check if the user has the appropriate dlcLevel. That's >0 for regular WK and
     * 3 for any data mod based on WK (to avoid further fragmentation)
     */
	if(fs::exists(nfzOutPath)) {
		this->ui->hotkeyChoice->setItemText(0,translation["hotkeys0"].c_str());
        if((patch >= 0 && dlcLevel == 3) || (patch < 0 && dlcLevel != 0))
			this->ui->runButton->setDisabled(false);
	} else {
		this->ui->hotkeyChoice->setItemText(0,translation["hotkeyChoice"].c_str());
        if(!this->ui->usePatch->isChecked()) {
			this->ui->runButton->setDisabled(true);
            this->ui->hotkeyChoice->setStyleSheet("border-style: solid; border-width: 2px; border-color: red;");
        }
		QObject::connect( this->ui->hotkeyChoice, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, [this]{
            if (this->ui->hotkeyChoice->currentIndex() != 0 && ((patch >= 0 && dlcLevel == 3) || (patch < 0 && dlcLevel != 0))) {
				this->ui->runButton->setDisabled(false);
                this->ui->hotkeyChoice->setStyleSheet("");
            } else {
				this->ui->runButton->setDisabled(true);
                this->ui->hotkeyChoice->setStyleSheet("border-style: solid; border-width: 2px; border-color: red;");
            }
		} );
	}
    /*
     * Based on whether a <language>.ini file exists, we can offer
     * modded tooltips to be enabled
     */
    fs::path patchFolder;
    switch (patch) {
        case 0: patchFolder = resourceDir/"patches/Balance Mod/";
        case 1: patchFolder = resourceDir/"patches/5.4/";
        default: patchFolder = resourceDir;
    }

    if(!fs::exists(patchFolder/(language+".ini"))) {
		this->ui->replaceTooltips->setEnabled(false);
		this->ui->replaceTooltips->setChecked(false);
	} else {
		this->ui->replaceTooltips->setEnabled(true);
	}
	qApp->processEvents();
}

void MainWindow::recCopy(fs::path const &src, fs::path const &dst, bool skip, bool force) {
    // recursive copy
	if (fs::is_directory(src)) {
		if(!fs::exists(dst)) {
			fs::create_directories(dst);
		}
		for (fs::directory_iterator current(src), end;current != end; ++current) {
			fs::path currentPath(current->path());
			recCopy(currentPath, dst / currentPath.filename(), skip, force);
		}
	}
	else {
		if (skip) {
			boost::system::error_code ec;
			fs::copy_file(src, dst, ec);
		} else if (force)
			fs::copy_file(src,dst,fs::copy_option::overwrite_if_exists);
		else
			fs::copy_file(src, dst);
	}
}

void MainWindow::indexDrsFiles(fs::path const &src, bool expansionFiles) {
	// Index files to be written to the drs
	if (fs::is_directory(src)) {
		for (fs::directory_iterator current(src), end;current != end; ++current) {
			fs::path currentPath(current->path());
            indexDrsFiles(currentPath, expansionFiles);
		}
	}
	else {
		std::string extension = src.extension().string();
        if (extension == ".slp") {
			int id = atoi(src.stem().string().c_str());
            if (!expansionFiles)
                aocSlpFiles.insert(id);
            else
                slpFiles[id] = src;
		}
		else if (extension == ".wav") {
			int id = atoi(src.stem().string().c_str());
			wavFiles[id] = src;
		}
	}
}

void MainWindow::indexTerrainFiles(fs::path const &src) {
	// Index files to be written to the drs
	if (fs::is_directory(src)) {
		for (fs::directory_iterator current(src), end;current != end; ++current) {
			fs::path currentPath(current->path());
			indexTerrainFiles(currentPath);
		}
	}
	else {
		if (src.extension().string() == ".slp") {
			newTerrainFiles[src.filename().string()] = src;
		}
	}
}

void MainWindow::convertLanguageFile(std::ifstream *in, std::ofstream *iniOut, genie::LangFile *dllOut, bool generateLangDll, std::map<int, std::string> *langReplacement) {
	std::string line;
	while (std::getline(*in, line)) {
		int spaceIdx = line.find(' ');
		std::string number = line.substr(0, spaceIdx);
		int nb;
		try {
			nb = stoi(number);
			if (nb == 0xFFFF) {
				/*
				 * this one seems to be used by AOC for dynamically-generated strings
				 * (like market tributes), maybe it's the maximum the game can read ?
				*/
				continue;
			}
			if (nb <= 1000) {
				// skip changes to fonts
				continue;
			}
			if (nb >= 20150 && nb <= 20167) {
				// skip the old civ descriptions
				continue;
			}
			/*
			 * Conquerors AI names start at 5800 (5800 = 4660+1140, so offset 1140 in the xml file)
			 * However, there's only space for 10 civ AI names. We'll shift AI names to 11500+ instead (offset 6840 or 1140+5700)
			 */

			if (nb >= 5800 && nb < 6000) {
				nb += 5700;
				number = std::to_string(nb);
			}
			if (nb >= 106000 && nb < 106160) { //AK&AoR AI names have 10xxxx id, get rid of the 10, then shift
				nb -= 100000;
				nb += 5700;
				number = std::to_string(nb);
			}

			if (nb >= 120150 && nb <= 120180) { // descriptions of the civs in the expansion
				//These civ descriptions can be too long for the tech tree, we'll take out some newlines
				if (nb == 120156 || nb == 120155) {
					boost::replace_all(line, "civilization \\n\\n", "civilization \\n");
				}
				if (nb == 120167) {
					boost::replace_all(line, "civilization \\n\\n", "civilization \\n");
					boost::replace_all(line, "\\n\\n<b>Unique Tech", "\\n<b>Unique Tech");
				}
				// replace the old descriptions of the civs in the base game
				nb -= 100000;
				number = std::to_string(nb);
			}
		}
		catch (std::invalid_argument const & e){
			continue;
		}

		if (langReplacement->count(nb)) {
			// this string has been changed by one of our patches (modified attributes etc.)
			line = (*langReplacement)[nb];
			langReplacement->erase(nb);
		}
		else {
			// load the string from the HD edition file
			int firstQuoteIdx = spaceIdx;
			do {
				firstQuoteIdx++;
			} while (line[firstQuoteIdx] != '"');
			int secondQuoteIdx = firstQuoteIdx;
			do {
				secondQuoteIdx++;
			} while (line[secondQuoteIdx] != '"');
			line = line.substr(firstQuoteIdx + 1, secondQuoteIdx - firstQuoteIdx - 1);
		}

		//convert UTF-8 into ANSI

		std::wstring wideLine = strtowstr(line);
		std::string outputLine;
		//if(language!="zh")
			ConvertUnicode2CP(wideLine.c_str(), outputLine, CP_ACP);
		//else
		//	ConvertUnicode2CP(wideLine.c_str(), outputLine, 1386);


		*iniOut << number << '=' << outputLine <<  std::endl;

		if (generateLangDll) {
			boost::replace_all(outputLine, "·", "\xb7"); // Dll can't handle that character.
			boost::replace_all(outputLine, "\\n", "\n"); // the dll file requires actual line feed, not escape sequences
			try {
				dllOut->setString(nb, outputLine);
			}
			catch (std::string const & e) {
				boost::replace_all(outputLine, "\xb7", "-"); // non-english dll files don't seem to like that character
				boost::replace_all(outputLine, "\xae", "R");
				dllOut->setString(nb, outputLine);
			}
		}

	}
	/*
	 * Stuff that's in lang replacement but not in the HD files (in this case extended language height box)
	 */
	for(std::map<int,std::string>::iterator it = langReplacement->begin(); it != langReplacement->end(); it++) {
		//convert UTF-8 into ANSI

		std::wstring wideLine = strtowstr(it->second);
		std::string outputLine;
		//if(language!="zh")
			ConvertUnicode2CP(wideLine.c_str(), outputLine, CP_ACP);
		//else
		//	ConvertUnicode2CP(wideLine.c_str(), outputLine, 1386);


		*iniOut << std::to_string(it->first) << '=' << outputLine <<  std::endl;

		if (generateLangDll) {
			boost::replace_all(outputLine, "·", "\xb7"); // Dll can't handle that character.
			boost::replace_all(outputLine, "\\n", "\n"); // the dll file requires actual line feed, not escape sequences
			try {
				dllOut->setString(it->first, outputLine);
			}
			catch (std::string const & e) {
				boost::replace_all(outputLine, "\xb7", "-"); // non-english dll files don't seem to like that character
				boost::replace_all(outputLine, "\xae", "R");
				dllOut->setString(it->first, outputLine);
			}
		}

	}
	/*
	 * Strings needed for code generation that are not in the regular hd text file
	 * Only needed offline since regular aoc has this in the normal language dlls.
	 * Would possibly be fixed by a comp patch update.
	 */
	if (generateLangDll) {
		for(std::vector<std::pair<int,std::string>>::iterator it = rmsCodeStrings.begin(); it != rmsCodeStrings.end(); it++) {
			dllOut->setString(it->first, it->second);
		}
	}
    in->close();
    iniOut->close();
}

void MainWindow::makeDrs(std::ofstream *out) {

	this->ui->label->setText((translation["working"]+"\n"+translation["workingDrs"]).c_str());
	this->ui->label->repaint();

	// Exclude Forgotten Empires leftovers
	slpFiles.erase(50163); // Forgotten Empires loading screen
	slpFiles.erase(50189); // Forgotten Empires main menu
	slpFiles.erase(53207); // Forgotten Empires in-game logo
	slpFiles.erase(53208); // Forgotten Empires objective window
	slpFiles.erase(53209); // ???
	/*
	 * Some of the interface files are duplicates because of the shifting,
	 * get rid of those
	 */
	std::map<int,fs::path>::iterator start;
	std::map<int,fs::path>::iterator end;
	start = slpFiles.find(51132);
	end = std::next(start,8);
	slpFiles.erase(start,end);
	start = slpFiles.find(51172);
	end = std::next(start,8);
	slpFiles.erase(start,end);

	const int numberOfTables = 2; // slp and wav
	int numberOfSlpFiles = slpFiles.size();
	int numberOfWavFiles = wavFiles.size();
	int offsetOfFirstFile = sizeof (wololo::DrsHeader) +
			sizeof (wololo::DrsTableInfo) * numberOfTables +
			sizeof (wololo::DrsFileInfo) * (numberOfSlpFiles + numberOfWavFiles);
	int offset = offsetOfFirstFile;


	// file infos

	std::vector<wololo::DrsFileInfo> slpFileInfos;
	std::vector<wololo::DrsFileInfo> wavFileInfos;

	for (std::map<int,fs::path>::iterator it = slpFiles.begin(); it != slpFiles.end(); it++) {
		wololo::DrsFileInfo slp;
		size_t size;
		size = fs::file_size(it->second);
		slp.file_id = it->first;
		slp.file_data_offset = offset;
		slp.file_size = size;
		offset += size;
		slpFileInfos.push_back(slp);
	}
	bar->setValue(bar->value()+1);bar->repaint(); //60

	for (std::map<int,fs::path>::iterator it = wavFiles.begin(); it != wavFiles.end(); it++) {
		wololo::DrsFileInfo wav;
		size_t size;
		size = fs::file_size(it->second);
		wav.file_id = it->first;
		wav.file_data_offset = offset;
		wav.file_size = size;
		offset += size;
		wavFileInfos.push_back(wav);
	}
	bar->setValue(bar->value()+1);bar->repaint(); //61

	// header infos

	wololo::DrsHeader const header = {
		{ 'C', 'o', 'p', 'y', 'r',
		  'i', 'g', 'h', 't', ' ',
		  '(', 'c', ')', ' ', '1',
		  '9', '9', '7', ' ', 'E',
		  'n', 's', 'e', 'm', 'b',
		  'l', 'e', ' ', 'S', 't',
		  'u', 'd', 'i', 'o', 's',
		  '.', '\x1a' }, // copyright
		{ '1', '.', '0', '0' }, // version
		{ 't', 'r', 'i', 'b', 'e' }, // ftype
		numberOfTables, // table_count
		offsetOfFirstFile // file_offset
	};

	// table infos

	wololo::DrsTableInfo const slpTableInfo = {
		0x20, // file_type, MAGIC
		{ 'p', 'l', 's' }, // file_extension, "slp" in reverse
		sizeof (wololo::DrsHeader) + sizeof (wololo::DrsFileInfo) * numberOfTables, // file_info_offset
		(int) slpFileInfos.size() // num_files
	};
	wololo::DrsTableInfo const wavTableInfo = {
		0x20, // file_type, MAGIC
		{ 'v', 'a', 'w' }, // file_extension, "wav" in reverse
		(int) (sizeof (wololo::DrsHeader) +  sizeof (wololo::DrsFileInfo) * numberOfTables + sizeof (wololo::DrsFileInfo) * slpFileInfos.size()), // file_info_offset
		(int) wavFileInfos.size() // num_files
	};
	bar->setValue(bar->value()+1);bar->repaint(); //62


	this->ui->label->setText((translation["working"]+"\n"+translation["workingDrs2"]).c_str());
	this->ui->label->repaint();
	// now write the actual drs file

	// header
	out->write(header.copyright, sizeof (wololo::DrsHeader::copyright));
	out->write(header.version, sizeof (wololo::DrsHeader::version));
	out->write(header.ftype, sizeof (wololo::DrsHeader::ftype));
	out->write(reinterpret_cast<const char *>(&header.table_count), sizeof (wololo::DrsHeader::table_count));
	out->write(reinterpret_cast<const char *>(&header.file_offset), sizeof (wololo::DrsHeader::file_offset));

	// table infos
	out->write(reinterpret_cast<const char *>(&slpTableInfo.file_type), sizeof (wololo::DrsTableInfo::file_type));
	out->write(slpTableInfo.file_extension, sizeof (wololo::DrsTableInfo::file_extension));
	out->write(reinterpret_cast<const char *>(&slpTableInfo.file_info_offset), sizeof (wololo::DrsTableInfo::file_info_offset));
	out->write(reinterpret_cast<const char *>(&slpTableInfo.num_files), sizeof (wololo::DrsTableInfo::num_files));

	out->write(reinterpret_cast<const char *>(&wavTableInfo.file_type), sizeof (wololo::DrsTableInfo::file_type));
	out->write(wavTableInfo.file_extension, sizeof (wololo::DrsTableInfo::file_extension));
	out->write(reinterpret_cast<const char *>(&wavTableInfo.file_info_offset), sizeof (wololo::DrsTableInfo::file_info_offset));
	out->write(reinterpret_cast<const char *>(&wavTableInfo.num_files), sizeof (wololo::DrsTableInfo::num_files));

	bar->setValue(bar->value()+1);bar->repaint(); //63
	// file infos
	for (std::vector<wololo::DrsFileInfo>::iterator it = slpFileInfos.begin(); it != slpFileInfos.end(); it++) {
		out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
		out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
		out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));
	}
	bar->setValue(bar->value()+1);bar->repaint(); //64
	for (std::vector<wololo::DrsFileInfo>::iterator it = wavFileInfos.begin(); it != wavFileInfos.end(); it++) {
		out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
		out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
		out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));
	}
	bar->setValue(bar->value()+1);bar->repaint(); //65

	this->ui->label->setText((translation["working"]+"\n"+translation["workingDrs3"]).c_str());
	this->ui->label->repaint();
	// now write the actual files
	for (std::map<int,fs::path>::iterator it = slpFiles.begin(); it != slpFiles.end();it++) {
			std::ifstream srcStream = std::ifstream(it->second.string(), std::ios::binary);
			*out << srcStream.rdbuf();
            srcStream.close();
	}
	bar->setValue(bar->value()+1);bar->repaint(); //66

	for (std::map<int,fs::path>::iterator it = wavFiles.begin(); it != wavFiles.end(); it++) {
		std::ifstream srcStream = std::ifstream(it->second.string(), std::ios::binary);
		*out << srcStream.rdbuf();
        srcStream.close();
	}
	bar->setValue(bar->value()+1);bar->repaint(); //67
    out->close();
}

void MainWindow::copyCivIntroSounds(fs::path inputDir, fs::path outputDir) {
	std::string const civs[] = {"italians", "indians", "incas", "magyars", "slavs",
								"portuguese", "ethiopians", "malians", "berbers", "burmese", "malay", "vietnamese", "khmer"};
	for (size_t i = 0; i < sizeof civs / sizeof (std::string); i++) {
		boost::system::error_code ec;
		fs::copy_file(inputDir / (civs[i] + ".mp3"), outputDir / (civs[i] + ".mp3"), ec);
	}
}

void MainWindow::uglyHudHack(fs::path assetsPath) {
	/*
	 * Shifts the offset between interface files by 10 so there's space for the new civs
	 */
	int const hudFiles[] = {51130, 51160};
	for (size_t baseIndex = 0; baseIndex < sizeof hudFiles / sizeof (int); baseIndex++) {
		for (size_t i = 1; i <= 23; i++) {
			slpFiles[hudFiles[baseIndex]+i+(baseIndex+1)*10] = assetsPath / (std::to_string(hudFiles[baseIndex]+i) + ".slp");
		}
	}
	/*
	 * copies the Slav hud files for AK civs, the good way of doing this would be to extract
	 * the actual AK civs hud files from
	 * Age2HD\resources\_common\slp\game_b[24-27].slp correctly, but I haven't found a way yet
	 */
	int const slavHudFiles[] = {51123, 51153, 51183};
	for (size_t baseIndex = 0; baseIndex < sizeof slavHudFiles / sizeof (int); baseIndex++) {
		for (size_t i = 1; i <= 8; i++) {
			slpFiles[slavHudFiles[baseIndex]+i+baseIndex*10] = assetsPath / (std::to_string(slavHudFiles[baseIndex]) + ".slp");
		}
	}
}

void MainWindow::copyWallFiles(fs::path inputDir) {
	/*
	 * Base IDS 2098 Stone 2110 Fortified 4169 Damaged 4173 Damaged Fortified
	 * Central European +0, Asian +1, Middle Eastern +2, West European +3
	 * +8 per damage increase
	 *
	 * New Types:
	 * 5000 American, 7000 Mediterranean, 15000 Eastern European, 16000 Indian, 17000 African, 18000 South East Asian
	 * Base IDs 124 Stone 126 Fortified 145 Damaged 147 Damaged Fortified
	 * +4 per damage increase
	 */
	indexDrsFiles(inputDir);
	int conversionTable[] = {3,-15,2,0,3,-18,-5,1,0,1,2,3,0,1,2};
	int newBaseSLP = 24000;
	for(size_t i = 0; i < sizeof(conversionTable)/sizeof(int); i++) {
		int archID = conversionTable[i];
		if (archID < 0) {
			archID = -archID;
			int digits = archID == 5 || archID == 7?124:324;
			slpFiles[newBaseSLP+i*1000+digits+200] = inputDir/(std::to_string(archID*1000+digits)+".slp");
			slpFiles[newBaseSLP+i*1000+digits+202] = inputDir/(std::to_string(archID*1000+digits+2)+".slp");
			for (int j = 0; j <= 10; j+=2)
				slpFiles[newBaseSLP+i*1000+digits+221+j] = inputDir/(std::to_string(archID*1000+digits+21+j)+".slp");
		} else {
			slpFiles[newBaseSLP+i*1000+324] = inputDir/(std::to_string(2098+archID)+".slp");
			slpFiles[newBaseSLP+i*1000+326] = inputDir/(std::to_string(2110+archID)+".slp");
			for (int j = 0; j <= 20; j+=4)
				slpFiles[newBaseSLP+i*1000+345+j] = inputDir/(std::to_string(4169+archID+j)+".slp");
		}
	}
}

std::string MainWindow::tolower(std::string line) {
	std::transform(line.begin(), line.end(), line.begin(), static_cast<int(*)(int)>(std::tolower));
	return line;
}

void MainWindow::createMusicPlaylist(std::string inputDir, std::string const outputDir) {
	boost::replace_all(inputDir, "/", "\\");
	std::ofstream outputFile(outputDir);
	for (int i = 1; i <= 23; i++ ) {
		outputFile << inputDir << "xmusic" << std::to_string(i) << ".mp3" <<  std::endl;
	}
    outputFile.close();
}

void MainWindow::copyHDMaps(fs::path inputDir, fs::path outputDir, bool replace) {

	std::vector<fs::path> mapNames;
	for (fs::directory_iterator end, it(inputDir); it != end; it++) {
		std::string extension = it->path().extension().string();
		if (extension == ".rms") {
			mapNames.push_back(it->path());
		}
	}
	bar->setValue(bar->value()+1);bar->repaint(); //13+17
	bar->setValue(bar->value()+1);bar->repaint(); //14+18
	std::map<std::string,fs::path> terrainOverrides;
	std::vector<std::tuple<std::string,std::string,std::string,std::string,std::string,std::string,bool,std::string,std::string>> replacements = {
		//<Name,Regex Pattern if needed,replace name,terrain ID, replace terrain ID,slp to replace,upgrade trees?,tree to replace,new tree>
		std::make_tuple("DRAGONFOREST","DRAGONFORES(T?)","DRAGONFORES$1","48","21","15029.slp",true,"SNOWPINETREE","DRAGONTREE"),
		//std::make_tuple("ACACIA_FOREST","AC(C?)ACIA(_?)FORES(T?)","AC$1ACIA$2FORES$3","50","13","15010.slp",true,"PALMTREE","ACACIA_TREEE"),
		std::make_tuple("ACACIA_FOREST","AC(C?)ACIA(_?)FORES(T?)","AC$1ACIA$2FORES$3","50","41","",false,"",""),
		std::make_tuple("DLC_RAINFOREST","","DLC_RAINFOREST","56","10","15011.slp",true,"FOREST_TREE","DLC_RAINTREE"),
		std::make_tuple("BAOBAB","","BAOBAB","49","16","",false,"",""),
		std::make_tuple("DLC_MANGROVESHALLOW","","DLC_MANGROVESHALLOW","54","4","15014.slp",false,"",""),
		std::make_tuple("DLC_MANGROVEFOREST","","DLC_MANGROVEFOREST","55","20","",false,"",""),
		std::make_tuple("DLC_NEWSHALLOW","","DLC_NEWSHALLOW","59","26","15024.slp",false,"",""),
		std::make_tuple("SAVANNAH","","SAVANNAH","41","14","15010.slp",false,"",""),
		std::make_tuple("DIRT4","((DLC_)?)DIRT4","$1DIRT4","42","3","15007.slp",false,"",""),
		std::make_tuple("MOORLAND","","MOORLAND","44","9","15009.slp",false,"",""),
		std::make_tuple("CRACKEDIT","","CRACKEDIT","45","6","15000.slp",false,"",""),
		std::make_tuple("QUICKSAND","","QUICKSAND","46","40","15018.slp",false,"",""),
		std::make_tuple("BLACK","","BLACK","47","40","15018.slp",false,"",""),
		std::make_tuple("DLC_ROCK","","DLC_ROCK","40","40","15018.slp",false,"",""),
		std::make_tuple("DLC_BEACH2","","DLC_BEACH2","51","2","15017.slp",false,"",""),
		std::make_tuple("DLC_BEACH3","","DLC_BEACH3","52","2","15017.slp",false,"",""),
		std::make_tuple("DLC_BEACH4","","DLC_BEACH4","53","2","15017.slp",false,"",""),
		std::make_tuple("DLC_DRYROAD","","DLC_DRYROAD","43","25","15019.slp",false,"",""),
		std::make_tuple("DLC_WATER4","","DLC_WATER4","57","22","",false,"",""),
		std::make_tuple("DLC_WATER5","","DLC_WATER5","58","1","",false,"",""),
		std::make_tuple("DLC_DRYROAD","","DLC_DRYROAD","43","25","15019.slp",false,"",""),
		std::make_tuple("DLC_JUNGLELEAVES","","DLC_JUNGLELEAVES","62","11","15006.slp",false,"",""),
		std::make_tuple("DLC_JUNGLEROAD","","DLC_JUNGLEROAD","62","39","15031.slp",false,"",""),
		std::make_tuple("DLC_JUNGLEGRASS","","DLC_JUNGLEGRASS","61","12","15008.slp",false,"","")
	};
	for (std::vector<fs::path>::iterator it = mapNames.begin(); it != mapNames.end(); it++) {
		std::string mapName = it->stem().string()+".rms";
		if (mapName.substr(0,3) == "ZR@") {
			fs::copy_file(*it,outputDir/mapName,fs::copy_option::overwrite_if_exists);
			continue;
		}
        if(replace)
            fs::remove(outputDir/it->filename());
        else
            continue;
        fs::remove(outputDir/("ZR@"+it->filename().string()));
		std::ifstream input(inputDir.string()+it->filename().string());
		std::string str(static_cast<std::stringstream const&>(std::stringstream() << input.rdbuf()).str());
        input.close();
		if(str.find("DLC_MANGROVESHALLOW")!=std::string::npos) {
			if(str.find("<PLAYER_SETUP>")!=std::string::npos)
				str = std::regex_replace(str, std::regex("<PLAYER_SETUP>\\s*(\\r*)\\n"),
					"<PLAYER_SETUP>$1\n  terrain_state 0 0 0 1$1\n");
			else
				str = std::regex_replace(str, std::regex("#include_drs\\s+random_map\\.def\\s*(\\r*)\\n"),
					"#include_drs random_map.def$1\n<PLAYER_SETUP>$1\n  terrain_state 0 0 0 1$1\n");
		}
		for (std::vector<std::tuple<std::string,std::string,std::string,std::string,std::string,std::string,bool,std::string,std::string>>::iterator repIt = replacements.begin(); repIt != replacements.end(); repIt++) {
			std::regex terrainConstDef;
			std::regex terrainName;
			if(std::get<1>(*repIt)=="") {
				terrainConstDef = std::regex("#const\\s+" +std::get<0>(*repIt)+ "\\s+" +std::get<3>(*repIt));
				terrainName = std::regex(std::get<0>(*repIt));
			} else {
				terrainConstDef = std::regex("#const\\s+" +std::get<1>(*repIt)+ "\\s+" +std::get<3>(*repIt));
				terrainName = std::regex(std::get<1>(*repIt));
			}
			if(std::regex_search(str,terrainName)) {
				str = std::regex_replace(str,terrainConstDef, "#const "+std::get<2>(*repIt)+" "+std::get<4>(*repIt));
				if(std::get<5>(*repIt) != "") {
					terrainOverrides[std::get<5>(*repIt)] = newTerrainFiles[std::get<0>(*repIt)+".slp"];
					if(std::get<6>(*repIt)) {
						if(str.find("<PLAYER_SETUP>")!=std::string::npos)
							str = std::regex_replace(str, std::regex("<PLAYER_SETUP>\\s*(\\r*)\\n"),
								"<PLAYER_SETUP>$1\n  effect_amount GAIA_UPGRADE_UNIT "+std::get<7>(*repIt)+" "+std::get<8>(*repIt)+" 0$1\n");
						else
							str = std::regex_replace(str, std::regex("#include_drs\\s+random_map\\.def\\s*(\\r*)\\n"),
								"#include_drs random_map.def$1\n<PLAYER_SETUP>$1\n  effect_amount GAIA_UPGRADE_UNIT "+std::get<7>(*repIt)+" "+std::get<8>(*repIt)+" 0$1\n");
					}
				}
			}
		}
		if(str.find("DLC_MANGROVESHALLOW")!=std::string::npos) {
			terrainOverrides["15004.slp"] = newTerrainFiles["15004.slp"];
			terrainOverrides["15005.slp"] = newTerrainFiles["15005.slp"];
			terrainOverrides["15021.slp"] = newTerrainFiles["15021.slp"];
			terrainOverrides["15022.slp"] = newTerrainFiles["15022.slp"];
			terrainOverrides["15023.slp"] = newTerrainFiles["15023.slp"];
		}
		//str = regex_replace(str, std::regex("#const\\s+BAOBAB\\s+49"), "#const BAOBAB 16");
		std::ofstream out(outputDir.string()+"/"+mapName);
		out << str;
		out.close();
		if (mapName.substr(0,3) == "rw_" || mapName.substr(0,3) == "sm_") {
			std::string scenarioFile = it->stem().string()+".scx";
			terrainOverrides[scenarioFile] = fs::path(inputDir.string()+"/"+scenarioFile);
		}
		if (terrainOverrides.size() != 0) {
			QuaZip zip(QString((outputDir.string()+"/ZR@"+mapName).c_str()));
			zip.open(QuaZip::mdAdd, NULL);
			terrainOverrides[mapName] = fs::path(outputDir.string()+"/"+mapName);
			for(std::map<std::string,fs::path>::iterator files = terrainOverrides.begin(); files != terrainOverrides.end(); files++) {
				QuaZipFile outFile(&zip);
				QuaZipNewInfo fileInfo(QString(files->first.c_str()));;
				fileInfo.uncompressedSize = fs::file_size(files->second);
				outFile.open(QIODevice::WriteOnly,fileInfo,NULL,0,0,0,false);
				QFile inFile;
				inFile.setFileName(files->second.string().c_str());
				inFile.open(QIODevice::ReadOnly);
				copyData(inFile, outFile);
				outFile.close();
				inFile.close();
			}
			zip.close();
			fs::remove(fs::path(outputDir.string()+"/"+mapName));
		}
		terrainOverrides.clear();
	}
	bar->setValue(bar->value()+1);bar->repaint(); //15+19
}

void MainWindow::transferHdDatElements(genie::DatFile *hdDat, genie::DatFile *aocDat) {

	aocDat->Sounds = hdDat->Sounds;
	aocDat->GraphicPointers = hdDat->GraphicPointers;
	aocDat->Graphics = hdDat->Graphics;
	aocDat->Techages = hdDat->Techages;
	aocDat->UnitHeaders = hdDat->UnitHeaders;
	aocDat->Civs = hdDat->Civs;
	aocDat->Researchs = hdDat->Researchs;
	aocDat->UnitLines = hdDat->UnitLines;
	aocDat->TechTree = hdDat->TechTree;

	//Copy Terrains
	aocDat->TerrainBlock.TerrainsUsed2 = 42;
	aocDat->TerrainsUsed1 = 42;
	//terrainSwap(hdDat, aocDat, 41,54,15030); //mangrove terrain
	terrainSwap(hdDat, aocDat, 20,55,15012); //mangrove forest
	terrainSwap(hdDat, aocDat, 16,49,15025); //baobab forest
	terrainSwap(hdDat, aocDat, 41,50,15013); //acacia forest

	slpFiles[15012] = newTerrainFiles["DLC_MANGROVEFOREST.slp"];
	slpFiles[15013] = newTerrainFiles["ACACIA_FOREST.slp"];
	slpFiles[15025] = newTerrainFiles["BAOBAB.slp"];

	aocDat->TerrainBlock.Terrains[35].TerrainToDraw = -1;
	aocDat->TerrainBlock.Terrains[35].SLP = 15024;
	aocDat->TerrainBlock.Terrains[35].Name2 = "g_ice";

	//terrainSwap(hdDat, aocDat, 15,45,15000); //cracked sand
}

void MainWindow::patchArchitectures(genie::DatFile *aocDat) {

	short buildingIDs[] = {10, 14, 18, 19, 20, 30, 31, 32, 47, 49, 51, 63, 64, 67, 71, 78, 79, 80, 81, 82, 84, 85, 86, 87, 88,
						90, 91, 92, 95, 101, 103, 104, 105, 110, 116, 117, 129, 130, 131, 132, 133, 137, 141, 142, 150, 153,
						155, 179, 190, 209, 210, 234, 235, 236, 276, 463, 464, 465, 481, 482, 483, 484, 487, 488, 490, 491, 498,
						562, 563, 564, 565, 566, 584, 585, 586, 587, 597, 611, 612, 613, 614, 615, 616, 617, 659, 660, 661,
                        662, 663, 664, 665, 666, 667, 668, 669, 670, 671, 672, 673, 674, 1102, 1189};
	short unitIDs[] = {17, 21, 420, 442, 527, 528, 529, 532, 539, 545, 691, 1103, 1104};
    short civIDs[] = {13,23,7,17,14,31,21,6,11,12,27,1,4,18,9,8,16,24};
	short burmese = 30; //These are used for ID reference
    for(short c = 0; c < sizeof(civIDs)/sizeof(short); c++) {
		std::map<short,short> replacedGraphics;
        if(civIDs[c] == 24) {
            std::vector<int> duplicatedGraphics;
            short newGraphicID;
            short * graphicID = &aocDat->Civs[civIDs[c]].Units[81].StandingGraphic.first;
            newGraphicID = duplicateGraphic(aocDat, duplicatedGraphics, *graphicID,
                    aocDat->Civs[burmese].Units[81].StandingGraphic.first, c, true);
            replacedGraphics[*graphicID] = newGraphicID;
            *graphicID = newGraphicID;
            slpFiles[41179] = HDPath/("resources/_common/drs/gamedata_x2/6979.slp");
        }
		//buildings
		for(unsigned int b = 0; b < sizeof(buildingIDs)/sizeof(short); b++) {
			replaceGraphic(aocDat, &aocDat->Civs[civIDs[c]].Units[buildingIDs[b]].StandingGraphic.first,
					aocDat->Civs[burmese].Units[buildingIDs[b]].StandingGraphic.first, c, replacedGraphics);
			short oldGraphicID = aocDat->Civs[civIDs[c]].Units[buildingIDs[b]].Building.ConstructionGraphicID;
			if(oldGraphicID > 130 && oldGraphicID != 4248) { //exclude standard construction graphics for all civs
				replaceGraphic(aocDat, &aocDat->Civs[civIDs[c]].Units[buildingIDs[b]].Building.ConstructionGraphicID,
							   aocDat->Civs[burmese].Units[buildingIDs[b]].Building.ConstructionGraphicID, c, replacedGraphics);
			}
			std::vector<genie::unit::DamageGraphic>::iterator compIt = aocDat->Civs[burmese].Units[buildingIDs[b]].DamageGraphics.begin();
			for(std::vector<genie::unit::DamageGraphic>::iterator it = aocDat->Civs[civIDs[c]].Units[buildingIDs[b]].DamageGraphics.begin();
				it != aocDat->Civs[civIDs[c]].Units[buildingIDs[b]].DamageGraphics.end(); it++) {
				replaceGraphic(aocDat, &(it->GraphicID), compIt->GraphicID, c, replacedGraphics);
				compIt++;
			}
		}
		//units like ships
		for(unsigned int u = 0; u < sizeof(unitIDs)/sizeof(short); u++) {
			replaceGraphic(aocDat, &aocDat->Civs[civIDs[c]].Units[unitIDs[u]].StandingGraphic.first, aocDat->Civs[burmese].Units[unitIDs[u]].StandingGraphic.first, c, replacedGraphics);
			replaceGraphic(aocDat, &aocDat->Civs[civIDs[c]].Units[unitIDs[u]].DeadFish.WalkingGraphic.first, aocDat->Civs[burmese].Units[unitIDs[u]].DeadFish.WalkingGraphic.first, c, replacedGraphics);
			replaceGraphic(aocDat, &aocDat->Civs[civIDs[c]].Units[unitIDs[u]].Type50.AttackGraphic, aocDat->Civs[burmese].Units[unitIDs[u]].Type50.AttackGraphic, c, replacedGraphics);
		}

		bar->setValue(bar->value()+1);bar->repaint(); //37-52
	}
	//Let the Berber Mill have 40 frames instead of 8/10, which is close to the african mill with 38 frames
	aocDat->Graphics[aocDat->Civs[27].Units[129].StandingGraphic.first].FrameCount = 40;
	aocDat->Graphics[aocDat->Civs[27].Units[130].StandingGraphic.first].FrameCount = 40;
	aocDat->Graphics[aocDat->Graphics[aocDat->Civs[27].Units[129].StandingGraphic.first].Deltas[0].GraphicID].FrameCount = 40;
	aocDat->Graphics[aocDat->Graphics[aocDat->Civs[27].Units[130].StandingGraphic.first].Deltas[1].GraphicID].FrameCount = 40;

	//Fix the missionary converting frames while we're at it
	aocDat->Graphics[6616].FrameCount = 14;

	//Separate Monks and DA buildings into 4 major regions (Europe, Asian, Southern, American)
	std::vector<std::vector<short>> civGroups = { {5,6,12,18,28,29,30,31},
					{8,9,10,20,25,26,27},
					{15,16,21}};
    std::map<int,int> slpIdConversion = {{2683,0},{376,2},{4518,1},{2223,3},{3482,4},{3483,5},{4172,6},{4330,7},{889,10},{4612,16},{891,17},{4611,15},{3596,12},
							 {4610,14},{3594,11},{3595,13},{774,131},{779,134},{433,10},{768,130},{433,10},{771,132},{775,133},{3831,138},{3827,137}};
	short cgBuildingIDs[] = {12, 68, 70, 109, 598, 618, 619, 620};
	short cgUnitIDs[] = {125,134,286};
	for(int i = 0; i < 3; i++) {
		short monkHealingGraphic;
		if (i != 2) {
			int newSLP = 60000+10000*i+135;
			genie::Graphic newGraphic = aocDat->Graphics[1597];
			monkHealingGraphic = aocDat->Graphics.size();
			newGraphic.ID = monkHealingGraphic;
			newGraphic.SLP = newSLP;
			aocDat->Graphics.push_back(newGraphic);
			aocDat->GraphicPointers.push_back(1);
			slpFiles[newSLP] = HDPath/("resources/_common/drs/graphics/776.slp");
		} else {
			monkHealingGraphic = 7340; //meso healing graphic
		}
		for(unsigned int cg = 0; cg < civGroups[i].size(); cg++) {
			std::map<short,short> replacedGraphics;
			for(unsigned int b = 0; b < sizeof(cgBuildingIDs)/sizeof(short); b++) {
                replaceGraphic(aocDat, &aocDat->Civs[civGroups[i][cg]].Units[cgBuildingIDs[b]].StandingGraphic.first, -1, i, replacedGraphics, slpIdConversion);
				for(std::vector<genie::unit::DamageGraphic>::iterator it = aocDat->Civs[civGroups[i][cg]].Units[cgBuildingIDs[b]].DamageGraphics.begin();
					it != aocDat->Civs[civGroups[i][cg]].Units[cgBuildingIDs[b]].DamageGraphics.end(); it++) {
                    replaceGraphic(aocDat, &(it->GraphicID), -1, i, replacedGraphics, slpIdConversion);
				}
			}

			//units like ships
			for(unsigned int u = 0; u < sizeof(cgUnitIDs)/sizeof(short); u++) {
                replaceGraphic(aocDat, &aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].StandingGraphic.first, -1, i, replacedGraphics, slpIdConversion);
				if (aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].DeadFish.WalkingGraphic.first != -1)
                    replaceGraphic(aocDat, &aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].DeadFish.WalkingGraphic.first, -1, i, replacedGraphics, slpIdConversion);
				if (aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].Type50.AttackGraphic != -1)
                    replaceGraphic(aocDat, &aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].Type50.AttackGraphic, -1, i, replacedGraphics, slpIdConversion);
				if (aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].DyingGraphic.first != -1)
                    replaceGraphic(aocDat, &aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].DyingGraphic.first, -1, i, replacedGraphics, slpIdConversion);
			}
			//special UP healing slp workaround
			for(unsigned int cg = 0; cg < civGroups[i].size(); cg++) {
				size_t code = 0x811E0000+monkHealingGraphic;
				int ccode = (int) code;
				aocDat->Civs[civGroups[i][cg]].Units[125].LanguageDLLHelp = ccode;

				if (i == 0) {
					aocDat->Civs[civGroups[i][cg]].Units[125].IconID = 218;
					aocDat->Civs[civGroups[i][cg]].Units[286].IconID = 218;
				} else if (i == 1) {
					aocDat->Civs[civGroups[i][cg]].Units[125].IconID = 169;
					aocDat->Civs[civGroups[i][cg]].Units[286].IconID = 169;
				}

			}

		}
		bar->setValue(bar->value()+1);bar->repaint(); //52-55
	}


}

void MainWindow::replaceGraphic(genie::DatFile *aocDat, short* graphicID, short compareID, short c, std::map<short,short>& replacedGraphics, std::map<int,int> slpIdConversion) {
	if(replacedGraphics[*graphicID] != 0)
		*graphicID = replacedGraphics[*graphicID];
	else {
		std::vector<int> duplicatedGraphics;
		short newGraphicID;
		if (compareID != -1)
			newGraphicID = duplicateGraphic(aocDat, duplicatedGraphics, *graphicID, compareID, c);
		else
            newGraphicID = duplicateGraphic(aocDat, duplicatedGraphics, *graphicID, compareID, c, false, slpIdConversion);
		replacedGraphics[*graphicID] = newGraphicID;
		*graphicID = newGraphicID;
	}
}

bool MainWindow::checkGraphics(genie::DatFile *aocDat, short graphicID, std::vector<int> checkedGraphics) {
	//Tests if any of the referenced graphic SLPs are in the right range, AKA civ-dependant
	checkedGraphics.push_back(graphicID);
	genie::Graphic newGraphic = aocDat->Graphics[graphicID];
	if (aocDat->Graphics[graphicID].SLP < 18000 || aocDat->Graphics[graphicID].SLP >= 19000) {
		for(std::vector<genie::GraphicDelta>::iterator it = newGraphic.Deltas.begin(); it != newGraphic.Deltas.end(); it++) {
			if(it->GraphicID != -1 && std::find(checkedGraphics.begin(), checkedGraphics.end(), it->GraphicID) == checkedGraphics.end()
					&& checkGraphics(aocDat, it->GraphicID, checkedGraphics))
				return true;
		}
		return false;
	} else
		return true;
}

short MainWindow::duplicateGraphic(genie::DatFile *aocDat, std::vector<int> duplicatedGraphics, short graphicID, short compareID, short offset, bool manual, std::map<int,int> slpIdConversion) {

    /*
     * Parameters:
     * aocDatFile: The Dat File of WK to be patched
     * duplicatedGraphics: These Graphics have already been duplicated, and don't need to be duped again. Passed on to avoid circular loops with recursive calls for deltas
     * graphicID: The ID of the graphic to be duplicated
     * compareID: The ID of the same building of the burmese, to serve as a comparison. If -1, it's one of the monk/dark age graphics to be duped, see slpIdConversion parameter
     * offset: The offset of the civ/civ group for the new SLPs (24000+offset*1000 and so on)
     * slpIdConversion: To compare what id to give the newly duped slp files for monk/dark age graphics
     * manual: Graphics where the original is different from the usual format. Needs manual adjustments & slp copying
     */

    /*
     * A segfault is caused if this isn't checked. I don't exactly remember why 11 #shittyDocumentation
     */

	if (compareID != -1 && (aocDat->Graphics[compareID].SLP < 18000 || aocDat->Graphics[compareID].SLP >= 19000)) {
		std::vector<int> checkedGraphics;
		if(!checkGraphics(aocDat, compareID, checkedGraphics))
			return graphicID;
	}

    genie::Graphic newGraphic = manual?aocDat->Graphics[compareID]:aocDat->Graphics[graphicID];
	int newBaseSLP = compareID==-1?60000:24000;

	short newGraphicID = aocDat->Graphics.size();
	int newSLP;
	if(compareID==-1) { //Monk or Dark Age Graphics for the 4 big civ groups
        if (slpIdConversion[aocDat->Graphics[graphicID].SLP] == 0 && aocDat->Graphics[graphicID].SLP != 2683)
			newSLP = aocDat->Graphics[graphicID].SLP;
		else
            newSLP = newBaseSLP+10000*offset+slpIdConversion[aocDat->Graphics[graphicID].SLP];
    } else if (!(slpFiles.count(aocDat->Graphics[compareID].SLP)+aocSlpFiles.count(aocDat->Graphics[compareID].SLP))) //TODO this doesn't work for all cases
        newSLP = aocDat->Graphics[compareID].SLP;
    else
		newSLP = aocDat->Graphics[compareID].SLP - 18000 + newBaseSLP + 1000*offset;
    if(!manual && newSLP != aocDat->Graphics[graphicID].SLP && (compareID == -1 || newSLP != aocDat->Graphics[compareID].SLP)) {
		fs::path src = HDPath/("resources/_common/drs/gamedata_x2/"+std::to_string(newGraphic.SLP)+".slp");
		if(fs::exists(src))
			slpFiles[newSLP] = src;
		else {
			src = HDPath/("resources/_common/drs/graphics/"+std::to_string(newGraphic.SLP)+".slp");
			if(fs::exists(src))
				slpFiles[newSLP] = src;
		}
	}

	duplicatedGraphics.push_back(newGraphic.ID);
	newGraphic.ID = newGraphicID;
	newGraphic.SLP = newSLP;
	aocDat->Graphics.push_back(newGraphic);
	aocDat->GraphicPointers.push_back(1);

	if (compareID == -1) {
			for(std::vector<genie::GraphicDelta>::iterator it = newGraphic.Deltas.begin(); it != newGraphic.Deltas.end(); it++) {
				if(it->GraphicID != -1 && std::find(duplicatedGraphics.begin(), duplicatedGraphics.end(), it->GraphicID) == duplicatedGraphics.end())
                    it->GraphicID = duplicateGraphic(aocDat, duplicatedGraphics, it->GraphicID, -1, offset, false, slpIdConversion);
			}
			aocDat->Graphics.at(newGraphicID) = newGraphic;
    } else if(aocDat->Graphics[compareID].Deltas.size() == newGraphic.Deltas.size()) {
		/* don't copy graphics files if the amount of deltas is different to the comparison,
		 * this is usually with damage graphics and different amount of Flames.
		*/
		std::vector<genie::GraphicDelta>::iterator compIt = aocDat->Graphics[compareID].Deltas.begin();
		for(std::vector<genie::GraphicDelta>::iterator it = newGraphic.Deltas.begin(); it != newGraphic.Deltas.end(); it++) {
			if(it->GraphicID != -1 && std::find(duplicatedGraphics.begin(), duplicatedGraphics.end(), it->GraphicID) == duplicatedGraphics.end())
				it->GraphicID = duplicateGraphic(aocDat, duplicatedGraphics, it->GraphicID, compIt->GraphicID, offset);
			compIt++;
		}
		aocDat->Graphics.at(newGraphicID) = newGraphic;
    } else if(manual) {
        std::vector<genie::GraphicDelta>::iterator compIt = aocDat->Graphics[compareID].Deltas.begin();
        for(std::vector<genie::GraphicDelta>::iterator it = newGraphic.Deltas.begin(); it != newGraphic.Deltas.end(); it++) {
            if(it->GraphicID != -1 && std::find(duplicatedGraphics.begin(), duplicatedGraphics.end(), it->GraphicID) == duplicatedGraphics.end())
                it->GraphicID = duplicateGraphic(aocDat, duplicatedGraphics, it->GraphicID, compIt->GraphicID, offset);
            compIt++;
        }
        for(; compIt != aocDat->Graphics[compareID].Deltas.end(); compIt++) {
            genie::GraphicDelta newDelta = *compIt;
            newDelta.GraphicID = duplicateGraphic(aocDat, duplicatedGraphics, compIt->GraphicID, compIt->GraphicID, offset, true);
            newGraphic.Deltas.push_back(newDelta);
        }
        aocDat->Graphics.at(newGraphicID) = newGraphic;
    }
	return newGraphicID;
}

void MainWindow::terrainSwap(genie::DatFile *hdDat, genie::DatFile *aocDat, int tNew, int tOld, int slpID) {
	aocDat->TerrainBlock.Terrains[tNew] = hdDat->TerrainBlock.Terrains[tOld];
	aocDat->TerrainBlock.Terrains[tNew].SLP = slpID;
	if (tNew == 41) {
		for(size_t j = 0; j < aocDat->TerrainRestrictions.size(); j++) {
			aocDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier.push_back(hdDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier[tOld]);
			aocDat->TerrainRestrictions[j].TerrainPassGraphics.push_back(hdDat->TerrainRestrictions[j].TerrainPassGraphics[tOld]);
			if (j == 4)
				aocDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier[tNew] = 1.2;
		}
	} else {
		for(size_t j = 0; j < aocDat->TerrainRestrictions.size(); j++) {
			aocDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier[tNew] = hdDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier[tOld];
			aocDat->TerrainRestrictions[j].TerrainPassGraphics[tNew] = hdDat->TerrainRestrictions[j].TerrainPassGraphics[tOld];
		}
	}
}

void MainWindow::hotkeySetup() {

	fs::path nfz1Path = resourceDir / "Player1.nfz";
	fs::path nfzPath = outPath / "player.nfz";
	fs::path aocHkiPath = resourceDir / "player1.hki";
	fs::path customHkiPath("player1.hki");
	fs::path hkiPath = HDPath / ("Profiles/player0.hki");
	fs::path hkiOutPath = outPath / "player1.hki";
	fs::path hki2OutPath = outPath / "player2.hki";

	fs::remove(nfzOutPath);
	fs::remove(nfzUpOutPath);
	if(!fs::exists(hkiPath)) { //If player0.hki doesn't exist, look for player1.hki, otherwise use default HD hotkeys
		if(fs::exists(HDPath/"Profiles/player1.hki"))
				hkiPath = HDPath/"Profiles/player1.hki";
		else
				hkiPath = resourceDir / "player1_age2hd.hki";
	}

	if(fs::exists(nfzPath)) //Copy the Aoc Profile
		fs::copy_file(nfzPath, nfzOutPath);
	else //otherwise copy the default profile included
		fs::copy_file(nfz1Path, nfzOutPath);
	if(this->ui->createExe->isChecked()) { //Profiles for UP
		if(fs::exists(nfzPath)) //Copy the Aoc Profile
			fs::copy_file(nfzPath,nfzUpOutPath);
		else //otherwise copy the default profile included
			fs::copy_file(nfz1Path,nfzUpOutPath);
	}
	//Copy hotkey files
	if(fs::exists(customHkiPath)) { //players put a custom .hki file into the WK converter folder that they want to use instead
		fs::copy_file(customHkiPath, modHkiOutPath,fs::copy_option::overwrite_if_exists);
		if(fs::exists(hki2OutPath))
			fs::copy_file(customHkiPath, modHki2OutPath,fs::copy_option::overwrite_if_exists);
		if(this->ui->createExe->isChecked()) {
			fs::copy_file(customHkiPath, upHkiOutPath,fs::copy_option::overwrite_if_exists);
			if(fs::exists(hki2OutPath))
				fs::copy_file(customHkiPath, upHki2OutPath,fs::copy_option::overwrite_if_exists);
		}
		return;
	}
    if (this->ui->hotkeyChoice->currentIndex() == 1 && !fs::exists(hkiOutPath)) {
        fs::remove(modHkiOutPath);
        fs::remove(upHkiOutPath);
		fs::copy_file(aocHkiPath, hkiOutPath);//use voobly hotkeys, copy standard aoc hotkeys if the file doesn't exist yet
    }
	if (this->ui->hotkeyChoice->currentIndex() == 2) {
		fs::copy_file(hkiPath, modHkiOutPath,fs::copy_option::overwrite_if_exists);
		if(fs::exists(hki2OutPath))
			fs::copy_file(hkiPath, modHki2OutPath,fs::copy_option::overwrite_if_exists);
		if(this->ui->createExe->isChecked()) {
			fs::copy_file(hkiPath, upHkiOutPath,fs::copy_option::overwrite_if_exists);
			if(fs::exists(hki2OutPath))
				fs::copy_file(hkiPath, upHki2OutPath,fs::copy_option::overwrite_if_exists);
		}
	}
	if(this->ui->hotkeyChoice->currentIndex() == 3) {
		fs::path backup = hkiOutPath;
		backup+=".bak";
		if(fs::exists(hkiOutPath))
			fs::copy_file(hkiOutPath, backup,fs::copy_option::overwrite_if_exists);
		fs::copy_file(hkiPath, hkiOutPath,fs::copy_option::overwrite_if_exists);
		if(fs::exists(hki2OutPath)) {
			backup = hki2OutPath;
			backup+=".bak";
			fs::copy_file(hki2OutPath, backup,fs::copy_option::overwrite_if_exists);
			fs::copy_file(hkiPath, hki2OutPath,fs::copy_option::overwrite_if_exists);
		}
	}
}

void MainWindow::symlinkSetup(fs::path newDir, fs::path xmlIn, fs::path xmlOut, bool voobly, bool datalink, bool copyLanguage) {

	std::string newDirString = newDir.string()+"/";
	std::string oldDirString = voobly?(vooblyDir.parent_path() / referenceDir).string()+"/"
									:(upDir.parent_path() / referenceDir).string()+"/";
	std::string vooblyDirString = (vooblyDir.parent_path() / referenceDir).string()+"/";
	boost::replace_all(newDirString,"/","\\");
	boost::replace_all(oldDirString,"/","\\");
	boost::replace_all(vooblyDirString,"/","\\");

	fs::create_directory(newDir);
	fs::copy_file(xmlIn, xmlOut, fs::copy_option::overwrite_if_exists);

	if(datalink)
		fs::remove_all(newDir/"Data");
	else
		fs::create_directory(newDir/"Data");
	fs::remove_all(newDir/"Taunt");
	fs::remove_all(newDir/"Sound");
	fs::remove_all(newDir/"Script.Rm");
	fs::remove_all(newDir/"Script.Ai");
	fs::remove_all(newDir/"Screenshots");
	fs::remove_all(newDir/"Scenario");
    fs::remove(newDir/"Player.nfz");
	std::string datastring = datalink?"mklink /J \""+newDirString+"Data\" \""+ oldDirString+"Data\" & ":
                                      "mklink /H \""+newDirString+"Data\\gamedata_x1_p1.drs\" \""+ oldDirString+"Data\\gamedata_x1_p1.drs\" & ";
    std::string languageString = copyLanguage?"mklink /H \""+newDirString+"language.ini\" \""+ vooblyDirString+"language.ini\" & ":"";
    if(copyLanguage)
		fs::remove(newDir/"language.ini");
	std::string cmd = "/C mklink /J \""+newDirString+"Taunt\" \""+ vooblyDirString+"Taunt\" & "
			+ datastring +
			"mklink /J \""+newDirString+"Script.Rm\" \""+ vooblyDirString+"Script.Rm\" & "
			"mklink /J \""+newDirString+"Script.Ai\" \""+ vooblyDirString+"Script.Ai\" & "
			"mklink /J \""+newDirString+"Sound\" \""+ vooblyDirString+"Sound\" & "
			"mklink /J \""+newDirString+"Screenshots\" \""+ vooblyDirString+"Screenshots\" & "
			"mklink /J \""+newDirString+"Scenario\" \""+ vooblyDirString+"Scenario\" & "
            + languageString +
			"mklink /H \""+newDirString+"Player.nfz\" \""+ vooblyDirString+"Player.nfz\"";
    std::wstring wcmd = strtowstr(cmd);
	ShellExecute(NULL,L"open",L"cmd.exe",wcmd.c_str(),NULL,SW_HIDE);
	fs::create_directories(newDir/"Savegame/Multi");
    if(fs::exists(oldDirString+"player1.hki"))
        fs::copy_file(oldDirString+"player1.hki",newDir/"player1.hki",fs::copy_option::overwrite_if_exists);
    if(fs::exists(oldDirString+"player2.hki"))
        fs::copy_file(oldDirString+"player2.hki",newDir/"player2.hki",fs::copy_option::overwrite_if_exists);
	if(voobly)
		fs::copy_file(oldDirString+"version.ini", newDir/"version.ini", fs::copy_option::overwrite_if_exists);
}

int MainWindow::run()
{
	this->setEnabled(false);
	this->ui->label->setText(translation["working"].c_str());
	this->ui->label->repaint();
	if(bar == NULL) {
		bar = new QProgressBar();
		this->ui->verticalLayout->addWidget(bar);
	} else {
		bar->setValue(0);
		bar->repaint();
	}
	qApp->processEvents();

	QDialog* dialog;
	if (dlcLevel == 0) { //This should never happen
		this->ui->label->setText(translation["noSteam"].c_str());
		dialog = new Dialog(this,"You shouldn't be here! "+translation["noSteam"],translation["errorTitle"]);
		dialog->exec();
		return -1;
	}

	if(outPath == HDPath/"WololoKingdoms/out/") {
		this->ui->label->setText(translation["noAoC"].c_str());
		dialog = new Dialog(this,translation["noAoC"].c_str(),translation["errorTitle"]);
		dialog->exec();
		return -1;
	}

	int ret = 0;
	slpFiles.clear();
	wavFiles.clear();
	newTerrainFiles.clear();


	try {
		fs::path keyValuesStringsPath = HDPath / "resources/" / language / "/strings/key-value/key-value-strings-utf8.txt";
		std::string aocDatPath = HDPath.string() + "/resources/_common/dat/empires2_x1_p1.dat";
		std::string hdDatPath = HDPath.string() + "/resources/_common/dat/empires2_x2_p1.dat";
		fs::path languageIniPath = vooblyDir / "language.ini";
		std::string versionIniPath = vooblyDir.string() + "/version.ini";
		fs::path soundsInputPath = HDPath / "resources/_common/sound/";
		fs::path soundsOutputPath = vooblyDir / "Sound/";
		fs::path tauntInputPath = HDPath / "resources/en/sound/taunt/";
		fs::path tauntOutputPath = vooblyDir / "Taunt/";
		fs::path xmlPath = resourceDir/"WK1.xml";
		fs::path xmlOutPath = vooblyDir / "age2_x1.xml";
		fs::path langDllFile("language_x1_p1.dll");
		fs::path langDllPath = langDllFile;
		fs::path xmlOutPathUP = outPath / "Games/WKFE.xml";
		fs::path aiInputPath = resourceDir/"Script.Ai";
		std::string drsOutPath = vooblyDir.string() + "/Data/gamedata_x1_p1.drs";
		fs::path assetsPath = HDPath / "resources/_common/drs/gamedata_x2/";
        fs::path aocAssetsPath = HDPath / "resources/_common/drs/graphics/";
		fs::path outputDatPath = vooblyDir / "Data/empires2_x1_p1.dat";
        std::string UPModdedExe = dlcLevel==3?"WK":dlcLevel==2?"WKAK":"WKFE";
		fs::path UPExe = resourceDir/"SetupAoc.exe";
		fs::path UPExeOut = outPath / "SetupAoc.exe";
		fs::path pwInputDir = resourceDir/"pussywood";
		fs::path gridInputDir = resourceDir/"Grid";
		fs::path newTerrainInputDir = resourceDir/"new terrains";
		fs::path newGridTerrainInputDir = resourceDir/"new grid terrains";
		fs::path modOverrideDir("mod_override/");
		fs::path terrainOverrideDir("new_terrain_override/");
		fs::path wallsInputDir = resourceDir/"short_walls";
		fs::path gamedata_x1 = resourceDir/"gamedata_x1.drs";
		std::string modLangIni = resourceDir.string()+language+".ini";

        if(secondAttempt) {
            logFile << std::endl << std::endl << "Second Attempt";
            logFile << std::endl;
        } else {
            logFile = std::ofstream("log.txt");
            logFile << std::endl << "New Run";
            logFile << std::endl;
        }
        logFile << std::endl << "Steam Path: ";
        logFile << steamPath << std::endl << "HD Path:";
        logFile << HDPath.string() << std::endl << "AoC Path:";
        logFile << vooblyDir.string() << std::endl;

		std::string line;
        std::map<int, std::string> langReplacement;

		bar->setValue(1);bar->repaint(); //1

        logFile << "Patch mode: ";
        logFile << std::to_string(patch);
        logFile << std::endl << "DLC level: ";
        logFile << std::to_string(dlcLevel);
		if (patch < 0) { 

            logFile << std::endl << "Removing base folders";
            fs::remove_all(vooblyDir/"Data");
            fs::remove_all(vooblyDir/"Script.Ai/Brutal2");
            fs::remove(vooblyDir/"Script.Ai/BruteForce3.1.ai");
            fs::remove(vooblyDir/"Script.Ai/BruteForce3.1.per");
            fs::remove(vooblyDir/"age2_x1.xml");
            logFile << std::endl << "Removing language.ini";
            fs::remove(languageIniPath);
            fs::remove(versionIniPath);


            logFile << std::endl << "Creating base folders";
            fs::create_directories(vooblyDir/"SaveGame/Multi");
            fs::create_directories(vooblyDir/"Sound/stream");
            fs::create_directory(vooblyDir/"Data");
            fs::create_directory(vooblyDir/"Taunt");
            fs::create_directory(vooblyDir/"Screenshots");
            fs::create_directory(vooblyDir/"Scenario");

            if(this->ui->createExe->isChecked()) {
                logFile << std::endl << "Removing UP base folders";
                fs::remove_all(upDir/"Data");
                fs::remove_all(upDir/"Script.Ai/Brutal2");
                fs::remove(upDir/"Script.Ai/BruteForce3.1.ai");
                fs::remove(upDir/"Script.Ai/BruteForce3.1.per");
                fs::create_directories(upDir/"Data");
            }


            logFile << std::endl << "index DRS files";
            indexDrsFiles(assetsPath); //Slp/wav files to be written into gamedata_x1_p1.drs
            indexDrsFiles(aocAssetsPath, false); //Aoc slp files, just needed for comparison purposes

            logFile << std::endl << "Visual Mod Stuff";
			if(this->ui->usePw->isChecked() || this->ui->useGrid->isChecked() || this->ui->useWalls->isChecked()) {
				this->ui->label->setText((translation["working"]+"\n"+translation["workingMods"]).c_str());
				this->ui->label->repaint();
			}
			if(this->ui->usePw->isChecked())
				indexDrsFiles(pwInputDir);
			bar->setValue(bar->value()+1);bar->repaint(); //2
			if(this->ui->useGrid->isChecked()) {
				indexDrsFiles(gridInputDir);
				bar->setValue(bar->value()+1);bar->repaint(); //3
				indexTerrainFiles(newGridTerrainInputDir);
				bar->setValue(bar->value()+2);bar->repaint(); //5
			} else {
				indexTerrainFiles(newTerrainInputDir);
				bar->setValue(bar->value()+3);bar->repaint(); //5
			}
			if(!fs::is_empty(terrainOverrideDir)) {
				indexTerrainFiles(terrainOverrideDir);
			}
			bar->setValue(bar->value()+1);bar->repaint(); //6
			if(this->ui->useWalls->isChecked())
				copyWallFiles(wallsInputDir);
			bar->setValue(bar->value()+1);bar->repaint(); //7


			this->ui->label->setText((translation["working"]+"\n"+translation["workingFiles"]).c_str());
			this->ui->label->repaint();


            logFile << std::endl << "Civ Intro Sounds";
			copyCivIntroSounds(soundsInputPath / "civ/", soundsOutputPath / "stream/");
			bar->setValue(bar->value()+1);bar->repaint(); //9
            logFile << std::endl << "Create Music Playlist";
			createMusicPlaylist(soundsInputPath.string() + "music/", soundsOutputPath.string() + "music.m3u");
			bar->setValue(bar->value()+1);bar->repaint(); //10
            logFile << std::endl << "Copy Taunts";
			recCopy(tauntInputPath, tauntOutputPath, true);
			bar->setValue(bar->value()+1);bar->repaint(); //11
            logFile << std::endl << "Copy XML";
			fs::copy_file(xmlPath, xmlOutPath);
			fs::path vooblyMapDir = vooblyDir/"Script.Rm";
            logFile << std::endl << "Copy Voobly Map folder";
			if (fs::exists(outPath/"Random")) {
				recCopy(outPath/"Random", vooblyMapDir, true);
			} else {
				create_directory(vooblyMapDir);
			}
			bar->setValue(bar->value()+1);bar->repaint(); //12
            logFile << std::endl << "Copy HD Maps";
			copyHDMaps(HDPath/"resources/_common/random-map-scripts/", vooblyMapDir);
            bar->setValue(bar->value()+1);bar->repaint(); //15
            logFile << std::endl << "Copy Special Maps";
			if(this->ui->copyMaps->isChecked())
				copyHDMaps("resources/Script.Rm/", vooblyMapDir, true);
			else
				bar->setValue(bar->value()+3);
			bar->setValue(bar->value()+1);bar->repaint(); //19
			//If wanted, the BruteForce AI could be included as a "standard" AI.
            logFile << std::endl << "Copying AI";
			recCopy(aiInputPath, vooblyDir/"Script.Ai", true);
			bar->setValue(bar->value()+1);bar->repaint(); //20
			bar->setValue(bar->value()+1);bar->repaint(); //21
            logFile << std::endl << "Hotkey Setup";
			if(this->ui->hotkeyChoice->currentIndex() != 0 || fs::exists("player1.hki"))
                hotkeySetup();
            logFile << std::endl << "Offline installation symlink";
			if(this->ui->createExe->isChecked()) {
				symlinkSetup(upDir, xmlPath, xmlOutPathUP, false, false);
			}

            bar->setValue(bar->value()+1);bar->repaint(); //22

            logFile << std::endl << "Opening dats";
            this->ui->label->setText((translation["working"]+"\n"+translation["workingAoc"]).c_str());
            this->ui->label->repaint();

            genie::DatFile aocDat;
            aocDat.setGameVersion(genie::GameVersion::GV_TC);
            aocDat.load(aocDatPath.c_str());
            bar->setValue(bar->value()+5);bar->repaint(); //28

            this->ui->label->setText((translation["working"]+"\n"+translation["workingHD"]).c_str());
            this->ui->label->repaint();
            genie::DatFile hdDat;
            hdDat.setGameVersion(genie::GameVersion::GV_Cysion);
            hdDat.load(hdDatPath.c_str());
            bar->setValue(bar->value()+5);bar->repaint(); //33

            logFile << std::endl << "Opening DRS";
            std::ofstream drsOut(drsOutPath, std::ios::binary);

            this->ui->label->setText((translation["working"]+"\n"+translation["workingInterface"]).c_str());
            this->ui->label->repaint();

            logFile << std::endl << "HUD Hack";
            uglyHudHack(assetsPath);
            bar->setValue(bar->value()+1);bar->repaint(); //38

            this->ui->label->setText((translation["working"]+"\n"+translation["workingDat"]).c_str());
            this->ui->label->repaint();
            logFile << std::endl << "Transfer HD Dat elements";
            transferHdDatElements(&hdDat, &aocDat);
            bar->setValue(bar->value()+1);bar->repaint(); //39

            logFile << std::endl << "Patch Architectures";
            patchArchitectures(&aocDat);
            bar->setValue(bar->value()+1);bar->repaint(); //54

            logFile << std::endl << "Mod Override Dir";
            if(!fs::is_empty(modOverrideDir))
                indexDrsFiles(modOverrideDir);
            bar->setValue(bar->value()+1);bar->repaint(); //55
            logFile << std::endl << "Make DRS";
            makeDrs(&drsOut);
            bar->setValue(bar->value()+1);bar->repaint(); //66
            bar->setValue(bar->value()+1);bar->repaint(); //67

            wololo::DatPatch patchTab[] = {

                wololo::berbersUTFix,
                wololo::vietFix,
                wololo::malayFix,
                wololo::ethiopiansFreePikeUpgradeFix,
                wololo::hotkeysFix,
                wololo::maliansFreeMiningUpgradeFix,
                wololo::portugueseFix,
                wololo::disableNonWorkingUnits,
                wololo::burmeseFix,
                wololo::incaFix,
                wololo::siegeTowerFix,
                wololo::khmerFix,
                wololo::smallFixes,
                wololo::ai900UnitIdFix
            };

            this->ui->label->setText((translation["working"]+"\n"+translation["workingPatches"]).c_str());
            this->ui->label->repaint();

            logFile << std::endl << "DAT Patches";

            for (size_t i = 0, nbPatches = sizeof patchTab / sizeof (wololo::DatPatch); i < nbPatches; i++) {
                patchTab[i].patch(&aocDat, &langReplacement);
                bar->setValue(bar->value()+1);bar->repaint(); //68-81
            }

            logFile << std::endl << "Save DAT";
            aocDat.saveAs(outputDatPath.string().c_str());

        } else { //If we use a balance mod or old patch, just copy the supplied dat file
            switch (patch) {            
                case 0: {
                    fs::path oldPatchFolder = resourceDir/"patches/Balance Patch/";
                    hdDatPath = oldPatchFolder.string()+"empires2_x1_p1.dat";
                    keyValuesStringsPath = oldPatchFolder / (language+".txt");
                    modLangIni = oldPatchFolder.string()+language+".ini";
                    UPModdedExe = "WKBP";
                } break;                
                case 1: {
                    fs::path oldPatchFolder = resourceDir/"patches/5.4/";
                    hdDatPath = oldPatchFolder.string()+"empires2_x1_p1.dat";
                    keyValuesStringsPath = oldPatchFolder / (language+".txt");
                    modLangIni = oldPatchFolder.string()+language+".ini";
                } break;
                default: //A patch in the list with an unknown index was selected
                    dialog = new Dialog(this,translation["dialogUnknownPatch"].c_str());
                    dialog->exec();
                    return -1;
            }
            logFile << std::endl << "Copy DAT file";
            fs::create_directories(outputDatPath.parent_path());
            fs::copy_file(hdDatPath,outputDatPath,fs::copy_option::overwrite_if_exists);
            bar->setValue(81);
        }


        logFile << std::endl << "copy gamedata_x1.drs";
        fs::copy_file(gamedata_x1, vooblyDir/"Data/gamedata_x1.drs", fs::copy_option::overwrite_if_exists);
        bar->setValue(bar->value()+1);bar->repaint(); //23

        /*
         * Generate version.ini based on the installer and the hash of the dat.
         */
        if (patch < 0) {
            logFile << std::endl << "Create Hash";
            QFile file(outputDatPath.string().c_str());

            if (file.open(QIODevice::ReadOnly))
            {
                QByteArray fileData = file.readAll();

                QByteArray hashData = QCryptographicHash::hash(fileData,QCryptographicHash::Md5);
                std::ofstream versionOut(versionIniPath);
                std::string hash = hashData.toBase64().toStdString().substr(0,6);
                (versionOut << version) << hash << std::endl;
                versionOut.close();
                if (hash != "vQPW5e") {
                    dialog = new Dialog(this,translation["dialogBeta"].c_str());
                    dialog->exec();
                }
            }
        } else {
            std::ofstream versionOut(versionIniPath);
            switch (patch) {
                case 0: version = "1.0"; break;
                case 1: version = "5.4"; break;
                default: version = "ERR_PATCH";
            }
            (versionOut << version) << std::endl;
            versionOut.close();
        }



		/*
		 * Create the language files (.ini for Voobly, .dll for offline)
		 */

        //terrain descriptions for new terrains in scenario editor
        langReplacement[10626] = translation["10626"];
		langReplacement[10619] = translation["10619"];
        langReplacement[10679] = translation["10679"];
        //relics victory condition
		langReplacement[30195] = translation["30195"];
        if(fs::exists("resources/"+language+".txt")) {
            //Fix mistakes in old terrain descriptions in scenario editor
            langReplacement[10622] = translation["10622"];
            langReplacement[10642] = translation["10642"];
            langReplacement[10648] = translation["10648"];
            langReplacement[10618] = translation["10618"];
            langReplacement[10707] = translation["10707"];
            //Typo
            langReplacement[10716] = translation["10716"];
            //Fix errors in civ descriptions
            langReplacement[20162] = translation["20162"];
            langReplacement[20166] = translation["20166"];
            langReplacement[20170] = translation["20170"];
            langReplacement[20165] = translation["20165"];
            langReplacement[20158] = translation["20158"];
            langReplacement[20163] = translation["20163"];
            //Add that the Genitour and Imperial Skirmishers are Mercenary Units, since there is no other visual difference in the tech tree
            langReplacement[26137] = translation["26137"];
            langReplacement[26139] = translation["26139"];
            langReplacement[26190] = translation["26190"];
            langReplacement[26419] = translation["26419"];
        }


        logFile << std::endl << "Open Missing strings";
		std::ifstream missingStrings(resourceDir.string()+"missing_strings.txt");
		while (std::getline(missingStrings, line)) {
			int spaceIdx = line.find('=');
			std::string number = line.substr(0, spaceIdx);
			int nb;
			try {
				nb = stoi(number);
			}
			catch (std::invalid_argument const & e){
				continue;
			}
			line = line.substr(spaceIdx + 1, std::string::npos);
			rmsCodeStrings.push_back(std::make_pair(nb,line));
		}
        missingStrings.close();


        logFile << std::endl << "Replace tooltips";
		if(this->ui->replaceTooltips->isChecked()) {
			/*
			 * Load modded strings instead of normal HD strings into lang replacement
			 */
			std::ifstream modLang(modLangIni);
			while (std::getline(modLang, line)) {
				int spaceIdx = line.find('=');
				std::string number = line.substr(0, spaceIdx);
				int nb;
				try {
					nb = stoi(number);
				}
				catch (std::invalid_argument const & e){
					continue;
				}
				line = line.substr(spaceIdx + 1, std::string::npos);

				std::wstring outputLine;
				ConvertCP2Unicode(line.c_str(), outputLine, CP_ACP);
				line = wstrtostr(outputLine);
				langReplacement[nb] = line;
			}
            modLang.close();
		}
		bar->setValue(bar->value()+1);bar->repaint(); //82


		std::ifstream langIn(keyValuesStringsPath.string());
		std::ofstream langOut(languageIniPath.string());
		genie::LangFile langDll;

		bool patchLangDll;
		if(this->ui->createExe->isChecked()) {
			langDllPath = outPath / langDllPath;
			patchLangDll = fs::exists(langDllPath);
			if(patchLangDll)
            {
                fs::remove(langDllFile);
				fs::copy_file(langDllPath,langDllFile);
			}
		} else {
			patchLangDll = false;
		}
		bar->setValue(bar->value()+1);bar->repaint(); //83
		bool dllPatched = true;

        logFile << std::endl << "Open Lang Dll";
		if (patchLangDll) {
			try {
				langDll.load((langDllFile.string()).c_str());
				langDll.setGameVersion(genie::GameVersion::GV_TC);
			} catch (const std::ifstream::failure& e) {
				//Try deleting and re-copying
				fs::remove(langDllFile);
				fs::copy_file(langDllPath,langDllFile);
				try {
					langDll.load((langDllFile.string()).c_str());
					langDll.setGameVersion(genie::GameVersion::GV_TC);
				} catch (const std::ifstream::failure& e) {
					fs::remove(langDllFile);
					if(!secondAttempt) {
                        secondAttempt = true;
						ret = run();
						return ret;
					}
					dllPatched = false;
					patchLangDll = false;
				}
			}
		}
		else {
			if(this->ui->createExe->isChecked()) {
				line = translation["working"]+"\n"+translation["workingNoDll"];
				boost::replace_all(line,"<dll>",langDllPath.string());
				this->ui->label->setText(line.c_str());
				this->ui->label->repaint();
			}
		}
		bar->setValue(bar->value()+1);bar->repaint(); //84

        logFile << std::endl << "convert language file";
		convertLanguageFile(&langIn, &langOut, &langDll, patchLangDll, &langReplacement);
		bar->setValue(bar->value()+1);bar->repaint(); //85

        logFile << std::endl << "save lang dll file";
		if (patchLangDll) {
            fs::create_directories(upDir/"data/");
            fs::path langDllOutput = upDir/"data/"/langDllFile;
			try {
				line = translation["working"]+"\n"+translation["workingDll"];
				boost::replace_all(line,"<dll>",langDllFile.string());
				langDll.save();
                fs::copy_file(langDllFile,langDllOutput,fs::copy_option::overwrite_if_exists);
				fs::remove(langDllFile);
				this->ui->label->setText(line.c_str());
				this->ui->label->repaint();
			} catch (const std::ofstream::failure& e) {
				this->ui->label->setText(translation["workingError"].c_str());
				this->ui->label->repaint();
                fs::remove(langDllFile);
                fs::remove(langDllOutput);
				try {
					langDll.save();
                    fs::copy_file(langDllFile,langDllOutput,fs::copy_option::overwrite_if_exists);
					fs::remove(langDllFile);
					this->ui->label->setText(line.c_str());
					this->ui->label->repaint();
				} catch (const std::ofstream::failure& e) {
                    fs::remove(langDllFile);
                    fs::remove(langDllOutput);
					if(!secondAttempt) {
                        secondAttempt = true;
						ret = run();
						return ret;
					}
					dllPatched = false;
					patchLangDll = false;

				}
			}
		}

		bar->setValue(bar->value()+1);bar->repaint(); //86


		bar->setValue(bar->value()+1);bar->repaint(); //87

		/*
		 * If a user has access to more than just FE, also generate those versions
		 * For an old patch, we'll just use the highest one
		 * For a data mod, not sure if we should generate all versions or the highest one
		 */

		if (patch >= 0) {

            logFile << std::endl << "Patch setup";
			fs::path xmlIn = resourceDir/"WKtemp.xml";
			std::ifstream input(resourceDir.string()+("WK"+std::to_string(dlcLevel)+".xml"));
			std::string str(static_cast<std::stringstream const&>(std::stringstream() << input.rdbuf()).str());
			boost::replace_all(str,dlcLevel==3?"WololoKingdoms":dlcLevel==2?"WololoKingdoms AK":"WololoKingdoms FE",modName);
			std::ofstream out(xmlIn.string());
			out << str;
            input.close();
			out.close();
            symlinkSetup(vooblyDir,xmlIn,vooblyDir/"age2_x1.xml",true,false,false);
            if(this->ui->createExe->isChecked()) {
                symlinkSetup(upDir, xmlIn, upDir.parent_path()/(UPModdedExe+".xml"), false, false, false);
            }
            fs::remove(resourceDir/"WKtemp.xml");
		} else {
			if (dlcLevel > 1) {

                logFile << std::endl << "AK Setup";
				fs::path xmlIn = resourceDir/"WK2.xml";
				fs::path vooblyDir2 = vooblyDir.parent_path() / "WololoKingdoms AK";
				fs::path upDir2 = upDir.parent_path() / "WololoKingdoms AK";
				symlinkSetup(vooblyDir2, xmlIn, vooblyDir2/"age2_x1.xml", true, true);
				if(this->ui->createExe->isChecked()) {
					symlinkSetup(upDir2, xmlIn, upDir.parent_path()/"WKAK.xml", false, true);
				}
			}
			if (dlcLevel > 2) {
                logFile << std::endl << "RotR Setup";
				fs::path xmlIn = resourceDir/"WK3.xml";
				fs::path vooblyDir3 = vooblyDir.parent_path() / "WololoKingdoms";
				fs::path upDir3 = upDir.parent_path() / "WololoKingdoms";
				symlinkSetup(vooblyDir3, xmlIn, vooblyDir3/"age2_x1.xml", true, true);
				if(this->ui->createExe->isChecked()) {
					symlinkSetup(upDir3, xmlIn, upDir.parent_path()/"WK.xml",  false, true);
				}
            } else {
                //Possible earlier 2.3 installation to be removed
                fs::remove_all(vooblyDir.parent_path() / "WololoKingdoms");
                fs::remove_all(upDir.parent_path() / "WololoKingdoms");
            }
        }

        /*
         * Copy the data folder from the Voobly folder and
         * create the offline exe
         */

        logFile << std::endl << "Create Offline Exe";
        if(this->ui->createExe->isChecked()) {
            this->ui->label->setText((translation["working"]+"\n"+translation["workingUp"]).c_str());
            this->ui->label->repaint();

            recCopy(vooblyDir / "Data", upDir / "Data", true);

            bar->setValue(bar->value()+1);bar->repaint(); //88
            if (!dllPatched) {
                dialog = new Dialog(this, translation["dialogNoDll"].c_str());
                dialog->exec();
            } else {
                fs::copy_file(UPExe, UPExeOut, fs::copy_option::overwrite_if_exists);

                bar->setValue(bar->value()+1);bar->repaint(); //89

                STARTUPINFO si;
                PROCESS_INFORMATION pi;
                ZeroMemory( &si, sizeof(si) );
                si.cb = sizeof(si);
                ZeroMemory( &pi, sizeof(pi) );
                std::string UPExeString = "\""+UPExeOut.string()+"\" -g:"+UPModdedExe;
                std::wstring wExeString = strtowstr(UPExeString);
                wchar_t cmdLineString[wExeString.length()+1];
                wcscpy(cmdLineString, wExeString.c_str());
                CreateProcess( NULL, cmdLineString, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );
                CloseHandle( pi.hProcess );
                CloseHandle( pi.hThread );

                bar->setValue(bar->value()+1);bar->repaint(); //90
                line = translation["dialogExe"];
                boost::replace_all(line,"<exe>",UPModdedExe);
                dialog = new Dialog(this,line.c_str());
                dialog->exec();
            }
        } else {
            dialog = new Dialog(this,translation["dialogDone"].c_str());
            dialog->exec();
        }
        this->ui->label->setText(translation["workingDone"].c_str());

        if (patch < 0) {

            logFile << std::endl << "Fix Compat Patch";
			/*
			 * Several small fixes for the compatibility patch. This only needs to be run once
			 * An update to the compatibility patch would make this unnecessary most likely.
			 */

            fs::remove_all(outPath/"/compatslp");

            fs::create_directory(outPath/"data/Load");
			if(outPath==HDPath && this->ui->createExe->isChecked()) { //this causes a crash with UP 1.5 otherwise
				this->ui->label->setText(translation["workingCP"].c_str());
				this->ui->label->repaint();
				if(fs::file_size(outPath/"/data/blendomatic.dat") < 400000) {
					fs::rename(outPath/"/data/blendomatic.dat",outPath/"/data/blendomatic.dat.bak");
					fs::rename(outPath/"/data/blendomatic_x1.dat",outPath/"/data/blendomatic.dat");
				}
				bar->setValue(bar->value()+1);bar->repaint();
			}
		}


		bar->setValue(100);
		bar->repaint();
        logFile.close();
	}
	catch (std::exception const & e) {
		dialog = new Dialog(this,translation["dialogException"]+std::string()+e.what(),translation["errorTitle"]);
		dialog->exec();
        logFile << std::endl << e.what();
		this->ui->label->setText(translation["error"].c_str());
		ret = 1;
        logFile.close();
	}
	catch (std::string const & e) {		
		dialog = new Dialog(this,translation["dialogException"]+e,translation["errorTitle"]);
		dialog->exec();
        logFile << std::endl << e;
		this->ui->label->setText(translation["error"].c_str());
		ret = 1;
        logFile.close();
	}

	this->setEnabled(true);
	this->ui->label->repaint();
	return ret;
}

bool MainWindow::copyData(QIODevice &inFile, QIODevice &outFile)
{
	while (!inFile.atEnd()) {
		char buf[4096];
		qint64 readLen = inFile.read(buf, 4096);
		if (readLen <= 0)
			return false;
		if (outFile.write(buf, readLen) != readLen)
			return false;
	}
	return true;
}


