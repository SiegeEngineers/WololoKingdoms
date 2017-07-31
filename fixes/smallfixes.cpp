#include "smallfixes.h"
#include "wololo/datPatch.h"

namespace wololo {

void smallPatches(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	size_t const eliteCamelArcherID = 1009;
	size_t const cannonGalleonID = 420;
	size_t const eliteCannonGalleonID = 691;
	size_t const teutonTeamBonusID = 404;
	size_t const relicID = 285;
	size_t const mountains[] = {310,311,744,745,1041,1042,1043,1044,1045,1046,1047};

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		aocDat->Civs[civIndex].Units[eliteCamelArcherID] = aocDat->Civs[0].Units[eliteCamelArcherID];
		aocDat->Civs[civIndex].Units[cannonGalleonID].Creatable.HeroMode -= 128;
		aocDat->Civs[civIndex].Units[eliteCannonGalleonID].Creatable.HeroMode -= 128;
	}
	aocDat->Techages[teutonTeamBonusID].Effects[1].B = 1;
	aocDat->Techages[teutonTeamBonusID].Effects[2].B = 1;
	aocDat->Civs[0].Units[relicID].CollisionSize = {0.5,0.5,2};
	for (size_t i = 0; i < sizeof(mountains)/sizeof(mountains[0]); i++) {
		aocDat->Civs[0].Units[mountains[i]].ClearanceSize = {3,3};
	}
	aocDat->Graphics[3387].SoundID = 428;
	aocDat->Graphics[3387].SoundID = 428;
}

DatPatch smallFixes = {
	&smallPatches,
	"Several small civ bug fixes"
};

}

