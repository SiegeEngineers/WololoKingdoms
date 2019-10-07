#include "portuguesefix.h"
#include "wololo/datPatch.h"

namespace wololo {

void PortuguesePatch(genie::DatFile* aocDat) {
  /*
   * The imported Portuguese team bonus makes the game crash
   * so let's replace it by something simpler (and functionally identical ?)
   */
  size_t const portugueseTBonusTechId = 32;        // empty by default
  size_t const portugueseBrokenTBonusTechId = 626; // why is this even used ?
  size_t const caravelId = 1004;
  size_t const eliteCaravelId = 1006;
  size_t const petardId = 440;
  size_t const dockId = 45;
  size_t const castleId = 82;

  std::vector<genie::EffectCommand> effects;
  genie::EffectCommand effect;
  effect.Type = 1;        // ressource modifier
  effect.TargetUnit = 50; // reveal ally
  effect.UnitClassID = 1; // (+/-)
  effect.Amount = 1;      // +1
  effects.push_back(effect);

  // broken : prevents from researching caravan
  //	effect.Type = 102; // disable research
  //	effect.D = 19; // cartography
  //	effects.push_back(effect);

  aocDat->Effects[portugueseTBonusTechId].EffectCommands = effects;
  aocDat->Effects[portugueseBrokenTBonusTechId].EffectCommands =
      std::vector<genie::EffectCommand>(); // nothing

  // This moves the Caraval in the Tech tree to the castle, so that it shows up
  // only for portuguese and doesn't obscure shipwright Delete Caravels from the
  // dock and add them to the castle
  for (auto& building : aocDat->TechTree.BuildingConnections) {
    // Iterate connected units of each age
    if (building.ID == castleId) {
      for (auto unitIt = building.Units.begin(), end = building.Units.end();
           unitIt != end; ++unitIt) {
        if (*unitIt == petardId) {
          unitIt = building.Units.insert(unitIt, caravelId);
          ++unitIt;
          unitIt = building.Units.insert(unitIt, eliteCaravelId);
          break;
        }
      }
    }
    if (building.ID == dockId) {
      bool visited = false;
      for (auto unitIt = building.Units.begin(), end = building.Units.end();
           unitIt != end; ++unitIt) {
        if (*unitIt == caravelId || *unitIt == eliteCaravelId) {
          building.Units.erase(unitIt--);
          end = building.Units.end();
          if (visited)
            break;
          visited = true;
        }
      }
    }
  }

  // Change UpperBuilding of Caravels to the Castle
  for (auto& unit : aocDat->TechTree.UnitConnections) {
    if (unit.ID == caravelId || unit.ID == eliteCaravelId) {
      unit.UpperBuilding = castleId;
    }
  }
}

DatPatch portugueseFix = {&PortuguesePatch, "Portuguese civ crash fix"};

} // namespace wololo
