#include "smallfixes.h"
#include "wololo/datPatch.h"

namespace wololo {

void smallPatches(genie::DatFile *aocDat) {

	size_t const eliteCamelArcherID = 1009;
    size_t const monkeyBoyID = 860;
	size_t const cannonGalleonID = 420;
	size_t const eliteCannonGalleonID = 691;
    size_t const longboatID = 250;
    size_t const eliteLongboatID = 533;
    size_t const battleElephantID = 1132;
    size_t const eliteBattleElephantID = 1134;
	size_t const teutonTeamBonusID = 404;
	size_t const PTWC = 444;
	size_t const pTrebId = 331;
    size_t const relicID = 285;
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
    size_t const FireShipDisablerId = 167;
    size_t const DemoShipDisablerId = 168;
    size_t const FireShipDisablerTechId = 56;
    size_t const DemoShipDisablerTechId = 57;
    size_t const tradeCartId = 128;
    size_t const xtradeCartId = 949;
    size_t const xPatchTechId = 66;
    size_t const xPatchDisablingTechId = 65;
    size_t const xPatchResearchId = 89;
    size_t const xPatchDisablingResearchId = 88;
    size_t const shadowMarketId = 128;
    size_t const coinageResearchId = 23;
    size_t const bankingResearchId = 17;
    size_t const castleCoinageDisablingResearchId = 223;
    size_t const impCoinageDisablingResearchId = 224;
    size_t const impBankingDisablingResearchId = 225;
    size_t const castleCoinageResearchId = 226;
    size_t const impCoinageResearchId = 227;
    size_t const impBankingResearchId = 228;
    size_t const castleCoinageDisablingTechId = 98;
    size_t const impCoinageDisablingTechId = 99;
    size_t const impBankingDisablingTechId = 100;
	size_t const mountains[] = {310,311,744,745,1041,1042,1043,1044,1045,1046,1047};
    size_t const gates[] = {85,88,90,91,490,491,667,688,669,670,673,674};
    //size_t const gateposts[] = {80,81,92,95,663,664,671,672};

	//unpackable TC
	aocDat->UnitHeaders[PTWC].Commands.push_back(aocDat->UnitHeaders[pTrebId].Commands[0]);
    aocDat->UnitHeaders[PTWC].Commands[0].SelectionMode = 0;
    aocDat->UnitHeaders[PTWC].Commands[0].SelectionEnabler = 0;

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
        aocDat->Civs[civIndex].Resources[198] = 1572; //Mod version: WK=1, version 5.7.2
		//fixes faulty elite camel archer data for non-gaia civs
		aocDat->Civs[civIndex].Units[eliteCamelArcherID] = aocDat->Civs[0].Units[eliteCamelArcherID];
		aocDat->Civs[civIndex].Units[cannonGalleonID].Creatable.HeroMode -= 128; //restore patrol for cannon galleons
		aocDat->Civs[civIndex].Units[eliteCannonGalleonID].Creatable.HeroMode -= 128;        
        aocDat->Civs[civIndex].Units[PTWC] = aocDat->Civs[0].Units[PTWC]; //unpackable TC
        aocDat->Civs[civIndex].Units[monkeyBoyID].Class = 10; //This makes gaia monkey boys attack instead of being captured
        aocDat->Civs[civIndex].Units[sharkatzorID].HideInEditor = 0; //unhide sharkatzor in editor
        aocDat->Civs[civIndex].Units[battleElephantID].Type50.BlastAttackLevel = 10; //give battle elephants half-attack blast damage
        aocDat->Civs[civIndex].Units[eliteBattleElephantID].Type50.BlastAttackLevel = 10;
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
    aocDat->Techages[fireShipTechId].Effects[0].A = fireGalleyId;
    genie::TechageEffect effect = aocDat->Techages[fireShipTechId].Effects[0];
    effect.A = demoRaftId;
    aocDat->Techages[demoShipTechId].Effects.push_back(effect);
    effect.Type = 3;
    effect.B = demoShipId;
    aocDat->Techages[demoShipTechId].Effects.push_back(effect);
    effect.A = fireGalleyId;
    effect.B = fireShipId;
    aocDat->Techages[fireShipTechId].Effects.push_back(effect);

    aocDat->Researchs[FireShipDisablerId].Name = "Fire Ship Disabler";
    aocDat->Researchs[FireShipDisablerId].TechageID = FireShipDisablerTechId;
    aocDat->Researchs[FireShipDisablerId].RequiredTechCount = 1;
    aocDat->Researchs[FireShipDisablerId].RequiredTechs = {feudalAgeResearchId, -1, -1, -1, -1, -1};
    aocDat->Researchs[FireShipDisablerId].Civ = -1;
    aocDat->Researchs[FireShipDisablerId].ResearchLocation = -1;
    aocDat->Techages[FireShipDisablerTechId].Effects[0].Type = 102;
    aocDat->Techages[FireShipDisablerTechId].Effects[0].D = fireShipResearchId;
    aocDat->Techages[FireShipDisablerTechId].Effects.pop_back();

    aocDat->Researchs[DemoShipDisablerId] = aocDat->Researchs[FireShipDisablerId];
    aocDat->Researchs[DemoShipDisablerId].Name = "Demo Ship Disabler";
    aocDat->Researchs[DemoShipDisablerId].TechageID = DemoShipDisablerTechId;
    //aocDat->Techages[DemoShipDisablerTechId].Effects[0].Type = 102;
    //aocDat->Techages[DemoShipDisablerTechId].Effects[0].D = DemoShipEnablerId;
    aocDat->Techages[DemoShipDisablerTechId].Effects[0].Type = 102;
    aocDat->Techages[DemoShipDisablerTechId].Effects[0].D = demoShipResearchId;
    aocDat->Techages[DemoShipDisablerTechId].Effects.pop_back();

    aocDat->Researchs[fireShipResearchId].Name = "Fire Ship (make avail)";
    aocDat->Researchs[demoShipResearchId].Name = "Demo Ship (make avail)";
    aocDat->Researchs[fireShipResearchId].RequiredTechCount = 1;
    aocDat->Researchs[demoShipResearchId].RequiredTechCount = 1;

    // Have an x-patch upgrade to trade carts that's disabled by default

    aocDat->UnitHeaders[xtradeCartId] = aocDat->UnitHeaders[tradeCartId];
    for(int i = 0; i< (int) aocDat->Civs.size(); i++) {
        aocDat->Civs[i].Units[xtradeCartId] = aocDat->Civs[i].Units[tradeCartId];
        aocDat->Civs[i].Units[xtradeCartId].HitPoints = 115;
        aocDat->Civs[i].Units[xtradeCartId].ResourceStorages[0].Amount *= 2;
        aocDat->Civs[i].Units[xtradeCartId].ResourceStorages[1].Amount *= 2;
        aocDat->Civs[i].Units[xtradeCartId].ResourceStorages[2].Amount *= 2;
        aocDat->Civs[i].Units[xtradeCartId].Bird.WorkRate *= 1.65;
        aocDat->Civs[i].Units[xtradeCartId].Creatable.TrainTime *= 1.65;
        aocDat->Civs[i].Units[xtradeCartId].HitPoints = 115;
        aocDat->Civs[i].Units[xtradeCartId].Creatable.ResourceCosts[0].Amount *= 1.65;
        aocDat->Civs[i].Units[xtradeCartId].Creatable.ResourceCosts[1].Amount *= 1.65;
    }

    aocDat->Researchs[xPatchResearchId] = aocDat->Researchs[FireShipDisablerId];
    aocDat->Researchs[xPatchResearchId].Name = "X Patch Trade Change";
    aocDat->Researchs[xPatchResearchId].TechageID = xPatchTechId;
    aocDat->Researchs[xPatchResearchId].RequiredTechCount = 2;
    aocDat->Researchs[xPatchResearchId].RequiredTechs[1] = shadowMarketId;
    aocDat->Researchs[xPatchDisablingResearchId] = aocDat->Researchs[FireShipDisablerId];
    aocDat->Researchs[xPatchDisablingResearchId].Name = "X Patch Disabler";
    aocDat->Researchs[xPatchDisablingResearchId].TechageID = xPatchDisablingTechId;
    aocDat->Techages[xPatchDisablingTechId].Effects[0].Type = 102;
    aocDat->Techages[xPatchDisablingTechId].Effects[0].D = xPatchResearchId;
    effect.Type = 3;
    effect.A = tradeCartId;
    effect.B = xtradeCartId;
    aocDat->Techages[xPatchTechId].Effects[0] = effect;

    // Make it possible to control the age of coinage and banking techs.
    aocDat->Researchs[castleCoinageDisablingResearchId] = aocDat->Researchs[FireShipDisablerId];
    aocDat->Researchs[castleCoinageDisablingResearchId].Name = "Castle Coinage Disabler";
    aocDat->Researchs[castleCoinageDisablingResearchId].TechageID = castleCoinageDisablingTechId;
    aocDat->Researchs[impCoinageDisablingResearchId] = aocDat->Researchs[FireShipDisablerId];
    aocDat->Researchs[impCoinageDisablingResearchId].Name = "Imp Coinage Disabler";
    aocDat->Researchs[impCoinageDisablingResearchId].TechageID = impCoinageDisablingTechId;
    aocDat->Researchs[impBankingDisablingResearchId] = aocDat->Researchs[FireShipDisablerId];
    aocDat->Researchs[impBankingDisablingResearchId].Name = "Imp Banking Disabler";
    aocDat->Researchs[impBankingDisablingResearchId].TechageID = impBankingDisablingTechId;

    aocDat->Researchs[castleCoinageResearchId] = aocDat->Researchs[coinageResearchId];
    aocDat->Researchs[castleCoinageResearchId].Name = "Castle Coinage";
    aocDat->Researchs[castleCoinageResearchId].RequiredTechs[0] += 1;
    aocDat->Researchs[impCoinageResearchId] = aocDat->Researchs[coinageResearchId];
    aocDat->Researchs[impCoinageResearchId].Name = "Imp Coinage";
    aocDat->Researchs[impCoinageResearchId].RequiredTechs[0] += 2;
    aocDat->Researchs[bankingResearchId].RequiredTechs[2] = castleCoinageResearchId;
    aocDat->Researchs[impBankingResearchId] = aocDat->Researchs[bankingResearchId];
    aocDat->Researchs[impBankingResearchId].Name = "Imp Banking";
    aocDat->Researchs[impBankingResearchId].RequiredTechs[0] += 1;
    aocDat->Researchs[impBankingResearchId].RequiredTechs[2] = castleCoinageResearchId;
    aocDat->Researchs[impBankingResearchId].RequiredTechs[3] = impCoinageResearchId;


    aocDat->Techages[castleCoinageDisablingTechId].Effects[0].Type = 102;
    aocDat->Techages[castleCoinageDisablingTechId].Effects[0].D = castleCoinageResearchId;
    aocDat->Techages[castleCoinageDisablingTechId].Effects.pop_back();
    aocDat->Techages[impCoinageDisablingTechId].Effects[0].Type = 102;
    aocDat->Techages[impCoinageDisablingTechId].Effects[0].D = impCoinageResearchId;
    aocDat->Techages[impCoinageDisablingTechId].Effects.pop_back();
    aocDat->Techages[impBankingDisablingTechId].Effects[0].Type = 102;
    aocDat->Techages[impBankingDisablingTechId].Effects[0].D = impBankingResearchId;
    aocDat->Techages[impBankingDisablingTechId].Effects.pop_back();


}

DatPatch smallFixes = {
	&smallPatches,
	"Several small civ bug fixes"
};

}

