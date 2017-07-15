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
#include "sdk/public/steam/steam_api.h"

#include "JlCompress.h"

namespace fs = boost::filesystem;

fs::path HDPath;
fs::path outPath;
std::map<int, fs::path> slpFiles;
std::map<int, fs::path> wavFiles;
std::map<std::string,fs::path> newTerrainFiles;
std::vector<fs::path> existingMapNames;
std::vector<std::pair<int,std::string>> rmsCodeStrings;
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
	std::string steamPath = getSteamPath();
	HDPath = getHDPath(steamPath);
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
	if(!fs::exists("resources/"+language+".ini")) {
		this->ui->replaceTooltips->setEnabled(false);
		this->ui->replaceTooltips->setChecked(false);
	} else {
		this->ui->replaceTooltips->setEnabled(true);
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

void MainWindow::indexDrsFiles(fs::path const &src) {
	// Index files to be written to the drs
	if (fs::is_directory(src)) {
		for (fs::directory_iterator current(src), end;current != end; ++current) {
			fs::path currentPath(current->path());
			indexDrsFiles(currentPath);
		}
	}
	else {
		std::string extension = src.extension().string();
		if (extension == ".slp") {
			int id = atoi(src.stem().string().c_str());
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
	 */
	if (generateLangDll) {
		for(std::vector<std::pair<int,std::string>>::iterator it = rmsCodeStrings.begin(); it != rmsCodeStrings.end(); it++) {
			dllOut->setString(it->first, it->second);
		}
	}
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
	}
	bar->setValue(bar->value()+1);bar->repaint(); //66

	for (std::map<int,fs::path>::iterator it = wavFiles.begin(); it != wavFiles.end(); it++) {
		std::ifstream srcStream = std::ifstream(it->second.string(), std::ios::binary);
		*out << srcStream.rdbuf();
	}
	bar->setValue(bar->value()+1);bar->repaint(); //67
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
		for (size_t i = 1; i <= 22; i++) {
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
		if(fs::exists(outputDir/it->filename())) {
			if(replace)
				fs::remove(outputDir/it->filename());
			else
				continue;
		}
		if(fs::exists(outputDir/("ZR@"+it->filename().string()))) {
			if(replace)
				fs::remove(outputDir/("ZR@"+it->filename().string()));
			else
				continue;
		}
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
	terrainSwap(hdDat, aocDat, 15,55,15012); //mangrove forest
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
	aocDat->Graphics[aocDat->Graphics[aocDat->Civs[27].Units[129].StandingGraphic.first].Deltas[0].GraphicID].FrameCount = 40;
	aocDat->Graphics[aocDat->Graphics[aocDat->Civs[27].Units[130].StandingGraphic.first].Deltas[1].GraphicID].FrameCount = 40;

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
			slpFiles[newSLP] = HDPath/("resources/_common/drs/graphics/776.slp");
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
	slpFiles.clear();
	wavFiles.clear();
	newTerrainFiles.clear();

	try {
		fs::path keyValuesStringsPath = HDPath / "resources/" / language / "/strings/key-value/key-value-strings-utf8.txt";
		fs::path vooblyDir = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms/";
		std::string aocDatPath = HDPath.string() + "resources/_common/dat/empires2_x1_p1.dat";
		std::string hdDatPath = HDPath.string() + "resources/_common/dat/empires2_x2_p1.dat";
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
		fs::path terrainOverrideDir("new_terrain_override/");
		fs::path gridNoSnowInputDir("resources/Grid No Snow");
		fs::path noSnowInputDir("resources/No Snow");
		fs::path wallsInputDir("resources/short_walls");
		fs::path gamedata_x1("resources/gamedata_x1.drs");

		std::string line;

		bar->setValue(1);bar->repaint(); //1

		indexDrsFiles(assetsPath);

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
			bar->setValue(bar->value()+1);bar->repaint(); //4
			if(this->ui->useNoSnow->isChecked())
				indexDrsFiles(gridNoSnowInputDir);
			bar->setValue(bar->value()+1);bar->repaint(); //5
		} else {
			indexTerrainFiles(newTerrainInputDir);
			bar->setValue(bar->value()+1);bar->repaint();//3
			if(this->ui->useNoSnow->isChecked())
				indexDrsFiles(noSnowInputDir);
			bar->setValue(bar->value()+2);bar->repaint(); //5
		}
		if(!fs::is_empty(terrainOverrideDir)) {
			indexTerrainFiles(terrainOverrideDir);
		}
		bar->setValue(bar->value()+1);bar->repaint(); //6
		if(this->ui->useWalls->isChecked())
			copyWallFiles(wallsInputDir);
		bar->setValue(bar->value()+1);bar->repaint(); //7
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
		fs::path vooblyMapDir = vooblyDir/"Script.Rm";
		copyHDMaps(HDPath/"resources/_common/random-map-scripts/", vooblyMapDir);
		bar->setValue(bar->value()+1);bar->repaint(); //20
		if(this->ui->copyMaps->isChecked())
			copyHDMaps("resources/Script.Rm/", vooblyMapDir, true);
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
			recCopy(vooblyDir / "Script.Rm", upDir / "Script.Rm", false, true);
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
		uglyHudHack(assetsPath);
		bar->setValue(bar->value()+1);bar->repaint(); //43?

		this->ui->label->setText((translation["working"]+"\n"+translation["workingDat"]).c_str());
		this->ui->label->repaint();
		transferHdDatElements(&hdDat, &aocDat);
		bar->setValue(bar->value()+1);bar->repaint(); //44?

		patchArchitectures(&aocDat);
		bar->setValue(bar->value()+1);bar->repaint(); //59
		if(!fs::is_empty(modOverrideDir))
			indexDrsFiles(modOverrideDir);
		bar->setValue(bar->value()+1);bar->repaint(); //60
		makeDrs(&drsOut);
		bar->setValue(bar->value()+1);bar->repaint(); //71
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
		langReplacement[10622] = translation["10622"];
		langReplacement[10642] = translation["10642"];
		langReplacement[10648] = translation["10648"];
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

		this->ui->label->setText((translation["working"]+"\n"+translation["workingPatches"]).c_str());
		this->ui->label->repaint();

		for (size_t i = 0, nbPatches = sizeof patchTab / sizeof (wololo::DatPatch); i < nbPatches; i++) {			
			patchTab[i].patch(&aocDat, &langReplacement);
			bar->setValue(bar->value()+1);bar->repaint(); //72-85
		}

		std::ifstream missingStrings("resources/missing_strings.txt");
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

		if(this->ui->replaceTooltips->isChecked()) {
			/*
			 * Load modded strings instead of normal HD strings into lang replacement
			 */
			std::ifstream modLang("resources/"+language+".ini");
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
			}
			if(!fs::exists(outPath/"/data/Load")) {
				fs::create_directory(outPath/"data/Load");
			}
			if(outPath==HDPath && this->ui->createExe->isChecked()) { //this causes a crash with UP 1.5 otherwise
				this->ui->label->setText(translation["workingCP"].c_str());
				this->ui->label->repaint();
				if(fs::file_size(outPath/"/data/blendomatic.dat") < 400000) {
					fs::rename(outPath/"/data/blendomatic.dat",outPath/"/data/blendomatic.dat.bak");
					fs::rename(outPath/"/data/blendomatic_x1.dat",outPath/"/data/blendomatic.dat");
				}
				bar->setValue(bar->value()+1);bar->repaint();
			}
		} else {
			this->ui->label->setText(translation["workingNoAoc"].c_str());
			dialog = new Dialog(this,translation["dialogNoAoc"].c_str(),translation["errorTitle"]);
			dialog->exec();
		}
		bar->setValue(100);
		bar->repaint();
	}
	catch (std::exception const & e) {
		dialog = new Dialog(this,translation["dialogException"]+std::string()+e.what(),translation["errorTitle"]);
		dialog->exec();
		this->ui->label->setText(translation["error"].c_str());
		ret = 1;
	}
	catch (std::string const & e) {		
		dialog = new Dialog(this,translation["dialogException"]+e,translation["errorTitle"]);
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


