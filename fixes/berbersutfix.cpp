#include "berbersutfix.h"
#include "wololo/fix.h"

namespace wololo {

void berbersUTPatch(genie::DatFile *aocDat) {
	/*
	 * None of the Berbers team bonuses work, and there's no real way around it without modifying the .exe
	 * So we have to make them a bit different, it will require playtesting to see if it's balanced
	 *
	 * The caste unique tech becomes an additionnal team bonus
	 * To compensate for the fact you don't even have to go castle and research it, it's effect is reduced from 25% to 15%
	 * For the imperial one, the regen. is replaced by +1/+2 armor
	 */

	size_t const berbersTBonusTechId = 38;
	size_t const berbersUT1TechId = 607;
	size_t const berbersUT2TechId = 608;
	size_t const berbersUT1ResearchId = 578;

	genie::TechageEffect effect;

	// castle age
	effect.Type = 5; // attribute modifier (multiply)
	effect.A = 82; // castle
	effect.C = 13; // working rate
	effect.D = 1.15; // amount
	aocDat->Techages[berbersTBonusTechId].Effects.push_back(effect);
	aocDat->Techages[berbersUT1TechId].Effects = std::vector<genie::TechageEffect>(); // nothing
	aocDat->Researchs[berbersUT1ResearchId].ResearchLocation = -1;

	// imperial age
	std::vector<genie::TechageEffect> *effectsPtr = &aocDat->Techages[berbersUT2TechId].Effects;
	std::vector<genie::TechageEffect> effectsToAdd;
	for (std::vector<genie::TechageEffect>::iterator it = effectsPtr->begin(), end = effectsPtr->end(); it != end; it++) {
		// replace the existing regen bonus by +1 melee armor
		it->C = 8; // armor
		int armorQuantity = 1;
		int armorType = 4; // melee;
		it->D = (armorType << 8) + armorQuantity;

		// now add a new attribute
		effect = *it;
		armorQuantity = 2;
		armorType = 3; // pierce
		effect.D = (armorType << 8) + armorQuantity;
		effectsToAdd.push_back(effect);
	}
	effectsPtr->insert(effectsPtr->end(), effectsToAdd.begin(), effectsToAdd.end());
}

Fix berbersUTFix = {
	&berbersUTPatch,
	"Berbers unique technologies alternative"
};

}

