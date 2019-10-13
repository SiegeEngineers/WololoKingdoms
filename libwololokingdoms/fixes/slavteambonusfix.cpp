#include "slavteambonusfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void slavTeamBonusFixPatch(genie::DatFile* aocDat) {


  // Effect ID of the team bonus for the Slavs.
  const size_t effectIdTeamBonusSlavs = 9;
  
  // Tech ID of the Dark Age tech.
  const size_t techIdAgeDark = 104;

  // Civilization ID of the Slavs.
  const size_t civIdSlavs = 23;

  // Tech ID of a "New Research" technology.
  const size_t techIdNew = 801;


  // Duplicates Effect 9 (Slav Bonus).
  auto slavFixEffect = aocDat->Effects[effectIdTeamBonusSlavs];

  slavFixEffect.Name = "Slav Fix Effect";
  // Note 10 is a custom value, not a pre-defined constant.
  for (auto& ec : slavFixEffect.EffectCommands) { ec.Type = 10; }

  auto slavFixTech = aocDat->Techs[techIdNew];
  slavFixTech.Name = "Slav Fix";
  slavFixTech.Name2 = "Slav Fix";
  slavFixTech.RequiredTechs = {static_cast<int16_t>(techIdAgeDark)};
  slavFixTech.RequiredTechCount = 1;
  slavFixTech.Civ = civIdSlavs;
  slavFixTech.EffectID = static_cast<int16_t>(aocDat->Effects.size());
  aocDat->Effects.push_back(slavFixEffect);
  aocDat->Techs.push_back(slavFixTech);

  // Clears all Effect Commands from the Slav Team Bonus.
  aocDat->Effects[effectIdTeamBonusSlavs].EffectCommands.clear();
}

DatPatch slavTeamBonusFix = {
  &slavTeamBonusFixPatch,
  "Apply Slav team bonus properly when maps start with military buildings."
};

} // namespace wololo
