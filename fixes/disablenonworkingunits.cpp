#include "disablenonworkingunits.h"
#include "wololo/datPatch.h"

namespace wololo {

void disableNonWorkingUnitsPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {
	/*
	 * Disabling units that are not supposed to show in the scenario editor
	 */

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		aocDat->Civs[civIndex].Units[1221].HideInEditor = 1;
		for (size_t unitIndex = 1224; unitIndex <= 1374; unitIndex++) {
			aocDat->Civs[civIndex].Units[unitIndex].HideInEditor = 1;
		}
		for (size_t unitIndex = 1378; unitIndex <= 1385; unitIndex++) {
			aocDat->Civs[civIndex].Units[unitIndex].HideInEditor = 1;
		}
	}

}

DatPatch disableNonWorkingUnits = {
	&disableNonWorkingUnitsPatch,
	"Hide units in the scenario editor"
};

}

