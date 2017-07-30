#include "disablenonworkingunits.h"
#include "wololo/datPatch.h"

namespace wololo {

void disableNonWorkingUnitsPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {
	/*
	 * Disabling units that are not supposed to show in the scenario editor
	 */

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		aocDat->Civs[civIndex].Units[1119].HideInEditor = 1;
		aocDat->Civs[civIndex].Units[1145].HideInEditor = 1;
		aocDat->Civs[civIndex].Units[1147].HideInEditor = 1;
		aocDat->Civs[civIndex].Units[1221].HideInEditor = 1;		
		aocDat->Civs[civIndex].Units[1401].HideInEditor = 1;
		for (size_t unitIndex = 1224; unitIndex <= 1390; unitIndex++) {
			aocDat->Civs[civIndex].Units[unitIndex].HideInEditor = 1;
		}
	}

}

DatPatch disableNonWorkingUnits = {
	&disableNonWorkingUnitsPatch,
	"Hide units in the scenario editor"
};

}

