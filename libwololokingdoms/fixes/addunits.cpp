#include "addunits.h"
#include "wololo/datPatch.h"

namespace wololo {

// Adds a Roaming Wolf unit to `df`.
// @param df the dat file to modify
void addRoamingWolf(genie::DatFile * df) {
  // Civilization ID of the gaia civilization.
  const size_t civIdGaia = 0;

  // Unit ID of the wolf.
  const size_t unitIdWolf = 126;

  // Language file entry for the roaming wolf.
  const size_t langFileNameRoamingWolf = 5055;

  // ID for graphics for the roaming wolf.
  const size_t graphicsIdWolfxWn = 1392;

  auto unit = df->Civs[civIdGaia].Units[unitIdWolf];
  unit.LanguageDLLName = langFileNameRoamingWolf;

  auto headerId = df->UnitHeaders.size();
  df->UnitHeaders.push_back(df->UnitHeaders[unitIdWolf]);
  auto taskList = &df->UnitHeaders[headerId].TaskList;
  auto taskFly = genie::Task();
  taskFly.IsDefault = true;
  taskFly.ActionType = genie::Task::ActionTypes::Fly;
  taskFly.MovingGraphicID = graphicsIdWolfxWn;
  taskFly.ProceedingGraphicID = graphicsIdWolfxWn;
  taskFly.WorkingGraphicID = graphicsIdWolfxWn;

  // The new task must be at position 0 in the task list.
  auto taskId = 0;
  unit.Action.DefaultTaskID = taskId;
  taskList->insert(taskList->begin(), taskFly);

  for (auto& civ : df->Civs) {
    civ.Units.push_back(unit);
    civ.UnitPointers.push_back(1);
  }
}

// Adds a Transport Sharkatzor unit to `df`.
// @param df the dat file to modify
void addTransportSharkatzor(genie::DatFile * df) {
  // Civilization ID of the gaia civilization.
  const size_t civIdGaia = 0;

  // Unit ID of the Sharkatzor;
  const size_t unitIdSharkatzor = 1222;

  // Resource capacity for the custom Transport Sharkatzor unit.
  const size_t sharkatzorTransportResourceCapacity = 20;

  // Garrison capacity for the custom Transport Sharkatzor unit.
  const size_t sharkatzorTransportGarrisonCapacity = 10;

  auto unit = df->Civs[civIdGaia].Units[unitIdSharkatzor];
  unit.Class = genie::Unit::UnitClass::TransportBoat;
  unit.ResourceCapacity = sharkatzorTransportResourceCapacity;
  unit.GarrisonCapacity = sharkatzorTransportGarrisonCapacity;
  unit.InterfaceKind = genie::Unit::InterfaceKinds::TransportShipInterface;
  unit.Trait = genie::Unit::Traits::GarrisonableTrait;

  auto headerId = df->UnitHeaders.size();
  df->UnitHeaders.push_back(df->UnitHeaders[unitIdSharkatzor]);
  auto taskList = &df->UnitHeaders[headerId].TaskList;

  // Removes the tasks at indices 0 and 2.
  taskList->erase(taskList->begin());
  taskList->erase(taskList->begin() + 1);

  auto taskUnload = genie::Task();
  taskUnload.ActionType = genie::Task::ActionTypes::UnloadBoat;
  taskList->push_back(taskUnload);

  for (auto& civ : df->Civs) {
    civ.Units.push_back(unit);
    civ.UnitPointers.push_back(1);
  }
}

// Adds a Unit Spawner unit to `df`.
// @param df the dat file to modify
void addUnitSpawner(genie::DatFile * df) {
  // Civilization ID of the gaia civilization.
  const size_t civIdGaia = 0;

  // Unit ID for the Llama building.
  const size_t unitIdLlamaBuilding = 888;

  auto unitA = df->Civs[civIdGaia].Units[unitIdLlamaBuilding];
  unitA.DeadUnitID = -1;
  unitA.Building.TechID = -1; // Sets "Initiates Technology".
  auto unitIdA = df->Civs[civIdGaia].Units.size();

  auto unitB = unitA;
  auto unitIdB = unitIdA + 1;
  unitA.Building.StackUnitID = unitIdB;

  auto unitC = unitA;
  unitC.Building.HeadUnit = unitIdA;
  auto annexC = genie::unit::BuildingAnnex();
  annexC.UnitID = unitIdA + 2;
  annexC.Misplacement = {0.5f, -0.5f};

  auto unitD = unitA;
  unitD.Building.HeadUnit = unitIdA;
  auto annexD = genie::unit::BuildingAnnex();
  annexD.UnitID = unitIdA + 3;
  annexD.Misplacement = {-0.5f, 0.5f};

  auto unitE = unitA;
  unitE.Building.HeadUnit = unitIdA;
  auto annexE = genie::unit::BuildingAnnex();
  annexE.UnitID = unitIdA + 4;
  annexE.Misplacement = {0.5f, 0.5f};

  auto unitF = unitA;
  unitF.Building.HeadUnit = unitIdA;
  auto annexF = genie::unit::BuildingAnnex();
  annexF.UnitID = unitIdA + 5;
  annexF.Misplacement = {-0.5f, -0.5f};

  std::vector<genie::unit::BuildingAnnex> annexes = {annexC, annexD, annexE,
                                                     annexF};
  unitA.Building.Annexes = annexes;

  auto units = {unitA, unitB, unitC, unitD, unitE, unitF};
  for (auto unit : units) {
    df->UnitHeaders.push_back(df->UnitHeaders[unitIdLlamaBuilding]);
    for (auto& civ : df->Civs) {
      civ.Units.push_back(unit);
      civ.UnitPointers.push_back(1);
    }
  }
}

// Adds several new units:
// * Roaming Wolf
// * Transport Sharkatzor
// * Unit Spawner
void addUnitsPatch(genie::DatFile* aocDat) {
  addRoamingWolf(aocDat);
  addTransportSharkatzor(aocDat);
  addUnitSpawner(aocDat);
}

DatPatch addUnits = {&addUnitsPatch, "Adds various additional units."};

} // namespace wololo
