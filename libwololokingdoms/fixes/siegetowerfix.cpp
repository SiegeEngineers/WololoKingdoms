#include "siegetowerfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void siegeTowerPatch(genie::DatFile* aocDat) {
  /*
   * This makes Siege Towers work (sooo much better than HD :O)
   */

  size_t const siegeTowerUnitId = 1105;
  aocDat->UnitHeaders[siegeTowerUnitId].TaskList[2].ActionType = 12;
  aocDat->UnitHeaders[siegeTowerUnitId].TaskList[2].CarryCheck = 0;
}

DatPatch siegeTowerFix = {&siegeTowerPatch, "Siege Tower Fix"};

} // namespace wololo
