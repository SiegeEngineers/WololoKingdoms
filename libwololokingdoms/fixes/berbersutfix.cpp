#include "berbersutfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void berbersUTPatch(genie::DatFile *aocDat) {
	/*
	 * Thanks to UP 1.5 Kasbah works natively, the regeneration works with the hero ability and just needs slight adjustments
	 */

	size_t const berbersUT2TechId = 608;

	genie::EffectCommand effect;

	// imperial age
	std::vector<genie::EffectCommand> *effectsPtr = &aocDat->Effects[berbersUT2TechId].EffectCommands;
	std::vector<genie::EffectCommand> effectsToAdd;
	for (std::vector<genie::EffectCommand>::iterator it = effectsPtr->begin(), end = effectsPtr->end(); it != end; it++) {
		//set a hero attribute for regen
		it->Type = 0; // set attribute
		it->AttributeID = 40; // hero attribute
		it->Amount = 4.0f; // regen

		// add an attribute to modify the timer
		effect = *it;
		effect.AttributeID = 45;
		effect.Amount = 4.0f;
		effectsToAdd.push_back(effect);
	}
	effectsPtr->insert(effectsPtr->end(), effectsToAdd.begin(), effectsToAdd.end());
}

DatPatch berbersUTFix = {
	&berbersUTPatch,
	"Berbers unique technologies alternative"
};

}

