#include <iostream>
#include <set>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "genie/dat/DatFile.h"
#include "genie/lang/LangFile.h"
#include "wololo/datPatch.h"
#include "wololo/Drs.h"
#include "fixes/berbersutfix.h"
#include "fixes/demoshipfix.h"
#include "fixes/portuguesefix.h"
#include "fixes/ethiopiansfreepikeupgradefix.h"
#include "fixes/maliansfreeminingupgradefix.h"
#include "fixes/ai900unitidfix.h"
#include "fixes/hotkeysfix.h"
#include "fixes/disablenonworkingunits.h"


using namespace std;

string const version = "1.1";

void recCopy(boost::filesystem::path const &src, boost::filesystem::path const &dst) {
	// recursive copy
	//boost::filesystem::path currentPath(current->path());
	if (boost::filesystem::is_directory(src)) {
		for (boost::filesystem::directory_iterator current(src), end;current != end; ++current) {
			boost::filesystem::path currentPath(current->path());
			recCopy(currentPath, dst / currentPath.filename());
		}
	}
	else {
		boost::filesystem::copy_file(src, dst);
	}

}

void listAssetFiles(string const path, vector<string> *listOfSlpFiles, std::vector<string> *listOfWavFiles) {
	const set<string> exclude = {
		// Exclude Forgotten Empires leftovers
		"50163", // Forgotten Empires loading screen
		"50189", // Forgotten Empires main menu
		"53207", // Forgotten Empires in-game logo
		"53208", // Forgotten Empires objective window
		"53209" // ???
	};
	for (boost::filesystem::directory_iterator end, it(path); it != end; it++) {
		string fileName = it->path().stem().string();
		if (exclude.find(fileName) == exclude.end()) {
			string extension = it->path().extension().string();
			if (extension == ".slp") {
				listOfSlpFiles->push_back(fileName);
			}
			else if (extension == ".wav") {
				listOfWavFiles->push_back(fileName);
			}
		}
	}
}

void convertLanguageFile(std::ifstream *in, std::ofstream *iniOut, genie::LangFile *dllOut, bool generateLangDll, std::map<int, std::string> *langReplacement) {
	string line;
	while (getline(*in, line)) {
		int spaceIdx = line.find(' ');
		string number = line.substr(0, spaceIdx);
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
			if (nb >= 120150 && nb <= 120176) { // descriptions of the civs in the expansion
				// replace the old descriptions of the civs in the base game
				nb -= 100000;
				number = to_string(nb);
			}
		}
		catch (invalid_argument const & e){
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

		boost::replace_all(line, "·", "\xb7"); // Workaround for UCS-2 to UTF-8 conversion
		*iniOut << number << '=' << line << endl;

		if (generateLangDll) {
			boost::replace_all(line, "\\n", "\n"); // the dll file requires actual line feed, not escape sequences
			try {
				dllOut->setString(nb, line);
			}
			catch (string const & e) {
				boost::replace_all(line, "\xb7", "-"); // non-english dll files don't seem to like that character
				dllOut->setString(nb, line);
			}
		}
	}
}

void makeDrs(string const inputDir ,std::ofstream *out) {
	const int numberOfTables = 2; // slp and wav

	vector<string> slpFilesNames;
	vector<string> wavFilesNames;
	listAssetFiles(inputDir, &slpFilesNames, &wavFilesNames);

	int numberOfSlpFiles = slpFilesNames.size();
	int numberOfWavFiles = wavFilesNames.size();
	int offsetOfFirstFile = sizeof (wololo::DrsHeader) +
			sizeof (wololo::DrsTableInfo) * numberOfTables +
			sizeof (wololo::DrsFileInfo) * (numberOfSlpFiles + numberOfWavFiles);
	int offset = offsetOfFirstFile;


	// file infos

	vector<wololo::DrsFileInfo> slpFiles;
	vector<wololo::DrsFileInfo> wavFiles;

	for (vector<string>::iterator it = slpFilesNames.begin(); it != slpFilesNames.end(); it++) {
		wololo::DrsFileInfo slp;
		size_t size = boost::filesystem::file_size(inputDir + "/" + *it + ".slp");
		slp.file_id = stoi(*it);
		slp.file_data_offset = offset;
		slp.file_size = size;
		offset += size;
		slpFiles.push_back(slp);
	}
	for (vector<string>::iterator it = wavFilesNames.begin(); it != wavFilesNames.end(); it++) {
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
		slpFiles.size() // num_files
	};
	wololo::DrsTableInfo const wavTableInfo = {
		0x20, // file_type, MAGIC
		{ 'v', 'a', 'w' }, // file_extension, "wav" in reverse
		sizeof (wololo::DrsHeader) + sizeof (wololo::DrsFileInfo) * numberOfTables + sizeof (wololo::DrsFileInfo) * slpFiles.size(), // file_info_offset
		wavFiles.size() // num_files
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
	for (vector<wololo::DrsFileInfo>::iterator it = slpFiles.begin(); it != slpFiles.end(); it++) {
		out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
		out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
		out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));
	}

	for (vector<wololo::DrsFileInfo>::iterator it = wavFiles.begin(); it != wavFiles.end(); it++) {
		out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
		out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
		out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));
	}

	// now write the actual files
	for (vector<string>::iterator it = slpFilesNames.begin(); it != slpFilesNames.end(); it++) {
		ifstream srcStream(inputDir + "/" + *it + ".slp", std::ios::binary);
		*out << srcStream.rdbuf();
	}

	for (vector<string>::iterator it = wavFilesNames.begin(); it != wavFilesNames.end(); it++) {
		ifstream srcStream(inputDir + "/" + *it + ".wav", std::ios::binary);
		*out << srcStream.rdbuf();
	}
}

void uglyHudHack(string const inputDir) {
	/*
	 * copies the Slav hud files for AK civs, the good way of doing this would be to extract
	 * the actual AK civs hud files from
	 * Age2HD\resources\_common\slp\game_b[24-27].slp correctly, but I haven't found a way yet
	 */
	int const slavHudFiles[] = {51123, 51153, 51183};
	for (size_t baseIndex = 0; baseIndex < sizeof slavHudFiles / sizeof (int); baseIndex++) {
		for (size_t i = 1; i <= 4; i++) {
			string dst = inputDir + "/" + to_string(slavHudFiles[baseIndex]+i) + ".slp";
			if (! (boost::filesystem::exists(dst) && boost::filesystem::file_size(dst)) > 0) {
				string src = inputDir + "/" + to_string(slavHudFiles[baseIndex]) + ".slp";
				recCopy(src, dst);
			}
		}
	}
}

void cleanTheUglyHudHack(string const inputDir) {
	int const slavHudFiles[] = {51123, 51153, 51183};
	for (size_t baseIndex = 0; baseIndex < sizeof slavHudFiles / sizeof (int); baseIndex++) {
		for (size_t i = 1; i <= 4; i++) {
			boost::filesystem::remove(inputDir + "/" + to_string(slavHudFiles[baseIndex]+i) + ".slp");
		}
	}
}

void copyCivIntroSounds(string const inputDir, string const outputDir) {
	string const civs[] = {"italians", "indians", "incas", "magyars", "slavs",
						   "portuguese", "ethiopians", "malians", "berbers"};
	for (size_t i = 0; i < sizeof civs / sizeof (string); i++) {
		recCopy(inputDir + "/" + civs[i] + ".mp3", outputDir + "/" + civs[i] + ".mp3");
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


int main(int argc, char *argv[]) {

	string const aocDatPath = "../resources/_common/dat/empires2_x1_p1.dat";
	string const hdDatPath = "../resources/_common/dat/empires2_x2_p1.dat";
	string const keyValuesStringsPath = "../resources/en/strings/key-value/key-value-strings-utf8.txt"; // TODO pick other languages
	string const languageIniPath = "out/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/language.ini";
	string const versionIniPath = "out/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/version.ini";
	string const civIntroSoundsInputPath = "../resources/_common/sound/civ/";
	string const civIntroSoundsOutputPath = "out/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Sound/stream/";
	string const xmlPath = "age2_x1.xml";
	string const xmlPathUP = "WK_African_Kingdoms.xml";
	string const xmlOutPath = "out/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/age2_x1.xml";
	string const xmlOutPathUP = "out/Games/WK_African_Kingdoms.xml";
	string const drsOutPath = "out/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Data/gamedata_x1_p1.drs";
	string const assetsPath = "../resources/_common/drs/gamedata_x2/";
	string const outputDatPath = "out/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Data/empires2_x1_p1.dat";
	string const langDllPath = "language_x1_p1.dll";
	string const vooblyDir = "out/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/";
	string const uPDIR = "out/Games/WK_African_Kingdoms/";


	int ret = 0;
	try {
		cout << "WololoKingdoms ver. " << version << endl;
		boost::filesystem::remove_all("out/");
		boost::filesystem::create_directories("out/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Data");
		boost::filesystem::create_directories("out/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Sound/stream");
		boost::filesystem::create_directories("out/Games/WK_African_Kingdoms/Data");
		boost::filesystem::create_directories("out/Games/WK_African_Kingdoms/Sound/stream");

		cout << "Opening the AOC dat file..." << endl << endl;
		genie::DatFile aocDat;
		aocDat.setVerboseMode(true);
		aocDat.setGameVersion(genie::GameVersion::GV_TC);
		aocDat.load(aocDatPath.c_str());

		cout << endl << "Opening the AOE2HD dat file..." << endl << endl;
		genie::DatFile hdDat;
		hdDat.setVerboseMode(true);
		hdDat.setGameVersion(genie::GameVersion::GV_Cysion);
		hdDat.load(hdDatPath.c_str());


		cout << "Preparing ressources files..." << endl;
		ofstream versionOut(versionIniPath);
		versionOut << version << endl;
		copyCivIntroSounds(civIntroSoundsInputPath, civIntroSoundsOutputPath);
		recCopy(xmlPath, xmlOutPath);
		recCopy(xmlPathUP, xmlOutPathUP);
		ofstream drsOut(drsOutPath, std::ios::binary);

		cout << "Generating gamedata_x1_p1.drs..." << endl;
		uglyHudHack(assetsPath);
		makeDrs(assetsPath, &drsOut);
		cleanTheUglyHudHack(assetsPath);


		cout << "Generating empires2_x1_p1.dat..." << endl;
		transferHdDatElements(&hdDat, &aocDat);

		wololo::DatPatch patchTab[] = {
			wololo::berbersUTFix,
//			wololo::demoShipFix,
			wololo::etiopiansFreePikeUpgradeFix,
			wololo::hotkeysFix,
			wololo::maliansFreeMiningUpgradeFix,
			wololo::portugueseFix,
			wololo::disableNonWorkingUnits,
			wololo::ai900UnitIdFix
		};

		map<int, string> langReplacement;
		for (size_t i = 0, nbPatches = sizeof patchTab / sizeof (wololo::DatPatch); i < nbPatches; i++) {
			cout << "Applying DAT patch " << i+1 << " of " << nbPatches << ": " << patchTab[i].name << endl;
			patchTab[i].patch(&aocDat, &langReplacement);
		}

		cout << endl << "Converting the language file..." << endl;
		ifstream langIn(keyValuesStringsPath);
		ofstream langOut(languageIniPath);
		genie::LangFile langDll;
		bool patchLangDll = boost::filesystem::exists(langDllPath);
		if (patchLangDll) {
			langDll.load(langDllPath.c_str());
			langDll.setGameVersion(genie::GameVersion::GV_TC);
		}
		else {
			cout << langDllPath << " not found, skipping dll patching for UserPatch." << endl;
		}
		convertLanguageFile(&langIn, &langOut, &langDll, patchLangDll, &langReplacement);
		if (patchLangDll) {
			langDll.save();
			cout << langDllPath << " patched." << endl;
		}


		cout << endl;

		aocDat.saveAs(outputDatPath.c_str());

		cout << endl << "Copying the files for UserPatch..." << endl;

		recCopy(vooblyDir + "/Data", uPDIR + "/Data");
		recCopy(vooblyDir + "/Sound", uPDIR + "/Sound");

		cout << "Conversion complete, open the \"out/\" folder and put it's content into your AOE2 folder." << endl;
	}
	catch (exception const & e) {
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (string const & e) {
		cerr << e << endl;
		ret = 1;
	}

	if (argc == 1) { // any argument = non-blocking install
		cout << "Press enter to exit..." << endl;
		cin.get();
	}
	return ret;
}
