#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/filesystem.hpp>

std::string const dls[] = {"C", "D", "E", "F", "G", "H"}; // TEMP

std::string getHDPath() {
	std::string HDPath = "../";

	if(!boost::filesystem::exists("../Launcher.exe")) {
		HDPath = ":/Program Files (x86)/Steam/steamapps/common/Age2HD/";
		for (size_t i = 0; i < sizeof dls / sizeof (std::string); i++) {
			if(boost::filesystem::exists(dls[i] + HDPath)) {
				HDPath = dls[i] + HDPath;
				break;
			}
		}
		if(HDPath == ":/Program Files (x86)/Steam/steamapps/common/Age2HD/") {
			if(boost::filesystem::exists("../../AoKDump")) {
				HDPath = "../../";
			} else {
				throw std::runtime_error("Could not find a HD installation, please put the WololoKingdoms folder into your HD installation folder (Age2HD).");
			}
		}
	}
	return HDPath;
}

std::string getOutPath(std::string HDPath) {
	std::string outPath = ":/Program Files (x86)/Microsoft Games/Age of Empires II/";
	bool aocFound = false;

	for (size_t i = 0; i < sizeof dls / sizeof (std::string); i++) {
		if(boost::filesystem::exists(dls[i] + outPath)) {
			outPath = dls[i] + outPath;
			aocFound = true;
			break;
		} else {
			std::cout << dls[i] + outPath + "not found" << std::endl;
		}
	}
	if(!aocFound) {
		if(boost::filesystem::exists(HDPath + "age2_x1")) {
			outPath = HDPath;
			aocFound = true;
		} else {
			std::cout << HDPath + "age2_x1" + "not found" << std::endl;
			outPath = HDPath+"WololoKingdoms/out/";
		}
	}
	return outPath;
}
