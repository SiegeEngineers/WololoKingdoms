#include "maliansfreeminingupgradefix.h"
#include "wololo/datPatch.h"

namespace wololo {

void maliansMiningUpgradesPatch(genie::DatFile *aocDat) {
	/*
	 * In AOC, free tech effects must be in the civ tech tree in order to work
	 */

	size_t const freeMiningUpgradesTechId = 621;
	size_t const maliansTechTreeTechId = 42;

	std::vector<genie::TechageEffect> *effectsPtr = &aocDat->Techages[freeMiningUpgradesTechId].Effects;
	for (std::vector<genie::TechageEffect>::iterator it = effectsPtr->begin(), end = effectsPtr->end(); it != end; it++) {
		aocDat->Techages[maliansTechTreeTechId].Effects.push_back(*it); // copy the effects into the ethiopians tech tree
	}
}

DatPatch maliansFreeMiningUpgradeFix = {
	&maliansMiningUpgradesPatch,
	"Malians free gold mining upgrades not working fix"
};

}

