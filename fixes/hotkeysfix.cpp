#include "hotkeysfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void hotkeysPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	int const wonderUnitId = 276; //We will use that hotkey for Feitorias and Palisade Gates
	int const palisadeGateUnitIdStart = 789;
	int const palisadeGateUnitIdStop = 804;
	int const feitoriaUnitId = 1021;
	int const siegeTowerUnitId = 1105; // hotkey to delete (useless)
	int const camelUnitId = 329; // we will use that hotkey for battle elephants
	int const battleEleId = 1132;
	int const genitourPlaceholderUnitId = 1079;
	int const longboatUnitId = 250; // we will use that hotkey for the turtle ship
	int const turtleUnitId = 831; //we will use this for siege towers and genitours now that it's free

	(*langReplacement)[19053] = "Longboat, Caravel, Turtle Ship";
	(*langReplacement)[19072] = "Genitour, Siege Tower";
	(*langReplacement)[19032] = "Genitour: Change in Dock Menu";
	(*langReplacement)[19048] = "Siege Tower: Change in Dock Menu";

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		for(int i = palisadeGateUnitIdStart; i<=palisadeGateUnitIdStop; i++) {
			aocDat->Civs[civIndex].Units[i].HotKey = aocDat->Civs[civIndex].Units[wonderUnitId].HotKey;
		}

		aocDat->Civs[civIndex].Units[feitoriaUnitId].HotKey = aocDat->Civs[civIndex].Units[wonderUnitId].HotKey;
		aocDat->Civs[civIndex].Units[battleEleId].HotKey = aocDat->Civs[civIndex].Units[camelUnitId].HotKey;
		aocDat->Civs[civIndex].Units[siegeTowerUnitId].HotKey = aocDat->Civs[civIndex].Units[turtleUnitId].HotKey;
		aocDat->Civs[civIndex].Units[genitourPlaceholderUnitId].HotKey = aocDat->Civs[civIndex].Units[turtleUnitId].HotKey;
		aocDat->Civs[civIndex].Units[turtleUnitId].HotKey = aocDat->Civs[civIndex].Units[longboatUnitId].HotKey;
	}
}

DatPatch hotkeysFix = {
	&hotkeysPatch,
	"Overlapping hotkeys fix"
};

}

