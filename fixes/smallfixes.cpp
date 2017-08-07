#include "smallfixes.h"
#include "wololo/datPatch.h"

namespace wololo {

void smallPatches(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	size_t const eliteCamelArcherID = 1009;
	size_t const cannonGalleonID = 420;
	size_t const eliteCannonGalleonID = 691;
	size_t const teutonTeamBonusID = 404;
	size_t const PTWC = 444;
	size_t const pTrebId = 331;
	size_t const relicID = 285;
	size_t const feitoriaID = 1021;
	size_t const tradeWorkshopID = 110;
	size_t const mountains[] = {310,311,744,745,1041,1042,1043,1044,1045,1046,1047};

	//unpackable TC
	for (size_t civIndex = 1; civIndex < aocDat->Civs.size(); civIndex++) {
		aocDat->Civs[civIndex].Units[PTWC] = aocDat->Civs[0].Units[PTWC];
	}
	aocDat->UnitHeaders[PTWC].Commands.push_back(aocDat->UnitHeaders[pTrebId].Commands[0]);
	aocDat->UnitHeaders[pTrebId].Commands[0].SelectionMode = 0;
	aocDat->UnitHeaders[pTrebId].Commands[0].SelectionEnabler = 0;
	//Trade Workshop with gold trickle
	aocDat->UnitHeaders[tradeWorkshopID].Commands.push_back(aocDat->UnitHeaders[feitoriaID].Commands[3]);
	aocDat->UnitHeaders[tradeWorkshopID].Commands[0].Quantity = 1;
	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		//fixes faulty elite camel archer data for non-gaia civs
		aocDat->Civs[civIndex].Units[eliteCamelArcherID] = aocDat->Civs[0].Units[eliteCamelArcherID];
		aocDat->Civs[civIndex].Units[cannonGalleonID].Creatable.HeroMode -= 128; //restore patrol for cannon galleons
		aocDat->Civs[civIndex].Units[eliteCannonGalleonID].Creatable.HeroMode -= 128;
	}
	//Fix teuton team bonus being overwritten in post-imp
	aocDat->Techages[teutonTeamBonusID].Effects[1].B = 1;
	aocDat->Techages[teutonTeamBonusID].Effects[2].B = 1;
	aocDat->Civs[0].Units[relicID].CollisionSize = {0.5,0.5,2}; //fixes holy line, relics block entire tile
	for (size_t i = 0; i < sizeof(mountains)/sizeof(mountains[0]); i++) {
		aocDat->Civs[0].Units[mountains[i]].ClearanceSize = {3,3}; //mountains were too small for their graphics
	}
	//fixes galleon sounds
	for (size_t fileID = 5555; fileID < 5563; fileID++) {
		genie::SoundItem* item = new genie::SoundItem();
		item->ResourceID = fileID;
		item->FileName = "wgal"+std::to_string(fileID-5554)+".wav";
		item->Probability = fileID < 5557?5:15;
		aocDat->Sounds[427].Items.push_back(*item);
	}
	aocDat->Graphics[3387].SoundID = 427;
	aocDat->Graphics[3388].SoundID = 427;
}

DatPatch smallFixes = {
	&smallPatches,
	"Several small civ bug fixes"
};

}

