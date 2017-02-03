#include "burmesefix.h"
#include "wololo/datPatch.h"

namespace wololo {

void burmesePatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {
	/*
	 * Relics now spawn invisible gaia units by them that can be discoverd like sheep
	 * The burmese bonus makes those not able to be stolen by enemies and gives them LoS
	 * This somewhat emulates the original bonus
	 */
	(*langReplacement)[20179] = "Monk and Elephant civilization \\n\\n· Free Lumbercamp upgrades \\n· Infantry +1 attack per Age\\n· Monastery techs 50% cheaper \\n\\n<b>Unique Unit:<b> Arambai (ranged cavalry)\\n\\n<b>Unique Techs:<b> Howdah (Battle Elephants +1/+2 armor); Manipur Cavalry (Cavalry and Arambai +6 attack vs buildings)\\n\\n<b>Team Bonus:<b> Relics visible on map (only enabled on WK_... maps)";


	/*size_t const incaScout = 888;
	size_t const createEagle = 889;
	size_t const relic = 285;
	size_t const sheep = 594;
	size_t const createRelicWatcher = 1375;
	size_t const createRelic = 1376;*/
	size_t const mapRevealer = 837;
	size_t const relicWatcher = 1378;
	size_t const burmeseTB = 651;
	//size_t const newRelic = 1378;

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
/*
		aocDat->Civs[civIndex].Units[newRelic] = aocDat->Civs[0].Units[relic];
		aocDat->Civs[civIndex].Units[newRelic].HideInEditor = 1;
		aocDat->Civs[civIndex].Units[createRelic] = aocDat->Civs[civIndex].Units[createEagle];
		aocDat->Civs[civIndex].Units[createRelicWatcher] = aocDat->Civs[civIndex].Units[createEagle];

		aocDat->Civs[civIndex].Units[createRelic].DeadUnitID = newRelic;
		aocDat->Civs[civIndex].Units[createRelicWatcher].DeadUnitID = relicWatcher; */

		aocDat->Civs[civIndex].Units[relicWatcher] = aocDat->Civs[civIndex].Units[mapRevealer];
		aocDat->Civs[civIndex].Units[relicWatcher].LineOfSight = 0;

		aocDat->Civs[civIndex].Units[relicWatcher].Type = 70;
		aocDat->Civs[civIndex].Units[relicWatcher].Bird.ActionWhenDiscoveredID = 0;
		aocDat->Civs[civIndex].Units[relicWatcher].Bird.SearchRadius = 4;

		//ID Fixes
		aocDat->Civs[civIndex].Units[relicWatcher].ID1 = relicWatcher;
		aocDat->Civs[civIndex].Units[relicWatcher].ID2 = relicWatcher;
		aocDat->Civs[civIndex].Units[relicWatcher].ID3 = relicWatcher;

		/*aocDat->Civs[civIndex].Units[newRelic].ID1 = newRelic;
		aocDat->Civs[civIndex].Units[newRelic].ID2 = newRelic;
		aocDat->Civs[civIndex].Units[newRelic].ID3 = newRelic;
		aocDat->Civs[civIndex].Units[createRelic].ID1 = createRelic;
		aocDat->Civs[civIndex].Units[createRelic].ID2 = createRelic;
		aocDat->Civs[civIndex].Units[createRelic].ID3 = createRelic;
		aocDat->Civs[civIndex].Units[createRelicWatcher].ID1 = createRelicWatcher;
		aocDat->Civs[civIndex].Units[createRelicWatcher].ID2 = createRelicWatcher;
		aocDat->Civs[civIndex].Units[createRelicWatcher].ID3 = createRelicWatcher; */

	}

	aocDat->Techages[burmeseTB].Effects[0].Type = 0;
	aocDat->Techages[burmeseTB].Effects[0].A = relicWatcher;
	aocDat->Techages[burmeseTB].Effects[0].B = -1;
	aocDat->Techages[burmeseTB].Effects[0].C = 1; //line of sight
	aocDat->Techages[burmeseTB].Effects[0].D = 1;
	//aocDat->UnitHeaders[relicWatcher].Commands.push_back(aocDat->UnitHeaders[sheep].Commands[0]);

	/*
	aocDat->Civs[0].Units[relic] = aocDat->Civs[0].Units[incaScout];
	aocDat->Civs[0].Units[relic].Name = "Burmese Relic";
	aocDat->Civs[0].Units[relic].StandingGraphic = aocDat->Civs[0].Units[newRelic].StandingGraphic;
	aocDat->Civs[0].Units[relic].ClearanceSize = aocDat->Civs[0].Units[newRelic].ClearanceSize;
	aocDat->Civs[0].Units[relic].SelectionShapeSize = aocDat->Civs[0].Units[newRelic].SelectionShapeSize;
	aocDat->Civs[0].Units[relic].HideInEditor = 0;
	aocDat->Civs[0].Units[relic].LanguageDLLName = aocDat->Civs[0].Units[newRelic].LanguageDLLName;
	aocDat->Civs[0].Units[relic].LanguageDLLCreation = aocDat->Civs[0].Units[newRelic].LanguageDLLCreation;
	aocDat->Civs[0].Units[relic].LanguageDLLHelp = aocDat->Civs[0].Units[newRelic].LanguageDLLHelp;
	aocDat->Civs[0].Units[relic].LanguageDLLHotKeyText = aocDat->Civs[0].Units[newRelic].LanguageDLLHotKeyText;
	aocDat->Civs[0].Units[relic].Building.Annexes[0].UnitID = createRelic;
	aocDat->Civs[0].Units[relic].Building.Annexes[1].UnitID = createRelicWatcher;
	aocDat->Civs[0].Units[relic].Building.Annexes[1].Misplacement = {0,0};
	//IDs
	aocDat->Civs[0].Units[relic].ID1 = relic;
	aocDat->Civs[0].Units[relic].ID2 = relic;
	aocDat->Civs[0].Units[relic].ID3 = relic; */
}

DatPatch burmeseFix = {
	&burmesePatch,
	"Burmese relic LoS Team bonus alternative"
};

}

