#include "malayfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void malayPatch(genie::DatFile* aocDat) {
  /*
   * In order to work in AoC, research time manipulation has to be part of the
   * tech tree tech
   */

  size_t const malayTechTreeId = 648;
  size_t const fishtrapResourceId = 88;
  size_t const malayEffectTechId = 674;
  size_t const fishtrapTechId = 677;

  for (size_t i = 0;
       i < aocDat->Effects[malayEffectTechId].EffectCommands.size(); i++) {
    aocDat->Effects[malayTechTreeId].EffectCommands.push_back(
        aocDat->Effects[malayEffectTechId].EffectCommands[i]);
  }
  if (aocDat->Effects[fishtrapTechId].EffectCommands.size() == 0 ||
      aocDat->Effects[fishtrapTechId].EffectCommands[0].TargetUnit != 88) {
    // Fish trap effect not fixed on HD yet
    genie::EffectCommand effect;
    effect.Type = 1; // ressource modifier
    effect.TargetUnit = fishtrapResourceId;
    effect.UnitClassID = 0; // Set
    effect.Amount = aocDat->Civs[29].Resources[fishtrapResourceId] > 715
                        ? aocDat->Civs[29].Resources[fishtrapResourceId]
                        : 70000000000000; // 7e+13...
    aocDat->Effects[malayTechTreeId].EffectCommands.push_back(effect);
    aocDat->Civs[29].Resources[fishtrapResourceId] = 700;
  }
}

DatPatch malayFix = {&malayPatch, "Fixing the Malay Age up-bonus"};

} // namespace wololo
