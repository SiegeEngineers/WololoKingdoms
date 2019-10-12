#include "houseattackfix.h"
#include "wololo/datPatch.h"

namespace wololo {
void houseAttackFixPatch(genie::DatFile* aocDat) {
  // TODO implement
  // Clears the task list for each house, removing the only task (which is the
  // combat task).
  const size_t unitIdsHouses[] = { 70, 463, 464, 465 };
  for (auto houseId : unitIdsHouses) {
    aocDat->UnitHeaders[houseId].TaskList.clear();
  }
}

DatPatch houseAttackFix = {
    &houseAttackFixPatch,
    "Demolition ship scenario editor default attack stance defensive."};

} // namespace wololo
