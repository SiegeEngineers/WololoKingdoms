#include "malayfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void malayPatch(genie::DatFile *aocDat) {
	/*
	 * In order to work in AoC, research time manipulation has to be part of the tech tree tech
	 */

	size_t const malayTechTreeId = 648;
    size_t const malayEffectTechId = 674;

	for(int i = 0; i < aocDat->Techages[malayEffectTechId].Effects.size() ; i++) {
		aocDat->Techages[malayTechTreeId].Effects.push_back(aocDat->Techages[malayEffectTechId].Effects[i]);
	}


}

DatPatch malayFix = {
	&malayPatch,
	"Fixing the Malay Age up-bonus"
};

}

