#include "separateshipgraphics.h"
#include "wololo/datPatch.h"

namespace wololo {

// Returns `true` if `unitClass` is one of
// `TransportBoat (20)`,
// `FishingBoat (21)`,
// `Warship (22)`.
// Returns `false` otherwise.
// @param unit the unit to check.
bool isShipClass(int16_t unitClass) {
  return unitClass == genie::Unit::UnitClass::TransportBoat ||
         unitClass == genie::Unit::UnitClass::FishingBoat ||
         unitClass == genie::Unit::UnitClass::Warship;
}

// Separates the ship sinking graphics of `aocDat`.
// Allows ships to have separate death animations in different directions.
//
// Must be executed before adding the additiona the Transport Sharkatzor unit, as that unit
// has the transport ship class. (Note the id for the Transport Sharkatzor is generated
// dynamically, so would take some work to skip).
void separateShipGraphicsPatch(genie::DatFile* aocDat) {
  // Unit ID for the STRBO unit.
  const size_t unitIdStrbo = 438;

  // Unit ID for the Saboteur.
  const size_t unitIdSaboteur = 706;

  // Mirroring mode for separated fishing ship sinking graphics.
  const size_t mirroringModeFishingShip = 6;

  // Angle count for separated fishing ship sinking graphics.
  const size_t angleCountFishingShip = 8;

  // Mirroring mode for separated warship and transport ship graphics.
  const size_t mirroringModeWarAndTransport = 12;

  // Angle count for separated warship and transport ship graphics.
  const size_t angleCountWarAndTransport = 16;

  for (auto& civ : aocDat->Civs) {
    for (int unitId = 0; unitId != civ.Units.size(); ++unitId) {
      auto unit = &civ.Units[unitId];
      // Skips units that are not ships.
      if (isShipClass(unit->Class) && unitId != unitIdStrbo && unitId != unitIdSaboteur) {
        auto dyingGraphicId = unit->DyingGraphic;
        auto dyingGraphic = &aocDat->Graphics[dyingGraphicId];
        if (unit->Class == genie::Unit::UnitClass::FishingBoat) {
          dyingGraphic->MirroringMode = mirroringModeFishingShip;
          dyingGraphic->AngleCount = angleCountFishingShip;
        } else {
          dyingGraphic->MirroringMode = mirroringModeWarAndTransport;
          dyingGraphic->AngleCount = angleCountWarAndTransport;
        }
      }
    }
  }
}

DatPatch separateShipGraphics = {&separateShipGraphicsPatch,
                           "Separates ship death animations to allow for multiple directions."};

} // namespace wololo
