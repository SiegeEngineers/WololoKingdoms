#include "splituniquetechicons.h"
#include "wololo/datPatch.h"

namespace wololo {

// Returns `true` if `tech` is a Castle Age unique technology.
// @param tech the technology to check
bool isUniqueCastleAgeTech(genie::Tech& tech) {
  // Location ID of the castle, for technologies researched there.
  const size_t locationCastle = 82;
  
  // Button ID of the Castle Age unique technologies.
  const size_t buttonIdCastleUt = 7;

  return tech.ResearchLocation == locationCastle &&
         tech.ButtonID == buttonIdCastleUt;
}

// Returns `true` if `tech` is an Imperial Age unique technology.
// @param tech the technology to check
bool isUniqueImperialAgeTech(genie::Tech& tech) {
  // Location ID of the castle, for technologies researched there.
  const size_t locationCastle = 82;

  // Button ID of the Imperial Age unique technologies.
  const int buttonIdImpUt = 8;

  return tech.ResearchLocation == locationCastle &&
         tech.ButtonID == buttonIdImpUt;
}

// Splits up the unique technology icons so that each technology has its own
// icon,
// instead of only two icons for Castle and Imperial Age techs, respectively.
// Must be called before creating the queueable technologies.
// Castle Age techs are given the ID `uniqueTechIdStartCastle + civId`.
// Imperial Age techs are given the ID `TECH_ID_START_IMPERIAL + civId`.
// @param df the dat file to modify
void splitUniqueTechIconsPatch(genie::DatFile* aocDat) {

  // The number of civilizations, not counting the gaia civilization.
  const size_t numCivs = 31;

  // The first ID for Castle Age unique tech icons.
  const size_t uniqueTechIdStartCastle = 121;

  // The first ID for Imperial Age unique tech icons.
  const size_t uniqueTechIdStartImperial = uniqueTechIdStartCastle + numCivs;

  for (auto& tech : aocDat->Techs) {
    if (isUniqueCastleAgeTech(tech)) {
      tech.IconID = uniqueTechIdStartCastle + tech.Civ;
    } else if (isUniqueImperialAgeTech(tech)) {
      tech.IconID = uniqueTechIdStartImperial + tech.Civ;
    }
  }
}

DatPatch splitUniqueTechIcons = {
  &splitUniqueTechIconsPatch,
  "Creates a separate icon for each unique technology."
};

} // namespace wololo
