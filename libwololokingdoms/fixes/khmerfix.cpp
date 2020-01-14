#include "khmerfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void khmerPatch(genie::DatFile* aocDat) {

  // Civ Bonus Fix
  size_t const khmerBonusTechId = 693;
  size_t const ballistaElephantId = 1120;
  size_t const eliteBallistaElephantId = 1122;
  size_t const ballistaElephantBoltId = 1167;
  size_t const petardId = 440;
  size_t const treePetardId = 1225;
  size_t const newElephantId = 946;
  size_t const chemistryId = 47;
  size_t const siegeEngineersId = 375;
  size_t const doubleCrossbowId = 663;
  size_t const eliteBallistaTechId = 655;
  size_t const khmerBuildingResearchId = 638;
  size_t const buildingResearchs[] = {216, 659, 660, 661, 666, 667, 668};

  genie::EffectCommand effect;

  // imperial age
  std::vector<genie::EffectCommand>* effectsPtr =
      &aocDat->Effects[khmerBonusTechId].EffectCommands;
  std::vector<genie::EffectCommand> effectsToAdd;
  for (std::vector<genie::EffectCommand>::iterator it = effectsPtr->begin(),
                                                   end = effectsPtr->end();
       it != end; it++) {
    // add an attribute to disable drop-off
    effect = *it;
    effect.Type = 0;
    effect.AttributeID = 31; // Drop-off of resources
    effect.Amount = 4.0f;
    effectsToAdd.push_back(effect);
  }
  effectsPtr->insert(effectsPtr->end(), effectsToAdd.begin(),
                     effectsToAdd.end());

  // We create a seperate Castle Age Ballista Elephant that can't cut trees, for
  // use in CtR maps
  aocDat->UnitHeaders[newElephantId] = aocDat->UnitHeaders[ballistaElephantId];
  aocDat->UnitHeaders[newElephantId].TaskList.pop_back();
  aocDat->UnitHeaders[treePetardId] = aocDat->UnitHeaders[petardId];
  aocDat->UnitHeaders[treePetardId].TaskList.pop_back();
  aocDat->UnitHeaders[treePetardId].TaskList.pop_back();
  aocDat->UnitHeaders[treePetardId].TaskList.pop_back();

  aocDat->Civs[0].Units[treePetardId] = aocDat->Civs[0].Units[petardId];
  aocDat->Civs[0].Units[treePetardId].ID = treePetardId;
  aocDat->Civs[0].Units[treePetardId].CopyID = treePetardId;
  aocDat->Civs[0].Units[treePetardId].BaseID = treePetardId;
  aocDat->Civs[0].Units[treePetardId].HitPoints = 0;
  aocDat->Civs[0].Units[treePetardId].Combat.BlastAttackLevel = 1;
  aocDat->Civs[0].Units[treePetardId].StandingGraphic = {-1, -1};
  aocDat->Civs[0].Units[treePetardId].Moving.WalkingGraphic = -1;
  aocDat->Civs[0].Units[treePetardId].DyingGraphic = -1;
  aocDat->Civs[0].Units[treePetardId].LanguageDLLName = 0;
  aocDat->Civs[0].Units[treePetardId].Name = "TREEFELLER";
  aocDat->Civs[0].Units[treePetardId].DyingGraphic = -1;
  aocDat->Civs[0].Units[treePetardId].Creatable.TrainLocationID = -1;
  aocDat->Civs[0].Units[treePetardId].Creatable.HeroMode = 32;
  aocDat->Civs[0].Units[treePetardId].HideInEditor = 1;
  aocDat->Civs[0].Units[treePetardId].DyingSound = -1;
  aocDat->Civs[0].Units[treePetardId].Action.AttackSound = -1;
  aocDat->Civs[0].Units[treePetardId].Action.MoveSound = -1;
  aocDat->Civs[0].Units[treePetardId].Combat.AttackGraphic = -1;
  aocDat->Civs[0].Units[treePetardId].Combat.BlastWidth = 0.1;
  aocDat->Civs[0].Units[treePetardId].Combat.Armours.clear();
  aocDat->Civs[0].Units[treePetardId].Combat.Attacks.resize(1);
  aocDat->Civs[0].Units[treePetardId].Combat.Attacks[0].Class = genie::unit::AttackOrArmor::Trees;
  aocDat->Civs[0].Units[treePetardId].Combat.Attacks[0].Amount = 100;    
	
  for (size_t i = 0; i < aocDat->Civs.size(); i++) {
    aocDat->Civs[i].Units[treePetardId] = aocDat->Civs[0].Units[treePetardId];
    aocDat->Civs[i].Units[ballistaElephantBoltId].DeadUnitID = treePetardId;
    
    aocDat->Civs[i].Units[ballistaElephantId].Combat.BlastWidth = 0;
    aocDat->Civs[i].Units[eliteBallistaElephantId].Combat.BlastWidth = 0;

    aocDat->Civs[i].Units[newElephantId] =
        aocDat->Civs[i].Units[ballistaElephantId];
    aocDat->Civs[i].Units[newElephantId].Combat.Attacks.erase(
        aocDat->Civs[i].Units[newElephantId].Combat.Attacks.begin() + 4);

    aocDat->Civs[i].Units[newElephantId].Combat.BlastAttackLevel = 2;
    aocDat->Civs[i].Units[newElephantId].LanguageDLLName = 5654;
    aocDat->Civs[i].Units[newElephantId].LanguageDLLCreation = 6654;
    aocDat->Civs[i].Units[newElephantId].LanguageDLLHelp = 105654;
  }
  aocDat->Effects[doubleCrossbowId].EffectCommands.push_back(
      aocDat->Effects[doubleCrossbowId].EffectCommands[0]);
  aocDat->Effects[doubleCrossbowId].EffectCommands.push_back(
      aocDat->Effects[doubleCrossbowId].EffectCommands[4]);
  aocDat->Effects[doubleCrossbowId].EffectCommands[8].TargetUnit =
      newElephantId;
  aocDat->Effects[doubleCrossbowId].EffectCommands[9].TargetUnit =
      newElephantId;

  aocDat->Effects[chemistryId].EffectCommands.push_back(
      aocDat->Effects[chemistryId].EffectCommands[66]);
  aocDat->Effects[chemistryId].EffectCommands[68].TargetUnit = newElephantId;

  aocDat->Effects[eliteBallistaTechId].EffectCommands.push_back(
      aocDat->Effects[eliteBallistaTechId].EffectCommands[0]);
  aocDat->Effects[eliteBallistaTechId].EffectCommands[1].TargetUnit =
      newElephantId;


  for (size_t i = 24; i < 28; i++) {
    aocDat->Effects[siegeEngineersId].EffectCommands.push_back(
        aocDat->Effects[siegeEngineersId].EffectCommands[i]);
    aocDat->Effects[siegeEngineersId].EffectCommands[i + 4].TargetUnit =
        newElephantId;
  }

  aocDat->Techs[khmerBuildingResearchId].Civ = 28;
  aocDat->Techs[khmerBuildingResearchId].Name = "Khmer Building Bonus";
  aocDat->Techs[khmerBuildingResearchId].RequiredTechCount = 0;
  aocDat->Techs[khmerBuildingResearchId].RequiredTechs[0] = -1;
  aocDat->Techs[khmerBuildingResearchId].RequiredTechs[1] = -1;

  for (size_t i = 0;
       i < sizeof(buildingResearchs) / sizeof(buildingResearchs[0]); i++) {
    aocDat->Techs[buildingResearchs[i]].RequiredTechs[1] =
        khmerBuildingResearchId;
  }
}

DatPatch khmerFix = {&khmerPatch,
                     "Villagers dropping resources in Khmer houses fix"};

} // namespace wololo
