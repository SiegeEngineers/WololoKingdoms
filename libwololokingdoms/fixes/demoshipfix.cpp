#include "demoshipfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void demoshipPatch(genie::DatFile* aocDat) {
  /*
   * There are a few hardcoded detonation units, UP 1.5 offers a way to do it
   * without. However, it does full damage in the entire blast radius.
   */

  size_t const demoShipUnitId = 527;
  
  for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
	// Hero mode for the demo ship must be 104 so that it's attack stance defaults to Defensive
	// when a Demolition ship is placed in the scenario editor.
    aocDat->Civs[civIndex].Units[demoShipUnitId].Creatable.HeroMode = 104;
  }

}

DatPatch demoShipFix = {&demoshipPatch, "Demolition ships not exploding fix"};

} // namespace wololo
