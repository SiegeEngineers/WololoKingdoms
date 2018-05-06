#include "smallfixes.h"
#include "wololo/datPatch.h"

namespace wololo {

void smallPatches(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	size_t const eliteCamelArcherID = 1009;
    size_t const monkeyBoyID = 860;
	size_t const cannonGalleonID = 420;
	size_t const eliteCannonGalleonID = 691;
    size_t const longboatID = 250;
    size_t const eliteLongboatID = 533;
	size_t const teutonTeamBonusID = 404;
	size_t const PTWC = 444;
	size_t const pTrebId = 331;
	size_t const relicID = 285;
	size_t const feitoriaID = 1021;
    size_t const tradeWorkshopID = 110;
    size_t const sharkatzorID = 1222;
    size_t const kingID = 434;
    size_t const cartographyID = 19;
    size_t const huntingDogsId = 526;
    size_t const feudalAgeResearchId = 101;
    size_t const fireShipResearchId = 243;
    size_t const demoShipResearchId = 242;
    size_t const demoShipTechId = 231;
    size_t const fireShipTechId = 232;
    size_t const demoShipId = 527;
    size_t const demoRaftId = 1104;
    size_t const fireShipId = 529;
    size_t const fireGalleyId = 1103;
    size_t const FireDemoShipEnablerId = 78;
    size_t const FireDemoShipDisablerId = 79;
    size_t const FireDemoShipDisablerTechId = 78;
	size_t const mountains[] = {310,311,744,745,1041,1042,1043,1044,1045,1046,1047};
    size_t const gates[] = {85,88,90,91,490,491,667,688,669,670,673,674};
    //size_t const gateposts[] = {80,81,92,95,663,664,671,672};

	//unpackable TC
	aocDat->UnitHeaders[PTWC].Commands.push_back(aocDat->UnitHeaders[pTrebId].Commands[0]);
    aocDat->UnitHeaders[PTWC].Commands[0].SelectionMode = 0;
    aocDat->UnitHeaders[PTWC].Commands[0].SelectionEnabler = 0;
	//Trade Workshop with gold trickle
	aocDat->UnitHeaders[tradeWorkshopID].Commands.push_back(aocDat->UnitHeaders[feitoriaID].Commands[3]);
	aocDat->UnitHeaders[tradeWorkshopID].Commands[0].Quantity = 1;
	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		//fixes faulty elite camel archer data for non-gaia civs
		aocDat->Civs[civIndex].Units[eliteCamelArcherID] = aocDat->Civs[0].Units[eliteCamelArcherID];
		aocDat->Civs[civIndex].Units[cannonGalleonID].Creatable.HeroMode -= 128; //restore patrol for cannon galleons
		aocDat->Civs[civIndex].Units[eliteCannonGalleonID].Creatable.HeroMode -= 128;        
        aocDat->Civs[civIndex].Units[PTWC] = aocDat->Civs[0].Units[PTWC]; //unpackable TC
        aocDat->Civs[civIndex].Units[monkeyBoyID].Class = 10; //This makes gaia monkey boys attack instead of being captured
        aocDat->Civs[civIndex].Units[sharkatzorID].HideInEditor = 0; //unhide sharkatzor in editor
        //Kings can't attack, to make sure there are no graphics issues we set the attack graphic to the standing graphic
        aocDat->Civs[civIndex].Units[kingID].Type50.AttackGraphic = aocDat->Civs[civIndex].Units[kingID].StandingGraphic.first;
        //fix gate rubbles
        for (size_t i = 0; i < sizeof(gates)/sizeof(gates[0]); i++) {
            aocDat->Civs[civIndex].Units[gates[i]].DeadUnitID = 144;
        }
	}
    //Fix longboats having an unload ability that could mess with attacks
    aocDat->UnitHeaders[longboatID].Commands.pop_back();
    aocDat->UnitHeaders[eliteLongboatID].Commands.pop_back();
	//Fix teuton team bonus being overwritten in post-imp
	aocDat->Techages[teutonTeamBonusID].Effects[1].B = 1;
	aocDat->Techages[teutonTeamBonusID].Effects[2].B = 1;
    //Fixes Autoresearch for cartography
    aocDat->Researchs[cartographyID].ResearchTime = 0;
    aocDat->Researchs[cartographyID].RequiredTechs[1] = 128;
    aocDat->Researchs[cartographyID].RequiredTechCount = 2;
    aocDat->TechTree.ResearchConnections.erase(aocDat->TechTree.ResearchConnections.begin()+6);
    //fixes holy line, relics block entire tile
    aocDat->Civs[0].Units[relicID].CollisionSize = {0.5,0.5,2};
    //mountains were too small for their graphics
	for (size_t i = 0; i < sizeof(mountains)/sizeof(mountains[0]); i++) {
        aocDat->Civs[0].Units[mountains[i]].ClearanceSize = {3,3};
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
    //Team Units tech tree fix
	aocDat->TechTree.UnitConnections[132].Unknown1 = 41;
	aocDat->TechTree.UnitConnections[101].Unknown1 = 29;
	aocDat->TechTree.UnitConnections[116].Unknown1 = 37;
	aocDat->TechTree.UnitConnections[117].Unknown1 = 37;
    //Hunting Dogs got auto-researched in feudal+ games
    aocDat->Researchs[huntingDogsId].RequiredTechs[0] = -1;
    //Make it possible for maps to go back to aoc water by disabling fire/demo galley and a disabler for fire/demo shop
    aocDat->Researchs[FireDemoShipEnablerId].Name = "Fire/Demo Enabler";
    aocDat->Researchs[fireShipResearchId].RequiredTechs[1] = FireDemoShipEnablerId;
    aocDat->Researchs[demoShipResearchId].RequiredTechs[1] = FireDemoShipEnablerId;
    aocDat->Techages[fireShipTechId].Effects[0].A = fireGalleyId;
    genie::TechageEffect effect = aocDat->Techages[fireShipTechId].Effects[0];
    effect.A = demoRaftId;;
    aocDat->Techages[demoShipTechId].Effects.push_back(effect);
    effect.Type = 3;
    effect.B = demoShipId;
    aocDat->Techages[demoShipTechId].Effects.push_back(effect);
    effect.A = fireGalleyId;
    effect.B = fireShipId;
    aocDat->Techages[fireShipTechId].Effects.push_back(effect);
    aocDat->Researchs[FireDemoShipDisablerId].Name = "Fire/Demo Disabler";
    aocDat->Researchs[FireDemoShipDisablerId].TechageID = FireDemoShipDisablerTechId;
    aocDat->Researchs[FireDemoShipDisablerId].RequiredTechCount = 1;
    aocDat->Researchs[FireDemoShipDisablerId].RequiredTechs = {feudalAgeResearchId, -1, -1, -1, -1, -1};
    aocDat->Researchs[FireDemoShipDisablerId].Civ = -1;
    aocDat->Researchs[FireDemoShipDisablerId].ResearchLocation = -1;
    aocDat->Techages[FireDemoShipDisablerTechId].Effects[0].Type = 102;
    aocDat->Techages[FireDemoShipDisablerTechId].Effects[0].D = FireDemoShipEnablerId;
    aocDat->Techages[FireDemoShipDisablerTechId].Effects[1].Type = 102;
    aocDat->Techages[FireDemoShipDisablerTechId].Effects[1].D = demoShipResearchId;
}

DatPatch smallFixes = {
	&smallPatches,
	"Several small civ bug fixes"
};

}

