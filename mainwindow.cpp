#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include <cctype>
#include <string>
#include <windows.h>

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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include <QWhatsThis>
#include <QPoint>

namespace fs = boost::filesystem;

fs::path HDPath;
fs::path outPath;
std::string const version = "2.1";
std::string language;
std::map<std::string, std::string> translation;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	language = "en";
	ui->setupUi(this);
	changeLanguage(language);

	HDPath = getHDPath();
	outPath = getOutPath(HDPath);

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
		}
	} );
	
	this->ui->hotkeyTip->setIcon(QIcon("resources/question.png"));
	this->ui->hotkeyTip->setIconSize(QSize(16,16));
	this->ui->hotkeyTip->setWhatsThis("You can choose a hotkey file to use for the WololoKingdoms Mod. You have three options\n"
									  "1) Use current AoC/Voobly hotkeys: This looks for a player1.hki file in your AoE2 installation folder, "
									  "which is what would be used when playing standard AoC via Voobly. If it can't find such a file, it will use the default AoC hotkeys\n"
									  "2) Use HD hotkeys for this mod only: This looks for a player0.hki file in your HD installation folder and uses those for the WololoKingdoms mod\n"
									  "3) Use HD hotkeys for this mod and AoC: Same as 2), but this will use your current HD hotkeys for standard AoC via Voobly as well and overwrite your current ones\n\n"
									  "If you've run this installer before, you'll also get an option to keep the current hotkey setup.");
	QObject::connect( this->ui->hotkeyTip, &QPushButton::clicked, this, [this]() {
			QWhatsThis::showText(this->ui->hotkeyTip->mapToGlobal(QPoint(0,0)),this->ui->hotkeyTip->whatsThis());
	} );
	this->ui->tooltipTip->setIcon(QIcon("resources/question.png"));
	this->ui->tooltipTip->setIconSize(QSize(16,16));
	this->ui->tooltipTip->setWhatsThis("This replaces the tooltips of units, technologies and buildings that are shown in the tech tree or in-game with extended help (F1) enabled\n"
										"It provides some error corrections, and more information such as research/creation time, attack speed, attack boni and more\n"
										"This is available as a mod for Voobly (), selecting the checkbox will make it permanent (and available offline as well)\n"
										"To remove it, you'll need to run the installer again with this checkbox unchecked");
	QObject::connect( this->ui->tooltipTip, &QPushButton::clicked, this, [this]() {
			QWhatsThis::showText(this->ui->tooltipTip->mapToGlobal(QPoint(0,0)),this->ui->tooltipTip->whatsThis());
	} );
	this->ui->exeTip->setIcon(QIcon("resources/question.png"));
	this->ui->exeTip->setIconSize(QSize(16,16));
	this->ui->exeTip->setWhatsThis(("This creates a WK.exe in your " +outPath.string() +"age2_x1 folder that can be used for playing WololoKingdoms without Voobly"
									"When checked an extra window for the Userpatch will open during the installation. Chosse the options you want, run it, and then close it.").c_str());
	QObject::connect( this->ui->exeTip, &QPushButton::clicked, this, [this]() {
			QWhatsThis::showText(this->ui->exeTip->mapToGlobal(QPoint(0,0)),this->ui->exeTip->whatsThis());
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

void MainWindow::recCopy(fs::path const &src, fs::path const &dst, bool force) {
	// recursive copy
	//fs::path currentPath(current->path());
	if (fs::is_directory(src)) {
		if(!fs::exists(dst)) {
			fs::create_directories(dst);
		}
		for (fs::directory_iterator current(src), end;current != end; ++current) {
			fs::path currentPath(current->path());
			recCopy(currentPath, dst / currentPath.filename(), force);
		}
	}
	else {
		if (force)
			fs::copy_file(src, dst, fs::copy_option::overwrite_if_exists);
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
				dllOut->setString(nb, outputLine);
			}
		}

	}
}

void MainWindow::makeDrs(std::string const inputDir, std::string const moddedInputDir, std::ofstream *out) {

	this->ui->label->setText("Working...\n"
							 "Looking for files to copy...");
	this->ui->label->repaint();
	const int numberOfTables = 2; // slp and wav

	std::vector<std::string> slpFilesNames;
	std::vector<std::string> wavFilesNames;
	std::vector<std::string> moddedFilesNames;
	listAssetFiles(inputDir, &slpFilesNames, &wavFilesNames);
	listAssetFiles(moddedInputDir, &moddedFilesNames, NULL);

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
			size = fs::file_size(moddedInputDir + "/" + *modIt + ".slp");
			slp.file_id = stoi(*modIt);
			modIt++;
			it++;
		} else if (comp < 0) {
			size = fs::file_size(moddedInputDir + "/" + *modIt + ".slp");
			slp.file_id = stoi(*modIt);
			modIt++;
		} else {
			size = fs::file_size(inputDir + "/" + *it + ".slp");
			slp.file_id = stoi(*it);
			it++;
		}
		slp.file_data_offset = offset;
		slp.file_size = size;
		offset += size;
		slpFiles.push_back(slp);
	}
	for (std::vector<std::string>::iterator it = wavFilesNames.begin(); it != wavFilesNames.end(); it++) {
		wololo::DrsFileInfo wav;
		size_t size = fs::file_size(inputDir + "/" + *it + ".wav");
		wav.file_id = stoi(*it);
		wav.file_data_offset = offset;
		wav.file_size = size;
		offset += size;
		wavFiles.push_back(wav);
	}

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


	this->ui->label->setText("Working...\n"
							 "Write to gamedata_x1_p1.drs...");
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

	// file infos
	for (std::vector<wololo::DrsFileInfo>::iterator it = slpFiles.begin(); it != slpFiles.end(); it++) {
		out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
		out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
		out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));
	}

	for (std::vector<wololo::DrsFileInfo>::iterator it = wavFiles.begin(); it != wavFiles.end(); it++) {
		out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
		out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
		out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));
	}


	this->ui->label->setText("Working...\n"
							 "Copying files to gamedata_x1_p1.drs...");
	this->ui->label->repaint();
	// now write the actual files
	modIt = moddedFilesNames.begin();
	for (std::vector<std::string>::iterator it = slpFilesNames.begin(); it != slpFilesNames.end();) {
		int comp = (modIt != moddedFilesNames.end())?(*modIt).compare(*it):1;
		if(comp == 0) {
			std::ifstream srcStream = std::ifstream(moddedInputDir + "/" + *modIt + ".slp", std::ios::binary);
			*out << srcStream.rdbuf();
			it++;
			modIt++;
		} else if (comp < 0) {
			std::ifstream srcStream = std::ifstream(moddedInputDir + "/" + *modIt + ".slp", std::ios::binary);
			*out << srcStream.rdbuf();
			modIt++;
		} else {
			std::ifstream srcStream = std::ifstream(inputDir + "/" + *it + ".slp", std::ios::binary);
			*out << srcStream.rdbuf();
			it++;
		}

	}

	for (std::vector<std::string>::iterator it = wavFilesNames.begin(); it != wavFilesNames.end(); it++) {
		std::ifstream srcStream(inputDir + "/" + *it + ".wav", std::ios::binary);
		*out << srcStream.rdbuf();
	}
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
		fs::copy_file(inputDir / (civs[i] + ".mp3"), outputDir / (civs[i] + ".mp3"));
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

	/*
	//Copy Forest Terrains
	aocDat->TerrainBlock.TerrainsUsed2 = 42;
	aocDat->TerrainsUsed1 = 42;
	int terrainswaps[] = {15,48,16,49,26,50};
	for(size_t i = 0; i < 6; i=i+2) {
		aocDat->TerrainBlock.Terrains[terrainswaps[i]] = hdDat->TerrainBlock.Terrains[terrainswaps[i+1]];
		aocDat->TerrainBlock.Terrains[terrainswaps[i]].SLP = 15000;
		aocDat->TerrainBlock.Terrains[terrainswaps[i]].Name2 = "g_des";
		for(size_t j = 0; j < aocDat->TerrainRestrictions.size(); j++) {
			aocDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier[terrainswaps[i]] = hdDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier[terrainswaps[i+1]];
			aocDat->TerrainRestrictions[j].TerrainPassGraphics[terrainswaps[i]] = hdDat->TerrainRestrictions[j].TerrainPassGraphics[terrainswaps[i+1]];
		}
	}
	aocDat->TerrainBlock.Terrains[35].TerrainToDraw = -1;
	aocDat->TerrainBlock.Terrains[35].SLP = 15024;
	aocDat->TerrainBlock.Terrains[35].Name2 = "g_ice";
	int tNew = 41;
	int tOld = 56;
	aocDat->TerrainBlock.Terrains[tNew] = hdDat->TerrainBlock.Terrains[tOld];
	aocDat->TerrainBlock.Terrains[tNew].TerrainToDraw = 10;
	for(size_t i = 0; i < aocDat->TerrainRestrictions.size(); i++) {
		aocDat->TerrainRestrictions[i].PassableBuildableDmgMultiplier.push_back(hdDat->TerrainRestrictions[i].PassableBuildableDmgMultiplier[tOld]);
		aocDat->TerrainRestrictions[i].TerrainPassGraphics.push_back(hdDat->TerrainRestrictions[i].TerrainPassGraphics[tOld]);
	}
	*/

}

void MainWindow::hotkeySetup() {

	fs::path nfz1Path("resources/Player1.nfz");
	fs::path nfz2Path("resources/Player2.nfz");
	fs::path nfz3Path("resources/Player3.nfz");
	fs::path nfzPath = outPath / "player.nfz";
	fs::path aocHkiPath("resources/player1.hki");
	fs::path hkiPath = HDPath / ("Profiles/player0.hki");
	fs::path hkiOutPath = outPath / "player1.hki";
	fs::path hki2OutPath = outPath / "player2.hki";
	fs::path hki3OutPath = outPath / "player3.hki";
	fs::path modHkiOutPath = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms/player1.hki";
	fs::path modHki2OutPath = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms/player1.hki";
	fs::path nfzOutPath = outPath / "Voobly Mods/AOC/Data Mods/WololoKingdoms/Player.nfz";
	fs::path nfzUpOutPath = outPath / "Games/WololoKingdoms/Player.nfz";

	if(this->ui->hotkeyChoice->currentIndex() != 0) {
		fs::remove(nfzOutPath);
		fs::remove(nfzUpOutPath);
		if(fs::exists(nfzPath)) //Copy the Aoc Profile
			fs::copy_file(nfzPath, nfzOutPath);
		else //otherwise copy the default profile included
			fs::copy_file(nfz1Path, nfzOutPath);
	}
	if(this->ui->createExe->isChecked()) { //Profiles for UP
		if (this->ui->hotkeyChoice->currentIndex() == 1 || this->ui->hotkeyChoice->currentIndex() == 3) {		
			if(fs::exists(nfzPath)) //Copy the Aoc Profile
				fs::copy_file(nfzPath,nfzUpOutPath);
			else //otherwise copy the default profile included
				fs::copy_file(nfz1Path,nfzUpOutPath);
		} 
		if(this->ui->hotkeyChoice->currentIndex() == 2) { 
			fs::copy_file(nfz2Path,nfzUpOutPath);
		}
	} 
	//Copy hotkey files
	if (this->ui->hotkeyChoice->currentIndex() == 1 && !fs::exists(hkiOutPath))
		fs::copy_file(aocHkiPath, hkiOutPath);
	if (this->ui->hotkeyChoice->currentIndex() == 2) {		
		fs::copy_file(hkiPath, modHkiOutPath,fs::copy_option::overwrite_if_exists);
		if(fs::exists(hki2OutPath))
			fs::copy_file(hkiPath, modHki2OutPath,fs::copy_option::overwrite_if_exists);
		if(this->ui->createExe->isChecked()) {			
			if(fs::exists(hki2OutPath)) {
				fs::copy_file(hkiPath, hki3OutPath);
			} else {
				fs::copy_file(nfz3Path,nfzUpOutPath);
				fs::copy_file(hkiPath, hki2OutPath);
			}
		}
	}
	if(this->ui->hotkeyChoice->currentIndex() == 3) {	
		fs::copy_file(hkiOutPath, hkiOutPath/".bak",fs::copy_option::overwrite_if_exists);
		fs::copy_file(hkiPath, hkiOutPath,fs::copy_option::overwrite_if_exists);
		if(fs::exists(hki2OutPath)) {
			fs::copy_file(hki2OutPath, hki2OutPath/".bak",fs::copy_option::overwrite_if_exists);
			fs::copy_file(hkiPath, hki2OutPath,fs::copy_option::overwrite_if_exists);
		}
	}
}

int MainWindow::run()
{
	this->setEnabled(false);
	this->ui->label->setText("Working...");
	this->ui->label->repaint();
	qApp->processEvents();
	QDialog* dialog;
	int ret = 0;

	try {
		fs::path keyValuesStringsPath = HDPath / "resources/" / language / "/strings/key-value/key-value-strings-utf8.txt";
		fs::path vooblyDataModPath = outPath / ("Voobly Mods/AOC/Data Mods/");
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
		fs::path nfzOutPath = vooblyDir / "Player.nfz";
		fs::path nfzUpOutPath = outPath / "Games/WololoKingdoms/Player.nfz";
		fs::path langDllFile("language_x1_p1.dll");
		fs::path langDllPath = langDllFile;
		fs::path xmlOutPathUP = outPath / "Games/WK.xml";
		fs::path aiInputPath("resources/Script.Ai");
		fs::path mapInputPath("resources/Script.Rm");
		std::string drsOutPath = vooblyDir.string() + "Data/gamedata_x1_p1.drs";
		fs::path assetsPath = HDPath / "resources/_common/drs/gamedata_x2/";
		fs::path moddedAssetsPath("assets/");
		fs::path outputDatPath = vooblyDir / "Data/empires2_x1_p1.dat";
		fs::path upDir = outPath / "Games/WololoKingdoms/";
		std::string const UPModdedExe = "WK";
		fs::path UPExe("resources/SetupAoc.exe");
		fs::path UPExeOut = outPath / "SetupAoc.exe";
		fs::path pwInputDir("resources/pussywood");
		fs::path gridInputDir("resources/grid");
		fs::path wallsInputDir("resources/short_walls");
		
		fs::remove_all(moddedAssetsPath);
		fs::create_directories(moddedAssetsPath);

		if(this->ui->usePw->isChecked() || this->ui->useGrid->isChecked() || this->ui->useWalls->isChecked()) {
			this->ui->label->setText("Working...\n"
									 "Preparing modded replacement files...");
			this->ui->label->repaint();
		}
		if(this->ui->usePw->isChecked())
			recCopy(pwInputDir, moddedAssetsPath);
		if(this->ui->useGrid->isChecked())
			recCopy(gridInputDir, moddedAssetsPath);
		if(this->ui->useWalls->isChecked())
			recCopy(wallsInputDir, moddedAssetsPath);
	

		if(fs::exists(nfzOutPath)) //Avoid deleting profile files
			fs::rename(nfzOutPath, vooblyDataModPath/"player.nfz");
		if(fs::exists(nfzUpOutPath))
			fs::rename(nfzUpOutPath, outPath/"Games/player.nfz");
		fs::remove_all(vooblyDir);
		fs::remove_all(outPath/"Games/WololoKingdoms");
		fs::remove(outPath/"Games/WK.xml");
		fs::create_directories(vooblyDir/"Data");
		fs::create_directories(vooblyDir/"Sound/stream");
		fs::create_directories(vooblyDir/"Taunt");
		fs::create_directories(upDir);
		if(fs::exists(vooblyDataModPath/"player.nfz")) //copy back profile files if required
			fs::rename(vooblyDataModPath/"player.nfz", nfzOutPath);
		if(fs::exists(outPath/"Games/player.nfz"))
			fs::rename(outPath/"Games/player.nfz", nfzUpOutPath);
		fs::create_directories(upDir / "Data");

		this->ui->label->setText("Working...\n"
								 "Preparing resource files...");
		this->ui->label->repaint();
		std::ofstream versionOut(versionIniPath);
		versionOut << version << std::endl;
		copyCivIntroSounds(soundsInputPath / "civ/", soundsOutputPath / "stream/");
		createMusicPlaylist(soundsInputPath.string() + "music/", soundsOutputPath.string() + "music.m3u");
		recCopy(vooblyDir / "Sound", upDir / "Sound");
		recCopy(tauntInputPath, tauntOutputPath);
		recCopy(vooblyDir / "Taunt", upDir / "Taunt");

		hotkeySetup();

		/*
		if(fs::exists(vooblyDir/"player.nfz")) {
			recCopy(vooblyDir / "player.nfz", upDir / "player.nfz");
		}
		*/

		fs::copy_file(xmlPath, xmlOutPath);
		fs::copy_file(xmlPath, xmlOutPathUP);
		boolean aocFound = outPath != HDPath/"WololoKingdoms/out/";
		if (aocFound) {
			recCopy(outPath/"Random", vooblyDir/"Script.Rm");
		}
		recCopy(mapInputPath, vooblyDir/"Script.Rm");
		recCopy(vooblyDir / "Script.Rm", upDir / "Script.Rm");


		//If wanted, the BruteForce AI could be included as a "standard" AI.
		//recCopy(aiInputPath, vooblyDir/"Script.Ai");
		//recCopy(vooblyDir / "Script.Ai", upDir / "Script.Ai");

		this->ui->label->setText("Working...\n"
								 "Opening the AOC dat file...");
		this->ui->label->repaint();

		genie::DatFile aocDat;
		aocDat.setVerboseMode(true);
		aocDat.setGameVersion(genie::GameVersion::GV_TC);
		aocDat.load(aocDatPath.c_str());

		this->ui->label->setText("Working...\n"
								 "Opening the AOE2HD dat file...");
		this->ui->label->repaint();
		genie::DatFile hdDat;
		hdDat.setVerboseMode(true);
		hdDat.setGameVersion(genie::GameVersion::GV_Cysion);
		hdDat.load(hdDatPath.c_str());

		std::ofstream drsOut(drsOutPath, std::ios::binary);

		this->ui->label->setText("Working...\n"
								 "Fixing Interface Files...");
		this->ui->label->repaint();
		uglyHudHack(assetsPath.string(),moddedAssetsPath.string());
		makeDrs(assetsPath.string(), moddedAssetsPath.string(), &drsOut);

		fs::remove_all(moddedAssetsPath);

		this->ui->label->setText("Working...\n"
								 "Generating empires2_x1_p1.dat...");
		this->ui->label->repaint();
		transferHdDatElements(&hdDat, &aocDat);

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

		this->ui->label->setText("Working...\n"
							 "Applying DAT patches...");
		this->ui->label->repaint();
		for (size_t i = 0, nbPatches = sizeof patchTab / sizeof (wololo::DatPatch); i < nbPatches; i++) {			
			patchTab[i].patch(&aocDat, &langReplacement);
		}

		if(this->ui->replaceTooltips->isChecked()) {
			/*
			 * Load modded strings instead of normal HD strings into lang replacement
			 */
			std::string line;
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
					dllPatched = false;
					patchLangDll = false;
				}
			}
		}
		else {
			if(this->ui->createExe->isChecked()) {
				this->ui->label->setText(("Working...\n"
									  + langDllPath.string() + " not found, skipping dll patching for UserPatch.").c_str());
				this->ui->label->repaint();
			}
		}
		convertLanguageFile(&langIn, &langOut, &langDll, patchLangDll, &langReplacement);
		if (patchLangDll) {
			try {
				langDll.save();
				fs::copy_file(langDllFile,upDir/"data/"/langDllFile);
				fs::remove(langDllFile);
				this->ui->label->setText(("Working...\n"
								  + langDllFile.string() + " patched.").c_str());
				this->ui->label->repaint();
			} catch (const std::ofstream::failure& e) {
				this->ui->label->setText("Working...\n"
								 "Error, trying again");
				this->ui->label->repaint();
				try {
					langDll.save();
					fs::copy_file(langDllFile,upDir/"data/"/langDllFile);
					fs::remove(langDllFile);
					this->ui->label->setText(("Working...\n"
								  + langDllFile.string() + " patched.").c_str());
					this->ui->label->repaint();
				} catch (const std::ofstream::failure& e) {
					dllPatched = false;
					patchLangDll = false;
				}
			}
		}

		aocDat.saveAs(outputDatPath.string().c_str());

		this->ui->label->setText("Working...\n"
								 "Copying the files for UserPatch...");
		this->ui->label->repaint();

		recCopy(vooblyDir / "Data", upDir / "Data");



		if (aocFound) {

			if(this->ui->createExe->isChecked()) {
				if (!dllPatched) {
					dialog = new Dialog(this, "Couldn't read/write the language_x1_p1.dll file!\n"
											  "Try running the converter again, if it still doesn't work, your language_x1_p1.dll file may be corrupt\n"
											  "You can still play this via Voobly, but for offline play the converter needs a valid language_x1_p1.dll file to write to.");
					dialog->exec();
				} else {
					if(fs::exists(UPExeOut)) {
						if(fs::file_size(UPExe) != fs::file_size((UPExeOut))) {
							fs::remove(UPExeOut);
							fs::copy_file(UPExe, UPExeOut);
						}
					} else {
						fs::copy_file(UPExe, UPExeOut);
					}
					system(("\""+UPExeOut.string()+"\" -g:"+UPModdedExe).c_str());
					dialog = new Dialog(this,"Conversion complete! The WololoKingdoms mod is now available as a Voobly Mod and "
											 "as a separate " + UPModdedExe + ".exe in the age2_x1 folder.");
					dialog->exec();
				}
			} else {
				dialog = new Dialog(this,"Conversion complete! The WololoKingdoms mod is now part of your AoC installation.");
				dialog->exec();
			}
			this->ui->label->setText("Done!");
			if(fs::exists(outPath/"/compatslp")) {
				if(fs::exists(outPath/"/compatslp2"))
					fs::remove_all(outPath/"/compatslp2");
				recCopy(outPath/"/compatslp",outPath/"/compatslp2");
				fs::remove_all(outPath/"/compatslp");
				this->ui->label->setText("Done! NOTE: To make this mod work with the HD compatibility patch, the 'compatslp' folder has been renamed (to 'compatslp2').\n"
										 "Voobly will give you an error message that the game is not correctly installed when joining a lobby, but that can safely be ignored.");
				if(this->ui->createExe->isChecked()) { //this causes a crash with UP 1.5 otherwise
					if(fs::file_size(outPath/"/data/blendomatic.dat") < 400000) {
						fs::rename(outPath/"/data/blendomatic.dat",outPath/"/data/blendomatic.dat.bak");
						fs::rename(outPath/"/data/blendomatic_x1.dat",outPath/"/data/blendomatic.dat");
					}
				}

			}
		} else {
			this->ui->label->setText("Done! Open the \"out/\" folder and put its contents into your AoE2 folder to make it work.");
			dialog = new Dialog(this,"Conversion complete. Installer did not find your AoE2 installation - \n"
						"open the \"out/\" folder and put its contents into your AoE2 folder to make it work.");			
			dialog->exec();
		}
	}
	catch (std::exception const & e) {
		dialog = new Dialog(this,"There was an exception! Please pass this error on to Jineapple:"+std::string()+e.what());		
		dialog->exec();
		ret = 1;
	}
	catch (std::string const & e) {		
		dialog = new Dialog(this,"There was an exception! Please pass this error on to Jineapple: "+e);		
		dialog->exec();
		ret = 1;
	}

	this->setEnabled(true);
	this->ui->label->repaint();
	return ret;
}


