#include "hotkeysfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void hotkeysPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	/*int const wonderUnitId = 276; //We will use that hotkey for palisade gates
	int const palisadeGateUnitIdStart = 789;
	int const palisadeGateUnitIdStop = 804;
	int const outpostUnitId = 598;
	int const feitoriaUnitId = 1021; */
//	int const siegeTowerUnitId = 1105; // hotkey to delete (useless)
	int const camelUnitId = 329; // we will use that hotkey for battle elephants
	int const battleEleId = 1132;
	int const cannonGalleonUnitId = 420; // we will use that hotkey for the genitour
	int const genitourPlaceholderUnitId = 1079;
	int const longboatUnitId = 250; // we will use that hotkey for the caravel
	int const caravelUnitId = 1004;

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		/* Probably not a good idea to set hotkeys for these buildings, any of these could overwrite the hotkey of another building
		 * which most likely will be used more often
		*/
//		aocDat->Civs[civIndex].Units[feitoriaUnitId].HotKey = aocDat->Civs[civIndex].Units[outpostUnitId].HotKey;
//		aocDat->Civs[civIndex].Units[siegeTowerUnitId].HotKey = aocDat->Civs[civIndex].Units[palisadeGateUnitId].HotKey;
/*
		for(int i = palisadeGateUnitIdStart; i<=palisadeGateUnitIdStop; i++) {
			aocDat->Civs[civIndex].Units[i].HotKey = aocDat->Civs[civIndex].Units[wonderUnitId].HotKey;
		}*/

		aocDat->Civs[civIndex].Units[battleEleId].HotKey = aocDat->Civs[civIndex].Units[camelUnitId].HotKey;
		aocDat->Civs[civIndex].Units[genitourPlaceholderUnitId].HotKey = aocDat->Civs[civIndex].Units[cannonGalleonUnitId].HotKey;
		aocDat->Civs[civIndex].Units[caravelUnitId].HotKey = aocDat->Civs[civIndex].Units[longboatUnitId].HotKey;
	}
}

DatPatch hotkeysFix = {
	&hotkeysPatch,
	"Overlapping hotkeys fix"
};

}

