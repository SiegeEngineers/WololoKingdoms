#include "burmesefix.h"
#include "wololo/datPatch.h"

namespace wololo {

void burmesePatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	//It doesn't work as a TB tech, has to be a normal tech with team effect

	size_t const burmeseTeamBonusTechId = 651;
	size_t const relicTechID = 803;
	size_t const relicResearchID = 803;

	if(aocDat->Researchs.size() < relicResearchID+1)
		aocDat->Researchs.resize(relicResearchID+1);
	aocDat->Researchs[relicResearchID].TechageID = relicTechID;
	aocDat->Researchs[relicResearchID].Civ = 30;
	aocDat->Researchs[relicResearchID].Name = "Enable Relic LoS";

	if(aocDat->Techages.size() < relicTechID+1)
		aocDat->Techages.resize(relicTechID+1);

	genie::TechageEffect effect = aocDat->Techages[burmeseTeamBonusTechId].Effects[0];
	effect.Type = 11;
	aocDat->Techages[relicTechID].Effects.push_back(effect);

	aocDat->Civs[30].Resources[210] = 0;


	/*
	 * Free upgrades have to be part of the tech tree effect in AoC
	 */


	size_t const burmeseRelicTech = 685;
	size_t const burmeseTechTree = 650;

	for (size_t i = 0; i < aocDat->Techages[burmeseRelicTech].Effects.size(); i++) {
		aocDat->Techages[burmeseTechTree].Effects.push_back(aocDat->Techages[burmeseRelicTech].Effects[i]);
	}
}

DatPatch burmeseFix = {
	&burmesePatch,
	"Burmese relic LoS Team bonus and Lumbercamp upgrade fix"
};

}

