#include "ethiopiansfreepikeupgradefix.h"
#include "wololo/fix.h"

namespace wololo {

void ethiopiansPikePatch(genie::DatFile *aocDat) {
	/*
	 * In AOC, free tech effects must be in the civ tech tree in order to work
	 */

	size_t const freePikeHalbTechId = 616;
	size_t const ethiopiansTechTreeTechId = 48;

	std::vector<genie::TechageEffect> *effectsPtr = &aocDat->Techages[freePikeHalbTechId].Effects;
	for (std::vector<genie::TechageEffect>::iterator it = effectsPtr->begin(), end = effectsPtr->end(); it != end; it++) {
		aocDat->Techages[ethiopiansTechTreeTechId].Effects.push_back(*it); // copy the effects into the ethiopians tech tree
	}
}

Fix etiopiansFreePikeUpgradeFix = {
	&ethiopiansPikePatch,
	"Ethiopians free pike/halbs upgrades not working fix"
};

}

