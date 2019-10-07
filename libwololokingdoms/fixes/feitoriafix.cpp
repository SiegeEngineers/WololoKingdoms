#include "feitoriafix.h"
#include "wololo/datPatch.h"

namespace wololo {

void feitoriaPatch(genie::DatFile* aocDat) {
  float const feitoriaCooldown = 1;
  float const woodAdded = 0.8;
  float const foodAdded = 0.8;
  float const goldAdded = 0.45;
  float const stoneAdded = 0.25;

  int16_t const feitoriaId = 1021;          // 960
  int16_t const woodAnnexId = 1391;         // 957
  int16_t const foodAnnexId = 1392;         // 956
  int16_t const goldAnnexId = 1393;         // 959
  int16_t const stoneAnnexId = 1394;        // 958
  int16_t const woodTrickleId = 1395;       // 952
  int16_t const foodTrickleId = 1396;       // 951
  int16_t const goldTrickleId = 1397;       // 954
  int16_t const stoneTrickleId = 1398;      // 954
  int16_t const feitoriaDeadStackId = 1399; // 955
  int16_t const feitoriaStackId = 1400;     // 949

  for (auto& Civ : aocDat->Civs) {
    Civ.Units[feitoriaStackId] = Civ.Units[feitoriaId];
    Civ.Units[woodAnnexId] = Civ.Units[feitoriaId];
    Civ.Units[foodAnnexId] = Civ.Units[feitoriaId];
    Civ.Units[goldAnnexId] = Civ.Units[feitoriaId];
    Civ.Units[stoneAnnexId] = Civ.Units[feitoriaId];
    Civ.Units[woodTrickleId] = Civ.Units[feitoriaId];
    Civ.Units[foodTrickleId] = Civ.Units[feitoriaId];
    Civ.Units[goldTrickleId] = Civ.Units[feitoriaId];
    Civ.Units[stoneTrickleId] = Civ.Units[feitoriaId];

    Civ.Units[feitoriaStackId].Name = "Feitoria";
    Civ.Units[woodAnnexId].Name = "AA-A Wood Annex";
    Civ.Units[foodAnnexId].Name = "AA-A Food Annex";
    Civ.Units[goldAnnexId].Name = "AA-A Stone Annex";
    Civ.Units[stoneAnnexId].Name = "AA-A gold Annex";
    Civ.Units[woodTrickleId].Name = "Wood Trickle";
    Civ.Units[foodTrickleId].Name = "Food Trickle";
    Civ.Units[goldTrickleId].Name = "Gold Trickle";
    Civ.Units[stoneTrickleId].Name = "Stone Trickle";

    Civ.Units[feitoriaId].Building.StackUnitID = feitoriaStackId;
    Civ.Units[feitoriaId].Building.DisappearsWhenBuilt = 1;
    Civ.Units[feitoriaId].HideInEditor = 1;

    Civ.Units[feitoriaStackId].DyingGraphic = -1;
    Civ.Units[feitoriaStackId].UndeadGraphic = -1;
    Civ.Units[feitoriaStackId].DeadUnitID = feitoriaDeadStackId;
    Civ.Units[feitoriaStackId].OcclusionMode = 6;
    Civ.Units[feitoriaStackId].Creatable.TrainLocationID = 0;
    Civ.Units[feitoriaStackId].ResourceStorages[2].Type = -1;
    Civ.Units[feitoriaStackId].ResourceStorages[2].Amount = 0;
    Civ.Units[feitoriaStackId].ResourceStorages[2].Paid = 0;
    Civ.Units[feitoriaStackId].Building.HeadUnit = feitoriaId;
    Civ.Units[feitoriaStackId].Building.Annexes[0].UnitID = woodAnnexId;
    Civ.Units[feitoriaStackId].Building.Annexes[1].UnitID = foodAnnexId;
    Civ.Units[feitoriaStackId].Building.Annexes[2].UnitID = goldAnnexId;
    Civ.Units[feitoriaStackId].Building.Annexes[3].UnitID = stoneAnnexId;
    for (int i = 0; i < 4; i++) {
      Civ.Units[feitoriaStackId].Building.Annexes[i].Misplacement = {0.5, 0.5};
    }

    // dead unit
    Civ.Units[feitoriaDeadStackId].Type = 80;
    Civ.Units[feitoriaDeadStackId].Class = 3;
    Civ.Units[feitoriaDeadStackId].DyingGraphic = 42;
    Civ.Units[feitoriaDeadStackId].UndeadGraphic = -1;
    Civ.Units[feitoriaDeadStackId].HitPoints = 0;
    Civ.Units[feitoriaDeadStackId].ResourceDecay = 1;
    Civ.Units[feitoriaDeadStackId].Enabled = 0;
    Civ.Units[feitoriaDeadStackId].HideInEditor = 1;
    Civ.Units[feitoriaDeadStackId].DeadUnitID = 148; // Rubble 8 x 8

    // CRASH -> INVALID NAMES for new units ?

    // annex
    genie::Unit annex;
    annex.Type = 80;
    annex.Class = 3;
    annex.HitPoints = -1;
    annex.HideInEditor = 1;
    // wood
    annex.DeadUnitID = woodTrickleId;
    Civ.Units[woodAnnexId] = annex;
    // food
    annex.DeadUnitID = foodTrickleId;
    Civ.Units[foodAnnexId] = annex;
    // gold
    annex.DeadUnitID = goldTrickleId;
    Civ.Units[goldAnnexId] = annex;
    // stone
    annex.DeadUnitID = stoneTrickleId;
    Civ.Units[stoneAnnexId] = annex;

    // trickle
    genie::Unit trickle;
    trickle.Type = 30;
    trickle.Class = 11;
    trickle.HitPoints = 0;
    trickle.ResourceDecay = 1;
    trickle.CanBeBuiltOn = 1;
    trickle.HideInEditor = 1;
    trickle.ResourceStorages[0].Type = 12; // Corpse Decay Time
    trickle.ResourceStorages[0].Amount =
        feitoriaCooldown; // time it takes to generate ressources again
    trickle.ResourceStorages[0].Paid = 0; // Decayable ressource
    trickle.ResourceStorages[1].Paid = 1; // Stored after death also
    // wood
    trickle.ResourceStorages[1].Type = 0; // Wood storage
    trickle.ResourceStorages[1].Amount = woodAdded;
    trickle.DeadUnitID = woodTrickleId;
    Civ.Units[woodTrickleId] = trickle;
    // food
    trickle.ResourceStorages[1].Type = 1; // Food storage
    trickle.ResourceStorages[1].Amount = foodAdded;
    trickle.DeadUnitID = foodTrickleId;
    Civ.Units[foodTrickleId] = trickle;
    // gold
    trickle.ResourceStorages[1].Type = 3; // Gold storage
    trickle.ResourceStorages[1].Amount = goldAdded;
    trickle.DeadUnitID = goldTrickleId;
    Civ.Units[goldTrickleId] = trickle;
    // stone
    trickle.ResourceStorages[1].Type = 2; // Stone storage
    trickle.ResourceStorages[1].Amount = stoneAdded;
    trickle.DeadUnitID = stoneTrickleId;
    Civ.Units[stoneTrickleId] = trickle;

    // Fix IDs
    Civ.Units[feitoriaStackId].ID = feitoriaStackId;
    Civ.Units[woodAnnexId].ID = woodAnnexId;
    Civ.Units[foodAnnexId].ID = foodAnnexId;
    Civ.Units[goldAnnexId].ID = goldAnnexId;
    Civ.Units[stoneAnnexId].ID = stoneAnnexId;
    Civ.Units[woodTrickleId].ID = woodTrickleId;
    Civ.Units[foodTrickleId].ID = foodTrickleId;
    Civ.Units[goldTrickleId].ID = goldTrickleId;
    Civ.Units[stoneTrickleId].ID = stoneTrickleId;

    Civ.Units[feitoriaStackId].CopyID = feitoriaStackId;
    Civ.Units[woodAnnexId].CopyID = woodAnnexId;
    Civ.Units[foodAnnexId].CopyID = foodAnnexId;
    Civ.Units[goldAnnexId].CopyID = goldAnnexId;
    Civ.Units[stoneAnnexId].CopyID = stoneAnnexId;
    Civ.Units[woodTrickleId].CopyID = woodTrickleId;
    Civ.Units[foodTrickleId].CopyID = foodTrickleId;
    Civ.Units[goldTrickleId].CopyID = goldTrickleId;
    Civ.Units[stoneTrickleId].CopyID = stoneTrickleId;

    Civ.Units[feitoriaStackId].BaseID = feitoriaStackId;
    Civ.Units[woodAnnexId].BaseID = woodAnnexId;
    Civ.Units[foodAnnexId].BaseID = foodAnnexId;
    Civ.Units[goldAnnexId].BaseID = goldAnnexId;
    Civ.Units[stoneAnnexId].BaseID = stoneAnnexId;
    Civ.Units[woodTrickleId].BaseID = woodTrickleId;
    Civ.Units[foodTrickleId].BaseID = foodTrickleId;
    Civ.Units[goldTrickleId].BaseID = goldTrickleId;
    Civ.Units[stoneTrickleId].BaseID = stoneTrickleId;
  }
}

DatPatch feitoriaFix = {&feitoriaPatch, "Feitoria fix"};

} // namespace wololo
