#include "incafix.h"
#include "wololo/datPatch.h"

namespace wololo {

void incaPatch(genie::DatFile *aocDat) {

	//Civ Bonus Fix
	/*
     * The incan llama bonus doesn't work if the upgrade effect is part of the civ techs
	 */
	size_t const incaTechTree = 3;
	size_t const llamaTechID = 804;
    size_t const llamaDisableTechID = 805;

    if(aocDat->Researchs.size() < llamaDisableTechID+1)
        aocDat->Researchs.resize(llamaDisableTechID+1);
    aocDat->Researchs[llamaTechID].TechageID = llamaTechID;
    aocDat->Researchs[llamaTechID].Civ = 21;
    aocDat->Researchs[llamaTechID].Name = "Llama bonus";

    if(aocDat->Techages.size() < llamaDisableTechID+1)
        aocDat->Techages.resize(llamaDisableTechID+1);

    aocDat->Techages[llamaTechID].Name = "Llama bonus";
    genie::TechageEffect effect = aocDat->Techages[incaTechTree].Effects.back();
    aocDat->Techages[incaTechTree].Effects.pop_back();
    aocDat->Techages[llamaTechID].Effects.push_back(effect);

}

DatPatch incaFix = {
	&incaPatch,
	"Inca Llama bonus fix"
};

}

