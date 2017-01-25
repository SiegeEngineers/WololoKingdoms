#include <iostream>
#include <set>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "genie/dat/DatFile.h"
#include "genie/lang/LangFile.h"
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

std::string const version = "1.1";

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

void convertLanguageFile(std::ifstream *in, std::ofstream *iniOut, genie::LangFile *dllOut, bool generateLangDll, std::map<int, std::string> *langReplacement) {
    std::string line;
    while (getline(*in, line)) {
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

        boost::replace_all(line, "·", "\xb7"); // Workaround for UCS-2 to UTF-8 conversion
        *iniOut << number << '=' << line <<  std::endl;

        if (generateLangDll) {
            boost::replace_all(line, "\\n", "\n"); // the dll file requires actual line feed, not escape sequences
            try {
                dllOut->setString(nb, line);
            }
            catch (std::string const & e) {
                boost::replace_all(line, "\xb7", "-"); // non-english dll files don't seem to like that character
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
     * copies the Slav hud files for AK civs, the good way of doing this would be to extract
     * the actual AK civs hud files from
     * Age2HD\resources\_common\slp\game_b[24-27].slp correctly, but I haven't found a way yet
     */
    int const slavHudFiles[] = {51123, 51153, 51183};
    for (size_t baseIndex = 0; baseIndex < sizeof slavHudFiles / sizeof (int); baseIndex++) {
        for (size_t i = 1; i <= 8; i++) {
            std::string dst = inputDir + "/" + std::to_string(slavHudFiles[baseIndex]+i) + ".slp";
            if (! (boost::filesystem::exists(dst) && boost::filesystem::file_size(dst) > 0)) {
                std::string src = inputDir + "/" + std::to_string(slavHudFiles[baseIndex]) + ".slp";
                recCopy(src, dst);
            }
        }
    }
}

void cleanTheUglyHudHack(std::string const inputDir) {
    int const slavHudFiles[] = {51123, 51153, 51183};
    for (size_t baseIndex = 0; baseIndex < sizeof slavHudFiles / sizeof (int); baseIndex++) {
        for (size_t i = 1; i <= 8; i++) {
            boost::filesystem::remove(inputDir + "/" + std::to_string(slavHudFiles[baseIndex]+i) + ".slp");
        }
    }
}

void copyCivIntroSounds(std::string const inputDir, std::string const outputDir) {
    std::string const civs[] = {"italians", "indians", "incas", "magyars", "slavs",
                           "portuguese", "ethiopians", "malians", "berbers", "burmese", "malay", "vietnamese", "khmer"};
    for (size_t i = 0; i < sizeof civs / sizeof (std::string); i++) {
        recCopy(inputDir + civs[i] + ".mp3", outputDir + civs[i] + ".mp3");
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

int main(int argc, char *argv[])
{

    std::string HDPath = "../";
    if(!boost::filesystem::exists("../AoKDump")) {
        std::string const dls[] = {"C", "D", "E", "F", "G", "H"};
        HDPath = ":/Program Files (x86)/Steam/steamapps/common/Age2HD";
        for (size_t i = 0; i < sizeof dls / sizeof (std::string); i++) {
            if(boost::filesystem::exists(dls[i] + HDPath)) {
                HDPath = dls[i] + HDPath;
                break;
            }
        }
    }
    std::string outPath = HDPath+"/WololoKingdoms/out";

    std::string const aocDatPath = HDPath + "/resources/_common/dat/empires2_x1_p1.dat";
    std::string const hdDatPath = HDPath + "/resources/_common/dat/empires2_x2_p1.dat";
    std::string const keyValuesStringsPath = HDPath + "/resources/en/strings/key-value/key-value-strings-utf8.txt"; // TODO pick other languages
    std::string const languageIniPath = outPath + "/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/language.ini";
    std::string const versionIniPath = outPath +  "/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/version.ini";
    std::string const civIntroSoundsInputPath = HDPath + "/resources/_common/sound/civ/";
    std::string const civIntroSoundsOutputPath = outPath + "/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Sound/stream/";
    std::string const musicInputPath = "music.m3u";
    //TODO create music.m3u based on HDPath
    std::string const musicOutputPath = outPath + "/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Sound/music.m3u";
    std::string const xmlPath = "age2_x1.xml";
    std::string const xmlPathUP = "WK_African_Kingdoms.xml";
    std::string const xmlOutPath = outPath +  "/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/age2_x1.xml";
    std::string const xmlOutPathUP = outPath +  "/Games/WK_African_Kingdoms.xml";
    std::string const drsOutPath = outPath + "/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Data/gamedata_x1_p1.drs";
    std::string const assetsPath = HDPath + "/resources/_common/drs/gamedata_x2/";
    std::string const outputDatPath = outPath + "/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Data/empires2_x1_p1.dat";
    std::string const langDllPath = "/language_x1_p1.dll";
    std::string const vooblyDir = outPath + "/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/";
    std::string const uPDIR = outPath + "/Games/WK_African_Kingdoms/";


    int ret = 0;

    try {
        std::cout << "WololoKingdoms ver. " << version << std::endl;
        boost::filesystem::remove_all(outPath+"/");
        boost::filesystem::create_directories(outPath+"/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Data");
        boost::filesystem::create_directories(outPath+"/Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms/Sound/stream");
        boost::filesystem::create_directories(outPath+"/Games/WK_African_Kingdoms/Data");
        boost::filesystem::create_directories(outPath+"/Games/WK_African_Kingdoms/Sound/stream");

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


        std::cout << "Preparing resource files..." << std::endl;
        std::ofstream versionOut(versionIniPath);
        versionOut << version << std::endl;
        copyCivIntroSounds(civIntroSoundsInputPath, civIntroSoundsOutputPath);
        recCopy(musicInputPath,musicOutputPath);
        recCopy(xmlPath, xmlOutPath);
        recCopy(xmlPathUP, xmlOutPathUP);
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
            wololo::etiopiansFreePikeUpgradeFix,
            wololo::hotkeysFix,
            wololo::maliansFreeMiningUpgradeFix,
            wololo::portugueseFix,
            wololo::disableNonWorkingUnits,
            wololo::ai900UnitIdFix
        };


        std::map<int, std::string> langReplacement;

        for (size_t i = 0, nbPatches = sizeof patchTab / sizeof (wololo::DatPatch); i < nbPatches; i++) {
            std::cout << "Applying DAT patch " << i+1 << " of " << nbPatches << ": " << patchTab[i].name << std::endl;
            patchTab[i].patch(&aocDat, &langReplacement);
        }

        std::cout << std::endl << "Converting the language file..." << std::endl;
        std::ifstream langIn(keyValuesStringsPath);
        std::ofstream langOut(languageIniPath);
        genie::LangFile langDll;
        bool patchLangDll = boost::filesystem::exists(langDllPath);
        if (patchLangDll) {
            langDll.load(langDllPath.c_str());
            langDll.setGameVersion(genie::GameVersion::GV_TC);
        }
        else {
            std::cout << langDllPath << " not found, skipping dll patching for UserPatch." << std::endl;
        }
        convertLanguageFile(&langIn, &langOut, &langDll, patchLangDll, &langReplacement);
        if (patchLangDll) {
            langDll.save();
            std::cout << langDllPath << " patched." << std::endl;
        }


        std::cout << std::endl;

        aocDat.saveAs(outputDatPath.c_str());

        std::cout << std::endl << "Copying the files for UserPatch..." << std::endl;

        recCopy(vooblyDir + "/Data", uPDIR + "/Data");
        recCopy(vooblyDir + "/Sound", uPDIR + "/Sound");

        std::cout << "Conversion complete, open the \"out/\" folder and put it's content into your AOE2 folder." << std::endl;
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
