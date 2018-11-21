#include "berbersutfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void berbersUTPatch(genie::DatFile *aocDat) {
	/*
	 * Thanks to UP 1.5 Kasbah works natively, the regeneration works with the hero ability and just needs slight adjustments
	 */

	size_t const berbersUT2TechId = 608;

	genie::TechageEffect effect;

	// imperial age
	std::vector<genie::TechageEffect> *effectsPtr = &aocDat->Techages[berbersUT2TechId].Effects;
	std::vector<genie::TechageEffect> effectsToAdd;
	for (std::vector<genie::TechageEffect>::iterator it = effectsPtr->begin(), end = effectsPtr->end(); it != end; it++) {
		//set a hero attribute for regen
		it->Type = 0; // set attribute
		it->C = 40; // hero attribute
		it->D = 4; // regen

		// add an attribute to modify the timer
		effect = *it;
		effect.C = 45;
		effect.D = 4;
		effectsToAdd.push_back(effect);
	}
	effectsPtr->insert(effectsPtr->end(), effectsToAdd.begin(), effectsToAdd.end());
}

DatPatch berbersUTFix = {
	&berbersUTPatch,
	"Berbers unique technologies alternative"
};

}

