#include "hotkeysfix.h"
#include "wololo/fix.h"

namespace wololo {

void hotkeysPatch(genie::DatFile *aocDat) {
	/*
	 *
	 */

	int const palissadeGateUnitId = 789; // no hotkey : good
	int const feitoriaUnitId = 1021; // hotkey to delete (useless)
	int const siegeTowerUnitId = 1105; // hotkey to delete (useless)
	int const condottieroUnitId = 882; // we will use that hotkey for the genitour
	int const genitourPlaceholderUnitId = 1079;
	int const longboatUnitId = 250; // we will use that hotkey for the caravel
	int const caravelUnitId = 1004;

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		aocDat->Civs[civIndex].Units[feitoriaUnitId].HotKey = aocDat->Civs[civIndex].Units[palissadeGateUnitId].HotKey;
		aocDat->Civs[civIndex].Units[siegeTowerUnitId].HotKey = aocDat->Civs[civIndex].Units[palissadeGateUnitId].HotKey;
		aocDat->Civs[civIndex].Units[genitourPlaceholderUnitId].HotKey = aocDat->Civs[civIndex].Units[condottieroUnitId].HotKey;
		aocDat->Civs[civIndex].Units[caravelUnitId].HotKey = aocDat->Civs[civIndex].Units[longboatUnitId].HotKey;
	}
}

Fix hotkeysFix = {
	&hotkeysPatch,
	"Overlapping hotkeys fix"
};

}

