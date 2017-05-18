#include "smallfixes.h"
#include "wololo/datPatch.h"

namespace wololo {

void smallPatches(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	size_t const eliteCamelArcherID = 1009;
	size_t const teutonTeamBonusID = 404;

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		aocDat->Civs[civIndex].Units[eliteCamelArcherID] = aocDat->Civs[0].Units[eliteCamelArcherID];
	}
	aocDat->Techages[teutonTeamBonusID].Effects[1].B = 1;
	aocDat->Techages[teutonTeamBonusID].Effects[2].B = 1;
}

DatPatch smallFixes = {
	&smallPatches,
	"Several small civ bug fixes"
};

}

