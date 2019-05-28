#include "smallfixes.h"
#include "wololo/datPatch.h"

namespace wololo {

void smallPatches(genie::DatFile *aocDat) {
  int16_t const eliteCamelArcherID = 1009;
  int16_t const monkeyBoyID = 860;
  int16_t const cannonGalleonID = 420;
  int16_t const eliteCannonGalleonID = 691;
  int16_t const longboatID = 250;
  int16_t const eliteLongboatID = 533;
  int16_t const battleElephantID = 1132;
  int16_t const eliteBattleElephantID = 1134;
  int16_t const siegeTowerID = 1105;
  int16_t const PTWC = 444;
  int16_t const pTrebId = 331;
  int16_t const relicID = 285;
  int16_t const sharkatzorID = 1222;
  int16_t const kingID = 434;
  int16_t const cartographyID = 19;
  int16_t const feudalAgeResearchId = 101;
  int16_t const teutonTeamBonusID = 404;
  int16_t const huntingDogsId = 526;
  int16_t const fireShipResearchId = 243;
  int16_t const demoShipResearchId = 242;
  int16_t const demoShipTechId = 231;
  int16_t const fireShipTechId = 232;
  int16_t const demoShipId = 527;
  int16_t const demoRaftId = 1104;
  int16_t const fireShipId = 529;
  int16_t const fireGalleyId = 1103;
  int16_t const FireShipDisablerId = 167;
  int16_t const DemoShipDisablerId = 168;
  int16_t const FireShipDisablerTechId = 56;
  int16_t const DemoShipDisablerTechId = 57;
  int16_t const tradeCartId = 128;
  int16_t const xtradeCartId = 949;
  int16_t const xPatchTechId = 66;
  int16_t const xPatchDisablingTechId = 65;
  int16_t const xPatchResearchId = 89;
  int16_t const xPatchDisablingResearchId = 88;
  int16_t const shadowMarketId = 128;
  int16_t const coinageResearchId = 23;
  int16_t const bankingResearchId = 17;
  int16_t const castleCoinageDisablingResearchId = 223;
  int16_t const impCoinageDisablingResearchId = 224;
  int16_t const impBankingDisablingResearchId = 225;
  int16_t const castleCoinageResearchId = 226;
  int16_t const impCoinageResearchId = 227;
  int16_t const impBankingResearchId = 228;
  int16_t const castleCoinageDisablingTechId = 98;
  int16_t const impCoinageDisablingTechId = 99;
  int16_t const impBankingDisablingTechId = 100;
  int16_t const mountains[] = {310,311,744,745,1041,1042,1043,1044,1045,1046,1047};
  int16_t const gates[] = {85,88,90,91,490,491,667,688,669,670,673,674};
  //int16_t const gateposts[] = {80,81,92,95,663,664,671,672};

	//unpackable TC
	aocDat->UnitHeaders[PTWC].TaskList.push_back(aocDat->UnitHeaders[pTrebId].TaskList[0]);
    aocDat->UnitHeaders[PTWC].TaskList[0].TargetDiplomacy = 0;
    aocDat->UnitHeaders[PTWC].TaskList[0].EnableTargeting = 0;

    for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		//fixes faulty elite camel archer data for non-gaia civs
		aocDat->Civs[civIndex].Units[eliteCamelArcherID] = aocDat->Civs[0].Units[eliteCamelArcherID];
		aocDat->Civs[civIndex].Units[cannonGalleonID].Creatable.HeroMode -= 128; //restore patrol for cannon galleons
		aocDat->Civs[civIndex].Units[eliteCannonGalleonID].Creatable.HeroMode -= 128;        
        aocDat->Civs[civIndex].Units[PTWC] = aocDat->Civs[0].Units[PTWC]; //unpackable TC
        aocDat->Civs[civIndex].Units[monkeyBoyID].Class = 10; //This makes gaia monkey boys attack instead of being captured
        aocDat->Civs[civIndex].Units[sharkatzorID].HideInEditor = 0; //unhide sharkatzor in editor
        aocDat->Civs[civIndex].Units[battleElephantID].Combat.BlastAttackLevel = 10; //give battle elephants half-attack blast damage
        aocDat->Civs[civIndex].Units[eliteBattleElephantID].Combat.BlastAttackLevel = 10;
        //UP Siege Tower works better with 0.5 collision size
        aocDat->Civs[civIndex].Units[siegeTowerID].Size = { 0.5, 0.5, 2};
        //Kings can't attack, to make sure there are no graphics issues we set the attack graphic to the standing graphic
        aocDat->Civs[civIndex].Units[kingID].Combat.AttackGraphic = aocDat->Civs[civIndex].Units[kingID].StandingGraphic.first;
        //fix gate rubbles
        for (size_t i = 0; i < sizeof(gates)/sizeof(gates[0]); i++) {
            aocDat->Civs[civIndex].Units[gates[i]].DeadUnitID = 144;
        }
	}
    //Fix longboats having an unload ability that could mess with attacks
    aocDat->UnitHeaders[longboatID].TaskList.pop_back();
    aocDat->UnitHeaders[eliteLongboatID].TaskList.pop_back();
    //Fixes Autoresearch for cartography
    aocDat->Techs[cartographyID].ResearchTime = 0;
    aocDat->Techs[cartographyID].RequiredTechs[1] = 128;
    aocDat->Techs[cartographyID].RequiredTechCount = 2;
    aocDat->TechTree.ResearchConnections.erase(aocDat->TechTree.ResearchConnections.begin()+6);
    //fixes holy line, relics block entire tile
    aocDat->Civs[0].Units[relicID].Size = {0.5,0.5,2};
    //Fix teuton team bonus being overwritten in post-imp
    aocDat->Effects[teutonTeamBonusID].EffectCommands[1].UnitClassID = 1;
    aocDat->Effects[teutonTeamBonusID].EffectCommands[2].UnitClassID = 1;

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
	aocDat->TechTree.UnitConnections[132].Status = 41;
	aocDat->TechTree.UnitConnections[101].Status = 29;
	aocDat->TechTree.UnitConnections[116].Status = 37;
    aocDat->TechTree.UnitConnections[117].Status = 37;
    //Hunting Dogs got auto-researched in feudal+ games
    aocDat->Techs[huntingDogsId].RequiredTechs[0] = -1;
    //Make it possible for maps to go back to aoc water by disabling fire/demo galley and a disabler for fire/demo shop
    aocDat->Effects[fireShipTechId].EffectCommands[0].TargetUnit = fireGalleyId;
    genie::EffectCommand effect = aocDat->Effects[fireShipTechId].EffectCommands[0];
    effect.TargetUnit = demoRaftId;
    aocDat->Effects[demoShipTechId].EffectCommands.push_back(effect);
    effect.Type = 3;
    effect.UnitClassID = demoShipId;
    aocDat->Effects[demoShipTechId].EffectCommands.push_back(effect);
    effect.TargetUnit = fireGalleyId;
    effect.UnitClassID = fireShipId;
    aocDat->Effects[fireShipTechId].EffectCommands.push_back(effect);

    aocDat->Techs[FireShipDisablerId].Name = "Fire Ship Disabler";
    aocDat->Techs[FireShipDisablerId].EffectID = FireShipDisablerTechId;
    aocDat->Techs[FireShipDisablerId].RequiredTechCount = 1;
    aocDat->Techs[FireShipDisablerId].RequiredTechs = {feudalAgeResearchId, -1, -1, -1, -1, -1};
    aocDat->Techs[FireShipDisablerId].Civ = -1;
    aocDat->Techs[FireShipDisablerId].ResearchLocation = -1;
    aocDat->Effects[FireShipDisablerTechId].EffectCommands[0].Type = 102;
    aocDat->Effects[FireShipDisablerTechId].EffectCommands[0].Amount = fireShipResearchId;
    aocDat->Effects[FireShipDisablerTechId].EffectCommands.pop_back();

    aocDat->Techs[DemoShipDisablerId] = aocDat->Techs[FireShipDisablerId];
    aocDat->Techs[DemoShipDisablerId].Name = "Demo Ship Disabler";
    aocDat->Techs[DemoShipDisablerId].EffectID = DemoShipDisablerTechId;
    aocDat->Effects[DemoShipDisablerTechId].EffectCommands[0].Type = 102;
    aocDat->Effects[DemoShipDisablerTechId].EffectCommands[0].Amount = demoShipResearchId;
    aocDat->Effects[DemoShipDisablerTechId].EffectCommands.pop_back();

    aocDat->Techs[fireShipResearchId].Name = "Fire Ship (make avail)";
    aocDat->Techs[demoShipResearchId].Name = "Demo Ship (make avail)";
    aocDat->Techs[fireShipResearchId].RequiredTechCount = 1;
    aocDat->Techs[demoShipResearchId].RequiredTechCount = 1;

    // Have an x-patch upgrade to trade carts that's disabled by default

    aocDat->UnitHeaders[xtradeCartId] = aocDat->UnitHeaders[tradeCartId];
    for(size_t i = 0; i< aocDat->Civs.size(); i++) {
        aocDat->Civs[i].Units[xtradeCartId] = aocDat->Civs[i].Units[tradeCartId];
        aocDat->Civs[i].Units[xtradeCartId].HitPoints = 115;
        aocDat->Civs[i].Units[xtradeCartId].ResourceStorages[0].Amount *= 2;
        aocDat->Civs[i].Units[xtradeCartId].ResourceStorages[1].Amount *= 2;
        aocDat->Civs[i].Units[xtradeCartId].ResourceStorages[2].Amount *= 2;
        aocDat->Civs[i].Units[xtradeCartId].Action.WorkRate *= 1.65;
        aocDat->Civs[i].Units[xtradeCartId].Creatable.TrainTime *= 1.65;
        aocDat->Civs[i].Units[xtradeCartId].HitPoints = 115;
        aocDat->Civs[i].Units[xtradeCartId].Creatable.ResourceCosts[0].Amount *= 1.65;
        aocDat->Civs[i].Units[xtradeCartId].Creatable.ResourceCosts[1].Amount *= 1.65;
    }

    aocDat->Techs[xPatchResearchId] = aocDat->Techs[FireShipDisablerId];
    aocDat->Techs[xPatchResearchId].Name = "X Patch Trade Change";
    aocDat->Techs[xPatchResearchId].EffectID = xPatchTechId;
    aocDat->Techs[xPatchResearchId].RequiredTechCount = 2;
    aocDat->Techs[xPatchResearchId].RequiredTechs[1] = shadowMarketId;
    aocDat->Techs[xPatchDisablingResearchId] = aocDat->Techs[FireShipDisablerId];
    aocDat->Techs[xPatchDisablingResearchId].Name = "X Patch Disabler";
    aocDat->Techs[xPatchDisablingResearchId].EffectID = xPatchDisablingTechId;
    aocDat->Effects[xPatchDisablingTechId].EffectCommands[0].Type = 102;
    aocDat->Effects[xPatchDisablingTechId].EffectCommands[0].Amount = xPatchResearchId;
    effect.Type = 3;
    effect.TargetUnit = tradeCartId;
    effect.UnitClassID = xtradeCartId;
    aocDat->Effects[xPatchTechId].EffectCommands[0] = effect;

    // Make it possible to control the age of coinage and banking techs.
    aocDat->Techs[castleCoinageDisablingResearchId] = aocDat->Techs[FireShipDisablerId];
    aocDat->Techs[castleCoinageDisablingResearchId].Name = "Castle Coinage Disabler";
    aocDat->Techs[castleCoinageDisablingResearchId].EffectID = castleCoinageDisablingTechId;
    aocDat->Techs[impCoinageDisablingResearchId] = aocDat->Techs[FireShipDisablerId];
    aocDat->Techs[impCoinageDisablingResearchId].Name = "Imp Coinage Disabler";
    aocDat->Techs[impCoinageDisablingResearchId].EffectID = impCoinageDisablingTechId;
    aocDat->Techs[impBankingDisablingResearchId] = aocDat->Techs[FireShipDisablerId];
    aocDat->Techs[impBankingDisablingResearchId].Name = "Imp Banking Disabler";
    aocDat->Techs[impBankingDisablingResearchId].EffectID = impBankingDisablingTechId;

    aocDat->Techs[castleCoinageResearchId] = aocDat->Techs[coinageResearchId];
    aocDat->Techs[castleCoinageResearchId].Name = "Castle Coinage";
    aocDat->Techs[castleCoinageResearchId].RequiredTechs[0] = 102;
    aocDat->Techs[impCoinageResearchId] = aocDat->Techs[coinageResearchId];
    aocDat->Techs[impCoinageResearchId].Name = "Imp Coinage";
    aocDat->Techs[impCoinageResearchId].RequiredTechs[0] = 103;
    aocDat->Techs[bankingResearchId].RequiredTechs[2] = castleCoinageResearchId;
    aocDat->Techs[impBankingResearchId] = aocDat->Techs[bankingResearchId];
    aocDat->Techs[impBankingResearchId].Name = "Imp Banking";
    aocDat->Techs[impBankingResearchId].RequiredTechs[0] = 103;
    aocDat->Techs[impBankingResearchId].RequiredTechs[2] = castleCoinageResearchId;
    aocDat->Techs[impBankingResearchId].RequiredTechs[3] = impCoinageResearchId;


    aocDat->Effects[castleCoinageDisablingTechId].EffectCommands[0].Type = 102;
    aocDat->Effects[castleCoinageDisablingTechId].EffectCommands[0].Amount = castleCoinageResearchId;
    aocDat->Effects[castleCoinageDisablingTechId].EffectCommands.pop_back();
    aocDat->Effects[impCoinageDisablingTechId].EffectCommands[0].Type = 102;
    aocDat->Effects[impCoinageDisablingTechId].EffectCommands[0].Amount = impCoinageResearchId;
    aocDat->Effects[impCoinageDisablingTechId].EffectCommands.pop_back();
    aocDat->Effects[impBankingDisablingTechId].EffectCommands[0].Type = 102;
    aocDat->Effects[impBankingDisablingTechId].EffectCommands[0].Amount = impBankingResearchId;
    aocDat->Effects[impBankingDisablingTechId].EffectCommands.pop_back();


}

DatPatch smallFixes = {
	&smallPatches,
	"Several small civ bug fixes"
};

}

