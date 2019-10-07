#include "hotkeysfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void hotkeysPatch(genie::DatFile* aocDat) {

  /*
      int const wonderUnitId = 276; //We will use that hotkey for Feitorias and
  Palisade Gates int const palisadeGateUnitIdStart = 789; int const
  palisadeGateUnitIdStop = 804; int const feitoriaUnitId = 1021; int const
  siegeTowerUnitId = 1105; // hotkey to delete (useless) int const
  genitourPlaceholderUnitId = 1079; int const longboatUnitId = 250; // we will
  use that hotkey for the turtle ship int const turtleUnitId = 831; //we will
  use this for siege towers and genitours now that it's free
  */

  int const camelUnitId = 329; // we will use that hotkey for battle elephants
  int const battleEleId = 1132;

  for (auto& Civ : aocDat->Civs) {
    /*for(int i = palisadeGateUnitIdStart; i<=palisadeGateUnitIdStop; i++) {
                    aocDat->Civs[civIndex].Units[i].HotKey =
    aocDat->Civs[civIndex].Units[wonderUnitId].HotKey;
            }

            aocDat->Civs[civIndex].Units[feitoriaUnitId].HotKey =
    aocDat->Civs[civIndex].Units[wonderUnitId].HotKey;
    aocDat->Civs[civIndex].Units[wonderUnitId].HotKey = -1; //Wonder hotkey
    isn't necessary and this avoids conflicts in imp
            aocDat->Civs[civIndex].Units[siegeTowerUnitId].HotKey =
    aocDat->Civs[civIndex].Units[turtleUnitId].HotKey;
            aocDat->Civs[civIndex].Units[genitourPlaceholderUnitId].HotKey =
    aocDat->Civs[civIndex].Units[turtleUnitId].HotKey;
            aocDat->Civs[civIndex].Units[turtleUnitId].HotKey =
    aocDat->Civs[civIndex].Units[longboatUnitId].HotKey;
    */

    Civ.Units[battleEleId].HotKey = Civ.Units[camelUnitId].HotKey;
  }
}

DatPatch hotkeysFix = {&hotkeysPatch, "Overlapping hotkeys fix"};

} // namespace wololo
