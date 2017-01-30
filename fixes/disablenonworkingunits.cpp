#include "disablenonworkingunits.h"
#include "wololo/datPatch.h"

namespace wololo {

void disableNonWorkingUnitsPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {
	/*
	 * Not a real fix, but will avoid to have players making non working units
	 */

	size_t const siegeTowerUnitId = 1105;
	size_t const feitoriaUnitId = 1021;

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		aocDat->Civs[civIndex].Units[siegeTowerUnitId].Creatable.TrainLocationID = 0;
        //aocDat->Civs[civIndex].Units[feitoriaUnitId].Creatable.TrainLocationID = 0;
	}

}

DatPatch disableNonWorkingUnits = {
	&disableNonWorkingUnitsPatch,
	"Disable Siege Tower and Feitoria"
};

}

