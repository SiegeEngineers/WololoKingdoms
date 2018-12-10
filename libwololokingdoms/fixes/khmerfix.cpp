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


	genie::EffectCommand effect;

	// imperial age
	std::vector<genie::EffectCommand> *effectsPtr = &aocDat->Effects[khmerBonusTechId].EffectCommands;
	std::vector<genie::EffectCommand> effectsToAdd;
	for (std::vector<genie::EffectCommand>::iterator it = effectsPtr->begin(), end = effectsPtr->end(); it != end; it++) {
		// add an attribute to disable drop-off
		effect = *it;
		effect.Type = 0;
		effect.AttributeID = 31; // Drop-off of resources
		effect.Amount = 4.0f;
		effectsToAdd.push_back(effect);
	}
	effectsPtr->insert(effectsPtr->end(), effectsToAdd.begin(), effectsToAdd.end());

    //We create a seperate Castle Age Ballista Elephant that can't cut trees, for use in CtR maps
    aocDat->UnitHeaders[newElephantId] = aocDat->UnitHeaders[ballistaElephantId];
    aocDat->UnitHeaders[newElephantId].TaskList.pop_back();
    for(int i = 0; i<aocDat->Civs.size(); i++) {
        aocDat->Civs[i].Units[newElephantId] = aocDat->Civs[i].Units[ballistaElephantId];
        aocDat->Civs[i].Units[newElephantId].Combat.Attacks.erase(aocDat->Civs[i].Units[newElephantId].Combat.Attacks.begin()+4);
        aocDat->Civs[i].Units[newElephantId].Combat.BlastAttackLevel = 2;
    }
    aocDat->Effects[doubleCrossbowId].EffectCommands.push_back(aocDat->Effects[doubleCrossbowId].EffectCommands[0]);
    aocDat->Effects[doubleCrossbowId].EffectCommands.push_back(aocDat->Effects[doubleCrossbowId].EffectCommands[4]);
    aocDat->Effects[doubleCrossbowId].EffectCommands[8].TargetUnit = newElephantId;
    aocDat->Effects[doubleCrossbowId].EffectCommands[9].TargetUnit = newElephantId;

    aocDat->Effects[chemistryId].EffectCommands.push_back(aocDat->Effects[chemistryId].EffectCommands[66]);
    aocDat->Effects[chemistryId].EffectCommands[68].TargetUnit = newElephantId;

    aocDat->Effects[eliteBallistaTechId].EffectCommands.push_back(aocDat->Effects[eliteBallistaTechId].EffectCommands[0]);
    aocDat->Effects[eliteBallistaTechId].EffectCommands[1].TargetUnit = newElephantId;

    for(int i = 24; i < 28; i++) {
        aocDat->Effects[siegeEngineersId].EffectCommands.push_back(aocDat->Effects[siegeEngineersId].EffectCommands[i]);
        aocDat->Effects[siegeEngineersId].EffectCommands[i+4].TargetUnit = newElephantId;
    }

    aocDat->Techs[khmerBuildingResearchId].Civ = 28;
    aocDat->Techs[khmerBuildingResearchId].Name = "Khmer Building Bonus";
    aocDat->Techs[khmerBuildingResearchId].RequiredTechCount = 0;
    aocDat->Techs[khmerBuildingResearchId].RequiredTechs[0] = -1;
    aocDat->Techs[khmerBuildingResearchId].RequiredTechs[1] = -1;

    for(int i = 0; i < sizeof(buildingResearchs)/sizeof(buildingResearchs[0]); i++) {
        aocDat->Techs[buildingResearchs[i]].RequiredTechs[1] = khmerBuildingResearchId;
    }


}

DatPatch khmerFix = {
	&khmerPatch,
	"Villagers dropping resources in Khmer houses fix"
};

}

