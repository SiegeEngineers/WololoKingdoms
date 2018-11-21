#include "khmerfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void khmerPatch(genie::DatFile *aocDat) {

	//Civ Bonus Fix
	size_t const khmerBonusTechId = 693;
    size_t const ballistaElephantId = 1120;
    size_t const newElephantId = 946;
    size_t const chemistryId = 47;
    size_t const siegeEngineersId = 375;
    size_t const doubleCrossbowId = 663;
    size_t const eliteBallistaTechId = 655;
    size_t const khmerBuildingResearchId = 638;
    size_t const buildingResearchs[] = {216,659,660,661,666,667,668};


	genie::TechageEffect effect;

	// imperial age
	std::vector<genie::TechageEffect> *effectsPtr = &aocDat->Techages[khmerBonusTechId].Effects;
	std::vector<genie::TechageEffect> effectsToAdd;
	for (std::vector<genie::TechageEffect>::iterator it = effectsPtr->begin(), end = effectsPtr->end(); it != end; it++) {
		// add an attribute to disable drop-off
		effect = *it;
		effect.Type = 0;
		effect.C = 31; // Drop-off of resources
		effect.D = 4;
		effectsToAdd.push_back(effect);
	}
	effectsPtr->insert(effectsPtr->end(), effectsToAdd.begin(), effectsToAdd.end());

    //We create a seperate Castle Age Ballista Elephant that can't cut trees, for use in CtR maps
    aocDat->UnitHeaders[newElephantId] = aocDat->UnitHeaders[ballistaElephantId];
    aocDat->UnitHeaders[newElephantId].Commands.pop_back();
    for(int i = 0; i<aocDat->Civs.size(); i++) {
        aocDat->Civs[i].Units[newElephantId] = aocDat->Civs[i].Units[ballistaElephantId];
        aocDat->Civs[i].Units[newElephantId].Type50.Attacks.erase(aocDat->Civs[i].Units[newElephantId].Type50.Attacks.begin()+4);
        aocDat->Civs[i].Units[newElephantId].Type50.BlastAttackLevel = 2;
    }
    aocDat->Techages[doubleCrossbowId].Effects.push_back(aocDat->Techages[doubleCrossbowId].Effects[0]);
    aocDat->Techages[doubleCrossbowId].Effects.push_back(aocDat->Techages[doubleCrossbowId].Effects[4]);
    aocDat->Techages[doubleCrossbowId].Effects[8].A = newElephantId;
    aocDat->Techages[doubleCrossbowId].Effects[9].A = newElephantId;

    aocDat->Techages[chemistryId].Effects.push_back(aocDat->Techages[chemistryId].Effects[66]);
    aocDat->Techages[chemistryId].Effects[68].A = newElephantId;

    aocDat->Techages[eliteBallistaTechId].Effects.push_back(aocDat->Techages[eliteBallistaTechId].Effects[0]);
    aocDat->Techages[eliteBallistaTechId].Effects[1].A = newElephantId;

    for(int i = 24; i < 28; i++) {
        aocDat->Techages[siegeEngineersId].Effects.push_back(aocDat->Techages[siegeEngineersId].Effects[i]);
        aocDat->Techages[siegeEngineersId].Effects[i+4].A = newElephantId;
    }

    aocDat->Researchs[khmerBuildingResearchId].Civ = 28;
    aocDat->Researchs[khmerBuildingResearchId].Name = "Khmer Building Bonus";
    aocDat->Researchs[khmerBuildingResearchId].RequiredTechCount = 0;
    aocDat->Researchs[khmerBuildingResearchId].RequiredTechs[0] = -1;
    aocDat->Researchs[khmerBuildingResearchId].RequiredTechs[1] = -1;

    for(int i = 0; i < sizeof(buildingResearchs)/sizeof(buildingResearchs[0]); i++) {
        aocDat->Researchs[buildingResearchs[i]].RequiredTechs[1] = khmerBuildingResearchId;
    }


}

DatPatch khmerFix = {
	&khmerPatch,
	"Villagers dropping resources in Khmer houses fix"
};

}

