#include "incafix.h"
#include "wololo/datPatch.h"

namespace wololo {

void incaPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	//Civ Bonus Fix
	/*
	 * The incan llama bonus doesn't work if the upgrade effect is part of the civ techs
	 */
	size_t const incaTechTree = 3;
	size_t const llamaTechID = 804;
	size_t const llamaResearchID = 804;

	if(aocDat->Researchs.size() < llamaResearchID+1)
		aocDat->Researchs.resize(llamaResearchID+1);
	aocDat->Researchs[llamaResearchID].TechageID = llamaTechID;
	aocDat->Researchs[llamaResearchID].Civ = 21;
	aocDat->Researchs[llamaResearchID].Name = "Llama bonus";

	if(aocDat->Techages.size() < llamaTechID+1)
		aocDat->Techages.resize(llamaTechID+1);

	aocDat->Techages[llamaTechID].Effects.push_back(aocDat->Techages[incaTechTree].Effects.back());
	aocDat->Techages[incaTechTree].Effects.pop_back();
}

DatPatch incaFix = {
	&incaPatch,
	"Inca Llama bonus fix"
};

}

