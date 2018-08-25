#include "malayfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void malayPatch(genie::DatFile *aocDat) {
	/*
	 * In order to work in AoC, research time manipulation has to be part of the tech tree tech
	 */

    size_t const malayTechTreeId = 648;
    size_t const fishtrapResourceId = 88;
    size_t const malayEffectTechId = 674;
    size_t const fishtrapTechId = 677;

    for(int i = 0; i < aocDat->Techages[malayEffectTechId].Effects.size() ; i++) {
        aocDat->Techages[malayTechTreeId].Effects.push_back(aocDat->Techages[malayEffectTechId].Effects[i]);
    }
    if(aocDat->Techages[fishtrapTechId].Effects.size() == 0 || aocDat->Techages[fishtrapTechId].Effects[0].A != 88) {
        //Fish trap effect not fixed on HD yet
        genie::TechageEffect effect;
        effect.Type = 1; // ressource modifier
        effect.A = fishtrapResourceId;
        effect.B = 0; // Set
        effect.D = aocDat->Civs[29].Resources[fishtrapResourceId] > 715 ? aocDat->Civs[29].Resources[fishtrapResourceId] : 70000000000000; //7e+13...
        aocDat->Techages[malayTechTreeId].Effects.push_back(effect);
        aocDat->Civs[29].Resources[fishtrapResourceId] = 700;
    }

}

DatPatch malayFix = {
	&malayPatch,
	"Fixing the Malay Age up-bonus"
};

}

