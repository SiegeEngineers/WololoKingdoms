#include "vietfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void vietPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {
	/*
     * Paper Money uses a team resource modifier not available in AoC
     *
     * Instead it becomes an aditional team bonus to have the research available
     * Resource amounts will have to be adjusted
     *
     * The reveal enemy civ bonus has to be adjusted
	 */

    // Paper Money Fix
    (*langReplacement)[28298] = "Research <b>Paper Money<b> (<cost>) \\nYou receive +300 Gold.";

    size_t const vietTBonusTechId = 653;
    size_t const vietUT2TechId = 669;
    size_t const vietUT2ResearchId = 629;
    size_t const allCivPaperMoneyResearchId = 802; // NEW
    size_t const allCivPaperMoneyDisableResearchId = 803; // NEW
	size_t const allCivGenitourDisableResearchId = 606;
    size_t const allCivPaperMoneyDisableTechId = 803; // NEW
	size_t const allCivGenitourDisableTechId = 632;
    size_t const vietTechTreeTechId = 652;

	genie::TechageEffect effect;

    // decrease cost to 600 F - you're paying 600 F for 300 G
    aocDat->Researchs[vietUT2ResearchId].ResourceCosts[0].Amount -= 200;
    aocDat->Researchs[vietUT2ResearchId].ResourceCosts[1].Amount -= 200;
    //change the Paper Money tech to resource add instead of team resource add
    aocDat->Techages[vietUT2TechId].Effects[0].Type = 1;
    aocDat->Techages[vietUT2TechId].Effects[0].D = 300; //Amount

    if(aocDat->Researchs.size() < allCivPaperMoneyDisableResearchId+1)
        aocDat->Researchs.resize(allCivPaperMoneyDisableResearchId+1);
    // copy Paper Money research to the all-civ one
    aocDat->Researchs[allCivPaperMoneyResearchId] = aocDat->Researchs[vietUT2ResearchId];
    aocDat->Researchs[allCivPaperMoneyResearchId].ButtonID = 4;
    // next to Petard, If you also have a berber ally, you need to research kasbah first.
    //Didn't see a way to solve this, all other slots are taken (bottom right doesn't seem to exist
    aocDat->Researchs[allCivPaperMoneyResearchId].Civ = -1;
    aocDat->Researchs[allCivPaperMoneyResearchId].Name = "all-civs Paper Money";

    if(aocDat->Techages.size() < allCivPaperMoneyDisableTechId+1)
        aocDat->Techages.resize(allCivPaperMoneyDisableTechId+1);
    // tech that disables the all civ Paper Money
    aocDat->Techages[allCivPaperMoneyDisableTechId] = aocDat->Techages[allCivGenitourDisableTechId];
    aocDat->Techages[allCivPaperMoneyDisableTechId].Effects[0].D = allCivPaperMoneyResearchId;
    aocDat->Techages[allCivPaperMoneyDisableTechId].Name = "all-civs Paper Money (disable)";
    // research that disables the all civ Paper Money
    aocDat->Researchs[allCivPaperMoneyDisableResearchId] = aocDat->Researchs[allCivGenitourDisableResearchId];
    aocDat->Researchs[allCivPaperMoneyDisableResearchId].TechageID = allCivPaperMoneyDisableTechId;
    aocDat->Researchs[allCivPaperMoneyDisableResearchId].Name = "all-civs Paper Money (disable)";
    // avoid Paper Money being researched twice for the Vietnamese
    aocDat->Techages[vietTechTreeTechId].Effects.push_back(aocDat->Techages[allCivPaperMoneyDisableTechId].Effects[0]);
    // enable Paper Money for Vietnamese teammates
    effect = aocDat->Techages[vietTBonusTechId].Effects[0]; // disable the disabling research (for genitours)
    effect.D = allCivPaperMoneyDisableResearchId;
    aocDat->Techages[vietTBonusTechId].Effects.push_back(effect);

    //Civ Bonus Fix
    //This would reveal everything until the first LC or mill is built - probably too strong :|
    /*
    genie::TechageEffect effect2;
    size_t const vietLosBonusTechId = 698;
    size_t const vietLosBonusResearchId = 665;
    size_t const LosResource = 183;
    aocDat->Researchs[vietLosBonusResearchId].ResearchTime = 0;
    int reqRes[] = {110,258,108,122,282,-1};
    aocDat->Researchs[vietLosBonusResearchId].RequiredTechs = std::vector<short> (reqRes, reqRes + sizeof(reqRes) / sizeof(int));
    aocDat->Techages[vietLosBonusTechId].Effects[0].A = 183;

    aocDat->Civs[31].Resources[LosResource] = 1;
    */

}

DatPatch vietFix = {
    &vietPatch,
    "Vietnamese Paper Money alternative"
};

}

