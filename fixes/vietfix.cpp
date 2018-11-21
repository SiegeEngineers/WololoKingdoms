#include "vietfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void vietPatch(genie::DatFile *aocDat) {

	//Civ Bonus Fix

	size_t const vietResearchID = 665;
    size_t const vietTechID = 698;
	aocDat->Researchs[vietResearchID].ResearchTime = 0;
	aocDat->Researchs[vietResearchID].RequiredTechCount = 0;
    aocDat->Techages[vietTechID].Effects[0].D = 3;

	aocDat->Civs[31].Resources[209] = 0;
}

DatPatch vietFix = {
	&vietPatch,
	"Vietnamese enemy LoS bonus fix"
};

}

