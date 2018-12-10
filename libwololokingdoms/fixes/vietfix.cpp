#include "vietfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void vietPatch(genie::DatFile *aocDat) {

	//Civ Bonus Fix

	size_t const vietResearchID = 665;
    size_t const vietTechID = 698;
	aocDat->Techs[vietResearchID].ResearchTime = 0;
	aocDat->Techs[vietResearchID].RequiredTechCount = 0;
    aocDat->Effects[vietTechID].EffectCommands[0].Amount = 3;

	aocDat->Civs[31].Resources[209] = 0;
}

DatPatch vietFix = {
	&vietPatch,
	"Vietnamese enemy LoS bonus fix"
};

}

