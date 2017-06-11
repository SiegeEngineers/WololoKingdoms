#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <windows.h>
#include <regex>

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
#include "fixes/feitoriafix.h"
#include "fixes/burmesefix.h"
#include "fixes/incafix.h"
#include "fixes/smallfixes.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include <QWhatsThis>
#include <QPoint>
#include <QProgressBar>

#include "JlCompress.h"

namespace fs = boost::filesystem;

fs::path HDPath;
fs::path outPath;
fs::path moddedAssetsPath("assets/");
fs::path tempMapDir("map_temp/");
std::string const version = "2.1";
std::string language;
std::map<std::string, std::string> translation;
bool secondAttempt = false;
QProgressBar* bar = NULL;

fs::path nfzUpOutPath;
fs::path nfzOutPath;
fs::path modHkiOutPath;
fs::path modHki2OutPath;
fs::path upHkiOutPath;
fs::path upHki2OutPath;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	language = "en";
	ui->setupUi(this);
	HDPath = getHDPath();
	outPath = getOutPath(HDPath);
	changeLanguage(language);

	nfzUpOutPath = outPath / "Games/WololoKingdoms/Player.nfz";
	nfzOutPath = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms/Player.nfz";
	modHkiOutPath = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms/player1.hki";
	modHki2OutPath = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms/player2.hki";
	upHkiOutPath = outPath / "Games/WololoKingdoms/player1.hki";
	upHki2OutPath = outPath / "Games/WololoKingdoms/player2.hki";

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
		changeLanguage(language);
		if(this->ui->languageChoice->currentIndex() != 2) {
			this->ui->replaceTooltips->setEnabled(false);
			this->ui->replaceTooltips->setChecked(false);
		} else {
			this->ui->replaceTooltips->setEnabled(true);
		}
	} );
	
	//TODO do this in a loop
	this->ui->hotkeyTip->setIcon(QIcon("resources/question.png"));
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

	this->ui->tooltipTip->setIcon(QIcon("resources/question.png"));
	this->ui->tooltipTip->setIconSize(QSize(16,16));
	this->ui->tooltipTip->setWhatsThis(translation["tooltipTip"].c_str());
	QObject::connect( this->ui->tooltipTip, &QPushButton::clicked, this, [this]() {
		QWhatsThis::showText(this->ui->tooltipTip->mapToGlobal(QPoint(0,0)),this->ui->tooltipTip->whatsThis());
	} );

	this->ui->exeTip->setIcon(QIcon("resources/question.png"));
	this->ui->exeTip->setIconSize(QSize(16,16));
	std::string line = translation["exeTip"];
	boost::replace_all(line, "<folder>", outPath.string()+"\\age2_x1");
	this->ui->exeTip->setWhatsThis(line.c_str());
	QObject::connect( this->ui->exeTip, &QPushButton::clicked, this, [this]() {
		QWhatsThis::showText(this->ui->exeTip->mapToGlobal(QPoint(0,0)),this->ui->exeTip->whatsThis());
	} );
	this->ui->modsTip->setIcon(QIcon("resources/question.png"));
	this->ui->modsTip->setIconSize(QSize(16,16));
	this->ui->modsTip->setWhatsThis(translation["modsTip"].c_str());
	QObject::connect( this->ui->modsTip, &QPushButton::clicked, this, [this]() {
			QWhatsThis::showText(this->ui->modsTip->mapToGlobal(QPoint(0,0)),this->ui->modsTip->whatsThis());
	} );
	this->ui->mapsTip->setIcon(QIcon("resources/question.png"));
	this->ui->mapsTip->setIconSize(QSize(16,16));
	this->ui->mapsTip->setWhatsThis(translation["mapsTip"].c_str());
	QObject::connect( this->ui->mapsTip, &QPushButton::clicked, this, [this]() {
			QWhatsThis::showText(this->ui->mapsTip->mapToGlobal(QPoint(0,0)),this->ui->mapsTip->whatsThis());
	} );
	QObject::connect( this->ui->runButton, &QPushButton::clicked, this, &MainWindow::run);

	this->ui->label->setText(("WololoKingdoms ver. " + version).c_str());
	if(!fs::exists(HDPath/"EmptySteamDepot")) { //This checks whether at least either AK or FE is installed, no way to check for all DLCs unfortunately.
		this->ui->runButton->setDisabled(true);
		return;
	}

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::changeLanguage(std::string language) {

	std::string line;
	std::ifstream translationFile("resources/"+language+".txt");
	while (std::getline(translationFile, line)) {
		boost::replace_all(line, "\\n", "\n");
		int index = line.find('=');
		translation[line.substr(0, index)] = line.substr(index+1, std::string::npos);
	}
	this->ui->runButton->setText(translation["runButton"].c_str());
	this->ui->replaceTooltips->setText(translation["replaceTooltips"].c_str());
	this->ui->createExe->setText(translation["createExe"].c_str());
	this->ui->useGrid->setText(translation["useGrid"].c_str());
	this->ui->usePw->setText(translation["usePw"].c_str());
	this->ui->useWalls->setText(translation["useWalls"].c_str());
	this->ui->hotkeyChoice->setItemText(1,translation["hotkeys1"].c_str());
	this->ui->hotkeyChoice->setItemText(2,translation["hotkeys2"].c_str());
	this->ui->hotkeyChoice->setItemText(3,translation["hotkeys3"].c_str());

	fs::path vooblyDir = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms/";
	fs::path nfzOutPath = vooblyDir / "Player.nfz";

	if(fs::exists(nfzOutPath)) {
		this->ui->hotkeyChoice->setItemText(0,translation["hotkeys0"].c_str());
	} else {
		this->ui->hotkeyChoice->setItemText(0,translation["hotkeyChoice"].c_str());
		this->ui->runButton->setDisabled(true);
		QObject::connect( this->ui->hotkeyChoice, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, [this]{
			if (this->ui->hotkeyChoice->currentIndex() != 0)
				this->ui->runButton->setDisabled(false);
			else
				this->ui->runButton->setDisabled(true);
		} );
	}
	qApp->processEvents();
}

void MainWindow::recCopy(fs::path const &src, fs::path const &dst, bool skip, bool force) {
	// recursive copy
	//fs::path currentPath(current->path());
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

void MainWindow::shallowRecCopy(fs::path const &src, fs::path const &dst, bool skip, bool force) {
	// recursive copy, but directory structure is ignored and all files copied directly into dst
	//fs::path currentPath(current->path());
	if (fs::is_directory(src)) {
		for (fs::directory_iterator current(src), end;current != end; ++current) {
			fs::path currentPath(current->path());
			if (fs::is_directory(currentPath))
				shallowRecCopy(currentPath, dst, skip, force);
			else
				shallowRecCopy(currentPath, dst / currentPath.filename(), skip, force);
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

void MainWindow::listAssetFiles(fs::path path, std::vector<std::string> *listOfSlpFiles, std::vector<std::string> *listOfWavFiles) {
	const std::set<std::string> exclude = {
		// Exclude Forgotten Empires leftovers
		"50163", // Forgotten Empires loading screen
		"50189", // Forgotten Empires main menu
		"53207", // Forgotten Empires in-game logo
		"53208", // Forgotten Empires objective window
		"53209" // ???
	};

	for (fs::directory_iterator end, it(path); it != end; it++) {
		std::string fileName = it->path().stem().string();
		if (exclude.find(fileName) == exclude.end()) {
			std::string extension = it->path().extension().string();
			if (extension == ".slp") {
				listOfSlpFiles->push_back(fileName);
			}
			else if (extension == ".wav") {
				listOfWavFiles->push_back(fileName);
			}
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

		auto strReplace = langReplacement->find(nb);
		if (strReplace != langReplacement->end()) {
			// this string has been changed by one of our patches (modified attributes etc.)
			line = strReplace->second;
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
}

void MainWindow::makeDrs(std::string const inputDir, std::string const moddedInputDir, std::ofstream *out) {

	this->ui->label->setText((translation["working"]+"\n"+translation["workingDrs"]).c_str());
	this->ui->label->repaint();
	const int numberOfTables = 2; // slp and wav

	std::vector<std::string> slpFilesNames;
	std::vector<std::string> wavFilesNames;
	std::vector<std::string> moddedFilesNames;
	listAssetFiles(inputDir, &slpFilesNames, &wavFilesNames);
	bar->setValue(bar->value()+1);bar->repaint(); //57
	listAssetFiles(moddedInputDir, &moddedFilesNames, NULL);
	bar->setValue(bar->value()+1);bar->repaint(); //58

	int numberOfSlpFiles = 0;
	std::vector<std::string>::iterator modIt = moddedFilesNames.begin();
	for (std::vector<std::string>::iterator it = slpFilesNames.begin(); it != slpFilesNames.end();) {
		int comp = (modIt != moddedFilesNames.end())?(*modIt).compare(*it):1;
		if(comp == 0) {
			modIt++;
			it++;
		} else if (comp < 0) {
			modIt++;
		} else
			it++;
		numberOfSlpFiles++;
	}
	bar->setValue(bar->value()+1);bar->repaint(); //59
	int numberOfWavFiles = wavFilesNames.size();
	int offsetOfFirstFile = sizeof (wololo::DrsHeader) +
			sizeof (wololo::DrsTableInfo) * numberOfTables +
			sizeof (wololo::DrsFileInfo) * (numberOfSlpFiles + numberOfWavFiles);
	int offset = offsetOfFirstFile;


	// file infos

	std::vector<wololo::DrsFileInfo> slpFiles;
	std::vector<wololo::DrsFileInfo> wavFiles;

	modIt = moddedFilesNames.begin();
	for (std::vector<std::string>::iterator it = slpFilesNames.begin(); it != slpFilesNames.end();) {
		wololo::DrsFileInfo slp;
		int comp = (modIt != moddedFilesNames.end())?(*modIt).compare(*it):1;
		size_t size;
		if(comp == 0) {
			size = fs::file_size(moddedInputDir + *modIt + ".slp");
			slp.file_id = stoi(*modIt);
			modIt++;
			it++;
		} else if (comp < 0) {
			size = fs::file_size(moddedInputDir + *modIt + ".slp");
			slp.file_id = stoi(*modIt);
			modIt++;
		} else {
			size = fs::file_size(inputDir + *it + ".slp");
			slp.file_id = stoi(*it);
			it++;
		}
		slp.file_data_offset = offset;
		slp.file_size = size;
		offset += size;
		slpFiles.push_back(slp);
	}
	bar->setValue(bar->value()+1);bar->repaint(); //60

	for (std::vector<std::string>::iterator it = wavFilesNames.begin(); it != wavFilesNames.end(); it++) {
		wololo::DrsFileInfo wav;
		size_t size = fs::file_size(inputDir + *it + ".wav");
		wav.file_id = stoi(*it);
		wav.file_data_offset = offset;
		wav.file_size = size;
		offset += size;
		wavFiles.push_back(wav);
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
		(int) slpFiles.size() // num_files
	};
	wololo::DrsTableInfo const wavTableInfo = {
		0x20, // file_type, MAGIC
		{ 'v', 'a', 'w' }, // file_extension, "wav" in reverse
		(int) (sizeof (wololo::DrsHeader) +  sizeof (wololo::DrsFileInfo) * numberOfTables + sizeof (wololo::DrsFileInfo) * slpFiles.size()), // file_info_offset
		(int) wavFiles.size() // num_files
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
	for (std::vector<wololo::DrsFileInfo>::iterator it = slpFiles.begin(); it != slpFiles.end(); it++) {
		out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
		out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
		out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));
	}
	bar->setValue(bar->value()+1);bar->repaint(); //64
	for (std::vector<wololo::DrsFileInfo>::iterator it = wavFiles.begin(); it != wavFiles.end(); it++) {
		out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
		out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
		out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));
	}
	bar->setValue(bar->value()+1);bar->repaint(); //65

	this->ui->label->setText((translation["working"]+"\n"+translation["workingDrs3"]).c_str());
	this->ui->label->repaint();
	// now write the actual files
	modIt = moddedFilesNames.begin();
	for (std::vector<std::string>::iterator it = slpFilesNames.begin(); it != slpFilesNames.end();) {
		int comp = (modIt != moddedFilesNames.end())?(*modIt).compare(*it):1;
		if(comp == 0) {
			std::ifstream srcStream = std::ifstream(moddedInputDir + *modIt + ".slp", std::ios::binary);
			*out << srcStream.rdbuf();
			it++;
			modIt++;
		} else if (comp < 0) {
			std::ifstream srcStream = std::ifstream(moddedInputDir + *modIt + ".slp", std::ios::binary);
			*out << srcStream.rdbuf();
			modIt++;
		} else {
			std::ifstream srcStream = std::ifstream(inputDir + *it + ".slp", std::ios::binary);
			*out << srcStream.rdbuf();
			it++;
		}

	}
	bar->setValue(bar->value()+1);bar->repaint(); //66

	for (std::vector<std::string>::iterator it = wavFilesNames.begin(); it != wavFilesNames.end(); it++) {
		std::ifstream srcStream(inputDir + *it + ".wav", std::ios::binary);
		*out << srcStream.rdbuf();
	}
	bar->setValue(bar->value()+1);bar->repaint(); //67
}

void MainWindow::uglyHudHack(std::string const inputDir, std::string const moddedDir) {
	/*
	 * We have more than 30 civs, so we need to space the interface files further apart
	 * This adds +10 for each gap between different file types
	 */
	this->ui->label->repaint();
	int const HudFiles[] = {51100, 51130, 51160};
	for (size_t baseIndex = 0; baseIndex < sizeof HudFiles / sizeof (int); baseIndex++) {
		for (int i = 1; i < 24; i++) {
			std::string dst = moddedDir + std::to_string(HudFiles[baseIndex]+i+baseIndex*10) + ".slp";
			std::string src = inputDir + std::to_string(HudFiles[baseIndex]+i) + ".slp";
			if(fs::exists(src))
				fs::copy_file(src, dst);
		}
	}

	/*
	 * copies the Slav hud files for AK civs, the good way of doing this would be to extract
	 * the actual AK civs hud files from
	 * Age2HD\resources\_common\slp\game_b[24-27].slp correctly, but I haven't found a way yet
	 */
	int const slavHudFiles[] = {51123, 51163, 51203};
	for (size_t baseIndex = 0; baseIndex < sizeof slavHudFiles / sizeof (int); baseIndex++) {
		for (size_t i = 1; i <= 8; i++) {
			std::string dst = moddedDir + std::to_string(slavHudFiles[baseIndex]+i) + ".slp";
			std::string src = moddedDir + std::to_string(slavHudFiles[baseIndex]) + ".slp";
			fs::copy_file(src, dst);
		}
	}
}

void MainWindow::copyCivIntroSounds(fs::path inputDir, fs::path outputDir) {
	std::string const civs[] = {"italians", "indians", "incas", "magyars", "slavs",
								"portuguese", "ethiopians", "malians", "berbers", "burmese", "malay", "vietnamese", "khmer"};
	for (size_t i = 0; i < sizeof civs / sizeof (std::string); i++) {
		boost::system::error_code ec;
		fs::copy_file(inputDir / (civs[i] + ".mp3"), outputDir / (civs[i] + ".mp3"), ec);
	}
}

void MainWindow::copyWallFiles(fs::path inputDir, fs::path outputDir) {
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
	recCopy(inputDir,outputDir);
	int conversionTable[] = {3,-15,2,0,3,-18,-5,1,0,1,2,3,0,1,2};
	int newBaseSLP = 24000;
	for(size_t i = 0; i < sizeof(conversionTable)/sizeof(int); i++) {
		int archID = conversionTable[i];
		if (archID < 0) {
			archID = -archID;
			int digits = archID == 5 || archID == 7?124:324;
			fs::copy_file(inputDir/(std::to_string(archID*1000+digits)+".slp"),outputDir/(std::to_string(newBaseSLP+i*1000+digits+200)+".slp"));
			fs::copy_file(inputDir/(std::to_string(archID*1000+digits+2)+".slp"),outputDir/(std::to_string(newBaseSLP+i*1000+digits+202)+".slp"));
			for (int j = 0; j <= 10; j+=2)
				fs::copy_file(inputDir/(std::to_string(archID*1000+digits+21+j)+".slp"),outputDir/(std::to_string(newBaseSLP+i*1000+digits+221+j)+".slp"));
		} else {
			fs::copy_file(inputDir/(std::to_string(2098+archID)+".slp"),outputDir/(std::to_string(newBaseSLP+i*1000+324)+".slp"));
			fs::copy_file(inputDir/(std::to_string(2110+archID)+".slp"),outputDir/(std::to_string(newBaseSLP+i*1000+326)+".slp"));
			for (int j = 0; j <= 20; j+=4)
				fs::copy_file(inputDir/(std::to_string(4169+archID+j)+".slp"),outputDir/(std::to_string(newBaseSLP+i*1000+345+j)+".slp"));
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
}

void MainWindow::copyHDMaps(fs::path inputDir, fs::path outputDir) {

	const std::set<std::string> exclude = {
		"Arabia",
		"Archipelago",
		"Arena",
		"Baltic",
		"Black_Forest",
		"Blind_Random",
		"Coastal",
		"Continental",
		"Crater_Lake",
		"Fortress",
		"Ghost_Lake",
		"Gold_Rush",
		"Highland",
		"Islands",
		"Mediterranean",
		"Migration",
		"Mongolia",
		"nomad",
		"Oasis",
		"Rivers",
		"Salt_Marsh",
		"Scandinavia",
		"Team_Islands",
		"Yucatan"
	};

	std::vector<fs::path> mapNames;
	std::vector<fs::path> mapNamesSorted;
	std::vector<fs::path> existingMapNames;
	for (fs::directory_iterator end, it(inputDir); it != end; it++) {
		std::string stem = it->path().stem().string();
		if (exclude.find(stem) == exclude.end()) {
			std::string extension = it->path().extension().string();
			if ((extension == ".rms" || extension == ".rms2") && stem.substr(0,10) != "real_world" && stem.substr(0,11) != "special_map" && stem.substr(0,3) != "CtR") {
				mapNames.push_back(*it);
			}
		}
	}
	for (fs::directory_iterator end, it(outputDir); it != end; it++) {
		std::string filename = it->path().filename().string();
		if (filename.substr(0,3) == "ZR@")
			existingMapNames.push_back(it->path().parent_path()/filename.substr(3,std::string::npos));
		else
			existingMapNames.push_back(it->path());
	}
	bar->setValue(bar->value()+1);bar->repaint(); //13+17
	sort(existingMapNames.begin(), existingMapNames.end());
	sort(mapNames.begin(), mapNames.end());
	std::vector<fs::path>::iterator modIt = existingMapNames.begin();
	for (std::vector<fs::path>::iterator it = mapNames.begin(); it != mapNames.end();) {
		int comp = modIt != existingMapNames.end()?(modIt->stem().string()).compare(it->stem().string()):1;
		if(comp == 0) {
			it++;
			modIt++;
		} else if (comp < 0) {
			modIt++;
		} else {
			mapNamesSorted.push_back(*it);
			it++;
		}
	}
	bar->setValue(bar->value()+1);bar->repaint(); //14+18
	std::set<fs::path> terrainOverrides;
	std::vector<std::tuple<std::string,std::string,std::string,std::string,std::string,std::string,bool,std::string,std::string>> replacements = {
		//<Name,Regex Pattern if needed,replace name,terrain ID, replace terrain ID,slp to replace,upgrade trees?,tree to replace,new tree>
		std::make_tuple("DRAGONFOREST","DRAGONFORES(T?)","DRAGONFORES$1","48","21","15029.slp",true,"SNOWPINETREE","DRAGONTREE"),
		//std::make_tuple("ACACIA_FOREST","AC(C?)ACIA(_?)FORES(T?)","AC$1ACIA$2FORES$3","50","13","15010.slp",true,"PALMTREE","ACACIA_TREEE"),
		std::make_tuple("ACACIA_FOREST","AC(C?)ACIA(_?)FORES(T?)","AC$1ACIA$2FORES$3","50","41","",true,"",""),
		std::make_tuple("DLC_RAINFOREST","","DLC_RAINFOREST","56","10","15011.slp",true,"FOREST_TREE","DLC_RAINTREE"),
		std::make_tuple("BAOBAB","","BAOBAB","49","16","",false,"",""),
		std::make_tuple("DLC_MANGROVESHALLOW","","DLC_MANGROVESHALLOW","54","4","15014.slp",false,"",""),
		std::make_tuple("DLC_MANGROVEFOREST","","DLC_MANGROVEFOREST","55","16","",false,"",""),
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
		std::make_tuple("DLC_WATER4","","DLC_WATER4","57","22","15015.slp",false,"",""),
		std::make_tuple("DLC_WATER5","","DLC_WATER5","58","1","15002.slp",false,"",""),
		std::make_tuple("DLC_DRYROAD","","DLC_DRYROAD","43","25","15019.slp",false,"",""),
		std::make_tuple("DLC_JUNGLELEAVES","","DLC_JUNGLELEAVES","62","11","15006.slp",false,"",""),
		std::make_tuple("DLC_JUNGLEROAD","","DLC_JUNGLEROAD","62","39","15031.slp",false,"",""),
		std::make_tuple("DLC_JUNGLEGRASS","","DLC_JUNGLEGRASS","61","12","15008.slp",false,"","")
	};
	for (std::vector<fs::path>::iterator it = mapNamesSorted.begin(); it != mapNamesSorted.end(); it++) {
		std::ifstream input(inputDir.string()+it->filename().string());
		std::string str(static_cast<std::stringstream const&>(std::stringstream() << input.rdbuf()).str());
		if(str.find("DLC_MANGROVESHALLOW")!=std::string::npos) {
			if(str.find("<PLAYER_SETUP>")!=std::string::npos)
				str = std::regex_replace(str, std::regex("<PLAYER_SETUP>\\s*(\\r*)\\n"),
					"<PLAYER_SETUP>$1\n  terrain_state 0 0 0 1$1\n");
			else
				str = std::regex_replace(str, std::regex("#include_drs\\s+random_map\\.def\\s*(\\r*)\\n"),
					"#include_drs random_map.def$1\n<PLAYER_SETUP>$1\n  terrain_state 0 0 0 1$1\n");
		}
		for (std::vector<std::tuple<std::string,std::string,std::string,std::string,std::string,std::string,bool,std::string,std::string>>::iterator repIt = replacements.begin(); repIt != replacements.end(); repIt++) {
			if((std::get<0>(*repIt)=="DLC_WATER4"||std::get<0>(*repIt)=="DLC_WATER5") && (str.find("MED_WATER")!=std::string::npos || str.find("DEEP_WATER")!=std::string::npos)) {
				boost::replace_all(str, "#const "+std::get<0>(*repIt)+" "+std::get<3>(*repIt), "#const "+std::get<0>(*repIt)+" "+std::get<4>(*repIt));
				continue;
			}
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
					fs::copy_file(tempMapDir/(std::get<0>(*repIt)+".slp"),tempMapDir/std::get<5>(*repIt),fs::copy_option::overwrite_if_exists);
					terrainOverrides.insert(tempMapDir/std::get<5>(*repIt));
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
			terrainOverrides.insert(tempMapDir/"15004.slp");
			terrainOverrides.insert(tempMapDir/"15005.slp");
			terrainOverrides.insert(tempMapDir/"15021.slp");
			terrainOverrides.insert(tempMapDir/"15022.slp");
			terrainOverrides.insert(tempMapDir/"15023.slp");
		}
		//str = regex_replace(str, std::regex("#const\\s+BAOBAB\\s+49"), "#const BAOBAB 16");

		std::string mapName = it->stem().string()+".rms";
		std::ofstream out(outputDir.string()+"/"+mapName);
		out << str;
		out.close();
		if (mapName.substr(0,3) == "rw_" || mapName.substr(0,3) == "sm_") {
			terrainOverrides.insert(fs::path(inputDir.string()+"/"+it->stem().string()+".scx"));
		}
		if (terrainOverrides.size() != 0) {
			QuaZip zip(QString((outputDir.string()+"/ZR@"+mapName).c_str()));
			zip.open(QuaZip::mdAdd, NULL);
			terrainOverrides.insert(fs::path(outputDir.string()+"/"+mapName));
			for(std::set<fs::path>::iterator files = terrainOverrides.begin(); files != terrainOverrides.end(); files++) {
				QuaZipFile outFile(&zip);
				QuaZipNewInfo fileInfo(QString((*files).filename().string().c_str()));
				fileInfo.uncompressedSize = fs::file_size((*files));
				outFile.open(QIODevice::WriteOnly,fileInfo,NULL,0,0,0,false);
				QFile inFile;
				inFile.setFileName((*files).string().c_str());
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
	terrainSwap(hdDat, aocDat, 15,55,15012); //mangrove forest
	terrainSwap(hdDat, aocDat, 16,49,15025); //baobab forest
	terrainSwap(hdDat, aocDat, 41,50,15013); //acacia forest
	fs::copy_file(tempMapDir/"DLC_MANGROVEFOREST.slp",moddedAssetsPath/"15012.slp");
	fs::copy_file(tempMapDir/"ACACIA_FOREST.slp",moddedAssetsPath/"15013.slp");
	fs::copy_file(tempMapDir/"BAOBAB.slp",moddedAssetsPath/"15025.slp");
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
						662, 663, 664, 665, 666, 667, 668, 669, 670, 671, 672, 673, 674, 806, 807, 808, 1102, 1189};
	short unitIDs[] = {17, 21, 420, 442, 527, 528, 529, 532, 539, 545, 691, 1103, 1104};
	short civIDs[] = {13,23,7,17,14,31,21,6,11,12,27,1,4,18,9};
	short burmese = 30; //These are used for ID reference
	for(unsigned int c = 0; c < sizeof(civIDs)/sizeof(short); c++) {
		std::map<short,short> replacedGraphics;
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

	//Fix the missionary converting frames while we're at it
	aocDat->Graphics[6616].FrameCount = 14;

	//Separate Monks and DA buildings into 4 major regions (Europe, Asian, Southern, American)
	std::vector<std::vector<short>> civGroups = { {5,6,12,18,28,29,30,31},
					{7,8,9,10,20,25,26,27},
					{15,16,21}};
	std::map<int,int> slps = {{2683,0},{376,2},{4518,1},{2223,3},{3482,4},{3483,5},{4172,6},{4330,7},{889,10},{4612,16},{891,17},{4611,15},{3596,12},
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
			fs::path src = HDPath/("resources/_common/drs/graphics/776.slp");
			fs::path dst = moddedAssetsPath/(std::to_string(newSLP)+".slp");
			boost::system::error_code ec;
			fs::copy_file(src,dst,ec);
		}
		for(unsigned int cg = 0; cg < civGroups[i].size(); cg++) {
			std::map<short,short> replacedGraphics;
			for(unsigned int b = 0; b < sizeof(cgBuildingIDs)/sizeof(short); b++) {
				replaceGraphic(aocDat, &aocDat->Civs[civGroups[i][cg]].Units[cgBuildingIDs[b]].StandingGraphic.first, -1, i, replacedGraphics, slps);
				for(std::vector<genie::unit::DamageGraphic>::iterator it = aocDat->Civs[civGroups[i][cg]].Units[cgBuildingIDs[b]].DamageGraphics.begin();
					it != aocDat->Civs[civGroups[i][cg]].Units[cgBuildingIDs[b]].DamageGraphics.end(); it++) {
					replaceGraphic(aocDat, &(it->GraphicID), -1, i, replacedGraphics, slps);
				}
			}

			//units like ships
			for(unsigned int u = 0; u < sizeof(cgUnitIDs)/sizeof(short); u++) {
				replaceGraphic(aocDat, &aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].StandingGraphic.first, -1, i, replacedGraphics, slps);
				if (aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].DeadFish.WalkingGraphic.first != -1)
					replaceGraphic(aocDat, &aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].DeadFish.WalkingGraphic.first, -1, i, replacedGraphics, slps);
				if (aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].Type50.AttackGraphic != -1)
					replaceGraphic(aocDat, &aocDat->Civs[civGroups[i][cg]].Units[cgUnitIDs[u]].Type50.AttackGraphic, -1, i, replacedGraphics, slps);
			}
			//special UP healing slp workaround
			if (i != 2) {
				for(unsigned int cg = 0; cg < civGroups[i].size(); cg++) {
					size_t code = 0x811E0000+monkHealingGraphic;
					int ccode = (int) code;
					aocDat->Civs[civGroups[i][cg]].Units[125].LanguageDLLHelp = ccode;

					if (i == 0) {
						aocDat->Civs[civGroups[i][cg]].Units[125].IconID = 218;
						aocDat->Civs[civGroups[i][cg]].Units[286].IconID = 218;
					} else {
						aocDat->Civs[civGroups[i][cg]].Units[125].IconID = 169;
						aocDat->Civs[civGroups[i][cg]].Units[286].IconID = 169;
					}

				}
			}
		}
		bar->setValue(bar->value()+1);bar->repaint(); //52-55
	}


}

void MainWindow::replaceGraphic(genie::DatFile *aocDat, short* graphicID, short compareID, short c, std::map<short,short>& replacedGraphics, std::map<int,int> slps) {
	if(replacedGraphics[*graphicID] != 0)
		*graphicID = replacedGraphics[*graphicID];
	else {
		std::vector<int> duplicatedGraphics;
		short newGraphicID;
		if (compareID != -1)
			newGraphicID = duplicateGraphic(aocDat, duplicatedGraphics, *graphicID, compareID, c);
		else
			newGraphicID = duplicateGraphic(aocDat, duplicatedGraphics, *graphicID, compareID, c, slps);
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

short MainWindow::duplicateGraphic(genie::DatFile *aocDat, std::vector<int> duplicatedGraphics, short graphicID, short compareID, short offset, std::map<int,int> slps) {


	if (compareID != -1 && (aocDat->Graphics[compareID].SLP < 18000 || aocDat->Graphics[compareID].SLP >= 19000)) {
		std::vector<int> checkedGraphics;
		if(!checkGraphics(aocDat, compareID, checkedGraphics))
			return graphicID;
	}
	genie::Graphic newGraphic = aocDat->Graphics[graphicID];
	int newBaseSLP = compareID==-1?60000:24000;

	short newGraphicID = aocDat->Graphics.size();
	int newSLP;
	if(compareID==-1) {
		if (slps[aocDat->Graphics[graphicID].SLP] == 0 && aocDat->Graphics[graphicID].SLP != 2683)
			newSLP = aocDat->Graphics[graphicID].SLP;
		else
			newSLP = newBaseSLP+10000*offset+slps[aocDat->Graphics[graphicID].SLP];
	} else
		newSLP = aocDat->Graphics[compareID].SLP - 18000 + newBaseSLP + 1000*offset;
	if(newSLP != aocDat->Graphics[graphicID].SLP) {
		fs::path src = HDPath/("resources/_common/drs/gamedata_x2/"+std::to_string(newGraphic.SLP)+".slp");
		fs::path dst = moddedAssetsPath/(std::to_string(newSLP)+".slp");
		boost::system::error_code ec;
		if(fs::exists(src)) {
			fs::copy_file(src,dst, ec);
		} else {
			src = HDPath/("resources/_common/drs/graphics/"+std::to_string(newGraphic.SLP)+".slp");
			fs::copy_file(src,dst, ec);
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
					it->GraphicID = duplicateGraphic(aocDat, duplicatedGraphics, it->GraphicID, -1, offset, slps);
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

	fs::path nfz1Path("resources/Player1.nfz");
	fs::path nfzPath = outPath / "player.nfz";
	fs::path aocHkiPath("resources/player1.hki");
	fs::path customHkiPath("player1.hki");
	fs::path hkiPath = HDPath / ("Profiles/player0.hki");
	fs::path hkiOutPath = outPath / "player1.hki";
	fs::path hki2OutPath = outPath / "player2.hki";
	fs::path nfzOutPath = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms/Player.nfz";

	fs::remove(nfzOutPath);
	fs::remove(nfzUpOutPath);
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
	if(fs::exists(customHkiPath)) {
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
	if (this->ui->hotkeyChoice->currentIndex() == 1 && !fs::exists(hkiOutPath))
		fs::copy_file(aocHkiPath, hkiOutPath);
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
	int ret = 0;

	try {
		fs::path keyValuesStringsPath = HDPath / "resources/" / language / "/strings/key-value/key-value-strings-utf8.txt";
		fs::path vooblyDir = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms/";
		std::string aocDatPath = HDPath.string() + "resources/_common/dat/empires2_x1_p1.dat";
		std::string hdDatPath = HDPath.string() + "resources/_common/dat/empires2_x2_p1.dat";
		fs::path modLangPath("resources/language.ini");
		fs::path languageIniPath = vooblyDir / "language.ini";
		std::string versionIniPath = vooblyDir.string() + "version.ini";
		fs::path soundsInputPath = HDPath / "resources/_common/sound/";
		fs::path soundsOutputPath = vooblyDir / "Sound/";
		fs::path tauntInputPath = HDPath / "resources/en/sound/taunt/";
		fs::path tauntOutputPath = vooblyDir / "Taunt/";
		fs::path xmlPath("resources/WK.xml");
		fs::path xmlOutPath = vooblyDir / "age2_x1.xml";
		fs::path langDllFile("language_x1_p1.dll");
		fs::path langDllPath = langDllFile;
		fs::path xmlOutPathUP = outPath / "Games/WK.xml";
		fs::path aiInputPath("resources/Script.Ai");
		std::string drsOutPath = vooblyDir.string() + "Data/gamedata_x1_p1.drs";
		fs::path assetsPath = HDPath / "resources/_common/drs/gamedata_x2/";
		fs::path outputDatPath = vooblyDir / "Data/empires2_x1_p1.dat";
		fs::path upDir = outPath / "Games/WololoKingdoms/";
		std::string const UPModdedExe = "WK";
		fs::path UPExe("resources/SetupAoc.exe");
		fs::path UPExeOut = outPath / "SetupAoc.exe";
		fs::path pwInputDir("resources/pussywood");
		fs::path gridInputDir("resources/Grid");
		fs::path newTerrainInputDir("resources/new terrains");
		fs::path newGridTerrainInputDir("resources/new grid terrains");
		fs::path modOverrideDir("mod_override/");
		fs::path gridNoSnowInputDir("resources/Grid No Snow");
		fs::path noSnowInputDir("resources/No Snow");
		fs::path wallsInputDir("resources/short_walls");
		fs::path gamedata_x1("resources/gamedata_x1.drs");

		std::string line;

		boost::system::error_code ec;
		try {
			fs::remove_all(moddedAssetsPath, ec);
			fs::remove_all(tempMapDir, ec);
		} catch (const fs::filesystem_error& e) {
			if(e.code() != boost::system::errc::directory_not_empty)
				throw e;
		}
		bar->setValue(1);bar->repaint(); //1
		fs::create_directories(moddedAssetsPath);
		fs::create_directories(tempMapDir);

		if(this->ui->usePw->isChecked() || this->ui->useGrid->isChecked() || this->ui->useWalls->isChecked()) {
			this->ui->label->setText((translation["working"]+"\n"+translation["workingMods"]).c_str());
			this->ui->label->repaint();
		}
		if(this->ui->usePw->isChecked())
			recCopy(pwInputDir, moddedAssetsPath);
		bar->setValue(bar->value()+1);bar->repaint(); //2
		if(this->ui->useGrid->isChecked()) {
			recCopy(gridInputDir, moddedAssetsPath);
			bar->setValue(bar->value()+1);bar->repaint(); //3
			recCopy(newGridTerrainInputDir,tempMapDir);
			bar->setValue(bar->value()+1);bar->repaint(); //4
			if(this->ui->useNoSnow->isChecked())
				recCopy(gridNoSnowInputDir, moddedAssetsPath, false, true);
			bar->setValue(bar->value()+1);bar->repaint(); //5
		} else {
			recCopy(newTerrainInputDir,tempMapDir);
			bar->setValue(bar->value()+1);bar->repaint();//3
			if(this->ui->useNoSnow->isChecked())
				recCopy(noSnowInputDir, moddedAssetsPath);
			bar->setValue(bar->value()+2);bar->repaint(); //5
		}
		bar->setValue(bar->value()+1);bar->repaint(); //6
		if(this->ui->useWalls->isChecked())
			copyWallFiles(wallsInputDir, moddedAssetsPath);
		bar->setValue(bar->value()+1);bar->repaint(); //7
		if(!fs::is_empty(modOverrideDir))
			shallowRecCopy(modOverrideDir, moddedAssetsPath, false, true);
		bar->setValue(bar->value()+1);bar->repaint(); //8
		fs::remove_all(vooblyDir/"Data");
		fs::remove_all(vooblyDir/"Script.Ai/Brutal");
		fs::remove(vooblyDir/"Script.Ai/BruteForce.ai");
		fs::remove(vooblyDir/"Script.Ai/BruteForce.per");
		fs::remove(vooblyDir/"age2_x1.xml");
		fs::remove(vooblyDir/"language.ini");
		fs::remove(vooblyDir/"version.ini");
		fs::remove_all(upDir/"Data");
		fs::remove_all(upDir/"Script.Ai/Brutal");
		fs::remove(upDir/"Script.Ai/BruteForce.ai");
		fs::remove(upDir/"Script.Ai/BruteForce.per");
		fs::remove(outPath/"Games/WK.xml");
		fs::create_directories(vooblyDir/"Data");
		fs::create_directories(vooblyDir/"Sound/stream");
		fs::create_directories(vooblyDir/"Taunt");
		fs::create_directories(upDir);

		this->ui->label->setText((translation["working"]+"\n"+translation["workingFiles"]).c_str());
		this->ui->label->repaint();
		std::ofstream versionOut(versionIniPath);
		versionOut << version << std::endl;

		boolean aocFound = outPath != HDPath/"WololoKingdoms/out/";
		copyCivIntroSounds(soundsInputPath / "civ/", soundsOutputPath / "stream/");
		bar->setValue(bar->value()+1);bar->repaint(); //9
		createMusicPlaylist(soundsInputPath.string() + "music/", soundsOutputPath.string() + "music.m3u");
		bar->setValue(bar->value()+1);bar->repaint(); //10
		recCopy(tauntInputPath, tauntOutputPath, true);
		bar->setValue(bar->value()+1);bar->repaint(); //11
		fs::copy_file(xmlPath, xmlOutPath);
		if (aocFound) {
			recCopy(outPath/"Random", vooblyDir/"Script.Rm", true);
		}
		bar->setValue(bar->value()+1);bar->repaint(); //12
		if(!fs::is_empty("new_terrain_override")) {
			recCopy(tempMapDir, fs::path("new_terrain_override/"), true);
			tempMapDir = fs::path("new_terrain_override/");
		}
		copyHDMaps(assetsPath, vooblyDir/"Script.Rm");
		bar->setValue(bar->value()+1);bar->repaint(); //16
		copyHDMaps(HDPath/"resources/_common/random-map-scripts/", vooblyDir/"Script.Rm");
		bar->setValue(bar->value()+1);bar->repaint(); //20
		if(this->ui->copyMaps->isChecked())
			copyHDMaps("resources/Script.Rm/", vooblyDir/"Script.Rm");
		else
			bar->setValue(bar->value()+3);
		bar->setValue(bar->value()+1);bar->repaint(); //24
		//If wanted, the BruteForce AI could be included as a "standard" AI.
		recCopy(aiInputPath, vooblyDir/"Script.Ai", true);
		bar->setValue(bar->value()+1);bar->repaint(); //25
		if(this->ui->createExe->isChecked()) {
			fs::create_directories(upDir / "Data");
			recCopy(vooblyDir / "Sound", upDir / "Sound", true);
			recCopy(vooblyDir / "Taunt", upDir / "Taunt", true);
			fs::copy_file(xmlPath, xmlOutPathUP);
			recCopy(vooblyDir / "Script.Rm", upDir / "Script.Rm", true);
			recCopy(vooblyDir / "Script.Ai", upDir / "Script.Ai", true);
		}
		bar->setValue(bar->value()+1);bar->repaint(); //26
		if(this->ui->hotkeyChoice->currentIndex() != 0 || fs::exists("player1.hki"))
			hotkeySetup();
		bar->setValue(bar->value()+1);bar->repaint(); //24
		recCopy(gamedata_x1, vooblyDir/"Data/gamedata_x1.drs", false);
		bar->setValue(bar->value()+1);bar->repaint(); //28

		this->ui->label->setText((translation["working"]+"\n"+translation["workingAoc"]).c_str());
		this->ui->label->repaint();

		genie::DatFile aocDat;
		//aocDat.setVerboseMode(true);
		aocDat.setGameVersion(genie::GameVersion::GV_TC);
		aocDat.load(aocDatPath.c_str());
		bar->setValue(bar->value()+5);bar->repaint(); //33

		this->ui->label->setText((translation["working"]+"\n"+translation["workingHD"]).c_str());
		this->ui->label->repaint();
		genie::DatFile hdDat;
		//hdDat.setVerboseMode(true);
		hdDat.setGameVersion(genie::GameVersion::GV_Cysion);
		hdDat.load(hdDatPath.c_str());
		bar->setValue(bar->value()+5);bar->repaint(); //38

		std::ofstream drsOut(drsOutPath, std::ios::binary);

		this->ui->label->setText((translation["working"]+"\n"+translation["workingInterface"]).c_str());
		this->ui->label->repaint();
		uglyHudHack(assetsPath.string(),moddedAssetsPath.string());
		bar->setValue(bar->value()+1);bar->repaint(); //43?

		this->ui->label->setText((translation["working"]+"\n"+translation["workingDat"]).c_str());
		this->ui->label->repaint();
		transferHdDatElements(&hdDat, &aocDat);
		bar->setValue(bar->value()+1);bar->repaint(); //44?

		patchArchitectures(&aocDat);
		bar->setValue(bar->value()+1);bar->repaint(); //59
		makeDrs(assetsPath.string(), moddedAssetsPath.string(), &drsOut);
		bar->setValue(bar->value()+1);bar->repaint(); //71

		try {
			fs::remove_all(moddedAssetsPath, ec);
			fs::remove_all(tempMapDir, ec);
		} catch (const fs::filesystem_error& e) {
			if(e.code() != boost::system::errc::directory_not_empty)
				throw e;
		}
		bar->setValue(bar->value()+1);bar->repaint(); //72
		wololo::DatPatch patchTab[] = {

			wololo::berbersUTFix,
			wololo::vietFix,
			wololo::malayFix,
			//			wololo::demoShipFix,
			wololo::ethiopiansFreePikeUpgradeFix,
			wololo::hotkeysFix,
			wololo::maliansFreeMiningUpgradeFix,
			wololo::portugueseFix,
			wololo::disableNonWorkingUnits,
			wololo::feitoriaFix,
			wololo::burmeseFix,
			wololo::incaFix,
			wololo::smallFixes,
			wololo::ai900UnitIdFix
		};


		std::map<int, std::string> langReplacement;
		//Fix errors in civ descriptions
		if (language == "en") {
			langReplacement[20162] = "Infantry civilization \\n\\n· Infantry move 15% faster \\n· Lumberjacks work 15% faster \\n· Siege weapons fire 20% faster \\n· Can convert sheep even if enemy units are next to them. \\n\\n<b>Unique Unit:<b> Woad Raider (infantry) \\n\\n<b>Unique Techs:<b> Stronghold (Castles and towers fire 20% faster); Furor Celtica (Siege Workshop units have +40% HP)\\n\\n<b>Team Bonus:<b> Siege Workshops work 20% faster";
			langReplacement[20166] = "Cavalry civilization \\n\\n· Do not need houses, but start with -100 wood \\n· Cavalry Archers cost -10% Castle, -20% Imperial Age \\n· Trebuchets +35% accuracy against units \\n\\n<b>Unique Unit:<b> Tarkan (cavalry) \\n\\n<b>Unique Techs:<b> Marauders (Create Tarkans at stables); Atheism (+100 years Relic, Wonder victories; Spies/Treason costs -50%)\\n\\n<b>Team Bonus:<b> Stables work 20% faster";
			langReplacement[20170] = "Infantry civilization \\n\\n· Start with a free llama \\n· Villagers affected by Blacksmith upgrades \\n· Houses support 10 population \\n· Buildings cost -15% stone\\n\\n<b>Unique Units:<b> Kamayuk (infantry), Slinger (archer)\\n\\n<b>Unique Techs:<b> Andean Sling (Skirmishers and Slingers no minimum range); Couriers (Kamayuks, Slingers, Eagles +1 armor/+2 pierce armor)\\n\\n<b>Team Bonus:<b> Farms built 2x faster";
			langReplacement[20165] = "Archer civilization \\n\\n· Start with +1 villager, but -50 food \\n· Resources last 15% longer \\n· Archers cost -10% Feudal, -20% Castle, -30% Imperial Age \\n\\n<b>Unique Unit:<b> Plumed Archer (archer) \\n\\n<b>Unique Techs:<b> Obsidian Arrows (Archers, Crossbowmen and Arbalests +12 attack vs. Towers/Stone Walls, +6 attack vs. other buildings); El Dorado (Eagle Warriors have +40 hit points)\\n\\n<b>Team Bonus:<b> Walls cost -50%";
			langReplacement[20158] = "Camel and naval civilization \\n· Market trade cost only 5% \\n· Market costs -75 wood \\n· Transport Ships 2x hit points, \\n 2x carry capacity \\n· Galleys attack 20% faster \\n· Cavalry archers +4 attack vs. buildings \\n\\n<b>Unique Unit:<b> Mameluke (camel) \\n\\n<b>Unique Techs:<b> Madrasah (Killed monks return 33% of their cost); Zealotry (Camels, Mamelukes +30 hit points)\\n\\n<b>Team Bonus:<b> Foot archers +2 attack vs. buildings";
			langReplacement[20163] = "Gunpowder and Monk civilization \\n\\n· Builders work 30% faster \\n· Blacksmith upgrades don't cost gold \\n· Cannon Galleons fire faster and with Ballistics) \\n· Gunpowder units fire 15% faster\\n\\n<b>Unique Units:<b> Conquistador (mounted hand cannoneer), Missionary (mounted Monk) \\n\\n<b>Unique Techs:<b> Inquisition (Monks convert faster); Supremacy (villagers better in combat)\\n\\n<b>Team Bonus:<b> Trade units generate +25% gold";
			//Add that the Genitour and Imperial Skirmishers are Mercenary Units, since there is no other visual difference in the tech tree
			langReplacement[26137] = "Create <b> Genitour<b> (<cost>) \\nBerber mercenary unit, available when teamed with a Berber player. Mounted skirmisher. Effective against Archers.<i> Upgrades: speed, hit points (Stable); attack, range, armor (Blacksmith); attack, accuracy (University); accuracy, armor, to Elite Genitour 500F, 450W (Archery Range); creation speed (Castle); more resistant to Monks (Monastery).<i> \\n<hp> <attack> <armor> <piercearmor> <range>";
			langReplacement[26139] = "Create <b> Elite Genitour<b> (<cost>) \\nBerber mercenary unit, available when teamed with a Berber player. Stronger than Genitour.<i> Upgrades: speed, hit points (Stable); attack, range, armor (Blacksmith); attack, accuracy (University); accuracy, armor (Archery Range); creation speed (Castle); more resistant to Monks (Monastery).<i> \\n<hp> <attack> <armor> <piercearmor> <range>";
			langReplacement[26190] = "Create <b> Imperial Skirmisher<b> (<cost>) \\nVietnamese mercenary unit, available when teamed with a Vietnamese player. Stronger than Elite Skirmisher. Attack bonus vs. archers. <i> Upgrades: attack, range, armor (Blacksmith); attack, accuracy (University); accuracy (Archery Range); creation speed (Castle); more resistant to Monks (Monastery).<i> \\n<hp> <attack> <armor> <piercearmor> <range>";
			langReplacement[26419] = "Create <b> Imperial Camel<b> (<cost>) \nUnique Indian upgrade. Stronger than Heavy Camel. Attack bonus vs. cavalry. <i> Upgrades: attack, armor (Blacksmith); speed, hit points (Stable); creation speed (Castle); more resistant to Monks (Monastery).<i> \n<hp> <attack> <armor> <piercearmor> <range>";
		}

		this->ui->label->setText((translation["working"]+"\n"+translation["workingPatches"]).c_str());
		this->ui->label->repaint();

		for (size_t i = 0, nbPatches = sizeof patchTab / sizeof (wololo::DatPatch); i < nbPatches; i++) {			
			patchTab[i].patch(&aocDat, &langReplacement);
			bar->setValue(bar->value()+1);bar->repaint(); //72-85
		}


		if(this->ui->replaceTooltips->isChecked()) {
			/*
			 * Load modded strings instead of normal HD strings into lang replacement
			 */
			std::ifstream modLang(modLangPath.string());
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
				boost::replace_all(line, "\n", "\\n");
				line = wstrtostr(outputLine);
				langReplacement[nb] = line;
			}
		}
		bar->setValue(bar->value()+1);bar->repaint(); //86


		std::ifstream langIn(keyValuesStringsPath.string());
		std::ofstream langOut(languageIniPath.string());
		genie::LangFile langDll;

		bool patchLangDll;
		if(this->ui->createExe->isChecked()) {
			if(!aocFound)
				patchLangDll = fs::exists(langDllPath);
			else {
				langDllPath = outPath / langDllPath;
				patchLangDll = fs::exists(langDllPath);
				if(patchLangDll)
				{
					if(fs::exists(langDllFile))
						fs::remove(langDllFile);
					fs::copy_file(langDllPath,langDllFile);
				}
			}
		} else {
			patchLangDll = false;
		}
		bar->setValue(bar->value()+1);bar->repaint(); //87
		bool dllPatched = true;
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
		bar->setValue(bar->value()+1);bar->repaint(); //88
		convertLanguageFile(&langIn, &langOut, &langDll, patchLangDll, &langReplacement);
		bar->setValue(bar->value()+1);bar->repaint(); //89
		if (patchLangDll) {
			try {
				line = translation["working"]+"\n"+translation["workingDll"];
				boost::replace_all(line,"<dll>",langDllFile.string());
				langDll.save();
				fs::copy_file(langDllFile,upDir/"data/"/langDllFile);
				fs::remove(langDllFile);
				this->ui->label->setText(line.c_str());
				this->ui->label->repaint();
			} catch (const std::ofstream::failure& e) {
				this->ui->label->setText(translation["workingError"].c_str());
				this->ui->label->repaint();
				try {
					langDll.save();
					fs::copy_file(langDllFile,upDir/"data/"/langDllFile);
					fs::remove(langDllFile);
					this->ui->label->setText(line.c_str());
					this->ui->label->repaint();
				} catch (const std::ofstream::failure& e) {
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

		bar->setValue(bar->value()+1);bar->repaint(); //90

		aocDat.saveAs(outputDatPath.string().c_str());		
		bar->setValue(bar->value()+1);bar->repaint(); //92

		this->ui->label->setText((translation["working"]+"\n"+translation["workingUp"]).c_str());
		this->ui->label->repaint();

		if (aocFound) {

			if(this->ui->createExe->isChecked()) {
				recCopy(vooblyDir / "Data", upDir / "Data");

				bar->setValue(bar->value()+1);bar->repaint(); //92
				if (!dllPatched) {
					dialog = new Dialog(this, translation["dialogNoDll"].c_str());
					dialog->exec();
				} else {
					fs::copy_file(UPExe, UPExeOut, fs::copy_option::overwrite_if_exists);

					bar->setValue(bar->value()+1);bar->repaint(); //93
					system(("\""+UPExeOut.string()+"\" -g:"+UPModdedExe).c_str());
					fs::copy_file(upDir/"Sound/music.m3u",vooblyDir/"Sound/music.m3u",fs::copy_option::overwrite_if_exists);

					bar->setValue(bar->value()+1);bar->repaint(); //94
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
			if(fs::exists(outPath/"/compatslp")) {
				if(fs::exists(outPath/"/compatslp2"))
					fs::remove_all(outPath/"/compatslp2");
				recCopy(outPath/"/compatslp",outPath/"/compatslp2");
				fs::remove_all(outPath/"/compatslp");
				this->ui->label->setText(translation["workingCP"].c_str());
				if(this->ui->createExe->isChecked()) { //this causes a crash with UP 1.5 otherwise
					if(fs::file_size(outPath/"/data/blendomatic.dat") < 400000) {
						fs::rename(outPath/"/data/blendomatic.dat",outPath/"/data/blendomatic.dat.bak");
						fs::rename(outPath/"/data/blendomatic_x1.dat",outPath/"/data/blendomatic.dat");
					}
				}				
				bar->setValue(bar->value()+1);bar->repaint();

			}
		} else {
			this->ui->label->setText(translation["workingNoAoc"].c_str());
			dialog = new Dialog(this,translation["dialogNoAoc"].c_str());
			dialog->exec();
		}
		bar->setValue(100);
		bar->repaint();
	}
	catch (std::exception const & e) {
		dialog = new Dialog(this,translation["dialogException"]+std::string()+e.what());
		dialog->exec();
		this->ui->label->setText(translation["error"].c_str());
		ret = 1;
	}
	catch (std::string const & e) {		
		dialog = new Dialog(this,translation["dialogException"]+e);
		dialog->exec();
		this->ui->label->setText(translation["error"].c_str());
		ret = 1;
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


