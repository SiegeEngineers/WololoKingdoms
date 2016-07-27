#include "portuguesefix.h"
#include "wololo/datPatch.h"

namespace wololo {

void PortuguesePatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {
	/*
	 * The imported Portuguese team bonus makes the game crash
	 * so let's replace it by something simpler (and functionally identical ?)
	 */
	size_t const portugueseTBonusTechId = 32; // empty by default
	size_t const portugueseBrokenTBonusTechId = 626; // why is this even used ?

	std::vector<genie::TechageEffect> effects;
	genie::TechageEffect effect;
	effect.Type = 1; // ressource modifier
	effect.A = 50; // reveal ally
	effect.B = 1; // (+/-)
	effect.D = 1; // +1
	effects.push_back(effect);

	// broken : prevents from researching caravan
//	effect.Type = 102; // disable research
//	effect.D = 19; // cartography
//	effects.push_back(effect);

	aocDat->Techages[portugueseTBonusTechId].Effects = effects;
	aocDat->Techages[portugueseBrokenTBonusTechId].Effects = std::vector<genie::TechageEffect>(); // nothing
}

DatPatch portugueseFix = {
	&PortuguesePatch,
	"Portuguese civ crash fix"
};

}

