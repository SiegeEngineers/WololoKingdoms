#include "vietfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void vietPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {
	/*
	 * Paper Money uses a team resource modifier not available in AoC
	 *
	 * Instead it becomes an aditional team bonus to have the research available
	 * Resource amounts will have to be adjusted
	 *
	 * The reveal enemy civ bonus has to be adjusted
	 */

	// Paper Money Fix
	(*langReplacement)[28298] = "Research <b>Paper Money<b> (<cost>) \\nYou receive +300 Gold.";

	size_t const vietTBonusTechId = 653;
	size_t const vietUT2TechId = 669;
	size_t const vietUT2ResearchId = 629;
	size_t const allCivPaperMoneyResearchId = 802; // NEW
	size_t const allCivPaperMoneyDisableResearchId = 803; // NEW
	size_t const allCivGenitourDisableResearchId = 606;
	size_t const allCivPaperMoneyDisableTechId = 803; // NEW
	size_t const allCivGenitourDisableTechId = 632;
	size_t const vietTechTreeTechId = 652;

	genie::TechageEffect effect;

	// decrease cost to 600 F - you're paying 600 F for 300 G
	aocDat->Researchs[vietUT2ResearchId].ResourceCosts[0].Amount -= 200;
	aocDat->Researchs[vietUT2ResearchId].ResourceCosts[1].Amount -= 200;
	//change the Paper Money tech to resource add instead of team resource add
	aocDat->Techages[vietUT2TechId].Effects[0].Type = 1;
	aocDat->Techages[vietUT2TechId].Effects[0].D = 300; //Amount

	if(aocDat->Researchs.size() < allCivPaperMoneyDisableResearchId+1)
		aocDat->Researchs.resize(allCivPaperMoneyDisableResearchId+1);
	// copy Paper Money research to the all-civ one
	aocDat->Researchs[allCivPaperMoneyResearchId] = aocDat->Researchs[vietUT2ResearchId];
	aocDat->Researchs[allCivPaperMoneyResearchId].ButtonID = 4;
	// next to Petard, If you also have a berber ally, you need to research kasbah first.
	//Didn't see a way to solve this, all other slots are taken (bottom right doesn't seem to exist
	aocDat->Researchs[allCivPaperMoneyResearchId].Civ = -1;
	aocDat->Researchs[allCivPaperMoneyResearchId].Name = "all-civs Paper Money";

	if(aocDat->Techages.size() < allCivPaperMoneyDisableTechId+1)
		aocDat->Techages.resize(allCivPaperMoneyDisableTechId+1);
	// tech that disables the all civ Paper Money
	aocDat->Techages[allCivPaperMoneyDisableTechId] = aocDat->Techages[allCivGenitourDisableTechId];
	aocDat->Techages[allCivPaperMoneyDisableTechId].Effects[0].D = allCivPaperMoneyResearchId;
	aocDat->Techages[allCivPaperMoneyDisableTechId].Name = "all-civs Paper Money (disable)";
	// research that disables the all civ Paper Money
	aocDat->Researchs[allCivPaperMoneyDisableResearchId] = aocDat->Researchs[allCivGenitourDisableResearchId];
	aocDat->Researchs[allCivPaperMoneyDisableResearchId].TechageID = allCivPaperMoneyDisableTechId;
	aocDat->Researchs[allCivPaperMoneyDisableResearchId].Name = "all-civs Paper Money (disable)";
	// avoid Paper Money being researched twice for the Vietnamese
	aocDat->Techages[vietTechTreeTechId].Effects.push_back(aocDat->Techages[allCivPaperMoneyDisableTechId].Effects[0]);
	// enable Paper Money for Vietnamese teammates
	effect = aocDat->Techages[vietTBonusTechId].Effects[0]; // disable the disabling research (for genitours)
	effect.D = allCivPaperMoneyDisableResearchId;
	aocDat->Techages[vietTBonusTechId].Effects.push_back(effect);

	//Civ Bonus Fix
	/*
	 * This lets the spies effect run for 15 seconds
	 * It uses the same technique as the incan llama bonus
	 */

	size_t const vietLosBonusTechId = 698;
	size_t const vietLosBonusResearchId = 665;
	size_t const losResource = 183;
	size_t const incaScout = 888;
	size_t const incaEagle = 889;
	size_t const deadArcher = 3;
	size_t const incaLlamaCreation = 1118;
	size_t const vietScout = 1387;
	size_t const vietScoutCav = 1388;
	size_t const vietTimerCreation = 1389;
	size_t const vietTimer = 1386;
	size_t const vietBuild = 1390;
	size_t const scoutCav = 448;
	size_t const oldAcademy = 0;
	aocDat->Researchs[vietLosBonusResearchId].ResearchTime = 0;

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		//The original scout dies into two units - one becomes a scout cav, the other to disable the spies resarch
		aocDat->Civs[civIndex].Units[vietScout] = aocDat->Civs[civIndex].Units[incaScout];
		aocDat->Civs[civIndex].Units[vietScout].Building.Annexes[0].UnitID = vietScoutCav;
		aocDat->Civs[civIndex].Units[vietScout].Building.Annexes[1].UnitID = vietTimerCreation;
		//This unit creates the standard scout cav
		aocDat->Civs[civIndex].Units[vietScoutCav] = aocDat->Civs[civIndex].Units[incaEagle];
		aocDat->Civs[civIndex].Units[vietScoutCav].DeadUnitID = scoutCav;
		//This unit creates a decaying unit used as a timer
		aocDat->Civs[civIndex].Units[vietTimerCreation] = aocDat->Civs[civIndex].Units[incaLlamaCreation];
		aocDat->Civs[civIndex].Units[vietTimerCreation].DeadUnitID = vietTimer;
		//This unit is used as a timer
		aocDat->Civs[civIndex].Units[vietTimer] = aocDat->Civs[civIndex].Units[deadArcher];
		aocDat->Civs[civIndex].Units[vietTimer].StandingGraphic = {-1,-1};
		aocDat->Civs[civIndex].Units[vietTimer].DeadUnitID = vietBuild;
		aocDat->Civs[civIndex].Units[vietTimer].ResourceStorages[0].Amount = 15;
		//This unit researches the disabling tech for the LOS bonus
		aocDat->Civs[civIndex].Units[vietBuild] = aocDat->Civs[civIndex].Units[oldAcademy];
		aocDat->Civs[civIndex].Units[vietBuild].Building.ResearchID = 665;

		//ID Fixes
		aocDat->Civs[civIndex].Units[vietScoutCav].ID1 = vietScoutCav;
		aocDat->Civs[civIndex].Units[vietScoutCav].ID2 = vietScoutCav;
		aocDat->Civs[civIndex].Units[vietScoutCav].ID3 = vietScoutCav;
		aocDat->Civs[civIndex].Units[vietScout].ID1 = vietScout;
		aocDat->Civs[civIndex].Units[vietScout].ID2 = vietScout;
		aocDat->Civs[civIndex].Units[vietScout].ID3 = vietScout;
		aocDat->Civs[civIndex].Units[vietTimerCreation].ID1 = vietTimerCreation;
		aocDat->Civs[civIndex].Units[vietTimerCreation].ID2 = vietTimerCreation;
		aocDat->Civs[civIndex].Units[vietTimerCreation].ID3 = vietTimerCreation;
		aocDat->Civs[civIndex].Units[vietTimer].ID1 = vietTimer;
		aocDat->Civs[civIndex].Units[vietTimer].ID2 = vietTimer;
		aocDat->Civs[civIndex].Units[vietTimer].ID3 = vietTimer;
		aocDat->Civs[civIndex].Units[vietBuild].ID1 = vietBuild;
		aocDat->Civs[civIndex].Units[vietBuild].ID2 = vietBuild;
		aocDat->Civs[civIndex].Units[vietBuild].ID3 = vietBuild;
	}

	aocDat->Techages[vietLosBonusTechId].Effects[0].A = losResource;
	effect = aocDat->Techages[vietLosBonusTechId].Effects[0];
	effect.D = 1;
	aocDat->Techages[vietTechTreeTechId].Effects.push_back(effect);
}

DatPatch vietFix = {
	&vietPatch,
	"Vietnamese LoS Bonus and Paper Money alternative"
};

}

