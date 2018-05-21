#include "malayfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void malayPatch(genie::DatFile *aocDat) {
	/*
	 * In order to work in AoC, research time manipulation has to be part of the tech tree tech
	 */

	size_t const malayTechTreeId = 648;
	size_t const malayEffectTechId = 674;
	size_t const fishtrapResourceId = 88;

	for(int i = 0; i < aocDat->Techages[malayEffectTechId].Effects.size() ; i++) {
		aocDat->Techages[malayTechTreeId].Effects.push_back(aocDat->Techages[malayEffectTechId].Effects[i]);
	}
	genie::TechageEffect effect;
	effect.Type = 1; // ressource modifier
	effect.A = fishtrapResourceId;
	effect.B = 0; // Set
	effect.D = aocDat->Civs[29].Resources[fishtrapResourceId];
	aocDat->Techages[malayTechTreeId].Effects.push_back(effect);
    aocDat->Civs[29].Resources[fishtrapResourceId] = 700;



}

DatPatch malayFix = {
	&malayPatch,
	"Fixing the Malay Age up-bonus"
};

}

