#include "cuttingfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void cuttingPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {

	//Civ Bonus Fix
    size_t const mangonelID = 280;
    size_t const onagerID = 550;
    size_t const siegeOnagerID = 588;
    size_t const newMangonelID = 947;
    size_t const newOnagerID = 948;
    size_t const siegeOnagerTechID = 320;


    size_t const cuttingResearchID = 152;
    size_t const autoOnagerCuttingResearchID = 153;
    size_t const onagerCuttingResearchID = 154;
    size_t const disablingResearchID = 155;

    size_t const cuttingTechID = 307;
    size_t const onagerCuttingTechID = 308;
    size_t const disablingTechID = 309;

    size_t const chemistryID = 47;
    size_t const furorCelticaID = 239;
    size_t const koreanTeamBonusID = 505;
    size_t const shinkichonID = 506;


    genie::TechageEffect effect = aocDat->Techages[247].Effects[0];

    aocDat->UnitHeaders[newOnagerID] = aocDat->UnitHeaders[onagerID];
    aocDat->UnitHeaders[newMangonelID] = aocDat->UnitHeaders[mangonelID];
    aocDat->UnitHeaders[newMangonelID].Commands.push_back(aocDat->UnitHeaders[onagerID].Commands[4]);
    aocDat->UnitHeaders[onagerID].Commands.erase(aocDat->UnitHeaders[onagerID].Commands.begin()+4);
    for(int i = 0; i< (int) aocDat->Civs.size(); i++) {
        aocDat->Civs[i].Units[newOnagerID] = aocDat->Civs[i].Units[onagerID];
        aocDat->Civs[i].Units[newMangonelID] = aocDat->Civs[i].Units[mangonelID];
        aocDat->Civs[i].Units[onagerID].Type50.BlastAttackLevel = 2;
        aocDat->Civs[i].Units[newMangonelID].Type50.BlastAttackLevel = 1;
    }
    effect.B = newMangonelID;
    aocDat->Techages[cuttingTechID].Effects.push_back(effect);
    effect.B = newOnagerID;
    aocDat->Techages[onagerCuttingTechID].Effects.push_back(effect);
    effect.A = onagerID;
    aocDat->Techages[onagerCuttingTechID].Effects.push_back(effect);
    effect.A = newMangonelID;
    effect.B = siegeOnagerID;
    aocDat->Techages[siegeOnagerTechID].Effects.push_back(effect);
    effect.A = newOnagerID;
    aocDat->Techages[siegeOnagerTechID].Effects.push_back(effect);
    effect.Type = 102;
    effect.D = cuttingResearchID;
    aocDat->Techages[disablingTechID].Effects.push_back(effect);
    effect.D = onagerCuttingResearchID;
    aocDat->Techages[disablingTechID].Effects.push_back(effect);

    aocDat->Techages[chemistryID].Effects.push_back(aocDat->Techages[chemistryID].Effects[66]);
    aocDat->Techages[chemistryID].Effects.push_back(aocDat->Techages[chemistryID].Effects[66]);
    aocDat->Techages[chemistryID].Effects[69].A = newOnagerID;
    aocDat->Techages[chemistryID].Effects[70].A = newMangonelID;

    aocDat->Techages[furorCelticaID].Effects.push_back(aocDat->Techages[furorCelticaID].Effects[0]);
    aocDat->Techages[furorCelticaID].Effects.push_back(aocDat->Techages[furorCelticaID].Effects[0]);
    aocDat->Techages[furorCelticaID].Effects[12].A = newOnagerID;
    aocDat->Techages[furorCelticaID].Effects[13].A = newMangonelID;

    aocDat->Techages[koreanTeamBonusID].Effects.push_back(aocDat->Techages[koreanTeamBonusID].Effects[0]);
    aocDat->Techages[koreanTeamBonusID].Effects.push_back(aocDat->Techages[koreanTeamBonusID].Effects[0]);
    aocDat->Techages[koreanTeamBonusID].Effects[3].A = newOnagerID;
    aocDat->Techages[koreanTeamBonusID].Effects[4].A = newMangonelID;

    aocDat->Techages[shinkichonID].Effects.push_back(aocDat->Techages[shinkichonID].Effects[0]);
    aocDat->Techages[shinkichonID].Effects.push_back(aocDat->Techages[shinkichonID].Effects[0]);
    aocDat->Techages[shinkichonID].Effects.push_back(aocDat->Techages[shinkichonID].Effects[1]);
    aocDat->Techages[shinkichonID].Effects.push_back(aocDat->Techages[shinkichonID].Effects[1]);
    aocDat->Techages[shinkichonID].Effects[12].A = newOnagerID;
    aocDat->Techages[shinkichonID].Effects[13].A = newMangonelID;
    aocDat->Techages[shinkichonID].Effects[14].A = newOnagerID;
    aocDat->Techages[shinkichonID].Effects[15].A = newMangonelID;

    aocDat->Researchs[cuttingResearchID] = aocDat->Researchs[siegeOnagerTechID];
    aocDat->Researchs[cuttingResearchID].RequiredTechs[1] = -1;
    aocDat->Researchs[cuttingResearchID].RequiredTechCount = 1;
    aocDat->Researchs[cuttingResearchID].TechageID = cuttingTechID;
    aocDat->Researchs[cuttingResearchID].ButtonID = 10;
    aocDat->Researchs[cuttingResearchID].IconID = 5;
    aocDat->Researchs[cuttingResearchID].Name = "Cutting Research";
    aocDat->Researchs[cuttingResearchID].LanguageDLLDescription = 8440;
    aocDat->Researchs[cuttingResearchID].LanguageDLLHelp = 107440;
    aocDat->Researchs[cuttingResearchID].LanguageDLLName = 7440;
    aocDat->Researchs[cuttingResearchID].LanguageDLLTechTree = 157440;

    aocDat->Researchs[autoOnagerCuttingResearchID] = aocDat->Researchs[siegeOnagerTechID];
    aocDat->Researchs[autoOnagerCuttingResearchID].ResearchTime = 0;
    aocDat->Researchs[autoOnagerCuttingResearchID].ResearchLocation = -1;
    aocDat->Researchs[autoOnagerCuttingResearchID].TechageID = onagerCuttingTechID;
    aocDat->Researchs[autoOnagerCuttingResearchID].ResourceCosts[0].Amount = 0;
    aocDat->Researchs[autoOnagerCuttingResearchID].ResourceCosts[0].Enabled = 0;
    aocDat->Researchs[autoOnagerCuttingResearchID].ResourceCosts[0].Type = -1;
    aocDat->Researchs[autoOnagerCuttingResearchID].ResourceCosts[1].Amount = 0;
    aocDat->Researchs[autoOnagerCuttingResearchID].ResourceCosts[1].Enabled = 0;
    aocDat->Researchs[autoOnagerCuttingResearchID].ResourceCosts[1].Type = -1;
    aocDat->Researchs[autoOnagerCuttingResearchID].Name = "Auto Onager Cutting";

    aocDat->Researchs[onagerCuttingResearchID] = aocDat->Researchs[autoOnagerCuttingResearchID];
    aocDat->Researchs[onagerCuttingResearchID].RequiredTechs[0] = cuttingResearchID;
    aocDat->Researchs[onagerCuttingResearchID].Name = "Onager Cutting";

    aocDat->Researchs[disablingResearchID] = aocDat->Researchs[332];
    aocDat->Researchs[disablingResearchID].TechageID = disablingTechID;
    aocDat->Researchs[disablingResearchID].Name = "Cutting Research Disabler";
    aocDat->Researchs[disablingResearchID].RequiredTechs[0] = 101;
    aocDat->Researchs[disablingResearchID].RequiredTechCount = 1;


}

DatPatch cuttingFix = {
    &cuttingPatch,
    "Option of non-cutting onagers and cutting tech per map"
};

}

