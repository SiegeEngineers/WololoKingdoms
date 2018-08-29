#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <windows.h>
#include <shellapi.h>

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
#include "fixes/cuttingfix.h"
#include "fixes/smallfixes.h"
#include "fixes/tricklebuildingfix.h"

#include <QCryptographicHash>
#include "sdk/public/steam/steam_api.h"

#include "JlCompress.h"
#include "wkconverter.h"

void WKConverter::process() {
    run();
}

void WKConverter::callExternalExe(std::wstring exe) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    wchar_t cmdLineString[exe.length()+1];
    wcscpy(cmdLineString, exe.c_str());
    CreateProcess( nullptr, cmdLineString, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi );
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

void WKConverter::recCopy(fs::path const &src, fs::path const &dst, bool skip, bool force) {
    /*
     * Recursive copy of a folder (or file, but then this isn't necessary)
     * Parameters:
     * src: The folder to be copied from
     * dst: The folder to be copied into
     * skip: If true, if a file already exists, copying will be skipped
     * force: If true, if a file already exists, it will be replaced
     * If both skip and force are false, an exception is thrown if a file already exists
     */
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

void WKConverter::loadGameStrings(std::map<int,std::string>& langReplacement) {
    std::string line;
    std::ifstream translationFile("resources\\"+settings->language+"_game.txt");
    while (std::getline(translationFile, line)) {
        /*
         *  \\\\n -> \\n, means we want a \n in the text files for aoc
         *  If no such line is found, it might be a line for the installer itself, where we want actual linebreaks,
         * so replace \\n -> \n with a linebreak
         */
        if(line.find("\\\\n") == std::string::npos)
            boost::replace_all(line, "\\n", "\n");
        unsigned int index = line.find('=');
        std::string key = line.substr(0, index);
        try {
            int keyNo = std::stoi(key);
            langReplacement[keyNo] = line.substr(index+1, std::string::npos);
        } catch (std::invalid_argument e) {
            continue;
        }
    }
    translationFile.close();
}

void WKConverter::indexDrsFiles(fs::path const &src, bool expansionFiles, bool terrainFiles) {
    /*
     * Index files to be written into the drs into a map, with the ID the file will have later
     * being the key, and the path it should be copied from being the value
     * Parameters:
     * src: The directory to iterate through. All .slp and .wav files in this directory will be indexed
     * expansionFiles: If false, files are written to a seperate map. They are not written to the drs later,
     *                  but we need them for comparison purposes in the independent architecture patching.
     * terrainFiles: If true, these are terrain files, written to a seperate map, as we need them for
     *                  expansion map creation.
     */
	if (fs::is_directory(src)) {
		for (fs::directory_iterator current(src), end;current != end; ++current) {
			fs::path currentPath(current->path());
            indexDrsFiles(currentPath, expansionFiles, terrainFiles);
		}
	}
	else {
		std::string extension = src.extension().string();
        if(terrainFiles) {
            if (extension == ".slp") {
                newTerrainFiles[src.filename().string()] = src;
            }
        } else {
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
}

void WKConverter::copyHistoryFiles(fs::path inputDir, fs::path outputDir) {
    std::string const civs[] = {"italians-utf8.txt", "indians-utf8.txt", "incas-utf8.txt", "magyars-utf8.txt", "slavs-utf8.txt",
                                "portuguese-utf8.txt", "ethiopians-utf8.txt", "malians-utf8.txt", "berbers-utf8.txt",
                                "burmese-utf8.txt", "malay-utf8.txt", "vietnamese-utf8.txt", "khmer-utf8.txt"};
    for (size_t i = 0; i < sizeof civs / sizeof (std::string); i++) {
        std::ifstream langIn(inputDir.string()+"\\"+civs[i]);
        std::ofstream langOut(outputDir.string()+"\\"+civs[i].substr(0,civs[i].length()-9)+".txt");
        std::string contents;
        langIn.seekg(0, std::ios::end);
        contents.resize(langIn.tellg());
        langIn.seekg(0, std::ios::beg);
        langIn.read(&contents[0], contents.size());
        langIn.close();
        std::wstring wideContent = strtowstr(contents);
        std::string outputContent;
        ConvertUnicode2CP(wideContent.c_str(), outputContent, CP_ACP);
        langOut << outputContent;
        langOut.close();
    }
}

std::pair<int,std::string> WKConverter::getTextLine(std::string line) {
    int spaceIdx = line.find(' ');
    std::string number = line.substr(0, spaceIdx);
    int nb = stoi(number);
    if (nb == 0xFFFF) {
        /*
         * this one seems to be used by AOC for dynamically-generated strings
         * (like market tributes), maybe it's the maximum the game can read ?
        */
        throw std::invalid_argument("FFFF");
    }
    if (nb <= 1000) {
        // skip changes to fonts
        throw std::invalid_argument("fonts");
    }
    if (nb >= 20150 && nb <= 20167) {
        // skip the old civ descriptions
        throw std::invalid_argument("old civ descriptions");
    }
    if (nb >= 9871 && nb <= 9946) {
        // skip the old civ descriptions
        throw std::invalid_argument("uncentered achievement screen stuff");
    }
    if (nb >= 20312 && nb <= 20341) {
        switch (nb) {
            case 20312: nb = 20334; break;
            case 20313: nb = 20312; break;
            case 20314: nb = 20338; break;
            case 20315: nb = 20313; break;
            case 20316: nb = 20314; break;
            case 20317: nb = 20315; break;
            case 20318: nb = 20335; break;
            case 20319: nb = 20316; break;
            case 20320: nb = 20317; break;
            case 20321: nb = 20318; break;
            case 20322: nb = 20329; break;
            case 20323: nb = 20330; break;
            case 20324: nb = 20331; break;
            case 20325: nb = 20319; break;
            case 20326: nb = 20339; break;
            case 20327: nb = 20320; break;
            case 20328: nb = 20332; break;
            case 20329: nb = 20340; break;
            case 20330: nb = 20336; break;
            case 20331: nb = 20321; break;
            case 20332: nb = 20322; break;
            case 20333: nb = 20323; break;
            case 20334: nb = 20337; break;
            case 20335: nb = 20324; break;
            case 20336: nb = 20333; break;
            case 20337: nb = 20325; break;
            case 20338: nb = 20326; break;
            case 20339: nb = 20327; break;
            case 20340: nb = 20341; break;
            case 20341: nb = 20328; break;
        }
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

    return std::make_pair(nb,line);
}

bool WKConverter::createLanguageFile(fs::path languageIniPath, fs::path patchFolder) {

    std::map<int, std::string> langReplacement;
    fs::path keyValuesStringsPath = settings->language == "zht"?resourceDir/"zht\\key-value-strings-utf8.txt":
                                                      settings->HDPath / "resources" / settings->language / "strings\\key-value\\key-value-strings-utf8.txt";
    std::string modLangIni = resourceDir.string()+settings->language+".ini";
    fs::path langDllFile("language_x1_p1.dll");
    fs::path langDllPath = langDllFile;
    /*
     * Create the language files (.ini for Voobly, .dll for offline)
     */

    loadGameStrings(langReplacement);

    emit log("Open Missing strings");
    std::string line;
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

    emit log("Replace tooltips");
    if(settings->replaceTooltips) {
        loadModdedStrings(modLangIni, langReplacement);
    }
    emit increaseProgress(1); //2

    if(settings->patch >= 0 && std::get<3>(settings->dataModList[settings->patch]) & 2) {
        /*
         * A data mod might need slightly changed strings.
         */
        std::ifstream modLang((patchFolder/(settings->language+".txt")).string());
        while (std::getline(modLang, line)) {
            try {
                std::pair<int,std::string> tmp = getTextLine(line);
                langReplacement[tmp.first] = tmp.second;
            } catch (std::invalid_argument const & e) {
                continue;
            }
        }
        modLang.close();
        if(settings->replaceTooltips) {
            loadModdedStrings((patchFolder/(settings->language+".ini")).string(), langReplacement);
        }
    }


    std::ifstream langIn(keyValuesStringsPath.string());
    std::ofstream langOut(languageIniPath.string());
    genie::LangFile langDll;

    bool patchLangDll;
    if(settings->useVoobly) {
        patchLangDll = false;
    } else {
        langDllPath = settings->outPath / langDllPath;
        patchLangDll = fs::exists(langDllPath);
        if(patchLangDll)
        {
            fs::remove(langDllFile);
            fs::copy_file(langDllPath,langDllFile);
        }
    }
    emit increaseProgress(1); //3
    bool dllPatched = true;

    emit log("Open Lang Dll");
    if (patchLangDll && !openLanguageDll(&langDll, langDllPath, langDllFile)) {
        dllPatched = false;
        patchLangDll = false;
        line = "working$\n$workingNoDll";
        emit setInfo(QString::fromStdString(line));

    }
    emit increaseProgress(1); //4

    emit log("convert language file");
    convertLanguageFile(&langIn, &langOut, &langDll, patchLangDll, &langReplacement);
    emit increaseProgress(1); //5
    emit log("save lang dll file");
    if (patchLangDll && !saveLanguageDll(&langDll, langDllFile)) {
        dllPatched = false;
        patchLangDll = false;
    }
    return dllPatched;
}

void WKConverter::loadModdedStrings(std::string moddedStringsFile, std::map<int, std::string>& langReplacement) {
    std::ifstream modLang(moddedStringsFile);
    std::string line;
    while (std::getline(modLang, line)) {
        unsigned int spaceIdx = line.find('=');
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

bool WKConverter::openLanguageDll(genie::LangFile *langDll, fs::path langDllPath, fs::path langDllFile) {
    try {
        langDll->load((langDllFile.string()).c_str());
        langDll->setGameVersion(genie::GameVersion::GV_TC);
    } catch (const std::ifstream::failure& e) {
        //Try deleting and re-copying
        fs::remove(langDllFile);
        fs::copy_file(langDllPath,langDllFile);
        try {
            langDll->load((langDllFile.string()).c_str());
            langDll->setGameVersion(genie::GameVersion::GV_TC);
        } catch (const std::ifstream::failure& e) {
            fs::remove(langDllFile);
            return false;
        }
    }
    return true;
}

bool WKConverter::saveLanguageDll(genie::LangFile *langDll, fs::path langDllFile) {
    fs::create_directories(settings->upDir/"data\\");
    fs::path langDllOutput = settings->upDir/"data"/langDllFile;
    std::string line;
    try {
        line = "working$\n$workingDll";
        langDll->save();
        fs::copy_file(langDllFile,langDllOutput,fs::copy_option::overwrite_if_exists);
        fs::remove(langDllFile);
        emit setInfo(QString::fromStdString(line));

    } catch (const std::ofstream::failure& e) {
        emit setInfo("workingError");

        fs::remove(langDllFile);
        fs::remove(langDllOutput);
        try {
            langDll->save();
            fs::copy_file(langDllFile,langDllOutput,fs::copy_option::overwrite_if_exists);
            fs::remove(langDllFile);
            emit setInfo(QString::fromStdString(line));

        } catch (const std::ofstream::failure& e) {
            fs::remove(langDllFile);
            fs::remove(langDllOutput);
            return false;
        }
    }
    return true;
}

void WKConverter::convertLanguageFile(std::ifstream *in, std::ofstream *iniOut, genie::LangFile *dllOut, bool generateLangDll, std::map<int, std::string> *langReplacement) {
	std::string line;
    int nb;
	while (std::getline(*in, line)) {

        try {
            std::pair<int,std::string> tmp = getTextLine(line);
            nb = tmp.first;
            line = tmp.second;
        } catch (std::invalid_argument const & e) {
            continue;
        }

		if (langReplacement->count(nb)) {
			// this string has been changed by one of our patches (modified attributes etc.)
			line = (*langReplacement)[nb];
			langReplacement->erase(nb);
        }

		//convert UTF-8 into ANSI
		std::wstring wideLine = strtowstr(line);
		std::string outputLine;
        ConvertUnicode2CP(wideLine.c_str(), outputLine, CP_ACP);

        *iniOut << std::to_string(nb) << '=' << outputLine <<  std::endl;

		if (generateLangDll) {
            boost::replace_all(line, "·", "\xb7"); // Dll can't handle that character.
            boost::replace_all(line, "\\n", "\n"); // the dll file requires actual line feed, not escape sequences
			try {
                dllOut->setString(nb, line);
			}
			catch (std::string const & e) {
                boost::replace_all(line, "\xb7", "-"); // non-english dll files don't seem to like that character
                boost::replace_all(line, "\xc5\xab", "u");
                boost::replace_all(line, "\xc4\x81", "a");
                boost::replace_all(line, "\xe1\xbb\x87", "ê");
                boost::replace_all(line, "\xe1\xbb\x8b", "i");
                boost::replace_all(line, "\xe1\xbb\xa3", "o");
                boost::replace_all(line, "\xe1\xbb\x85", "e");
                boost::replace_all(line, "\xe1\xbb\x87", "e");
                boost::replace_all(line, "\xe1\xba\xa2", "A");
                boost::replace_all(line, "\xc4\x90\xe1", "D");
                boost::replace_all(line, "\xba\xa1", "a");
                boost::replace_all(line, "\xc4\x90", "D");
                boost::replace_all(line, "\xc3\xaa", "e");
                boost::replace_all(line, "\xc3\xb9", "u");
                boost::replace_all(line, "\xc6\xb0", "u");
                boost::replace_all(line, "\xbb\x99", "o");
                try {
                    dllOut->setString(nb, line);
                }
                catch (std::string const & e) {
                    dllOut->setString(nb, line);
                }
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

        ConvertUnicode2CP(wideLine.c_str(), outputLine, CP_ACP);

		*iniOut << std::to_string(it->first) << '=' << outputLine <<  std::endl;

		if (generateLangDll) {
            boost::replace_all(it->second, "·", "\xb7"); // Dll can't handle that character.
            boost::replace_all(it->second, "\\n", "\n"); // the dll file requires actual line feed, not escape sequences
			try {
                dllOut->setString(it->first, it->second);
			}
			catch (std::string const & e) {
                boost::replace_all(it->second, "\xb7", "-"); // non-english dll files don't seem to like that character
                boost::replace_all(it->second, "\xae", "R");
                dllOut->setString(it->first, it->second);
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

void WKConverter::makeDrs(std::ofstream *out) {

    emit setInfo("working$\n$workingDrs");


	// Exclude Forgotten Empires leftovers
    //slpFiles.erase(50163); // Forgotten Empires loading screen
    slpFiles.erase(50189); // Forgotten Empires main menu
    //slpFiles.erase(53207); // Forgotten Empires in-game logo
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
    emit increaseProgress(1); //67

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
    emit increaseProgress(1); //68

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
    emit increaseProgress(1); //69


    emit setInfo("working$\n$workingDrs2");

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

    emit increaseProgress(1); //70
	// file infos
	for (std::vector<wololo::DrsFileInfo>::iterator it = slpFileInfos.begin(); it != slpFileInfos.end(); it++) {
		out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
		out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
		out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));

	}
    emit increaseProgress(1); //71
	for (std::vector<wololo::DrsFileInfo>::iterator it = wavFileInfos.begin(); it != wavFileInfos.end(); it++) {
		out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
		out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
		out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));

	}
    emit increaseProgress(1); //72

    emit setInfo("working$\n$workingDrs3");

	// now write the actual files
	for (std::map<int,fs::path>::iterator it = slpFiles.begin(); it != slpFiles.end();it++) {
			std::ifstream srcStream = std::ifstream(it->second.string(), std::ios::binary);
			*out << srcStream.rdbuf();
            srcStream.close();

	}
    emit increaseProgress(1); //73

	for (std::map<int,fs::path>::iterator it = wavFiles.begin(); it != wavFiles.end(); it++) {
		std::ifstream srcStream = std::ifstream(it->second.string(), std::ios::binary);
		*out << srcStream.rdbuf();
        srcStream.close();

	}
    emit increaseProgress(1); //74
    out->close();
}

/** A method to add extra (slp) files to an already existing drs file.
 *  Reads the old files, changes the offsets where necessary and writes back to a new file with changed offsets and the new files
 *  in: The stream of the old drs file
 *  out: The stream of the new drs file
 */
void WKConverter::editDrs(std::ifstream *in, std::ofstream *out) {

    int numberOfSlpFiles = slpFiles.size(); //These are the new files to be added to the drs


    emit log(QString("number of files")+QString().setNum(numberOfSlpFiles));
    emit setInfo("working$\n$workingDrs2");

    char * buffer;
    char * intBuffer = new char[4];
    emit increaseProgress(1); //22


    emit log("write header");
    // header, no changes here
    int length = sizeof (wololo::DrsHeader::copyright) + sizeof (wololo::DrsHeader::version) + sizeof (wololo::DrsHeader::ftype)
            +sizeof (wololo::DrsHeader::table_count);
    buffer = new char[length];
    in->read(buffer,length);
    emit log(QString(buffer));
    out->write(buffer, length);
    emit increaseProgress(1); //23

    //There are extra file infos added, so the offset of the first file changes
    int extraOffset = sizeof (wololo::DrsFileInfo) * (numberOfSlpFiles);
    in->read(intBuffer,4);
    int offsetOfFirstFile = *(reinterpret_cast<int *>(intBuffer));
    offsetOfFirstFile += extraOffset;
    out->write(reinterpret_cast<const char *>(&offsetOfFirstFile), sizeof (wololo::DrsHeader::file_offset));
    emit increaseProgress(1); //24

    emit log("slp table info");
    //slp table info
    length = sizeof (wololo::DrsTableInfo::file_type) + sizeof (wololo::DrsTableInfo::file_extension) + sizeof (wololo::DrsTableInfo::file_info_offset);
    buffer = new char[length];
    in->read(buffer,length);
    out->write(buffer, length);
    in->read(intBuffer,4);
    int numberOfOldSlpFiles = *(reinterpret_cast<int *>(intBuffer));
    emit log(intBuffer);
    emit log(QString().setNum(numberOfOldSlpFiles));
    int totalSlpFiles = numberOfOldSlpFiles+numberOfSlpFiles;
    out->write(reinterpret_cast<const char *>(&totalSlpFiles), sizeof (wololo::DrsTableInfo::num_files));
    emit increaseProgress(1); //25

    emit log("wav table info");
    length = sizeof (wololo::DrsTableInfo::file_type) + sizeof (wololo::DrsTableInfo::file_extension);
    buffer = new char[length];
    in->read(buffer,length);
    out->write(buffer, length);
    in->read(intBuffer,4);
    emit log(intBuffer);
    int wavInfoOffset = *(reinterpret_cast<int *>(intBuffer)) + extraOffset;
    out->write(reinterpret_cast<const char *>(&wavInfoOffset), sizeof (wololo::DrsTableInfo::file_info_offset));
    in->read(intBuffer,4);
    emit log(intBuffer);
    int numberOfOldWavFiles = *(reinterpret_cast<int *>(intBuffer));
    emit log(QString().setNum(numberOfOldWavFiles));
    out->write(reinterpret_cast<const char *>(&numberOfOldWavFiles), sizeof (wololo::DrsTableInfo::num_files));
    emit increaseProgress(1); //26


    // file infos
    int fileOffset = 0;
    int fileSize = 0;
    int slpBlockSize = 0;
    int wavBlockSize = 0;

    emit log("old slp file infos");
    for (int i = 0; i < numberOfOldSlpFiles; i++) {
        in->read(intBuffer,4);
        int fileId = *(reinterpret_cast<int *>(intBuffer));
        if(fileId >= 60000 && (fileId <= 60138 || (fileId >= 70000 && fileId <= 70138) || (fileId >= 80000 && fileId <= 80017))) {
            //First if is just a cheaper hardcoded precheck, can be removed if the function needs to be more general
            if(slpFiles.count(fileId) > 0) {
                fileId += 900000; //Doesn't really matter, just a large number that the game will never read
            }
        }
        in->read(intBuffer,4);
        fileOffset = *(reinterpret_cast<int *>(intBuffer))+extraOffset;
        in->read(intBuffer,4);
        fileSize = *(reinterpret_cast<int *>(intBuffer));
        slpBlockSize += fileSize;
        out->write(reinterpret_cast<const char *>(&fileId), sizeof (wololo::DrsFileInfo::file_id));
        out->write(reinterpret_cast<const char *>(&fileOffset), sizeof (wololo::DrsFileInfo::file_data_offset));
        out->write(reinterpret_cast<const char *>(&fileSize), sizeof (wololo::DrsFileInfo::file_size));

    }
    emit increaseProgress(1); //27
    int offset = fileOffset + fileSize;

    emit log("new slp file infos");

    std::vector<wololo::DrsFileInfo> slpFileInfos;

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
    emit increaseProgress(1);//28

    for (std::vector<wololo::DrsFileInfo>::iterator it = slpFileInfos.begin(); it != slpFileInfos.end(); it++) {
        out->write(reinterpret_cast<const char *>(&it->file_id), sizeof (wololo::DrsFileInfo::file_id));
        out->write(reinterpret_cast<const char *>(&it->file_data_offset), sizeof (wololo::DrsFileInfo::file_data_offset));
        out->write(reinterpret_cast<const char *>(&it->file_size), sizeof (wololo::DrsFileInfo::file_size));
    }
    emit increaseProgress(1);//29

    emit log("wav file infos");
    for (int i = 0; i < numberOfOldWavFiles; i++) {
        in->read(intBuffer,4);
        int fileId = *(reinterpret_cast<int *>(intBuffer));
        in->read(intBuffer,4); //Old offset, not relevant anymore
        in->read(intBuffer,4);
        fileSize = *(reinterpret_cast<int *>(intBuffer));
        wavBlockSize += fileSize;
        out->write(reinterpret_cast<const char *>(&fileId), sizeof (wololo::DrsFileInfo::file_id));
        out->write(reinterpret_cast<const char *>(&offset), sizeof (wololo::DrsFileInfo::file_data_offset));
        out->write(reinterpret_cast<const char *>(&fileSize), sizeof (wololo::DrsFileInfo::file_size));
        offset += fileSize;

    }
    emit increaseProgress(1);//30


    emit log("old slp files");
    int bufferSize = 4096;
    buffer = new char[bufferSize];
    int i;
    for(i = bufferSize; i < slpBlockSize; i+=bufferSize) {
        in->read(buffer, bufferSize);
        out->write(buffer, bufferSize);

    }
    length = slpBlockSize - i + bufferSize;
    in->read(buffer,length);
    out->write(buffer, length);
    emit increaseProgress(1);//31
    emit setInfo("working$\n$workingDrs3");


    emit log("new slp files");
    for (std::map<int,fs::path>::iterator it = slpFiles.begin(); it != slpFiles.end();it++) {
            std::ifstream srcStream = std::ifstream(it->second.string(), std::ios::binary);
            *out << srcStream.rdbuf();
            srcStream.close();

    }
    emit increaseProgress(1);//32


    emit log("old wav files");
    buffer = new char[bufferSize];
    for(i = bufferSize; i < wavBlockSize; i+=bufferSize) {
        in->read(buffer, bufferSize);
        out->write(buffer, bufferSize);

    }
    length = wavBlockSize - i + bufferSize;
    in->read(buffer,length);
    out->write(buffer, length);
    emit increaseProgress(1);//33

    in->close();
    out->close();
}

void WKConverter::copyCivIntroSounds(fs::path inputDir, fs::path outputDir) {
	std::string const civs[] = {"italians", "indians", "incas", "magyars", "slavs",
								"portuguese", "ethiopians", "malians", "berbers", "burmese", "malay", "vietnamese", "khmer"};
	for (size_t i = 0; i < sizeof civs / sizeof (std::string); i++) {
		boost::system::error_code ec;
		fs::copy_file(inputDir / (civs[i] + ".mp3"), outputDir / (civs[i] + ".mp3"), ec);
	}
}

void WKConverter::uglyHudHack(fs::path assetsPath) {
	/*
	 * Shifts the offset between interface files by 10 so there's space for the new civs
	 */
	int const hudFiles[] = {51130, 51160};
	for (size_t baseIndex = 0; baseIndex < sizeof hudFiles / sizeof (int); baseIndex++) {
		for (size_t i = 1; i <= 23; i++) {
			slpFiles[hudFiles[baseIndex]+i+(baseIndex+1)*10] = assetsPath / (std::to_string(hudFiles[baseIndex]+i) + ".slp");
		}
	}

    indexDrsFiles(resourceDir/"expansion interfaces fix");
}

void WKConverter::copyWallFiles(fs::path inputDir) {
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
    int conversionTable[] = {3,-15,2,0,3,-18,-5,1,0,1,2,3,0,1,2,2,-5,-7};
	int newBaseSLP = 24000;
	for(size_t i = 0; i < sizeof(conversionTable)/sizeof(int); i++) {
		int archID = conversionTable[i];
		if (archID < 0) {
            archID *= -1;
			int digits = archID == 5 || archID == 7?124:324;
            slpFiles[newBaseSLP+i*1000+324] = inputDir/(std::to_string(archID*1000+digits)+".slp");
            slpFiles[newBaseSLP+i*1000+326] = inputDir/(std::to_string(archID*1000+digits+2)+".slp");
			for (int j = 0; j <= 10; j+=2)
                slpFiles[newBaseSLP+i*1000+345+j] = inputDir/(std::to_string(archID*1000+digits+21+j)+".slp");
		} else {
			slpFiles[newBaseSLP+i*1000+324] = inputDir/(std::to_string(2098+archID)+".slp");
			slpFiles[newBaseSLP+i*1000+326] = inputDir/(std::to_string(2110+archID)+".slp");
            for (int j = 0; j <= 10; j+=2)
                slpFiles[newBaseSLP+i*1000+345+j] = inputDir/(std::to_string(4169+archID+j*2)+".slp");
		}
	}
}

std::string WKConverter::tolower(std::string line) {
	std::transform(line.begin(), line.end(), line.begin(), static_cast<int(*)(int)>(std::tolower));
	return line;
}

void WKConverter::createMusicPlaylist(std::string inputDir, std::string const outputDir) {
	std::ofstream outputFile(outputDir);
	for (int i = 1; i <= 23; i++ ) {
		outputFile << inputDir << "xmusic" << std::to_string(i) << ".mp3" <<  std::endl;
	}
    outputFile.close();
}

void WKConverter::copyHDMaps(fs::path inputDir, fs::path outputDir, bool replace) {

	std::vector<fs::path> mapNames;
	for (fs::directory_iterator end, it(inputDir); it != end; it++) {
		std::string extension = it->path().extension().string();
		if (extension == ".rms") {
			mapNames.push_back(it->path());
		}
	}
    emit increaseProgress(2); //15+19
	std::map<std::string,fs::path> terrainOverrides;
    std::array<std::map<int,std::regex>,6> terrainsPerType = { {
        {}, //The Order is important, see the TerrainTypes Enum!
        { {23,std::regex("\\WMED_WATER\\W")}, {22,std::regex("\\WDEEP_WATER\\W")} },
        {},
        { {0,std::regex("\\WGRASS\\W")}, {3,std::regex("\\WDIRT3\\W")}, {6,std::regex("\\WDIRT1\\W")}, {9,std::regex("\\WGRASS3\\W")},
          {12,std::regex("\\WGRASS2\\W")}, {14,std::regex("\\WDESERT\\W")}, {24,std::regex("\\WROAD\\W")}, {25,std::regex("\\WROAD2\\W")}, {39,std::regex("\\WROAD3\\W")} },
        { {10,std::regex("\\WFOREST\\W")}, {13,std::regex("\\WPALM_DESERT\\W")}, {21,std::regex("\\WSNOW_FOREST\\W")} },
        { {40,std::regex("\\WDLC_ROCK\\W")}, {35,std::regex("\\WICE\\W")} }
    } };
    std::vector<std::tuple<std::string,std::string,std::string,int,int,int>> replacements = {
        //<SLP Name,Regex Pattern,replace name,terrain ID, replace terrain ID,slp to replace,terrainType>
        std::make_tuple("DRAGONFOREST.slp","DRAGONFORES(T?)","DRAGONFORES$1",48,21,TerrainType::ForestTerrain),
        std::make_tuple("ACACIA_FOREST.slp","AC(C?)ACIA(_?)FORES(T?)","AC$1ACIA$2FORES$3",50,41,TerrainType::None),
        std::make_tuple("DLC_RAINFOREST.slp","DLC_RAINFOREST","DLC_RAINFOREST",56,10,TerrainType::ForestTerrain),
        std::make_tuple("BAOBAB.slp","BAOBAB(S|_FOREST)?","BAOBAB$1",49,16,TerrainType::None),
        std::make_tuple("DLC_MANGROVESHALLOW.slp","DLC_MANGROVESHALLOW","DLC_MANGROVESHALLOW",54,11,TerrainType::None),
        std::make_tuple("DLC_MANGROVEFOREST.slp","DLC_MANGROVEFOREST","DLC_MANGROVEFOREST",55,20,TerrainType::None),
        std::make_tuple("DLC_NEWSHALLOW.slp","DLC_NEWSHALLOW","DLC_NEWSHALLOW",59,4,TerrainType::FixedTerrain),
        std::make_tuple("SAVANNAH.slp","(DLC_)?SAVANNAH","$1SAVANNAH",41,14,TerrainType::LandTerrain),
        std::make_tuple("DIRT4.slp","(DLC_)?DIRT4","$1DIRT4",42,3,TerrainType::LandTerrain),
        std::make_tuple("MOORLAND.slp","(DLC_)?MOORLAND","$1MOORLAND",44,9,TerrainType::LandTerrain),
        std::make_tuple("CRACKEDIT.slp","CRACKEDIT","CRACKEDIT",45,38,TerrainType::None),
        std::make_tuple("QUICKSAND.slp","(DLC_)?QUICKSAND","$1QUICKSAND",46,40,TerrainType::FixedTerrain),
        std::make_tuple("BLACK.slp","(DLC_)?BLACK(?!_)","$1BLACK",47,40,TerrainType::FixedTerrain),
        std::make_tuple("DLC_BEACH2.slp","DLC_BEACH2","DLC_BEACH2",51,2,TerrainType::FixedTerrain),
        std::make_tuple("DLC_BEACH3.slp","DLC_BEACH3","DLC_BEACH3",52,2,TerrainType::FixedTerrain),
        std::make_tuple("DLC_BEACH4.slp","DLC_BEACH4","DLC_BEACH4",53,2,TerrainType::FixedTerrain),
        std::make_tuple("DLC_DRYROAD.slp","DLC_DRYROAD","DLC_DRYROAD",43,25,TerrainType::LandTerrain),
        std::make_tuple("DLC_WATER4.slp","DLC_WATER4","DLC_WATER4",57,22,TerrainType::WaterTerrain),
        std::make_tuple("DLC_WATER5.slp","DLC_WATER5","DLC_WATER5",58,1,TerrainType::WaterTerrain),
        std::make_tuple("DLC_JUNGLELEAVES.slp","DLC_JUNGLELEAVES","DLC_JUNGLELEAVES",62,5,TerrainType::LandTerrain),
        std::make_tuple("DLC_JUNGLEROAD.slp","DLC_JUNGLEROAD","DLC_JUNGLEROAD",61,39,TerrainType::LandTerrain),
        std::make_tuple("DLC_JUNGLEGRASS.slp","DLC_JUNGLEGRASS","DLC_JUNGLEGRASS",60,12,TerrainType::LandTerrain)
	};
    std::map<int,std::string> slpNumbers = {
        {0,"15001.slp"}, {1,"15002.slp"}, {2,"15017.slp"}, {3,"15007.slp"}, {4,"15014.slp"},
        {5,"15011.slp"}, {6,"15014.slp"}, {9,"15009.slp"}, {10,"15011.slp"}, {12,"15008.slp"},
        {13,"15010.slp"}, {14,"15010.slp"}, {21,"15029.slp"},  {22,"15015.slp"}, {23,"15016.slp"},
        {24,"15018.slp"},  {25,"15019.slp"}, {35,"15024.slp"}, {39,"15031.slp"}, {40,"15033.slp"}
    };
    std::vector<std::tuple<std::string,std::string,std::string,int,int,int>>::iterator repIt;

	for (std::vector<fs::path>::iterator it = mapNames.begin(); it != mapNames.end(); it++) {

		std::string mapName = it->stem().string()+".rms";
        if (mapName.substr(0,3) == "ZR@") {
			fs::copy_file(*it,outputDir/mapName,fs::copy_option::overwrite_if_exists);
			continue;
		}        
        if(fs::exists(outputDir/it->filename()) || fs::exists(outputDir/("ZR@"+it->filename().string()))) {
            if(replace)
                fs::remove(outputDir/it->filename());
            else
                continue;
        }
        fs::remove(outputDir/("ZR@"+it->filename().string()));
		std::ifstream input(inputDir.string()+it->filename().string());
        std::string map(static_cast<std::stringstream const&>(std::stringstream() << input.rdbuf()).str());
        input.close();
        /*
		if(str.find("DLC_MANGROVESHALLOW")!=std::string::npos) {
			if(str.find("<PLAYER_SETUP>")!=std::string::npos)
				str = std::regex_replace(str, std::regex("<PLAYER_SETUP>\\s*(\\r*)\\n"),
					"<PLAYER_SETUP>$1\n  terrain_state 0 0 0 1$1\n");
			else
				str = std::regex_replace(str, std::regex("#include_drs\\s+random_map\\.def\\s*(\\r*)\\n"),
					"#include_drs random_map.def$1\n<PLAYER_SETUP>$1\n  terrain_state 0 0 0 1$1\n");
		}
        */
        std::map<int,bool> terrainsUsed = {
            {0,false}, {1,false}, {2,false}, {3,false}, {4,false},
            {5,false}, {6,false}, {9,false}, {10,false}, {11,false},
            {12,false}, {13,false}, {14,false}, {16,false}, {20,false},
            {21,false}, {22,false}, {23,false}, {24,false}, {25,false},
            {35,false}, {38,false}, {39,false}, {40,false},

            {41,false}, {42,false}, {43,false}, {44,false}, {45,false},
            {46,false}, {47,false}, {48,false}, {49,false}, {50,false},
            {51,false}, {52,false}, {53,false}, {54,false}, {55,false},
            {56,false}, {57,false}, {58,false}, {59,false}, {60,false},
            {61,false}, {62,false}
        };

        for (repIt = replacements.begin(); repIt != replacements.end(); repIt++) {         
            std::regex terrainName = std::regex(rt_getPattern());
            if(std::regex_search(map,terrainName)) {
                if(rt_getNewId() < 41) //41 is also an expansion terrain, but that's okay, it's a fixed replacement
                    terrainsUsed.at(rt_getNewId()) = true;
                terrainsUsed.at(rt_getOldId()) = true;
            }
        }

        for (repIt = replacements.begin(); repIt != replacements.end(); repIt++) {
            if(!terrainsUsed.at(rt_getOldId()))
                continue;
            // Check if replacement candidate is already used
            std::regex terrainConstDef = std::regex("#const\\s+" +rt_getPattern()+ "\\s+" +std::to_string(rt_getOldId()));
            int usedTerrain = rt_getNewId();
            //If it's one of the terrains with a shared slp, we need to search the map for these other terrains too, else just the usedTerrain
            if(rt_getTerrainType() > TerrainType::FixedTerrain
                   && isTerrainUsed(usedTerrain, terrainsUsed, map, terrainsPerType[rt_getTerrainType()])) {
                bool success = false;
                for(std::map<int,std::regex>::iterator tIt = terrainsPerType[rt_getTerrainType()].begin();
                    tIt != terrainsPerType[rt_getTerrainType()].end(); tIt++) {
                    if(terrainsUsed.at(tIt->first))
                        continue;
                    else if(isTerrainUsed(tIt->first,terrainsUsed,map, terrainsPerType[rt_getTerrainType()])) {
                        continue;
                    }
                    success = true;
                    usedTerrain = tIt->first;
                    terrainsUsed.at(tIt->first) = true;
                    break;
                }
                if(!success && rt_getTerrainType() == TerrainType::LandTerrain && !isTerrainUsed(5, terrainsUsed, map, terrainsPerType[rt_getTerrainType()])) {
                    usedTerrain = 5; //Leaves is a last effort, usually likely to be used already
                    terrainsUsed[5] = true;
                }
            }

            if(usedTerrain != rt_getNewId()) {
                std::regex terrainName = std::regex(rt_getPattern());
                map = std::regex_replace(map,terrainName, "MY"+rt_getReplaceName());
                terrainConstDef = std::regex("#const\\sMY+" +rt_getPattern()+ "\\s+" +std::to_string(rt_getOldId()));
                std::string temp = std::regex_replace(map,terrainConstDef, "#const MY"+rt_getReplaceName()+" "+std::to_string(usedTerrain));
                if (temp != map)
                    map = temp;
                else  {
                    map = "#const MY"+rt_getReplaceName()+" "+std::to_string(usedTerrain)+"\n"+map;
                }
            } else {
                map = std::regex_replace(map,terrainConstDef, "#const "+rt_getReplaceName()+" "+std::to_string(usedTerrain));
            }


            if(rt_getTerrainType() == TerrainType::None ||
                    (rt_getTerrainType() == TerrainType::WaterTerrain && usesMultipleWaterTerrains(map,terrainsUsed)) )
                continue;

            terrainOverrides[slpNumbers.at(usedTerrain)] = newTerrainFiles.at(rt_getSLPName());

            if(rt_getTerrainType() == TerrainType::ForestTerrain) {
                upgradeTrees(usedTerrain, rt_getOldId(), map);
            }


		}
        if(terrainsUsed.at(11)) {
            terrainOverrides["15004.slp"] = newTerrainFiles.at("15004.slp");
            terrainOverrides["15005.slp"] = newTerrainFiles.at("15005.slp");
            terrainOverrides["15021.slp"] = newTerrainFiles.at("15021.slp");
            terrainOverrides["15022.slp"] = newTerrainFiles.at("15022.slp");
            terrainOverrides["15023.slp"] = newTerrainFiles.at("15023.slp");
        }
        std::ofstream out(outputDir.string()+"\\"+mapName);
        out << map;
		out.close();
		if (mapName.substr(0,3) == "rw_" || mapName.substr(0,3) == "sm_") {
			std::string scenarioFile = it->stem().string()+".scx";
            terrainOverrides[scenarioFile] = fs::path(inputDir.string()+"\\"+scenarioFile);
		}
		if (terrainOverrides.size() != 0) {
            createZRmap(terrainOverrides, outputDir, mapName);
		}
		terrainOverrides.clear();
	}
    emit increaseProgress(1); //16+20 22?
}

bool WKConverter::usesMultipleWaterTerrains(std::string& map, std::map<int,bool>& terrainsUsed) {
    if(!terrainsUsed[23]) {
        int hits = (int)std::regex_search(map,std::regex("\\WDLC_WATER4\\W")) + (int)std::regex_search(map,std::regex("\\DLC_WATER5\\W"))
                + (int)std::regex_search(map,std::regex("\\WATER\\W")) + (int)std::regex_search(map,std::regex("\\MED_WATER\\W"))
                + (int)std::regex_search(map,std::regex("\\DEEP_WATER\\W"));
        terrainsUsed[23] = hits > 1;
    }
    return terrainsUsed[23];
}

void WKConverter::upgradeTrees(int usedTerrain, int oldTerrain, std::string& map) {

    std::string newTree;
    std::string oldTree;
    switch(usedTerrain) {
        case 10: oldTree = "FOREST_TREE"; break;
        case 13: oldTree = "PALMTREE"; break;
        case 21: oldTree = "SNOWPINETREE"; break;
    }
    if(oldTerrain == 48) {
        newTree = "DRAGONTREE";
    } else {
        newTree = "DLC_RAINTREE";
    }
    if(map.find("<PLAYER_SETUP>")!=std::string::npos)
        map = std::regex_replace(map, std::regex("<PLAYER_SETUP>\\s*(\\r*)\\n"),
            "<PLAYER_SETUP>$1\n  effect_amount GAIA_UPGRADE_UNIT "+oldTree+" "+newTree+" 0$1\n");
    else
        map = std::regex_replace(map, std::regex("#include_drs\\s+random_map\\.def\\s*(\\r*)\\n"),
            "#include_drs random_map.def$1\n<PLAYER_SETUP>$1\n  effect_amount GAIA_UPGRADE_UNIT "+oldTree+" "+newTree+" 0$1\n");
}

bool WKConverter::isTerrainUsed(int terrain, std::map<int,bool>& terrainsUsed, std::string& map, std::map<int,std::regex>& patterns) {
    if(terrain == 5 || terrain == 10) {
        if(!terrainsUsed[63]) {
            terrainsUsed[63] = true;
            terrainsUsed[64] = std::regex_search(map,std::regex("\\WPINE_FOREST\\W")) || std::regex_search(map,std::regex("\\WLEAVES\\W"))
              || std::regex_search(map,std::regex("\\WJUNGLE\\W")) ||  std::regex_search(map,std::regex("\\BAMBOO\\W")) || std::regex_search(map,std::regex("\\WFOREST\\W"));
        }
        return terrainsUsed[64];
    }
    if(terrain == 13 || terrain == 14) {
        if(!terrainsUsed[65]) {
            terrainsUsed[65] = true;
            terrainsUsed[66] = std::regex_search(map,std::regex("\\WPALM_DESERT\\W")) || std::regex_search(map,std::regex("\\WDESERT\\W"));
        }
        return terrainsUsed[66];
    } else {
        return terrainsUsed[terrain] = std::regex_search(map,patterns[terrain]);
    }
}

void WKConverter::createZRmap(std::map<std::string,fs::path>& terrainOverrides, fs::path outputDir, std::string mapName) {
    QuaZip zip(QString((outputDir.string()+"\\ZR@"+mapName).c_str()));
    zip.open(QuaZip::mdAdd, nullptr);
    terrainOverrides[mapName] = fs::path(outputDir.string()+"\\"+mapName);
    for(std::map<std::string,fs::path>::iterator files = terrainOverrides.begin(); files != terrainOverrides.end(); files++) {
        QuaZipFile outFile(&zip);
        QuaZipNewInfo fileInfo(QString(files->first.c_str()));;
        fileInfo.uncompressedSize = fs::file_size(files->second);
        outFile.open(QIODevice::WriteOnly,fileInfo,nullptr,0,0,0,false);
        QFile inFile;
        inFile.setFileName(files->second.string().c_str());
        inFile.open(QIODevice::ReadOnly);
        copyData(inFile, outFile);
        outFile.close();
        inFile.close();
    }
    zip.close();
    fs::remove(fs::path(outputDir.string()+"\\"+mapName));
}

void WKConverter::transferHdDatElements(genie::DatFile *hdDat, genie::DatFile *aocDat) {

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

    terrainSwap(hdDat, aocDat, 33,38,15030); // Snow road to snow dirt
    terrainSwap(hdDat, aocDat, 38,45,15032); // Cracked Earth to Snow road

    terrainSwap(hdDat, aocDat, 11,54,15012); //mangrove terrain
    terrainSwap(hdDat, aocDat, 20,55,15012); //mangrove forest
    terrainSwap(hdDat, aocDat, 41,50,15013); //acacia forest
    terrainSwap(hdDat, aocDat, 16,49,15025); //baobab forest

    std::map<int,std::string> newTerrainSlps = {
        {15012,"DLC_MANGROVEFOREST.slp"},
        {15013,"ACACIA_FOREST.slp"},
        {15025,"BAOBAB.slp"},
        {15003,"15003.slp"},
        {15032,"CRACKEDIT.slp"},
        {15034,"ICE_SOLID.slp"},
        {15020,"ICE_BEACH.slp"}
    };

    for(std::map<int,std::string>::iterator iter = newTerrainSlps.begin(); iter != newTerrainSlps.end(); iter++) {
        if(slpFiles[iter->first].empty())
            slpFiles[iter->first] = newTerrainFiles[iter->second];
    }

	aocDat->TerrainBlock.Terrains[35].TerrainToDraw = -1;
    aocDat->TerrainBlock.Terrains[35].SLP = 15020;
	aocDat->TerrainBlock.Terrains[35].Name2 = "g_ice";

    aocDat->TerrainBlock.Terrains[37].TerrainToDraw = -1;
    aocDat->TerrainBlock.Terrains[37].SLP = 15034;

    aocDat->TerrainBlock.Terrains[36].TerrainToDraw = -1;
    aocDat->TerrainBlock.Terrains[36].SLP = 15027;

    aocDat->TerrainBlock.Terrains[15].SLP = 15003;
    aocDat->TerrainBlock.Terrains[40].SLP = 15033;
    aocDat->TerrainBlock.Terrains[40].TerrainToDraw = -1;
    aocDat->TerrainBlock.Terrains[15].TerrainToDraw = -1;
}

void WKConverter::adjustArchitectureFlags(genie::DatFile *aocDat, std::string flagFilename) {
    std::string line;
    std::ifstream flagFile(flagFilename);
    while (std::getline(flagFile, line)) {
        int index = line.find(',');
        int civID = std::atoi(line.substr(0,index).c_str());
        line = line.substr(index+1, std::string::npos);
        index = line.find(',');
        int unitID = std::atoi(line.substr(0,index).c_str());
        line = line.substr(index+1, std::string::npos);
        index = line.find(',');
        int delta = std::atoi(line.substr(0,index).c_str());
        line = line.substr(index+1, std::string::npos);
        index = line.find(',');
        int x = std::atoi(line.substr(0,index).c_str());
        int y = std::atoi(line.substr(index+1,std::string::npos).c_str());
        if (unitID == 18 || unitID == 103) {
            aocDat->Graphics[aocDat->Civs[civID].Units[unitID].StandingGraphic.first].Deltas[delta].DirectionX = x;
            aocDat->Graphics[aocDat->Civs[civID].Units[unitID].StandingGraphic.first].Deltas[delta].DirectionY = y;
        } else {
            aocDat->Graphics[aocDat->Civs[civID].Units[unitID].Creatable.GarrisonGraphic].Deltas[delta].DirectionX = x;
            aocDat->Graphics[aocDat->Civs[civID].Units[unitID].Creatable.GarrisonGraphic].Deltas[delta].DirectionY = y;
        }
    }
    flagFile.close();
}

void WKConverter::patchArchitectures(genie::DatFile *aocDat) {


    char const * civLetterList = "XEWMFI";
    civLetters.insert(civLetterList, civLetterList + strlen (civLetterList));

    /*
     * Manual Fixes before the IA seperation
     */

    aocDat->Graphics[3229].Deltas[0].GraphicID = 427;
    aocDat->Graphics[3229].Deltas[1].GraphicID = 428;

    aocDat->Graphics[9196].Deltas.erase(aocDat->Graphics[9196].Deltas.begin());

    /*
     * IA seperation
     */

    short buildingIDs[] = {10, 14, 18, 19, 20, 30, 31, 32, 47, 49, 51, 63, 64, 67, 71, 78, 79, 80, 81, 82, 84, 85, 86, 87, 88,
						90, 91, 92, 95, 101, 103, 104, 105, 110, 116, 117, 129, 130, 131, 132, 133, 137, 141, 142, 150, 153,
						155, 179, 190, 209, 210, 234, 235, 236, 276, 463, 464, 465, 481, 482, 483, 484, 487, 488, 490, 491, 498,
                        562, 563, 564, 565, 584, 585, 586, 587, 597, 611, 612, 613, 614, 615, 616, 617, 659, 660, 661,
                        662, 663, 664, 665, 666, 667, 668, 669, 670, 671, 672, 673, 674, 1102, 1189};
    short unitIDs[] = {17, 21, 420, 442, 527, 528, 529, 532, 539, 545, 691, 1103, 1104};
    short civIDs[] = {13,23,7,17,14,31,21,6,11,12,27,1,4,18,9,8,16,24};
    short burmese = 30; //These are used for ID reference
    for(short c = 0; c < sizeof(civIDs)/sizeof(short); c++) {

        std::map<short,short> replacedGraphics;
        std::map<short,short> replacedFlags;
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
            oldGraphicID = aocDat->Civs[civIDs[c]].Units[buildingIDs[b]].Creatable.GarrisonGraphic;
            if(oldGraphicID != -1) {
                if(replacedFlags[oldGraphicID] > 0)
                    aocDat->Civs[civIDs[c]].Units[buildingIDs[b]].Creatable.GarrisonGraphic = replacedFlags[oldGraphicID];
                else {
                    genie::Graphic newFlag = aocDat->Graphics[oldGraphicID];
                    newFlag.ID = aocDat->Graphics.size();
                    aocDat->Graphics.push_back(newFlag);
                    aocDat->GraphicPointers.push_back(1);
                    replacedFlags[oldGraphicID] = newFlag.ID;
                    aocDat->Civs[civIDs[c]].Units[buildingIDs[b]].Creatable.GarrisonGraphic = newFlag.ID;
                }
            }
		}
		//units like ships
		for(unsigned int u = 0; u < sizeof(unitIDs)/sizeof(short); u++) {
			replaceGraphic(aocDat, &aocDat->Civs[civIDs[c]].Units[unitIDs[u]].StandingGraphic.first, aocDat->Civs[burmese].Units[unitIDs[u]].StandingGraphic.first, c, replacedGraphics);
			replaceGraphic(aocDat, &aocDat->Civs[civIDs[c]].Units[unitIDs[u]].DeadFish.WalkingGraphic.first, aocDat->Civs[burmese].Units[unitIDs[u]].DeadFish.WalkingGraphic.first, c, replacedGraphics);
			replaceGraphic(aocDat, &aocDat->Civs[civIDs[c]].Units[unitIDs[u]].Type50.AttackGraphic, aocDat->Civs[burmese].Units[unitIDs[u]].Type50.AttackGraphic, c, replacedGraphics);
		}

        emit increaseProgress(1); //34-51
	}

    //Separate Units into 4 major regions (Europe, Asian, Southern, American)
    std::vector<std::vector<short>> civGroups = { {3,4,11}, {7,23}, {14,19,24}, //Central Eu, Orthodox, Mediterranean
                    {5},{6,18},{28,29,30,31}, //Japanese, East Asian, SE Asian
                    {8,9,10,27},{20},{25,26}, //Middle Eastern, Indian, African
                    {15,16,21}, //American
                    {17,12},{22} //Steppe, Magyars
                    };
    //std::map<int,int> slpIdConversion = {{2683,0},{376,2},{4518,1},{2223,3},{3482,4},{3483,5},{4172,6},{4330,7},{889,10},{4612,16},{891,17},{4611,15},{3596,12},
    //						 {4610,14},{3594,11},{3595,13},{774,131},{779,134},{433,10},{768,130},{433,10},{771,132},{775,133},{3831,138},{3827,137}};
    // short cgBuildingIDs[] = {12, 68, 70, 109, 598, 618, 619, 620}; // There's no IA dark age building mod, but regular ones that get broken by enabling this, so we won't do it.
    short cgUnitIDs[] = {125,134,286,4,3,5,98,6,100,7,238,24,26,37,113,38,111,39,34,74,152,75,154,77,180,93,140,283,139,329,330,495,358,501,
                        359,502,440,441,480,448,449,473,500,474,631,492,496,546,547,567,568,569,570};
    for(int cg = 0; cg < civGroups.size(); cg++) {
        if(cg == 3) {
            /* We'll temporarily give the monk 10 frames so this value is the one used for the new
             * Asian and African/Middle Eastern civs.
             */
            aocDat->Graphics[998].FrameCount = 10;
        } else if (cg == 11) {
            aocDat->Graphics[998].FrameCount = 6; //Old Value again
        }
		short monkHealingGraphic;
        if (cg != 9) {
            int newSLP = 60000+10000*cg+776;
			genie::Graphic newGraphic = aocDat->Graphics[1597];
			monkHealingGraphic = aocDat->Graphics.size();
			newGraphic.ID = monkHealingGraphic;
			newGraphic.SLP = newSLP;
			aocDat->Graphics.push_back(newGraphic);
			aocDat->GraphicPointers.push_back(1);
            slpFiles[newSLP] = settings->HDPath/("resources\\_common\\drs\\graphics\\776.slp");
		} else {
			monkHealingGraphic = 7340; //meso healing graphic
		}
        std::map<short,short> replacedGraphics;
        for(unsigned int civ = 0; civ < civGroups[cg].size(); civ++) {

            /*
			for(unsigned int b = 0; b < sizeof(cgBuildingIDs)/sizeof(short); b++) {
                replaceGraphic(aocDat, &aocDat->Civs[civGroups[cg][civ]].Units[cgBuildingIDs[b]].StandingGraphic.first, -1, cg, replacedGraphics, slpIdConversion);
                for(std::vector<genie::unit::DamageGraphic>::iterator it = aocDat->Civs[civGroups[cg][civ]].Units[cgBuildingIDs[b]].DamageGraphics.begin();
                    it != aocDat->Civs[civGroups[cg][civ]].Units[cgBuildingIDs[b]].DamageGraphics.end(); it++) {
                    replaceGraphic(aocDat, &(it->GraphicID), -1, cg, replacedGraphics, slpIdConversion);
				}
            }*/
            //Units
            for(unsigned int u = 0; u < sizeof(cgUnitIDs)/sizeof(short); u++) {
                replaceGraphic(aocDat, &aocDat->Civs[civGroups[cg][civ]].Units[cgUnitIDs[u]].StandingGraphic.first, aocDat->Civs[0].Units[cgUnitIDs[u]].StandingGraphic.first, cg, replacedGraphics, true);
                if (aocDat->Civs[civGroups[cg][civ]].Units[cgUnitIDs[u]].DeadFish.WalkingGraphic.first != -1) { //Not a Dead Unit
                    replaceGraphic(aocDat, &aocDat->Civs[civGroups[cg][civ]].Units[cgUnitIDs[u]].DeadFish.WalkingGraphic.first, aocDat->Civs[0].Units[cgUnitIDs[u]].DeadFish.WalkingGraphic.first, cg, replacedGraphics, true);
                    replaceGraphic(aocDat, &aocDat->Civs[civGroups[cg][civ]].Units[cgUnitIDs[u]].Type50.AttackGraphic, aocDat->Civs[0].Units[cgUnitIDs[u]].Type50.AttackGraphic, cg, replacedGraphics, true);
                    replaceGraphic(aocDat, &aocDat->Civs[civGroups[cg][civ]].Units[cgUnitIDs[u]].DyingGraphic.first, aocDat->Civs[0].Units[cgUnitIDs[u]].DyingGraphic.first, cg, replacedGraphics, true);
                }
            }
			//special UP healing slp workaround
            for(unsigned int civ = 0; civ < civGroups[cg].size(); civ++) {
				size_t code = 0x811E0000+monkHealingGraphic;
				int ccode = (int) code;
                aocDat->Civs[civGroups[cg][civ]].Units[125].LanguageDLLHelp = ccode;

                if ((cg >= 3 && cg <= 5) || cg == 10) { //Shaman icons, "Eastern" civs
                    aocDat->Civs[civGroups[cg][civ]].Units[125].IconID = 218;
                    aocDat->Civs[civGroups[cg][civ]].Units[286].IconID = 218;
                } else if (cg >= 6 && cg <= 8) { // Imam Icons, Middle Eastern/southern civ groups
                    aocDat->Civs[civGroups[cg][civ]].Units[125].IconID = 169;
                    aocDat->Civs[civGroups[cg][civ]].Units[286].IconID = 169;
				}

			}

        }
        emit increaseProgress(1); //52-63
    }

    /*
     * Manual fixes after IA seperation
     */

    //Let the Mongol Mill have 40 frames instead of 8/10, which is close to the dark age mill with 43 frames
    aocDat->Graphics[aocDat->Civs[12].Units[129].StandingGraphic.first].FrameCount = 40;
    aocDat->Graphics[aocDat->Civs[12].Units[130].StandingGraphic.first].FrameCount = 40;
    //aocDat->Graphics[aocDat->Graphics[aocDat->Civs[12].Units[129].StandingGraphic.first].Deltas[0].GraphicID].FrameCount = 40;
    //aocDat->Graphics[aocDat->Graphics[aocDat->Civs[12].Units[130].StandingGraphic.first].Deltas[1].GraphicID].FrameCount = 40;

    //Let the Berber Mill have 40 frames instead of 8/10, which is close to the african mill with 38 frames
    aocDat->Graphics[aocDat->Civs[27].Units[129].StandingGraphic.first].FrameCount = 40;
    aocDat->Graphics[aocDat->Civs[27].Units[130].StandingGraphic.first].FrameCount = 40;
    aocDat->Graphics[aocDat->Graphics[aocDat->Civs[27].Units[129].StandingGraphic.first].Deltas[0].GraphicID].FrameCount = 40;
    aocDat->Graphics[aocDat->Graphics[aocDat->Civs[27].Units[130].StandingGraphic.first].Deltas[1].GraphicID].FrameCount = 40;

    //Fix the missionary converting frames while we're at it
    aocDat->Graphics[6616].FrameCount = 14;
    //Manual fix for missing portugese flags
    slpFiles[41178] = settings->HDPath/("resources\\_common\\drs\\graphics\\4522.slp");
    slpFiles[41181] = settings->HDPath/("resources\\_common\\drs\\graphics\\4523.slp");

}

void WKConverter::replaceGraphic(genie::DatFile *aocDat, short* graphicID, short compareID, short c, std::map<short,short>& replacedGraphics, bool civGroups) {
    if(replacedGraphics.count(*graphicID) != 0)
		*graphicID = replacedGraphics[*graphicID];
    else {
        short newGraphicID;
        std::vector<short> duplicatedGraphics;
        newGraphicID = duplicateGraphic(aocDat, replacedGraphics, duplicatedGraphics, *graphicID, compareID, c, civGroups);
		*graphicID = newGraphicID;
	}
}

bool WKConverter::checkGraphics(genie::DatFile *aocDat, short graphicID, std::vector<int> checkedGraphics) {
    /*
     * Tests if any SLP of a graphic, or a graphic Delta is in the right range (18000-19000),
     * which means they are a civ-dependant graphic (in this case for SEA civs) instead of a shared graphic
     * (Like flags, flames or stuff like that)
     *
     * Returns: true if at least one SLP is in this range, false otherwise
     * Parameters:
     * aocDat: The dat file to be checked
     * graphicID: The ID of the graphic to be checked
     * checkedGraphics: The graphics checked so far, to avoid an endless recursion in case of circular references
     */
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

short WKConverter::duplicateGraphic(genie::DatFile *aocDat, std::map<short,short>& replacedGraphics, std::vector<short> duplicatedGraphics, short graphicID, short compareID, short offset, bool civGroups) {

    /*
     * Parameters:
     * aocDatFile: The Dat File of WK to be patched
     * replacedGraphics: These graphics have already been replaced, we can just return the id.
     * duplicatedGraphics: These Graphics have already been duplicated with a previous duplicateGraphic call. Passed on to avoid circular loops with recursive calls for deltas
     * graphicID: The ID of the graphic to be duplicated
     * compareID: The ID of the same building of the burmese, to serve as a comparison. If -1, it's one of the monk/dark age graphics to be duped, see slpIdConversion parameter
     * offset: The offset of the civ/civ group for the new SLPs (24000+offset*1000 and so on)
     * civGroup: This is mostly unit graphics, which are only seperated into civ groups, not per civs
     */

    if(replacedGraphics.count(graphicID)) //We've already replaced this, return the new graphics ID
        return replacedGraphics[graphicID];

    /*
     * Check if at least one SLP in this graphic or graphic deltas is in the right range,
     * else we don't need to do any duplication in which case we can just return the current graphic ID as a result
     */
    if (civGroups && aocDat->Graphics[compareID].SLP >= 10000)
        throw std::runtime_error("Unit slp over 10k");
    if (!civGroups && (aocDat->Graphics[compareID].SLP < 18000 || aocDat->Graphics[compareID].SLP >= 19000)) {
		std::vector<int> checkedGraphics;
		if(!checkGraphics(aocDat, compareID, checkedGraphics))
			return graphicID;
	}

    genie::Graphic newGraphic = aocDat->Graphics[graphicID];
    int newSLP = civGroups?60000:24000;

	short newGraphicID = aocDat->Graphics.size();
    if(civGroups) { //Unit Graphics for the 4 civ groups
        newSLP += 10000*offset+aocDat->Graphics[graphicID].SLP;
    } else if (aocDat->Graphics[compareID].SLP < 18000 || aocDat->Graphics[compareID].SLP >= 19000) {
        if(slpFiles.count(aocDat->Graphics[graphicID].SLP)+aocSlpFiles.count(aocDat->Graphics[graphicID].SLP) != 0) {
            newSLP = aocDat->Graphics[graphicID].SLP;
        } else
            newSLP = -1; //seems to happen only for 15516 and 15536 but not cause harm in these cases
    }
    else
        newSLP += aocDat->Graphics[compareID].SLP - 18000 + 1000*offset;

    replacedGraphics[newGraphic.ID] = newGraphicID;
    duplicatedGraphics.push_back(newGraphic.ID);
    newGraphic.ID = newGraphicID;
    if(newSLP > 0 && newSLP != aocDat->Graphics[graphicID].SLP && newSLP != aocDat->Graphics[compareID].SLP) {
        // This is a graphic where we want a new SLP file (as opposed to one where the a new SLP mayb just be needed for some deltas
        fs::path src = settings->HDPath/("resources\\_common\\drs\\gamedata_x2\\"+std::to_string(newGraphic.SLP)+".slp");
		if(fs::exists(src))
			slpFiles[newSLP] = src;
		else {
            src = settings->HDPath/("resources\\_common\\drs\\graphics\\"+std::to_string(newGraphic.SLP)+".slp");
			if(fs::exists(src))
				slpFiles[newSLP] = src;
		}        
        newGraphic.SLP = newSLP;
	}
    std::string civCode;
    if(civGroups) {
        switch (offset) {
            case 0: civCode = "AS"; break;
            case 1: civCode = "SO"; break;
            case 2: civCode = "AM"; break;
        }
    } else {
        switch (offset) {
            case 0: civCode = "CE"; break;
            case 1: civCode = "SL"; break;
            case 2: civCode = "BY"; break;
            case 3: civCode = "HU"; break;
            case 4: civCode = "SP"; break;
            case 5: civCode = "VI"; break;
            case 6: civCode = "IC"; break;
            case 7: civCode = "CH"; break;
            case 8: civCode = "VK"; break;
            case 9: civCode = "MO"; break;
            case 10: civCode = "BE"; break;
            case 11: civCode = "BR"; break;
            case 12: civCode = "TE"; break;
            case 13: civCode = "KO"; break;
            case 14: civCode = "SA"; break;
            case 15: civCode = "PE"; break;
            case 16: civCode = "MY"; break;
            case 17: civCode = "PO"; break;
        }
    }

    char civLetter = newGraphic.Name.at(newGraphic.Name.length()-1);
    if(civLetters.count(civLetter)) {
        if(newGraphic.Name2 == newGraphic.Name) {
            newGraphic.Name2.replace(newGraphic.Name2.length()-1,1,civCode);
            newGraphic.Name = newGraphic.Name2;
        } else
            newGraphic.Name.replace(newGraphic.Name.length()-1,1,civCode);
    }
	aocDat->Graphics.push_back(newGraphic);
	aocDat->GraphicPointers.push_back(1);

    if(!civGroups && aocDat->Graphics[compareID].Deltas.size() == newGraphic.Deltas.size()) {
		/* don't copy graphics files if the amount of deltas is different to the comparison,
		 * this is usually with damage graphics and different amount of Flames.
		*/
		std::vector<genie::GraphicDelta>::iterator compIt = aocDat->Graphics[compareID].Deltas.begin();
		for(std::vector<genie::GraphicDelta>::iterator it = newGraphic.Deltas.begin(); it != newGraphic.Deltas.end(); it++) {
            if(it->GraphicID != -1 && std::find(duplicatedGraphics.begin(), duplicatedGraphics.end(), it->GraphicID) == duplicatedGraphics.end())
                it->GraphicID = duplicateGraphic(aocDat, replacedGraphics, duplicatedGraphics, it->GraphicID, compIt->GraphicID, offset);
			compIt++;
		}
		aocDat->Graphics.at(newGraphicID) = newGraphic;
    }
	return newGraphicID;
}

void WKConverter::terrainSwap(genie::DatFile *hdDat, genie::DatFile *aocDat, int tNew, int tOld, int slpID) {
	aocDat->TerrainBlock.Terrains[tNew] = hdDat->TerrainBlock.Terrains[tOld];
	aocDat->TerrainBlock.Terrains[tNew].SLP = slpID;
	if (tNew == 41) {
		for(size_t j = 0; j < aocDat->TerrainRestrictions.size(); j++) {
			aocDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier.push_back(hdDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier[tOld]);
            aocDat->TerrainRestrictions[j].TerrainPassGraphics.push_back(hdDat->TerrainRestrictions[j].TerrainPassGraphics[tOld]);
		}
	} else {
		for(size_t j = 0; j < aocDat->TerrainRestrictions.size(); j++) {
			aocDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier[tNew] = hdDat->TerrainRestrictions[j].PassableBuildableDmgMultiplier[tOld];
			aocDat->TerrainRestrictions[j].TerrainPassGraphics[tNew] = hdDat->TerrainRestrictions[j].TerrainPassGraphics[tOld];
		}
	}
}

bool WKConverter::identifyHotkeyFile(fs::path directory, fs::path& maxHki, fs::path& lastEditedHki) {
    /*
     * Checks all .hki file in directory. The hotkey file with the highest number is saved in maxHki,
     * the hotkey file that was last edited in lastEditedHki. These are the two most likely candidates for the hotkey file
     * that's actually in use.
     *
     * Returns true if a hotkey file was found
     */
    int maxHkiNumber = -1;
    std::time_t lastHkiEdit = std::time_t(0);
    for (fs::directory_iterator current(directory), end;current != end; ++current) {
        fs::path currentPath(current->path());
        std::string extension = currentPath.extension().string();
        if (extension == ".hki") {
            std::string numberString = currentPath.stem().string().substr(6);
            if(numberString.find_first_not_of( "0123456789" ) != std::string::npos)
                continue;
            int hkiNumber = std::atoi(numberString.c_str());
            if (hkiNumber > maxHkiNumber) {
                maxHkiNumber = hkiNumber;
                maxHki = currentPath;
            }
            std::time_t lastModified = fs::last_write_time(currentPath);
            if (lastModified > lastHkiEdit) {
                lastHkiEdit = lastModified;
                lastEditedHki = currentPath;
            }

        }
    }
    if (maxHkiNumber != -1)
        return true;
    else
        return false;
}

void WKConverter::copyHotkeyFile(fs::path maxHki, fs::path lastEditedHki, fs::path dst) {
    /*
     * See identifyHotkeyFile for extra info on maxHki, lastEditedHki
     * copies the .hki file maxHki into the directory dst. If maxHki already exists, the last 2 versions are kept as backup
     * If maxHki is not the same as lastEditedHki, it also copies lastEditedHki into dst, with the suffix "_alt".
     * Players can remove the suffix and thus replace the .hki file in case maxHki did not contain the correct set
     */
    fs::path bak1 = dst.parent_path() / (dst.stem().string() + "_bak1.hki");
    fs::path bak2 = dst.parent_path() / (dst.stem().string() + "_bak2.hki");
    if(fs::exists(bak1))
        fs::copy_file(bak1,bak2,fs::copy_option::overwrite_if_exists);
    if(fs::exists(dst))
        fs::copy_file(dst,bak1,fs::copy_option::overwrite_if_exists);
    fs::copy_file(maxHki,dst,fs::copy_option::overwrite_if_exists);
    if(!fs::equivalent(lastEditedHki,maxHki)) {
        fs::copy_file(lastEditedHki,dst.parent_path() / (dst.stem().string() + "_alt.hki"),fs::copy_option::overwrite_if_exists);
    }
}

void WKConverter::removeWkHotkeys() {
    /*
     * This function removes hotkeys in the WK-specific folders, if a user wants to use the same
     * set of hotkeys for WK and AoC. With the copyHotkeyFile function, 2 backups are kept
     */

    fs::path maxDstHki;
    fs::path lastDstHki;
    fs::path aocHkiPath = resourceDir / "player1.hki";

    if(settings->useExe) {
        if(identifyHotkeyFile(settings->upDir,maxDstHki,lastDstHki)) {
            copyHotkeyFile(aocHkiPath,aocHkiPath,maxDstHki);
            fs::remove(maxDstHki);
            if(!fs::equivalent(maxDstHki,lastDstHki)) {
                copyHotkeyFile(aocHkiPath,aocHkiPath,lastDstHki);
                fs::remove(lastDstHki);
            }
        }
    } else if(identifyHotkeyFile(settings->vooblyDir,maxDstHki,lastDstHki)) {
        copyHotkeyFile(aocHkiPath,aocHkiPath,maxDstHki);
        fs::remove(maxDstHki);
        if(!fs::exists(lastDstHki)) {
            copyHotkeyFile(aocHkiPath,aocHkiPath,lastDstHki);
            fs::remove(lastDstHki);
        }
    }
}

void WKConverter::hotkeySetup() {

    /*
     * A method to hopefully copy the correct hotkey files so there's no extra setup needed. The User has 3 options:
     * 1) Choose "Voobly/Aoc hotkeys". If a voobly .hki file is in the correct place, it will just copy the .nfz profile file
     * so the same hki file is used for WK as well. If there's no .hki file in place, it will copy a set of default aoc hotkeys
     * 2) Choose "Hd hotkeys for this mod only". Copies the HD hotkeys but puts them in the WK folder so it will only be used for that.
     * If it can't find a HD hotkey file, uses default HD hotkeys instead
     * 3) Same as 2) except it replaces the hki file for regular aoc instead (so that it's used for both aoc and wk)
     *
     */

    fs::path maxDstHki;
    fs::path lastDstHki;
    fs::path maxSrcHki;
    fs::path lastSrcHki;

    fs::path nfz1Path = resourceDir / "player1.nfz";
    fs::path nfzPath = settings->outPath / "player.nfz";
    fs::path aocHkiPath = resourceDir / "player1.hki";
    fs::path nfzOutPath = settings->useExe ? settings->nfzUpOutPath : settings->nfzVooblyOutPath;

    /*
	if(!fs::exists(hkiPath)) { //If player0.hki doesn't exist, look for player1.hki, otherwise use default HD hotkeys
        if(fs::exists(settings->HDPath/"Profiles\\player1.hki"))
                hkiPath = settings->HDPath/"Profiles\\player1.hki";
		else
				hkiPath = resourceDir / "player1_age2hd.hki";
	}
    */
    boost::system::error_code ec;

	if(fs::exists(nfzPath)) //Copy the Aoc Profile
        fs::copy_file(nfzPath, nfzOutPath, ec);
    else{ //otherwise copy the default profile included
        fs::copy_file(nfz1Path, nfzOutPath, ec);
        fs::copy_file(nfz1Path, nfzPath, ec);
    }
    if(settings->useBoth) { //Profiles for UP
        fs::copy_file(nfzPath,settings->nfzUpOutPath, ec);
	}
	//Copy hotkey files
    if (settings->hotkeyChoice == 1) { //Use AoC/Voobly Hotkeys
        removeWkHotkeys();
        if(!identifyHotkeyFile(settings->outPath, maxDstHki, lastDstHki))//In case there are no voobly hotkeys, copy standard aoc hotkeys
            fs::copy_file(aocHkiPath, settings->outPath/"player1.hki");
    } else {
        if(!identifyHotkeyFile(settings->HDPath/"Profiles", maxSrcHki, lastSrcHki)) {
            maxSrcHki = resourceDir / "player1_age2hd.hki";
            lastSrcHki = maxSrcHki;
        }
    }
    if (settings->hotkeyChoice == 2) { //Use HD hotkeys only for WK
        if(!identifyHotkeyFile(installDir, maxDstHki, lastDstHki)) {
            if(!identifyHotkeyFile(settings->outPath, maxDstHki, lastDstHki)) {
                maxDstHki = installDir / "player1.hki";
                lastDstHki = maxDstHki;
            } else {
                maxDstHki = installDir / maxDstHki.filename();
                lastDstHki = installDir / lastDstHki.filename();
            }
        }
        copyHotkeyFile(maxSrcHki,lastSrcHki,maxDstHki);
        if(!fs::equivalent(maxDstHki,lastDstHki))
            copyHotkeyFile(maxSrcHki,lastSrcHki,lastDstHki);
        if(settings->useBoth) {
            fs::path maxUpDstHki;
            fs::path lastUpDstHki;
            if(!identifyHotkeyFile(settings->upDir, maxUpDstHki, lastUpDstHki)) {
                maxUpDstHki = settings->upDir / maxDstHki.filename();
                lastUpDstHki = settings->upDir / lastDstHki.filename();
            }
            copyHotkeyFile(maxSrcHki,lastSrcHki,maxUpDstHki);
            if(!fs::equivalent(maxUpDstHki,lastUpDstHki))
                copyHotkeyFile(maxSrcHki,lastSrcHki,lastUpDstHki);
        }
	}
    if(settings->hotkeyChoice == 3) {
        removeWkHotkeys();

        if(!identifyHotkeyFile(settings->outPath, maxDstHki, lastDstHki)) {
            maxDstHki = settings->outPath / "player1.hki";
            lastDstHki = maxDstHki;
        }
        copyHotkeyFile(maxSrcHki,lastSrcHki,maxDstHki);
        if(!fs::equivalent(maxDstHki,lastDstHki))
            copyHotkeyFile(maxSrcHki,lastSrcHki,lastDstHki);
	}
}

void WKConverter::symlinkSetup(fs::path oldDir, fs::path newDir, fs::path xmlIn, fs::path xmlOut, bool dataMod) {

    /* Sets up symlinks between the different mod versions (offline/AK/FE), so as much as possible is shared
     * and as little space is needed as possible
     * Parameters:
     * oldDir: The directory the symlink references.
     * newDir: The directory the symlink should be created in
     * xmlIn: The connected xml of the source folder
     * xmlOut: The connected xml of the destination folder
     * dataMod: If true, the symlink is for wk-based datamod
     */
    std::string newDirString = newDir.string()+"\\";
    std::string oldDirString = oldDir.string()+"\\";
    bool vooblySrc = tolower(oldDirString).find("\\voobly mods\\aoc") != std::string::npos;
    bool vooblyDst = tolower(newDirString).find("\\voobly mods\\aoc") != std::string::npos;

	fs::create_directory(newDir);
	fs::copy_file(xmlIn, xmlOut, fs::copy_option::overwrite_if_exists);
    bool datalink = vooblySrc == vooblyDst && !dataMod;

    if(datalink) {
        fs::remove_all(newDir/"Data");
    } else {
        fs::create_directory(newDir/"Data");
        fs::remove(newDir/"Data\\gamedata_x1.drs");
        fs::remove(newDir/"Data\\gamedata_x1_p1.drs");
    }

	fs::remove_all(newDir/"Taunt");
	fs::remove_all(newDir/"Sound");
    fs::remove_all(newDir/"History");
	fs::remove_all(newDir/"Script.Rm");
	fs::remove_all(newDir/"Script.Ai");
	fs::remove_all(newDir/"Screenshots");
	fs::remove_all(newDir/"Scenario");
    fs::remove(newDir/"player.nfz");
    for (fs::directory_iterator current(newDir), end;current != end; ++current) {
        std::string extension = current->path().extension().string();
        if (extension == ".hki") {
           fs::remove(current->path());
        }
    }
    std::string hotkeyString = "";
    for (fs::directory_iterator current(oldDirString), end;current != end; ++current) {
        fs::path currentPath = current->path();
        std::string extension = currentPath.extension().string();
        if (extension == ".hki") {
           hotkeyString += "mklink /H \""+newDirString+currentPath.filename().string()+"\" \""+ currentPath.string()+"\" & ";
        }
    }
	std::string datastring = datalink?"mklink /J \""+newDirString+"Data\" \""+ oldDirString+"Data\" & ":
                                      "mklink /H \""+newDirString+"Data\\gamedata_x1_p1.drs\" \""+ oldDirString+"Data\\gamedata_x1_p1.drs\" & "
                                      "mklink /H \""+newDirString+"Data\\gamedata_x1.drs\" \""+ oldDirString+"Data\\gamedata_x1.drs\" & ";
    std::string languageString = "";

    if(!dataMod) {
        if(vooblyDst) {
            fs::remove(newDir/"language.ini");
            languageString = "mklink /H \""+newDirString+"language.ini\" \""+ oldDirString+"language.ini\" & ";
        } else if (!vooblySrc) {
            fs::remove(newDir/"Data\\language_x1_p1.dll");
            languageString = "mklink /H \""+newDirString+"Data\\language_x1_p1.dll\" \""+ oldDirString+"Data\\language_x1_p1.dll\" & ";
        }
    }

    std::string cmd = "/C mklink /J \""+newDirString+"Taunt\" \""+ oldDirString+"Taunt\" & "
			+ datastring +
            hotkeyString +
            "mklink /J \""+newDirString+"Script.Rm\" \""+ oldDirString+"Script.Rm\" & "
            "mklink /J \""+newDirString+"Script.Ai\" \""+ oldDirString+"Script.Ai\" & "
            "mklink /J \""+newDirString+"Sound\" \""+ oldDirString+"Sound\" & "
            "mklink /J \""+newDirString+"History\" \""+ oldDirString+"History\" & "
            "mklink /J \""+newDirString+"Screenshots\" \""+ oldDirString+"Screenshots\" & "
            "mklink /J \""+newDirString+"Scenario\" \""+ oldDirString+"Scenario\" & "
            + languageString +
            "mklink /H \""+newDirString+"player.nfz\" \""+ oldDirString+"player.nfz\"";
    std::wstring wcmd = strtowstr(cmd);
    ShellExecute(nullptr,L"open",L"cmd.exe",wcmd.c_str(),nullptr,SW_HIDE);
    if(!fs::exists(newDir/"Taunt")) { //Symlink didn't work, we'll do a regular copy instead
        recCopy(oldDirString,newDir,true);
    }
    fs::create_directories(newDir/"Savegame\\Multi");
    if(vooblyDst && !dataMod)
        fs::copy_file(oldDirString+"version.ini", newDir/"version.ini", fs::copy_option::overwrite_if_exists);
}

void WKConverter::retryInstall() {

    emit log("Retry installation with removing folders first");
    fs::path tempFolder = "retryTemp";
    try {
        fs::create_directories(tempFolder/"Scenario");
        fs::create_directories(tempFolder/"SaveGame");
        fs::create_directories(tempFolder/"Script.RM");
        recCopy(installDir/"SaveGame",tempFolder/"SaveGame",false,true);
        recCopy(installDir/"Script.RM",tempFolder/"Script.RM",false,true);
        recCopy(installDir/"Scenario",tempFolder/"Scenario",false,true);
        fs::copy_file(installDir/"player.nfz",tempFolder/"player.nfz",fs::copy_option::overwrite_if_exists);
        if(fs::exists(installDir/"player1.hki"))
            fs::copy_file(installDir/"player1.hki",tempFolder/"player1.hki",fs::copy_option::overwrite_if_exists);
    } catch (std::exception const & e) {
        emit createDialog(QString("dialogException$")+QString(e.what()),"errorTitle");
        emit log(e.what());
        return;
    }

    fs::remove_all(installDir);
    fs::remove_all(installDir.parent_path()/"WololoKingdoms AK");
    fs::remove_all(installDir.parent_path()/"WololoKingdoms FE");

    run(true);
    recCopy(tempFolder/"SaveGame",installDir/"SaveGame",true);
    recCopy(tempFolder/"Script.RM",installDir/"Script.RM",true);
    recCopy(tempFolder/"Scenario",installDir/"Scenario",true);
    boost::system::error_code ec;
    fs::copy_file(tempFolder/"player.nfz", installDir/"player.nfz", ec);
    if(fs::exists(tempFolder/"player1.hki"))
        fs::copy_file(tempFolder/"player1.hki", installDir/"player1.hki", ec);
    fs::remove_all(tempFolder);
}


void WKConverter::setupFolders(fs::path xmlOutPathUP) {

    fs::path languageIniPath = settings->vooblyDir / "language.ini";
    std::string versionIniPath = settings->vooblyDir.string() + "\\version.ini";

    emit log("Check for symlink");
    if(fs::is_symlink(installDir/"Taunt")) {
        emit log("Removing all but SaveGame and profile");
        fs::path tempFolder = "temp";
        fs::create_directories(tempFolder/"Scenario");
        fs::create_directories(tempFolder/"SaveGame");
        fs::create_directories(tempFolder/"Script.RM");
        recCopy(installDir/"SaveGame",tempFolder/"SaveGame",false,true);
        recCopy(installDir/"Script.RM",tempFolder/"Script.RM",false,true);
        recCopy(installDir/"Scenario",tempFolder/"Scenario",false,true);
        fs::copy_file(installDir/"player.nfz",tempFolder/"player.nfz",fs::copy_option::overwrite_if_exists);
        if(fs::exists(installDir/"player1.hki"))
            fs::copy_file(installDir/"player1.hki",tempFolder/"player1.hki",fs::copy_option::overwrite_if_exists);
        fs::remove_all(installDir);
        fs::create_directories(installDir/"SaveGame");
        fs::create_directories(installDir/"SaveGame");
        fs::create_directories(installDir/"Script.RM");
        recCopy(tempFolder/"SaveGame",installDir/"SaveGame",true);
        recCopy(tempFolder/"Script.RM",installDir/"Script.RM",true);
        recCopy(tempFolder/"Scenario",installDir/"Scenario",true);
        boost::system::error_code ec;
        fs::copy_file(tempFolder/"player.nfz", installDir/"player.nfz", ec);
        if(fs::exists(tempFolder/"player1.hki"))
            fs::copy_file(tempFolder/"player1.hki", installDir/"player1.hki", ec);
        fs::remove_all(tempFolder);
    }

    emit log("Removing base folders");
    fs::remove_all(installDir/"Data");
    /*
    fs::remove_all(installDir/"Script.Ai\\Brutal2");
    fs::remove(installDir/"Script.Ai\\BruteForce3.1.ai");
    fs::remove(installDir/"Script.Ai\\BruteForce3.1.per");
    */

    emit log("Creating base folders");
    fs::create_directories(installDir/"SaveGame\\Multi");
    fs::create_directories(installDir/"Sound\\stream");
    fs::create_directory(installDir/"Data");
    fs::create_directory(installDir/"Taunt");
    fs::create_directory(installDir/"History");
    fs::create_directory(installDir/"Screenshots");
    fs::create_directory(installDir/"Scenario");

    if(!settings->useExe) {
        fs::remove(settings->vooblyDir/"age2_x1.xml");
        fs::remove(versionIniPath);
        emit log("Removing language.ini");
        fs::remove(languageIniPath);
    } else {
        emit log("Removing UP base folders");
        fs::remove(xmlOutPathUP);
        fs::remove(settings->upDir/"Data\\empires2_x1_p1.dat");
        fs::remove(settings->upDir/"Data\\gamedata_x1.drs");
        fs::remove(settings->upDir/"Data\\gamedata_x1_p1.drs");
        /*
        fs::remove_all(settings->upDir/"Script.Ai\\Brutal2");
        fs::remove(settings->upDir/"Script.Ai\\BruteForce3.1.ai");
        fs::remove(settings->upDir/"Script.Ai\\BruteForce3.1.per");
        */
        fs::create_directories(settings->upDir/"Data");
    }
}

int WKConverter::run(bool retry)
{
    emit setInfo("working");

    if (settings->dlcLevel == 0) { //This should never happen
        emit setInfo("noSteam");
        emit createDialog("You shouldn't be here! $noSteam","errorTitle");
		return -1;
	}

	int ret = 0;
	slpFiles.clear();
	wavFiles.clear();
	newTerrainFiles.clear();


    try {
        //Installer Resources
        fs::path pwInputDir = resourceDir/"pussywood";
        fs::path gridInputDir = resourceDir/"Grid";
        fs::path gridNoSnowInputDir = resourceDir/"grid no snow";
        fs::path noSnowInputDir = resourceDir/"no snow";
        fs::path monkInputDir = resourceDir/"regional monks";
        fs::path oldMonkInputDir = resourceDir/"anti-regional monks";
        fs::path scenarioInputDir = resourceDir/"Scenario";
        fs::path newTerrainInputDir = resourceDir/"new terrains";
        fs::path newGridTerrainInputDir = resourceDir/"new grid terrains";
        fs::path architectureFixDir = resourceDir/"architecture fixes";
        fs::path slpCompatDir = resourceDir/"old dat slp compatibility";
        fs::path modOverrideDir("mod_override\\");
        fs::path terrainOverrideDir("new_terrain_override\\");
        fs::path wallsInputDir = resourceDir/"short_walls";
        fs::path gamedata_x1 = resourceDir/"gamedata_x1.drs";
        fs::path aiInputPath = resourceDir/"Script.Ai";
        fs::path UPExe = resourceDir/"SetupAoc.exe";
        fs::path backupLangDll = resourceDir/"language_x1_p1.dll";
        fs::path patchFolder;

        //HD Resources
        fs::path historyInputPath = settings->language == "zht"?resourceDir/"zht\\history\\":
                                                      settings->HDPath / ("resources\\"+settings->language+"\\strings\\history\\");
        fs::path soundsInputPath = settings->HDPath / "resources\\_common\\sound\\";
        fs::path tauntInputPath = settings->HDPath / "resources\\en\\sound\\taunt\\";
        fs::path scenarioSoundsInputPath = settings->HDPath / "resources\\en\\sound\\scenario\\";
        fs::path assetsPath = settings->HDPath / "resources\\_common\\drs\\gamedata_x2\\";
        fs::path aocAssetsPath = settings->HDPath / "resources\\_common\\drs\\graphics\\";
        std::string aocDatString = settings->HDPath.string() + "\\resources\\_common\\dat\\empires2_x1_p1.dat";
        std::string hdDatString = settings->HDPath.string() + "\\resources\\_common\\dat\\empires2_x2_p1.dat";

        installDir  = settings->useExe ? settings->upDir : settings->vooblyDir;

        //Voobly Target
        fs::path languageIniPath = settings->vooblyDir / "language.ini";
        std::string versionIniPath = settings->vooblyDir.string() + "\\version.ini";
        fs::path xmlOutPath = settings->vooblyDir / "age2_x1.xml";
        fs::path xmlPath;

        //Offline Target
        fs::path xmlOutPathUP;
        std::string UPModdedExe;
        fs::path UPExeOut = settings->outPath / "SetupAoc.exe";

        //Any Target
        fs::path soundsOutputPath = installDir / "Sound\\";
        fs::path scenarioSoundsOutputPath = installDir / "Sound\\Scenario\\";
        fs::path historyOutputPath = installDir / "History\\";
        fs::path tauntOutputPath = installDir / "Taunt\\";
        std::string drsOutPath = installDir.string() + "\\Data\\gamedata_x1_p1.drs";
        fs::path outputDatPath = installDir / "Data\\empires2_x1_p1.dat";

        switch(settings->dlcLevel) {
        case 1:
            xmlOutPathUP = settings->outPath / "Games\\WKFE.xml";
            xmlPath = resourceDir/"WK1.xml";
            UPModdedExe = "WKFE";
            break;
        case 2:
            xmlOutPathUP = settings->outPath / "Games\\WKAK.xml";
            xmlPath = resourceDir/"WK2.xml";
            UPModdedExe = "WKAK";
            break;
        case 3:
            xmlOutPathUP = settings->outPath / "Games\\WK.xml";
            xmlPath = resourceDir/"WK3.xml";
            UPModdedExe = "WK";
            break;
        }

        if(secondAttempt) {
            emit log("\nSecond Attempt");
            emit log("\n");
        } else {
            emit log("New Run");
            emit log("\n");
        }      
        emit log("\nHD Path:");
        emit log(QString::fromStdString(settings->HDPath.string() + "\n" + "AoC Path:"));
        emit log(QString::fromStdString(installDir.string() + "\n"));

        emit log("Patch mode: ");
        emit log(QString().setNum(settings->patch));
        emit log("DLC level: ");
        emit log(QString().setNum(settings->dlcLevel));

        std::string line;

        emit setProgress(1); //1

        if (settings->patch < 0) {
            setupFolders(xmlOutPathUP);
        } else {
            fs::create_directories(outputDatPath.parent_path());
            patchFolder = resourceDir/("patches\\"+std::get<0>(settings->dataModList[settings->patch])+"\\");
            hdDatString = patchFolder.string()+"empires2_x1_p1.dat";
            UPModdedExe = std::get<1>(settings->dataModList[settings->patch]);
        }

        bool dllPatched = createLanguageFile(languageIniPath, patchFolder);

        if(!dllPatched) {
            if (!secondAttempt) {
                secondAttempt = true;
                ret = run();
                return ret;
            }
            else {
                fs::path langDllOutput = settings->upDir/"data\\language_x1_p1.dll";
                fs::copy_file(backupLangDll,langDllOutput,fs::copy_option::overwrite_if_exists);
            }
        }

        emit increaseProgress(1); //6

        if (settings->patch < 0) {
            emit log("index DRS files");
            indexDrsFiles(assetsPath); //Slp/wav files to be written into gamedata_x1_p1.drs
            indexDrsFiles(aocAssetsPath, false); //Aoc slp files, just needed for comparison purposes

            emit log("Visual Mod Stuff");
            if(settings->usePw || settings->useGrid || settings->useWalls || settings->useNoSnow) {
                emit setInfo("working$\n$workingMods");
			}
            if(settings->usePw)
				indexDrsFiles(pwInputDir);
            emit increaseProgress(1); //7
            if(settings->useGrid) {
				indexDrsFiles(gridInputDir);
                emit increaseProgress(1); //8
                indexDrsFiles(newGridTerrainInputDir, true, true);
                emit increaseProgress(2); //10
                if(settings->useNoSnow)
                    indexDrsFiles(gridNoSnowInputDir);
			} else {
                indexDrsFiles(newTerrainInputDir, true, true);
                emit increaseProgress(3); //10
                if(settings->useNoSnow)
                    indexDrsFiles(noSnowInputDir);
			}
			if(!fs::is_empty(terrainOverrideDir)) {
                indexDrsFiles(terrainOverrideDir, true, true);
			}
            emit increaseProgress(1); //11

            emit setInfo("working$\n$workingFiles");

            try {
                emit log("History Files");

                copyHistoryFiles(historyInputPath, historyOutputPath);
            } catch (std::exception const & e) {
                QString message = QString("historyError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                } else {
                    retryInstall();
                }
            }
            try {
                emit log("Civ Intro Sounds");

                copyCivIntroSounds(soundsInputPath / "civ\\", soundsOutputPath / "stream\\");
            } catch (std::exception const & e) {
                QString message = QString("civIntroError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                } else {
                    retryInstall();
                }
            }
            emit increaseProgress(1); //12
            emit log("Create Music Playlist");
            try {
                createMusicPlaylist(soundsInputPath.string() + "music\\", soundsOutputPath.string() + "music.m3u");


            } catch (std::exception const & e) {
               QString message =QString("musicPlaylistError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                } else {
                    retryInstall();
                }
            }
            emit increaseProgress(1); //13
            emit log("Copy Taunts");
            try {

                recCopy(tauntInputPath, tauntOutputPath, true);
            } catch (std::exception const & e) {
               QString message =QString("tauntError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                } else {
                    retryInstall();
                }
            }
            emit log("Copy Scenario Sounds");
            try {

                recCopy(scenarioSoundsInputPath, scenarioSoundsOutputPath, true);
            } catch (std::exception const & e) {
               QString message =QString("scenarioSoundError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                } else {
                    retryInstall();
                }
            }
            emit increaseProgress(1); //14
            emit log("Copy XML");
            if(settings->useExe) {
                fs::copy_file(xmlPath, xmlOutPathUP, fs::copy_option::overwrite_if_exists);
            } else {
                fs::copy_file(xmlPath, xmlOutPath, fs::copy_option::overwrite_if_exists);
            }

            fs::path installMapDir = installDir/"Script.Rm";
            emit log("Copy Voobly Map folder");
            if (fs::exists(settings->outPath/"Random")) {
                try {
                    recCopy(settings->outPath/"Random", installMapDir, true);
                } catch (std::exception const & e) {
                   QString message =QString("vooblyMapError$")+e.what();
                    emit log(message);
                    if(retry) {
                        emit createDialog(message,"errorTitle");
                    } else {
                        retryInstall();
                    }
                }
			} else {
                fs::create_directory(installMapDir);
			}
            emit increaseProgress(1); //15
            if(settings->copyCustomMaps) {
                emit log("Copy HD Maps");
                try {
                    copyHDMaps(settings->HDPath/"resources\\_common\\random-map-scripts\\", installMapDir);
                } catch (std::exception const & e) {
                    QString message = QString("hdMapError$")+e.what();
                    emit log(message);
                    if(retry) {
                        emit createDialog(message,"errorTitle");
                    } else {
                        retryInstall();
                    }
                }
            } else {
                emit increaseProgress(3); //18
            }
            emit increaseProgress(1); //19
            emit log("Copy Special Maps");
            if(settings->copyMaps) {
                try {
                    copyHDMaps("resources\\Script.Rm\\", installMapDir, true);
                } catch (std::exception const & e) {
                    QString message = QString("specialMapError$")+e.what();
                    emit log(message);
                    if(retry) {
                        emit createDialog(message,"errorTitle");
                    } else {
                        retryInstall();
                    }
                }
            } else {
                emit increaseProgress(3);
            }
            emit increaseProgress(1); //23
            try {
                recCopy(scenarioInputDir,installDir/"Scenario",false,true);

            } catch (std::exception const & e) {
                QString message = QString("scenarioError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                } else {
                    retryInstall();
                }
            }
            emit log("Copying AI");
            try {
                recCopy(aiInputPath, installDir/"Script.Ai", false, true);

            } catch (std::exception const & e) {
                QString message = QString("aiError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                } else {
                    retryInstall();
                }
            }
            emit increaseProgress(1); //24
            emit log("Hotkey Setup");
            try {
                if(settings->hotkeyChoice != 0 || fs::exists("player1.hki"))
                    hotkeySetup();
            } catch (std::exception const & e) {
                QString message = QString("hotkeyError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                } else {
                    retryInstall();
                }
            }

            emit increaseProgress(1); //25

            emit log("Opening dats");
            emit setInfo("working$\n$workingAoc");



            genie::DatFile aocDat;
            genie::DatFile hdDat;
            try {
                aocDat.setGameVersion(genie::GameVersion::GV_TC);
                aocDat.load(aocDatString.c_str());
                emit increaseProgress(3); //28

                emit setInfo("working$\n$workingHD");

                hdDat.setGameVersion(genie::GameVersion::GV_Cysion);
                hdDat.load(hdDatString.c_str());
                emit increaseProgress(3); //31

                emit setInfo("working$\n$workingInterface");


                emit log("HUD Hack");
                uglyHudHack(assetsPath);
                emit increaseProgress(1); //32

                emit setInfo("working$\n$workingDat");


                emit log("Transfer HD Dat elements");
                transferHdDatElements(&hdDat, &aocDat);
                emit increaseProgress(1); //33

                emit log("Patch Architectures");
                /*
                 * As usual, we have to fix some mediterranean stuff first where builidings that shouldn't
                 * share the same garrison flag graphics.
                 */

                short buildingIDs[] = { 47, 51, 116, 137, 234, 235, 236};
                for(short i = 0; i < sizeof(buildingIDs)/sizeof(short); i++) {
                    short oldGraphicID = aocDat.Civs[19].Units[buildingIDs[i]].Creatable.GarrisonGraphic;
                    genie::Graphic newFlag = aocDat.Graphics[oldGraphicID];
                    newFlag.ID = aocDat.Graphics.size();
                    aocDat.Graphics.push_back(newFlag);
                    aocDat.GraphicPointers.push_back(1);
                    aocDat.Civs[19].Units[buildingIDs[i]].Creatable.GarrisonGraphic = newFlag.ID;
                    aocDat.Civs[24].Units[buildingIDs[i]].Creatable.GarrisonGraphic = newFlag.ID;
                }

                adjustArchitectureFlags(&aocDat,"resources\\Flags.txt");

                patchArchitectures(&aocDat);

                if(settings->fixFlags)
                    adjustArchitectureFlags(&aocDat,"resources\\WKFlags.txt");

                if(settings->useWalls) //This needs to be AFTER patchArchitectures
                    copyWallFiles(wallsInputDir);
            } catch (std::exception const & e) {
                QString message = QString("datError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                    emit setInfo("error");
                    return -2;
                } else {
                    retryInstall();
                }
            }

            emit increaseProgress(1); //64
            try {
                if(settings->useMonks)
                    indexDrsFiles(monkInputDir);
                else
                    indexDrsFiles(oldMonkInputDir);
                emit increaseProgress(1); //65

                indexDrsFiles(architectureFixDir);
                emit log("Mod Override Dir");
                if(!fs::is_empty(modOverrideDir))
                    indexDrsFiles(modOverrideDir);
                emit increaseProgress(1); //66
                emit log("Opening DRS");
                std::ofstream drsOut(drsOutPath, std::ios::binary);
                emit log("Make DRS");
                makeDrs(&drsOut);
                emit increaseProgress(1); //75


                emit log("copy gamedata_x1.drs");
                fs::copy_file(gamedata_x1, installDir/"Data\\gamedata_x1.drs", fs::copy_option::overwrite_if_exists);
                emit increaseProgress(1); //76
            } catch (std::exception const & e) {
                QString message = QString("indexingError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                    emit setInfo("error");
                    return -2;
                } else {
                    retryInstall();
                }
            }

            /*
             * Read what the current patch number and what the expected hashes (with/without flag adjustment) are
             */


            std::ifstream versionFile((resourceDir/"version.txt").string());
            std::string patchNumber;
            std::getline(versionFile, patchNumber);
            std::string dataVersion;
            std::getline(versionFile, dataVersion);

            std::string hash1;
            std::string hash2;
            std::getline(versionFile, hash1);
            std::getline(versionFile, hash2);
            versionFile.close();


            wololo::DatPatch patchTab[] = {

                wololo::berbersUTFix,
                wololo::demoShipFix,
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
                wololo::trickleBuildingFix,
                wololo::smallFixes,
                wololo::cuttingFix,
                wololo::ai900UnitIdFix
            };

            emit setInfo("working$\n$workingPatches");

            emit log("DAT Patches");
            try{
                for (size_t i = 0, nbPatches = sizeof patchTab / sizeof (wololo::DatPatch); i < nbPatches; i++) {
                    patchTab[i].patch(&aocDat);
                    emit setInfo(QString::fromStdString("working$\n$"+patchTab[i].name));
                    emit increaseProgress(1); //77-93
                }

                for (size_t civIndex = 0; civIndex < aocDat.Civs.size(); civIndex++) {
                    aocDat.Civs[civIndex].Resources[198] = std::stoi(dataVersion); //Mod version: WK=1, last 3 digits are patch number
                }

                emit log("Save DAT");
                aocDat.saveAs(outputDatPath.string().c_str());
            } catch (std::exception const & e) {
                QString message = QString("datSaveError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                    emit setInfo("error");
                    return -2;
                } else {
                    retryInstall();
                }
            }
            if(!settings->useExe) {
                    try {
                    /*
                     * Generate version.ini based on the installer and the hash of the dat.
                     */
                    emit log("Create Hash");
                    QFile file(outputDatPath.string().c_str());

                    if (file.open(QIODevice::ReadOnly))
                    {
                        QByteArray fileData = file.readAll();

                        QByteArray hashData = QCryptographicHash::hash(fileData,QCryptographicHash::Md5);
                        std::ofstream versionOut(versionIniPath);
                        std::string hash = hashData.toBase64().toStdString().substr(0,6);
                        if (hash != hash1 && hash != hash2) {
                            emit createDialog("dialogBeta");

                            versionOut << (patchNumber + ".") << hash;
                        } else {
                            versionOut << patchNumber;
                        }
                        versionOut.close();
                    }
                } catch (std::exception const & e) {
                    QString message = QString("versionFileError$")+e.what();
                    emit log(message);
                    if(retry) {
                        emit createDialog(message,"errorTitle");
                        emit setInfo("error");
                        return -2;
                    } else {
                        retryInstall();
                    }
                }
            }
            if (settings->useBoth) {
                emit log("Offline installation symlink");
                try {
                    symlinkSetup(settings->vooblyDir, settings->upDir, xmlPath, xmlOutPathUP);
                } catch (std::exception const & e) {
                    QString message = QString("symlinkError$")+e.what();
                    emit log(message);
                    if(retry) {
                        emit createDialog(message,"errorTitle");
                        emit setInfo("error");
                        return -2;
                    } else {
                        retryInstall();
                    }
                }
            }

        } else { //If we use a balance mod or old patch, just copy the supplied dat fil
            try {
                emit log("Copy DAT file");
                fs::remove(outputDatPath);
                genie::DatFile dat;
                dat.setGameVersion(genie::GameVersion::GV_TC);
                dat.load(hdDatString.c_str());
                if(settings->fixFlags)
                    adjustArchitectureFlags(&dat,"resources\\WKFlags.txt");
                dat.saveAs(outputDatPath.string().c_str());
                emit setProgress(20);
                std::string patchNumber = std::get<2>(settings->dataModList[settings->patch]);
                std::ofstream versionOut(versionIniPath);
                versionOut << patchNumber;
                versionOut.close();
            } catch (std::exception const & e) {
                QString message = QString("patchDatError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                    emit setInfo("error");
                    return -2;
                } else {
                    retryInstall();
                }
            }
        }



        emit increaseProgress(1); //94

		/*
		 * If a user has access to more than just FE, also generate those versions
		 * For an old patch, we'll just use the highest one
		 * For a data mod, not sure if we should generate all versions or the highest one
		 */

        if (settings->patch >= 0) {
            try {
                emit log("Patch setup");
                fs::path xmlIn = resourceDir/"WKtemp.xml";
                std::ifstream input(resourceDir.string()+("WK"+std::to_string(settings->dlcLevel)+".xml"));
                std::string str(static_cast<std::stringstream const&>(std::stringstream() << input.rdbuf()).str());
                std::string dlcExtension = settings->dlcLevel == 3?"":settings->dlcLevel==2?" AK":" FE";
                boost::replace_all(str,baseModName+dlcExtension,settings->modName);
                std::ofstream out(xmlIn.string());
                out << str;
                input.close();
                out.close();
                if(settings->useBoth || settings->useVoobly)
                    symlinkSetup(settings->vooblyDir.parent_path() / (baseModName+dlcExtension), settings->vooblyDir,xmlIn,settings->vooblyDir/"age2_x1.xml",true);
                    if(std::get<3>(settings->dataModList[settings->patch]) & 4) {
                        indexDrsFiles(slpCompatDir);
                        std::ifstream oldDrs (settings->vooblyDir.parent_path().string() + "\\" + baseModName+dlcExtension+"\\data\\gamedata_x1_p1.drs", std::ios::binary);
                        std::ofstream newDrs (settings->vooblyDir.string()+"\\data\\gamedata_x1_p1.drs", std::ios::binary);
                        editDrs(&oldDrs, &newDrs);
                    }
                if(settings->useBoth || settings->useExe) {
                    symlinkSetup(settings->upDir.parent_path() / (baseModName+dlcExtension), settings->upDir, xmlIn, settings->upDir.parent_path()/(UPModdedExe+".xml"), true);
                    if(std::get<3>(settings->dataModList[settings->patch]) & 4) {
                        indexDrsFiles(slpCompatDir);
                        std::ifstream oldDrs (settings->upDir.parent_path().string() + "\\" + baseModName+dlcExtension+"\\data\\gamedata_x1_p1.drs", std::ios::binary);
                        std::ofstream newDrs (settings->upDir.string()+"\\data\\gamedata_x1_p1.drs", std::ios::binary);
                        editDrs(&oldDrs, &newDrs);
                    }
                }
                fs::remove(xmlIn);
            } catch (std::exception const & e) {
                QString message = QString("patchError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                    emit setInfo("error");
                    return -2;
                } else {
                    retryInstall();
                }
            }
        } else if(settings->restrictedCivMods) {
            try {
                if (settings->dlcLevel > 1) {
                    emit log("FE Setup");
                    fs::path xmlIn = resourceDir/"WK1.xml";
                    fs::path vooblyDir2 = settings->vooblyDir.parent_path() / (baseModName+" FE");
                    if(settings->useBoth || settings->useVoobly)
                        symlinkSetup(settings->vooblyDir, vooblyDir2, xmlIn, vooblyDir2/"age2_x1.xml");
                }
                if (settings->dlcLevel > 2) {
                    emit log("AK Setup");
                    fs::path xmlIn = resourceDir/"WK2.xml";
                    fs::path vooblyDir2 = settings->vooblyDir.parent_path() / (baseModName+" AK");
                    if(settings->useBoth || settings->useVoobly)
                        symlinkSetup(settings->vooblyDir, vooblyDir2, xmlIn, vooblyDir2/"age2_x1.xml");
                }
            } catch (std::exception const & e) {
                QString message = QString("restrictedCivError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                    emit setInfo("error");
                } else {
                    retryInstall();
                }
            }
        }

        /*
         * Copy the data folder from the Voobly folder and
         * create the offline exe
         */
        if(settings->useBoth) {
            try {
                fs::copy_file(settings->vooblyDir / "Data\\empires2_x1_p1.dat", settings->upDir / "Data\\empires2_x1_p1.dat", fs::copy_option::overwrite_if_exists);
            } catch (std::exception const & e) {
                QString message = e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                    emit setInfo("error");
                    return -2;
                } else {
                    retryInstall();
                }
            }
        }
        if(settings->useVoobly) {
            emit createDialog("dialogDone");

        } else {
            emit log("Create Offline Exe");
            emit setInfo("working$\n$workingUP");
            emit increaseProgress(1); //95
            if (!dllPatched)
                emit createDialog("dialogNoDll");

            try {
                fs::copy_file(UPExe, UPExeOut, fs::copy_option::overwrite_if_exists);

                emit increaseProgress(1); //96

                callExternalExe(strtowstr("\""+UPExeOut.string()+"\" -g:"+UPModdedExe).c_str());

                std::string newExeName;
                if(settings->patch >= 0 && (newExeName = std::get<4>(settings->dataModList[settings->patch])) != "") {
                    if(fs::exists(settings->outPath / ("age2_x1\\"+ newExeName+".exe"))) {
                        fs::rename(settings->outPath / ("age2_x1\\"+ newExeName+".exe"),
                                   settings->outPath / ("age2_x1\\"+ newExeName+".exe.bak"));
                    }
                    fs::rename(settings->outPath / ("age2_x1\\"+ UPModdedExe+".exe"),
                               settings->outPath / ("age2_x1\\"+ newExeName+".exe"));
                    UPModdedExe = newExeName;
                }
            } catch (std::exception const & e) {
                QString message = QString("exeError$")+e.what();
                emit log(message);
                if(retry) {
                    emit createDialog(message,"errorTitle");
                    emit setInfo("error");
                    return -2;
                } else {
                    retryInstall();
                }
            }

            emit increaseProgress(1); //97
            QString info;
            if(settings->useBoth)
                info = "dialogBoth";
            else
                info = "dialogExe";
            emit createDialog(info,"<exe>",QString::fromStdString(UPModdedExe));

        }
        emit setInfo("workingDone");

        if (settings->patch < 0 && fs::equivalent(settings->outPath,settings->HDPath)) {

            emit log("Fix Compat Patch");
			/*
			 * Several small fixes for the compatibility patch. This only needs to be run once
			 * An update to the compatibility patch would make this unnecessary most likely.
			 */

            fs::remove_all(settings->outPath/"compatslp");

            fs::create_directory(settings->outPath/"data\\Load");
            if(settings->useExe) { //this causes a crash with UP 1.5 otherwise
                emit setInfo("workingDone");

                if(fs::file_size(settings->outPath/"data\\blendomatic.dat") < 400000) {
                    fs::rename(settings->outPath/"data\\blendomatic.dat",settings->outPath/"data\\blendomatic.dat.bak");
                    fs::rename(settings->outPath/"data\\blendomatic_x1.dat",settings->outPath/"data\\blendomatic.dat");
                }
                emit increaseProgress(1); //98
            }
		}


        emit setProgress(100);
	}
	catch (std::exception const & e) {
        emit createDialog(QString("dialogException$$")+e.what(),"errorTitle");

        emit log(e.what());
        emit setInfo("error");
        ret = 1;
	}
	catch (std::string const & e) {		
        emit createDialog(QString::fromStdString("dialogException$"+e),"errorTitle");

        emit log(QString::fromStdString(e));
        emit setInfo("error");
		ret = 1;
	}


    if(settings->patch < 0 && std::get<0>(settings->dataModList[0]) == "Patch 5.8 Beta") {
        emit createDialog("The converter will install the Patch 5.8 Beta as a separate mod now");
        //Automatic Installation of Patch 5.8 Beta. Not super pretty as this duplicated code from mainwindow, but time restraints
        settings->patch = 0;
        settings->modName = "WK ";
        std::string dlcExtension = settings->dlcLevel == 3?"":settings->dlcLevel==2?" AK":" FE";
        settings->modName += std::get<0>(settings->dataModList[settings->patch]);
        if(std::get<3>(settings->dataModList[settings->patch]) & 1) {
            settings->modName += dlcExtension;
        }
        settings->vooblyDir = settings->vooblyDir.parent_path() / settings->modName;
        settings->upDir = settings->upDir.parent_path() / settings->modName;
        settings->nfzUpOutPath = settings->upDir / "Player.nfz";
        settings->nfzVooblyOutPath = settings->vooblyDir / "Player.nfz";
        run();
    }

    emit finished();
	return ret;
}

bool WKConverter::copyData(QIODevice &inFile, QIODevice &outFile)
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


