#include "maliansfreeminingupgradefix.h"
#include "wololo/datPatch.h"

namespace wololo {

void maliansMiningUpgradesPatch(genie::DatFile* aocDat) {
  /*
   * In AOC, free tech effects must be in the civ tech tree in order to work
   */

  size_t const freeMiningUpgradesTechId = 621;
  size_t const maliansTechTreeTechId = 42;

  std::vector<genie::EffectCommand>* effectsPtr =
      &aocDat->Effects[freeMiningUpgradesTechId].EffectCommands;
  for (auto& it : *effectsPtr) {
    aocDat->Effects[maliansTechTreeTechId].EffectCommands.push_back(
        it); // copy the effects into the ethiopians tech tree
  }
}

DatPatch maliansFreeMiningUpgradeFix = {
    &maliansMiningUpgradesPatch,
    "Malians free gold mining upgrades not working fix"};

} // namespace wololo
