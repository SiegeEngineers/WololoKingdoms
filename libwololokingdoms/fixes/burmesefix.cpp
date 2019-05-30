#include "burmesefix.h"
#include "wololo/datPatch.h"

namespace wololo {

void burmesePatch(genie::DatFile* aocDat) {

  // It doesn't work as a TB tech, has to be a normal tech with team effect

  size_t const burmeseTeamBonusTechId = 651;
  size_t const relicTechID = 803;
  size_t const relicResearchID = 803;

  if (aocDat->Techs.size() < relicResearchID + 1)
    aocDat->Techs.resize(relicResearchID + 1);
  aocDat->Techs[relicResearchID].EffectID = relicTechID;
  aocDat->Techs[relicResearchID].Civ = 30;
  aocDat->Techs[relicResearchID].Name = "Enable Relic LoS";

  if (aocDat->Effects.size() < relicTechID + 1)
    aocDat->Effects.resize(relicTechID + 1);

  genie::EffectCommand effect =
      aocDat->Effects[burmeseTeamBonusTechId].EffectCommands[0];
  effect.Type = 11;
  aocDat->Effects[relicTechID].EffectCommands.push_back(effect);

  aocDat->Civs[30].Resources[210] = 0;

  /*
   * Free upgrades have to be part of the tech tree effect in AoC
   */

  size_t const burmeseLcTech = 685;
  size_t const burmeseMonasteryTech = 689;
  size_t const burmeseTechTree = 650;

  for (size_t i = 0; i < aocDat->Effects[burmeseLcTech].EffectCommands.size();
       i++) {
    aocDat->Effects[burmeseTechTree].EffectCommands.push_back(
        aocDat->Effects[burmeseLcTech].EffectCommands[i]);
  }

  for (size_t i = 0;
       i < aocDat->Effects[burmeseMonasteryTech].EffectCommands.size(); i++) {
    aocDat->Effects[burmeseTechTree].EffectCommands.push_back(
        aocDat->Effects[burmeseMonasteryTech].EffectCommands[i]);
  }
}

DatPatch burmeseFix = {
    &burmesePatch,
    "Burmese relic LoS Team bonus and Lumbercamp/Monastery upgrades fix"};

} // namespace wololo
