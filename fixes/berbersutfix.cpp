#include "berbersutfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void berbersUTPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {
	/*
	 * None of the Berbers team bonuses work, and there's no real way around it without modifying the .exe
	 * So we have to make them a bit different, it will require playtesting to see if it's balanced
	 *
	 * The caste unique tech becomes an additionnal team bonus
	 * To compensate for the fact you don't even have to go castle and research it, it's effect is reduced from 25% to 15%
	 * For the imperial one, the regen. is replaced by +1 melee armor and +5 hp
	 */

	(*langReplacement)[20176] = "Cavalry and Naval civilization \\n\\n· Villagers move +10% faster \\n· Stable units cost -20% (starting from Castle Age)\\n· Ships move +10% faster \\n\\n<b>Unique Units:<b> Camel Archer (cavalry archer), Genitour (mounted skirmisher)\\n\\n<b>Unique Techs:<b> Kasbah (Castles work +25% faster); Maghrabi Camels (Camel troops +1 melee armor, +5hp)\\n\\n<b>Team Bonus:<b> Genitour available in Archery Range, Kasbah available in Castle";
	(*langReplacement)[28256] = "Research <b>Kasbah<b> (<cost>) \\nCastles work +25% faster.";
    (*langReplacement)[28257] = "Research <b>Maghrabi Camels<b> (<cost>) \\nCamel troops +1 Melee Armor, +5 HP.";

	size_t const berbersTBonusTechId = 38;
    size_t const berbersUT1TechId = 607;
	size_t const berbersUT2TechId = 608;
	size_t const berbersUT1ResearchId = 578;
	size_t const allCivKasbahResearchId = 799; // NEW
	size_t const allCivKasbahDisableResearchId = 800; // NEW
	size_t const allCivGenitourDisableResearchId = 606;
	size_t const allCivKasbahDisableTechId = 800; // NEW
	size_t const allCivGenitourDisableTechId = 632;
	size_t const berbersTechTreeTechId = 37;

	genie::TechageEffect effect;

	// castle age
	// decrease Kasbah cost from 250/250 to 150/150
	aocDat->Researchs[berbersUT1ResearchId].ResourceCosts[0].Amount -= 100;
	aocDat->Researchs[berbersUT1ResearchId].ResourceCosts[1].Amount -= 100;
    //change the Kasbah tech to multiply instead of team multipy
    aocDat->Techages[berbersUT1TechId].Effects[0].Type = 5;

    if(aocDat->Researchs.size() < allCivKasbahDisableResearchId+1)
        aocDat->Researchs.resize(allCivKasbahDisableResearchId+1);
	// copy Kasbah research to the all-civ one
	aocDat->Researchs[allCivKasbahResearchId] = aocDat->Researchs[berbersUT1ResearchId];
	aocDat->Researchs[allCivKasbahResearchId].ButtonID = 4; // next to petard
	aocDat->Researchs[allCivKasbahResearchId].Civ = -1;
    aocDat->Researchs[allCivKasbahResearchId].Name = "all-civs Kasbah";

    if(aocDat->Techages.size() < allCivKasbahDisableTechId+1)
        aocDat->Techages.resize(allCivKasbahDisableTechId+1);
	// tech that disables the all civ Kasbah
	aocDat->Techages[allCivKasbahDisableTechId] = aocDat->Techages[allCivGenitourDisableTechId];
	aocDat->Techages[allCivKasbahDisableTechId].Effects[0].D = allCivKasbahResearchId;
	aocDat->Techages[allCivKasbahDisableTechId].Name = "all-civs Kasbah (disable)";
	// research that disables the all civ Kasbah
	aocDat->Researchs[allCivKasbahDisableResearchId] = aocDat->Researchs[allCivGenitourDisableResearchId];
	aocDat->Researchs[allCivKasbahDisableResearchId].TechageID = allCivKasbahDisableTechId;
	aocDat->Researchs[allCivKasbahDisableResearchId].Name = "all-civs Kasbah (disable)";
	// avoid Kasbah being researched twice for the Berbers
	aocDat->Techages[berbersTechTreeTechId].Effects.push_back(aocDat->Techages[allCivKasbahDisableTechId].Effects[0]);
	// enable Kasbah for Berbers teammates
	effect = aocDat->Techages[berbersTBonusTechId].Effects[1]; // disable the disabling research (for genitours)
	effect.D = allCivKasbahDisableResearchId;
	aocDat->Techages[berbersTBonusTechId].Effects.push_back(effect);
//	effect.Type = 5; // attribute modifier (multiply)
//	effect.A = 82; // castle
//	effect.C = 13; // working rate
//	effect.D = 1.15; // amount
//	aocDat->Techages[berbersTBonusTechId].Effects.push_back(effect);
//	aocDat->Techages[berbersUT1TechId].Effects = std::vector<genie::TechageEffect>(); // nothing
//	aocDat->Researchs[berbersUT1ResearchId].ResearchLocation = -1;
	// new research for all civs




	// imperial age
	std::vector<genie::TechageEffect> *effectsPtr = &aocDat->Techages[berbersUT2TechId].Effects;
	std::vector<genie::TechageEffect> effectsToAdd;
	for (std::vector<genie::TechageEffect>::iterator it = effectsPtr->begin(), end = effectsPtr->end(); it != end; it++) {
		// replace the existing regen bonus by +1 melee armor
		it->C = 8; // armor
		int armorQuantity = 1;
		int armorType = 4; // melee;
		it->D = (armorType << 8) + armorQuantity;

//		// now add a new attribute : +1 pierce armor
//		effect = *it;
//		armorType = 3; // pierce
//		effect.D = (armorType << 8) + armorQuantity;
//		effectsToAdd.push_back(effect);

		// now add a new attribute : +5 hp
		effect = *it;
		effect.C = 0; // hit points
		effect.D = 5;
		effectsToAdd.push_back(effect);
	}
	effectsPtr->insert(effectsPtr->end(), effectsToAdd.begin(), effectsToAdd.end());
}

DatPatch berbersUTFix = {
	&berbersUTPatch,
	"Berbers unique technologies alternative"
};

}

