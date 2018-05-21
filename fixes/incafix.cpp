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
    size_t const llamaBuilding = 888;
    size_t const llama = 305;
    size_t const llamaDisableUnit = 889;
    size_t const emptyAnnexUnit = 890;
    size_t const incaAnnexUnit = 1118;
    size_t const TC = 109;
    size_t const TCs[] = {71,109,141,142};

    if(aocDat->Researchs.size() < llamaDisableTechID+1)
        aocDat->Researchs.resize(llamaDisableTechID+1);
    aocDat->Researchs[llamaTechID].TechageID = llamaTechID;
    aocDat->Researchs[llamaTechID].Civ = 21;
    aocDat->Researchs[llamaTechID].Name = "Llama bonus";
    aocDat->Researchs[llamaDisableTechID].TechageID = llamaDisableTechID;
    aocDat->Researchs[llamaDisableTechID].RequiredTechCount = 1;
    aocDat->Researchs[llamaDisableTechID].Name = "Disable Llama bonus";

    if(aocDat->Techages.size() < llamaDisableTechID+1)
        aocDat->Techages.resize(llamaDisableTechID+1);

    aocDat->Techages[llamaTechID].Name = "Llama bonus";
    aocDat->Techages[llamaDisableTechID].Name = "Disable Llama bonus";
    genie::TechageEffect effect = aocDat->Techages[incaTechTree].Effects.back();
    aocDat->Techages[incaTechTree].Effects.pop_back();
    aocDat->Techages[llamaTechID].Effects.push_back(effect);
    int tmp = effect.A;
    effect.A = effect.B;
    effect.B = llamaDisableUnit;
    aocDat->Techages[llamaDisableTechID].Effects.push_back(effect);
    effect.A = tmp;
    aocDat->Techages[llamaDisableTechID].Effects.push_back(effect);

    for(unsigned int i = 0; i<aocDat->Civs.size(); i++) {
        for(unsigned int j = 0; j<sizeof(TCs)/sizeof(TCs[0]); j++) {
            aocDat->Civs[i].Units[TCs[j]].Building.Annexes[3].UnitID = emptyAnnexUnit;
            aocDat->Civs[i].Units[TCs[j]].Building.Annexes[3].Misplacement = std::make_pair(-1.0,1.0);
        }
        aocDat->Civs[i].Units[llamaBuilding].Building.Annexes[0].UnitID = -1;
        aocDat->Civs[i].Units[llamaBuilding].Building.Annexes[1].UnitID = -1;
        aocDat->Civs[i].Units[llamaBuilding].DeadUnitID = llama;
        aocDat->Civs[i].Units[llamaBuilding].Building.ResearchID = llamaDisableTechID;
        aocDat->Civs[i].Units[llamaBuilding].Creatable.TrainTime = -1;
        aocDat->Civs[i].Units[llamaBuilding].Name = "Llama building";
        aocDat->Civs[i].Units[llamaDisableUnit].DeadUnitID = -1;
        aocDat->Civs[i].Units[llamaDisableUnit].Building.HeadUnit = TC;
        aocDat->Civs[i].Units[llamaDisableUnit].Creatable.TrainTime = -1;
        aocDat->Civs[i].Units[llamaDisableUnit].Name = "disables llama building";
        aocDat->Civs[i].Units[emptyAnnexUnit].Building.HeadUnit = TC;
        aocDat->Civs[i].Units[emptyAnnexUnit].Creatable.TrainTime = -1;
        aocDat->Civs[i].Units[emptyAnnexUnit].Name = "Empty llama annex";
        aocDat->Civs[i].Units[incaAnnexUnit].Building.HeadUnit = TC;
        aocDat->Civs[i].Units[incaAnnexUnit].Creatable.TrainTime = -1;
        aocDat->Civs[i].Units[incaAnnexUnit].DeadUnitID = llamaBuilding;
        aocDat->Civs[i].Units[incaAnnexUnit].Name = "Inca llama annex";

    }

}

DatPatch incaFix = {
	&incaPatch,
	"Inca Llama bonus fix"
};

}

