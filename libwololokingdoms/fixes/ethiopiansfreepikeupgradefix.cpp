#include "ethiopiansfreepikeupgradefix.h"
#include "wololo/datPatch.h"

namespace wololo {

void ethiopiansPikePatch(genie::DatFile* aocDat) {
  /*
   * In AOC, free tech effects must be in the civ tech tree in order to work
   */

  size_t const freePikeHalbTechId = 616;
  size_t const ethiopiansTechTreeTechId = 48;

  std::vector<genie::EffectCommand>* effectsPtr =
      &aocDat->Effects[freePikeHalbTechId].EffectCommands;
  for (std::vector<genie::EffectCommand>::iterator it = effectsPtr->begin(),
                                                   end = effectsPtr->end();
       it != end; it++) {
    aocDat->Effects[ethiopiansTechTreeTechId].EffectCommands.push_back(
        *it); // copy the effects into the ethiopians tech tree
  }
}

DatPatch ethiopiansFreePikeUpgradeFix = {
    &ethiopiansPikePatch,
    "Ethiopians free pike/halbs upgrades not working fix"};

} // namespace wololo
