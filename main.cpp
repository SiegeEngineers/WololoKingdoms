#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include <cctype>
#include <string>

#include <windows.h>
#include <tchar.h>
#include <string>
#include <assert.h>
#include <locale>
// #include <codecvt>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "genie/dat/DatFile.h"
#include "genie/lang/LangFile.h"
#include "paths.h"
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

std::string const version = "2.1";

void recCopy(boost::filesystem::path const &src, boost::filesystem::path const &dst) {
	// recursive copy
	//boost::filesystem::path currentPath(current->path());
	if (boost::filesystem::is_directory(src)) {
		if(!boost::filesystem::exists(dst)) {
			boost::filesystem::create_directories(dst);
		}
		for (boost::filesystem::directory_iterator current(src), end;current != end; ++current) {
			boost::filesystem::path currentPath(current->path());
			recCopy(currentPath, dst / currentPath.filename());
		}
	}
	else {
		boost::filesystem::copy_file(src, dst);
	}

}

void listAssetFiles(std::string const path, std::vector<std::string> *listOfSlpFiles, std::vector<std::string> *listOfWavFiles) {
	const std::set<std::string> exclude = {
		// Exclude Forgotten Empires leftovers
		"50163", // Forgotten Empires loading screen
		"50189", // Forgotten Empires main menu
		"53207", // Forgotten Empires in-game logo
		"53208", // Forgotten Empires objective window
		"53209" // ???
	};

	for (boost::filesystem::directory_iterator end, it(path); it != end; it++) {
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

DWORD ConvertUnicode2CP(const wchar_t *szText, std::string &resultString, UINT codePage = CP_ACP)
{
  resultString.clear();
  if (wcslen(szText) <= 0)
	return ERROR_SUCCESS;
  int iRes = WideCharToMultiByte(codePage, 0, szText, -1, NULL, 0, NULL, NULL);
  if (iRes <= 0)
	return GetLastError();
  char *szTemp = new char[iRes];

  iRes = WideCharToMultiByte(codePage, 0, szText, -1, szTemp, iRes, NULL, NULL);
  if (iRes <= 0)
  {
	delete [] szTemp;
	return GetLastError();
  }

  resultString = szTemp;
  delete [] szTemp;
  return ERROR_SUCCESS;
}

DWORD ConvertCP2Unicode(const char *szText, std::wstring &resultString, UINT codePage = CP_ACP)
{
  resultString.clear();
  if (strlen(szText) <= 0)
	return ERROR_SUCCESS;
  int iRes = MultiByteToWideChar(codePage, 0, szText, -1, NULL, 0);
  if (iRes <= 0)
	return GetLastError();
  wchar_t *szTemp = new wchar_t[iRes];

  iRes = MultiByteToWideChar(codePage, 0, szText, -1, szTemp, iRes);
  if (iRes <= 0)
  {
	delete [] szTemp;
	return GetLastError();
  }

  resultString = szTemp;
  delete [] szTemp;
  return ERROR_SUCCESS;
}

std::wstring strtowstr( const std::string& as )
{
			// deal with trivial case of empty string
	if( as.empty() )    return std::wstring();

			// determine required length of new string
	size_t reqLength = ::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), (int)as.length(), 0, 0 );

			// construct new string of required length
	std::wstring ret( reqLength, L'\0' );

			// convert old string to new string
	::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), (int)as.length(), &ret[0], (int)ret.length() );

			// return new string ( compiler should optimize this away )
	return ret;
}

std::string wstrtostr( const std::wstring& as )
{
			// deal with trivial case of empty string
	if( as.empty() )    return std::string();

			// determine required length of new string
	size_t reqLength = ::WideCharToMultiByte( CP_UTF8, 0, as.c_str(), (int)as.length(), 0, 0, 0, 0 );

			// construct new string of required length
	std::string ret( reqLength, L'\0' );

			// convert old string to new string
	::WideCharToMultiByte( CP_UTF8, 0, as.c_str(), (int)as.length(), &ret[0], (int)ret.length(), 0, 0 );

			// return new string ( compiler should optimize this away )
	return ret;
}

void convertLanguageFile(std::ifstream *in, std::ofstream *iniOut, genie::LangFile *dllOut, bool generateLangDll, std::map<int, std::string> *langReplacement) {
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
			if(nb >= 6080 && nb <= 6152 && (nb%20 <= 2)) {
				continue; //Need those lines for AI names
			}
			if (nb >= 20150 && nb <= 20167) {
				// skip the old civ descriptions
				continue;
			}
			if (nb >= 106000 && nb <= 106152) { // descriptions of the civs in the expansion
				// replace the old descriptions of the civs in the base game
				nb -= 100000;
				if(nb == 6060) { //Only space for 7 Berber AI names
					line.replace(8,2,"7");
					number = std::to_string(nb);
				} else if (nb >= 6068 && nb <= 6072) { //Skip those above 7 for Berbers
					continue;
				} else if (nb < 6080) { //All other AK Ai names are fine
					number = std::to_string(nb);
					/* AoR Ai names clash with "create unit x" lines when extended text is switched off
					 * Those are not suuper important though, so each civ gets 2 Ai names anyway
					 * Hopefully not more than 2 Ais will have the same aor civ in one game
					 * */
				} else if(nb%20 == 0) {
					line.replace(8,2,"2");
					number = std::to_string(nb);
				} else if (nb%20 == 1 || nb%20 == 2) {
					number = std::to_string(nb);
				} else continue; //Skip other Ai names
			}
			if (nb >= 120150 && nb <= 120180) { // descriptions of the civs in the expansion
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

		//boost::replace_all(line, "·", "\xb7"); // Workaround for UCS-2 to UTF-8 conversion, not needed anymore?

		//convert UTF-8 into ANSI

		std::wstring wideLine = strtowstr(line);
		std::string outputLine;
		ConvertUnicode2CP(wideLine.c_str(), outputLine, CP_ACP);

		*iniOut << number << '=' << outputLine <<  std::endl;

		if (generateLangDll) {
			boost::replace_all(line, "\\n", "\n"); // the dll file requires actual line feed, not escape sequences
			try {
				dllOut->setString(nb, line);
			}
			catch (std::string const & e) {
				boost::replace_all(line, "·", "-"); // non-english dll files don't seem to like that character
				dllOut->setString(nb, line);
			}
		}

	}
}

void makeDrs(std::string const inputDir ,std::ofstream *out) {
	const int numberOfTables = 2; // slp and wav

	std::vector<std::string> slpFilesNames;
	std::vector<std::string> wavFilesNames;
	listAssetFiles(inputDir, &slpFilesNames, &wavFilesNames);

	int numberOfSlpFiles = slpFilesNames.size();
	int numberOfWavFiles = wavFilesNames.size();
	int offsetOfFirstFile = sizeof (wololo::DrsHeader) +
			sizeof (wololo::DrsTableInfo) * numberOfTables +
			sizeof (wololo::DrsFileInfo) * (numberOfSlpFiles + numberOfWavFiles);
	int offset = offsetOfFirstFile;


	// file infos

	std::vector<wololo::DrsFileInfo> slpFiles;
	std::vector<wololo::DrsFileInfo> wavFiles;

	for (std::vector<std::string>::iterator it = slpFilesNames.begin(); it != slpFilesNames.end(); it++) {
		wololo::DrsFileInfo slp;
		size_t size = boost::filesystem::file_size(inputDir + "/" + *it + ".slp");
		slp.file_id = stoi(*it);
		slp.file_data_offset = offset;
		slp.file_size = size;
		offset += size;
		slpFiles.push_back(slp);
	}
	for (std::vector<std::string>::iterator it = wavFilesNames.begin(); it != wavFilesNames.end(); it++) {
		wololo::DrsFileInfo wav;
		size_t size = boost::filesystem::file_size(inputDir + "/" + *it + ".wav");
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

	// now write the actual files
	for (std::vector<std::string>::iterator it = slpFilesNames.begin(); it != slpFilesNames.end(); it++) {
		std::ifstream srcStream(inputDir + "/" + *it + ".slp", std::ios::binary);
		*out << srcStream.rdbuf();
	}

	for (std::vector<std::string>::iterator it = wavFilesNames.begin(); it != wavFilesNames.end(); it++) {
		std::ifstream srcStream(inputDir + "/" + *it + ".wav", std::ios::binary);
		*out << srcStream.rdbuf();
	}
}

void uglyHudHack(std::string const inputDir) {
	/*
	 * We have more than 30 civs, so we need to space the interface files further apart
	 * This adds +10 for each gap between different file types
	 */
	int const HudFiles[] = {51131, 51161};
	for (size_t baseIndex = sizeof HudFiles / sizeof (int); baseIndex >= 1; baseIndex--) {
		for (int i = 22; i >= 0; i--) {
			std::string dst = inputDir + std::to_string(HudFiles[baseIndex-1]+i+baseIndex*10) + ".slp";
			if (! (boost::filesystem::exists(dst) && boost::filesystem::file_size(dst) > 0)) {
				std::string src = inputDir + std::to_string(HudFiles[baseIndex-1]+i) + ".slp";
				boost::filesystem::rename(src, dst);
			}
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
			std::string dst = inputDir + std::to_string(slavHudFiles[baseIndex]+i) + ".slp";
			if (! (boost::filesystem::exists(dst) && boost::filesystem::file_size(dst) > 0)) {
				std::string src = inputDir + std::to_string(slavHudFiles[baseIndex]) + ".slp";
				boost::filesystem::copy_file(src, dst);
			}
		}
	}
}

void cleanTheUglyHudHack(std::string const inputDir) {
	int const slavHudFiles[] = {51123, 51163, 51203};
	for (size_t baseIndex = 0; baseIndex < sizeof slavHudFiles / sizeof (int); baseIndex++) {
		for (size_t i = 1; i <= 8; i++) {
			boost::filesystem::remove(inputDir + std::to_string(slavHudFiles[baseIndex]+i) + ".slp");
		}
	}
	int const HudFiles[] = {51141, 51181};
	for (size_t baseIndex = 1; baseIndex <= sizeof HudFiles / sizeof (int); baseIndex++) {
		for (int i = 0; i <= 22; i++) {
			std::string dst = inputDir + std::to_string(HudFiles[baseIndex-1]+i-baseIndex*10) + ".slp";
			if (! (boost::filesystem::exists(dst) && boost::filesystem::file_size(dst) > 0)) {
				std::string src = inputDir + std::to_string(HudFiles[baseIndex-1]+i) + ".slp";
				boost::filesystem::rename(src, dst);
			}
		}
	}

}

void copyCivIntroSounds(std::string const inputDir, std::string const outputDir) {
	std::string const civs[] = {"italians", "indians", "incas", "magyars", "slavs",
								"portuguese", "ethiopians", "malians", "berbers", "burmese", "malay", "vietnamese", "khmer"};
	for (size_t i = 0; i < sizeof civs / sizeof (std::string); i++) {
		boost::filesystem::copy_file(inputDir + civs[i] + ".mp3", outputDir + civs[i] + ".mp3");
	}
}

std::string tolower(std::string line) {
	std::transform(line.begin(), line.end(), line.begin(), static_cast<int(*)(int)>(std::tolower));
	return line;
}

void createMusicPlaylist(std::string inputDir, std::string const outputDir) {
	boost::replace_all(inputDir, "/", "\\");
	std::ofstream outputFile(outputDir);
	for (int i = 1; i <= 23; i++ ) {
		outputFile << inputDir << "xmusic" << std::to_string(i) << ".mp3" <<  std::endl;
	}
}

void transferHdDatElements(genie::DatFile *hdDat, genie::DatFile *aocDat) {
	aocDat->Sounds = hdDat->Sounds;
	aocDat->GraphicPointers = hdDat->GraphicPointers;
	aocDat->Graphics = hdDat->Graphics;
	aocDat->Techages = hdDat->Techages;
	aocDat->UnitHeaders = hdDat->UnitHeaders;
	aocDat->Civs = hdDat->Civs;
	aocDat->Researchs = hdDat->Researchs;
	aocDat->UnitLines = hdDat->UnitLines;
	aocDat->TechTree = hdDat->TechTree;
}

void hotkeySetup(std::string const HDPath, std::string const outPath) {

	//TODO make the confusing logic clearer

	std::string const nfz1Path = "Player1.nfz";
	std::string const nfz2Path = "Player2.nfz";
	std::string const aocHkiPath = "player1.hki";
	std::string const hkiPath = HDPath + "Profiles/player0.hki";
	std::string const hki1OutPath = outPath +  "player1.hki";
	std::string const hki2OutPath = outPath +  "player2.hki";
	std::string const nfzOutPath = outPath +  "Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Player.nfz";

	std::cout << "Setting up Hotkeys..." << std::endl;
	std::string line = ""; //stores the most recent line of input

	if(boost::filesystem::exists(nfzOutPath)) {
		std::cout << "It looks like you have set up your hotkeys for this mod before." << std::endl;
		std::cout << "Do you want to keep your current hotkey setup?" << std::endl;

		std::cout << "Type y or n (Yes/No) to continue." << std::endl;
		while(std::getline(std::cin, line)) {
			if(tolower(line) == "n") {
				boost::filesystem::remove(nfzOutPath);
				break;
			}
			else if (tolower(line) == "y") break;
			std::cout << "Type y or n (Yes/No) to continue." << std::endl;
		}
	}
	if(line != "y") {
		std::cout << "Do you want to use your current AoC/Voobly Hotkeys for this mod?" << std::endl;
		std::cout << "Note that this means new units might not have hotkeys or different ones that you expect!" << std::endl;
		std::cout << "When answering no, this program will copy your current HD edition hotkeys instead" << std::endl;

		std::cout << "Type y or n (Yes/No) to continue." << std::endl;
		while(std::getline(std::cin, line)) {
			if(tolower(line) == "y" || tolower(line) == "n") break;
			std::cout << "Type y or n (Yes/No) to continue." << std::endl;
		}

		if(line == "y") {
			boost::filesystem::copy_file(nfz1Path, nfzOutPath);
			if(!boost::filesystem::exists(hki1OutPath))
				boost::filesystem::copy_file(aocHkiPath, hki1OutPath);
			line = "";
		} else {
			std::cout << "Do you want to use your current HD edition Hotkeys for (Voobly) AoC as well?" << std::endl;
			std::cout << "WARNING! This will overwrite your current hotkey file if you have one!" << std::endl;

			std::cout << "Type y or n (Yes/No) to continue." << std::endl;
			while(std::getline(std::cin, line)) {
				if(tolower(line) == "y" || tolower(line) == "n") break;
				std::cout << "Type y or n (Yes/No) to continue." << std::endl;
			}
		}
		if(line == "y") {
			boost::filesystem::copy_file(nfz1Path, nfzOutPath);
			if(boost::filesystem::exists(hki1OutPath))
				boost::filesystem::remove(hki1OutPath);
			boost::filesystem::copy_file(hkiPath, hki1OutPath);
		} else if (line == "n") {
			boost::filesystem::copy_file(nfz2Path, nfzOutPath);
			if(!boost::filesystem::exists(hki2OutPath)) {
				boost::filesystem::copy_file(hkiPath, hki2OutPath);
				line == "y";
			} else {
				std::cout << "WARNING! This will overwrite one ofyour current hotkey files (player2.hki)!" << std::endl;
				std::cout << "Do you want to continue anyway?" << std::endl;
				std::cout << "Type y or n (Yes/No) to continue." << std::endl;
				while(std::getline(std::cin, line))
				{
					std::cout << "Type y or n (Yes/No) to continue." << std::endl;
					if(tolower(line) == "y") { //Asking the inverted question is clearer here, so the logic has to inverted again
						line = "n";
						break;
					} else if (tolower(line) == "n") {
						line = "y";
						break;
					}
				}
			}
		}
		if(tolower(line) == "n") {
			boost::filesystem::remove(hki2OutPath);
			boost::filesystem::copy(hkiPath, hki2OutPath);
		}
	}
	std::cout << "Hotkey setup done." << std::endl;
}

int main(int argc, char *argv[])
{
	//(de)activate some stuff for debugging (bit ugly)
	bool debug = false;

	int ret = 0;

	try {

		std::string HDPath = getHDPath();
		std::string outPath = getOutPath(HDPath);
		std::string const aocDatPath = HDPath + "resources/_common/dat/empires2_x1_p1.dat";
		std::string const hdDatPath = HDPath + "resources/_common/dat/empires2_x2_p1.dat";
		std::string const keyValuesStringsPath = HDPath + "resources/en/strings/key-value/key-value-strings-utf8.txt"; // TODO pick other languages
		std::string const modLangPath = "language.ini";
		std::string const languageIniPath = outPath + "Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/language.ini";
		std::string const versionIniPath = outPath +  "Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/version.ini";
		std::string const soundsInputPath =HDPath + "resources/_common/sound/";
		std::string const soundsOutputPath = outPath + "Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Sound/";
		std::string const tauntInputPath = HDPath + "resources/en/sound/taunt/";
		std::string const tauntOutputPath = outPath + "Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Taunt/";
		std::string const xmlPath = "WK_African_Kingdoms.xml";
		std::string const xmlOutPath = outPath +  "Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/age2_x1.xml";
		std::string const nfzOutPath = outPath +  "Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Player.nfz";
		std::string const langDllFile = "language_x1_p1.dll";
		std::string langDllPath = langDllFile;
		std::string const xmlOutPathUP = outPath +  "Games/WK_African_Kingdoms.xml";
		std::string const aiInputPath = "Script.Ai";
		std::string const drsOutPath = outPath + "Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Data/gamedata_x1_p1.drs";
		std::string const assetsPath = HDPath + "resources/_common/drs/gamedata_x2/";
		std::string const outputDatPath = outPath + "Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Data/empires2_x1_p1.dat";
		std::string const vooblyDir = outPath + "Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/";
		std::string const uPDIR = outPath + "Games/WK_African_Kingdoms/";
		std::string const UPModdedExe = "WK_African_Kingdoms";
		std::string const UPExe = "SetupAoc.exe";


		std::cout << "WololoKingdoms ver. " << version << std::endl;
		if(boost::filesystem::exists(nfzOutPath)) { //Avoid deleting Player.nfz
			boost::filesystem::remove(outPath+"Voobly Mods/AOC/Data Mods/player.nfz");
			boost::filesystem::copy_file(nfzOutPath, outPath+"Voobly Mods/AOC/Data Mods/player.nfz");
		}
		boost::filesystem::remove_all(outPath+"Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms");
		boost::filesystem::remove_all(outPath+"Games/WK_African_Kingdoms");
		boost::filesystem::remove(outPath+"Games/WK_African_Kingdoms.xml");
		boost::filesystem::create_directories(outPath+"Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Data");
		boost::filesystem::create_directories(outPath+"Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Sound/stream");
		boost::filesystem::create_directories(outPath+"Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Taunt");
		if(boost::filesystem::exists(outPath+"Voobly Mods/AOC/Data Mods/player.nfz")) {
			boost::filesystem::copy_file(outPath+"Voobly Mods/AOC/Data Mods/player.nfz", nfzOutPath);
			boost::filesystem::remove(outPath+"Voobly Mods/AOC/Data Mods/player.nfz");
		}

		std::cout << "Preparing resource files..." << std::endl;
		std::ofstream versionOut(versionIniPath);
		versionOut << version << std::endl;
		copyCivIntroSounds(soundsInputPath + "civ/", soundsOutputPath + "stream/");
		createMusicPlaylist(soundsInputPath + "music/", soundsOutputPath + "music.m3u");
		recCopy(vooblyDir + "Sound", uPDIR + "Sound");
		recCopy(tauntInputPath, tauntOutputPath);
		recCopy(vooblyDir + "Taunt", uPDIR + "Taunt");

		//HOTKEYS disabled for debug
		if(!debug)
			hotkeySetup(HDPath, outPath);

		boost::filesystem::copy_file(xmlPath, xmlOutPath);
		boost::filesystem::copy_file(xmlPath, xmlOutPathUP);
		boolean aocFound = outPath != HDPath+"WololoKingdoms/out/";
		if (aocFound) {
			recCopy(outPath+"Random", vooblyDir+"Script.Rm");
			recCopy(vooblyDir + "Script.Rm", uPDIR + "Script.Rm");
		}
		//If wanted, the BruteForce AI could be included as a "standard" AI.
		//recCopy(aiInputPath, vooblyDir+"Script.Ai");
		//recCopy(vooblyDir + "Script.Ai", uPDIR + "Script.Ai");

		std::cout << "Opening the AOC dat file..." << std::endl << std::endl;

		genie::DatFile aocDat;
		aocDat.setVerboseMode(true);
		aocDat.setGameVersion(genie::GameVersion::GV_TC);
		aocDat.load(aocDatPath.c_str());

		std::cout << std::endl << "Opening the AOE2HD dat file..." << std::endl << std::endl;
		genie::DatFile hdDat;
		hdDat.setVerboseMode(true);
		hdDat.setGameVersion(genie::GameVersion::GV_Cysion);
		hdDat.load(hdDatPath.c_str());

		std::ofstream drsOut(drsOutPath, std::ios::binary);

		std::cout << "Generating gamedata_x1_p1.drs..." << std::endl;
		uglyHudHack(assetsPath);
		makeDrs(assetsPath, &drsOut);
		cleanTheUglyHudHack(assetsPath);


		std::cout << "Generating empires2_x1_p1.dat..." << std::endl;
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
			wololo::ai900UnitIdFix,
			wololo::feitoriaFix
		};


		std::map<int, std::string> langReplacement;
		langReplacement[20162] = "Infantry civilization \\n\\n· Infantry move 15% faster \\n· Lumberjacks work 15% faster \\n· Siege weapons fire 20% faster \\n· Can convert sheep even if enemy units are next to them. \\n\\n<b>Unique Unit:<b> Woad Raider (infantry) \\n\\n<b>Unique Techs:<b> Stronghold (Castles and towers fire 20% faster); Furor Celtica (Siege Workshop units have +40% HP)\\n\\n<b>Team Bonus:<b> Siege Workshops work 20% faster";
		langReplacement[20166] = "Cavalry civilization \\n\\n· Do not need houses, but start with -100 wood \\n· Cavalry Archers cost -10% Castle, -20% Imperial Age \\n· Trebuchets +35% accuracy against units \\n\\n<b>Unique Unit:<b> Tarkan (cavalry) \\n\\n<b>Unique Techs:<b> Marauders (Create Tarkans at stables); Atheism (+100 years Relic, Wonder victories; Spies/Treason costs -50%)\\n\\n<b>Team Bonus:<b> Stables work 20% faster";
		langReplacement[20170] = "Infantry civilization \\n\\n· Start with a free llama \\n· Villagers affected by Blacksmith upgrades \\n· Houses support 10 population \\n· Buildings cost -15% stone\\n\\n<b>Unique Units:<b> Kamayuk (infantry), Slinger (archer)\\n\\n<b>Unique Techs:<b> Andean Sling (Skirmishers and Slingers no minimum range); Couriers (Kamayuks, Slingers, Eagles +1 armor/+2 pierce armor)\\n\\n<b>Team Bonus:<b> Farms built 2x faster";
		langReplacement[20165] = "Archer civilization \\n\\n· Start with +1 villager, but -50 food \\n· Resources last 15% longer \\n· Archers cost -10% Feudal, -20% Castle, -30% Imperial Age \\n\\n<b>Unique Unit:<b> Plumed Archer (archer) \\n\\n<b>Unique Techs:<b> Obsidian Arrows (Archers, Crossbowmen and Arbalests +12 attack vs. Towers/Stone Walls, +6 attack vs. other buildings); El Dorado (Eagle Warriors have +40 hit points)\\n\\n<b>Team Bonus:<b> Walls cost -50%";
		langReplacement[20158] = "Camel and naval civilization \\n· Market trade cost only 5% \\n· Market costs -75 wood \\n· Transport Ships 2x hit points, \\n 2x carry capacity \\n· Galleys attack 20% faster \\n· Cavalry archers +4 attack vs. buildings \\n\\n<b>Unique Unit:<b> Mameluke (camel) \\n\\n<b>Unique Techs:<b> Madrasah (Killed monks return 33% of their cost); Zealotry (Camels, Mamelukes +30 hit points)\\n\\n<b>Team Bonus:<b> Foot archers +2 attack vs. buildings";
		langReplacement[20163] = "Gunpowder and Monk civilization \\n\\n· Builders work 30% faster \\n· Blacksmith upgrades don't cost gold \\n· Cannon Galleons fire faster and with Ballistics) \\n· Gunpowder units fire 15% faster\\n\\n<b>Unique Units:<b> Conquistador (mounted hand cannoneer), Missionary (mounted Monk) \\n\\n<b>Unique Techs:<b> Inquisition (Monks convert faster); Supremacy (villagers better in combat)\\n\\n<b>Team Bonus:<b> Trade units generate +25% gold";


		for (size_t i = 0, nbPatches = sizeof patchTab / sizeof (wololo::DatPatch); i < nbPatches; i++) {
			std::cout << "Applying DAT patch " << i+1 << " of " << nbPatches << ": " << patchTab[i].name << std::endl;
			patchTab[i].patch(&aocDat, &langReplacement);
		}

		std::cout << std::endl << "Do you want to replace the original tech tree and unit descriptions with more detailed descriptions (Attack speed, hidden bonuses etc.)?" << std::endl;
		std::cout << "Type y or n (Yes/No) to continue." << std::endl;
		std::string line;
		while(std::getline(std::cin, line)) {
			if(tolower(line) == "y" || tolower(line) == "n") break;
			std::cout << "Type y or n (Yes/No) to continue." << std::endl;
		}
		if(tolower(line) == "y") {
			/*
			 * Load modded strings instead of normal HD strings into lang replacement
			 */
			std::ifstream modLang(modLangPath);
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


		std::cout << std::endl << "Converting the language file..." << std::endl;
		std::ifstream langIn(keyValuesStringsPath);
		std::ofstream langOut(languageIniPath);
		genie::LangFile langDll;
		bool patchLangDll = boost::filesystem::exists(langDllPath);
		if(!patchLangDll && aocFound) {
			langDllPath = outPath + langDllPath;
			patchLangDll = boost::filesystem::exists(langDllPath);
		}
		if(debug)
			patchLangDll = false;
		if (patchLangDll) {
			/*
			 * Apparently langDll.save() doesn't work if admin permissions are required, even if they are give
			 * We'll copy the file into the WK folder, patch it there and copy it back instead.
			 */
			if(!boost::filesystem::exists(langDllFile))
				boost::filesystem::copy_file(langDllPath,langDllFile);
			langDll.load((langDllFile).c_str());
			langDll.setGameVersion(genie::GameVersion::GV_TC);
		}
		else {
			std::cout << langDllPath << " not found, skipping dll patching for UserPatch." << std::endl;
		}
		convertLanguageFile(&langIn, &langOut, &langDll, patchLangDll, &langReplacement);
		if (patchLangDll) {
			langDll.save();
			boost::filesystem::copy_file(langDllFile,uPDIR+langDllFile);
			boost::filesystem::remove(langDllFile);
			std::cout << langDllPath << " patched." << std::endl;
		}


		std::cout << std::endl;

		aocDat.saveAs(outputDatPath.c_str());

		std::cout << std::endl << "Copying the files for UserPatch..." << std::endl;

		recCopy(vooblyDir + "Data", uPDIR + "Data");



		if (aocFound) {
			std::cout << "Conversion complete! The WololoKingdoms mod is now part of your AoC installation." << std::endl << std::endl;
			if(boost::filesystem::exists(HDPath+"/compatslp")) {
				recCopy(HDPath+"/compatslp",HDPath+"/compatslp2");
				boost::filesystem::remove_all(HDPath+"/compatslp");
				std::cout << "NOTE: To make this mod work with the HD compatibility patch, the 'compatslp' folder has been renamed (to 'compatslp2')." << std::endl;
				std::cout << "Voobly will give you an error message that the game is not correctly installed when joining a lobby, but that can safely be ignored." << std::endl << std::endl;
			}
			if (patchLangDll && !debug) {
				std::cout << "Do you want to create an additional installation with Userpatch that can be used without Voobly?" << std::endl;
				std::cout << "(This will launch in an extra window, simply close it after the installation is done.)" << std::endl;
				std::cout << "Type y or n (Yes/No) to continue." << std::endl;
				std::string line;
				while(std::getline(std::cin, line)) {
					if(tolower(line) == "y" || tolower(line) == "n") break;
					std::cout << "Type y or n (Yes/No) to continue." << std::endl;
				}
				if(tolower(line) == "y") {
					std::string backupPath = outPath+"old_language_x1_p1_"+std::to_string(boost::filesystem::file_size(langDllPath))+".dll";
					if(!boost::filesystem::exists(backupPath))
						boost::filesystem::rename(langDllPath,backupPath);
					else
						boost::filesystem::remove(langDllPath);
					boost::filesystem::copy_file(uPDIR+langDllFile,langDllPath);
					if(!boost::filesystem::exists(outPath+UPExe))
						boost::filesystem::copy_file(UPExe, outPath+UPExe);
					system(("\""+outPath+UPExe+"\" -g:"+UPModdedExe).c_str());
					std::cout << std::endl << UPModdedExe << ".exe installation for playing without Voobly created in the age2_x1 folder." << std::endl << std::endl;
				}
			}
		} else {
			std::cout << "Conversion complete. Installer did not find your AoC installation - " << std::endl;
			std::cout << "open the \"out/\" folder and put its contents into your AOE2 folder to make it work." << std::endl << std::endl;
		}
	}
	catch (std::exception const & e) {
		std::cerr << e.what() << std::endl;
		ret = 1;
	}
	catch (std::string const & e) {
		std::cerr << e << std::endl;
		ret = 1;
	}

	if (argc == 1) { // any argument = non-blocking install
		std::cout << "Press enter to exit..." << std::endl;
		std::cin.get();
	}

	return ret;
}
