#include "vietfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void vietPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	//Civ Bonus Fix

	size_t const vietResearchID = 665;
	size_t const vietTechID = 698;
	//size_t const vietTechTreeID = 652;
	aocDat->Researchs[vietResearchID].ResearchTime = 0;
	aocDat->Researchs[vietResearchID].RequiredTechCount = 0;
	aocDat->Techages[vietTechID].Effects[0].D = 1;
	//aocDat->Techages[vietTechTreeID].Effects.push_back(aocDat->Techages[vietTechID].Effects[0]);

	//Only this seems to work, though it means it's still active in all-techs games
	aocDat->Civs[31].Resources[209] = 0;
}

DatPatch vietFix = {
	&vietPatch,
	"Vietnamese enemy LoS bonus fix"
};

}

