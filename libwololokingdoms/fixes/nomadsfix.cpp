#include "nomadsfix.h"
#include "wololo/datPatch.h"

namespace wololo {

// Requires that the house attack fix has already been applied.
void nomadsFixPatch(genie::DatFile * aocDat) {
  // Unit ID of a Castle Age House.
  const size_t unitIdHouseC = 464;

  // Unit ID of an Imperial Age House.
  const size_t unitIdHouseI = 465;

  // Rubble replaced by a Castle Age house copy.
  const size_t unitIdRubble1 = 191;

  // Rubble replaced by an Imperial Age house copy.
  const size_t unitIdRubble2 = 192;

  // The ID of the Effect used by the Nomads tech.
  const size_t effectIdFalconry = 542;

  // The house IDs to be used for the Castle Age version of Nomads.
  const size_t nomadsHouseIds[] = {70, 463, 464};

  // The house IDs to be used for the Imperial Age version of Nomads.
  const size_t nomadsHouseIdsImp[] = {70, 463, 464, 465, 191};

  // Effect ID of a "New Effect" effect.
  const size_t effectIdNew = 798;

  // Tech ID of a "New Research" technology.
  const size_t techIdNew = 801;

  // Tech ID of the Imperial Age tech.
  const size_t techIdAgeImperial = 103;

  // Tech ID for the Mongol Nomads unique tech.
  const size_t techIdNomads = 487;

  // Civilization ID of the Mongols.
  const size_t civIdMongols = 12;

  // The IDs for units that need to have their dead unit ID changed for the Nomads fix.
  const size_t nomadsDeadUnitFixIds[] = {70, 463, 464, 465, 191, 192};

  // The dead unit ID of Rubble 2x2.
  const size_t nomadsDeadUnitId = 144;

  for (auto & civ : aocDat->Civs) {
    auto house1 = civ.Units[unitIdHouseC];
    house1.ResourceStorages[0].Paid = genie::ResourceStoreMode::UpResourceType; // `Paid` is the Store Mode.
    civ.Units[unitIdRubble1] = house1;
    auto house2 = civ.Units[unitIdHouseI];
    house2.ResourceStorages[0].Paid = genie::ResourceStoreMode::UpResourceType; // `Paid` is the Store Mode.
    civ.Units[unitIdRubble2] = house2;
  }

  auto nomadsEffect = &aocDat->Effects[effectIdFalconry];
  nomadsEffect->EffectCommands.clear();
  for (auto id : nomadsHouseIds) {
    auto updateHouse = genie::EffectCommand();
    updateHouse.Type = genie::EffectCommand::EffectType::UpgradeUnit;
    updateHouse.TargetUnit = id;
    updateHouse.UnitClassID = unitIdRubble1;
    nomadsEffect->EffectCommands.push_back(updateHouse);
  }

  auto impNomadsEffect = aocDat->Effects[effectIdNew];
  impNomadsEffect.Name = "Imperial Nomads Effect";
  for (auto id : nomadsHouseIdsImp) {
    auto updateHouse = genie::EffectCommand();
    updateHouse.Type = genie::EffectCommand::EffectType::UpgradeUnit;
    updateHouse.TargetUnit = id;
    updateHouse.UnitClassID = unitIdRubble2;
    impNomadsEffect.EffectCommands.push_back(updateHouse);
  }
  
  auto impNomads = aocDat->Techs[techIdNew];
  impNomads.Name = "Imperial Nomads";
  impNomads.Name2 = "Imperial Nomads";
  impNomads.RequiredTechs = {static_cast<int16_t>(techIdAgeImperial),
                             static_cast<int16_t>(techIdNomads)};
  impNomads.RequiredTechCount = 2;
  impNomads.Civ = civIdMongols;
  impNomads.EffectID = static_cast<int16_t>(aocDat->Effects.size());
  aocDat->Effects.push_back(impNomadsEffect);
  aocDat->Techs.push_back(impNomads);

  for (auto& civ : aocDat->Civs) {
    for (auto id : nomadsDeadUnitFixIds) {
      civ.Units[id].DeadUnitID = nomadsDeadUnitId;
    }
  }
}

DatPatch nomadsFix = {&nomadsFixPatch, "Fixes the nomads tech exploit for DM games (and all game modes)."};

} // namespace wololo
