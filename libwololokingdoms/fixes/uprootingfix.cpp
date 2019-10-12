#include "uprootingfix.h"
#include "wololo/datPatch.h"

namespace wololo {
void uprootingFixPatch(genie::DatFile* aocDat) {
  const size_t techIdSiegeOnager = 320;
  const size_t techIdUprooting = 152;
  for (auto & civ : aocDat->Civs) {
    auto techTreeId = civ.TechTreeID;
    auto techTreeEffect = &aocDat->Effects[civ.TechTreeID];
    // Checks if the tech tree effect is not disabled.
    bool foundDisabled = false;
    for (auto ec : techTreeEffect->EffectCommands) {
      if (ec.Type == genie::EffectCommand::DisableTech && ec.Amount == techIdSiegeOnager) {
        foundDisabled = true;
        break;
      }
    }
    // Disables Uprooting if Siege Onager is available.
    if (!foundDisabled) {
      auto disablerUprooting = genie::EffectCommand();
      disablerUprooting.Type = genie::EffectCommand::EffectType::DisableTech;
      disablerUprooting.Amount = techIdUprooting;
      techTreeEffect->EffectCommands.push_back(disablerUprooting);
    }
  }
}

DatPatch uprootingFix = {&uprootingFixPatch,
                         "Ensures SO civs cannot research uprooting."};
} // namespace wololo
