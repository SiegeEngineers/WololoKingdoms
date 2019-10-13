#include "aztecbonusfix.h"
#include "wololo/datPatch.h"

namespace wololo {

// Must be executed before creating queueable technologies.
void aztecBonusFixPatch(genie::DatFile* aocDat) {
  // Effect ID of the Aztec technology tree.
  const size_t effectIdTechTreeAztecs = 447;

  // Index of the Effect Command to change for the Aztecs.
  const size_t effectCommandAztecMultiplier = 47;

  // Unit ID of a packet trebuchet.
  const size_t unitIdTrebuchetPacked = 331;

  auto effectCommand = &aocDat->Effects[effectIdTechTreeAztecs].EffectCommands[effectCommandAztecMultiplier];
  effectCommand->TargetUnit = unitIdTrebuchetPacked;
  effectCommand->UnitClassID = -1;
}

DatPatch aztecBonusFix = {&aztecBonusFixPatch,
                           "Changes the Aztec Bonus to apply just to trebuchets and not to the packed siege class."};

} // namespace wololo
