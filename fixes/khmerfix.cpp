#include "khmerfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void khmerPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	//Civ Bonus Fix
	size_t const khmerBonusTechId = 693;

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
}

DatPatch khmerFix = {
	&khmerPatch,
	"Villagers dropping resources in Khmer houses fix"
};

}

