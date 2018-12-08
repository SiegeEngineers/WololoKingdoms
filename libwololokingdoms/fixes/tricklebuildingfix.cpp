#include "tricklebuildingfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void trickleBuildingPatch(genie::DatFile *aocDat) {
    size_t const tradeWorkshopID = 110;
    size_t const newUnitsStartID = 1300;
    size_t const feitoriaID = 1021;
    size_t const sheepID = 594;
    size_t const newStringStartID = 5045;

    aocDat->UnitHeaders[newUnitsStartID+8].Commands.push_back(aocDat->UnitHeaders[sheepID].Commands[0]);
    for(int i = 0; i < 4; i++) {
        aocDat->UnitHeaders[newUnitsStartID+i].Commands.push_back(aocDat->UnitHeaders[sheepID].Commands[0]);
        aocDat->UnitHeaders[newUnitsStartID+i].Commands.push_back(aocDat->UnitHeaders[feitoriaID].Commands[i]);
        aocDat->UnitHeaders[newUnitsStartID+i].Commands[1].Quantity = 1;
        aocDat->UnitHeaders[newUnitsStartID+4+i].Commands.push_back(aocDat->UnitHeaders[feitoriaID].Commands[i]);
        aocDat->UnitHeaders[newUnitsStartID+4+i].Commands[0].Quantity = 1;
        aocDat->UnitHeaders[newUnitsStartID+8].Commands.push_back(aocDat->UnitHeaders[feitoriaID].Commands[i]);
        aocDat->UnitHeaders[newUnitsStartID+8].Commands[1+i].Quantity = 1;
        aocDat->UnitHeaders[newUnitsStartID+9].Commands.push_back(aocDat->UnitHeaders[feitoriaID].Commands[i]);
        aocDat->UnitHeaders[newUnitsStartID+9].Commands[0+i].Quantity = 1;
        int graphic1 = getNewGraphicId(aocDat, i);
        int graphic2 = getNewGraphicId(aocDat, 4+i);
        for(int j = 0; j < aocDat->Civs.size(); j++) {
            aocDat->Civs[j].Units[newUnitsStartID+i] = aocDat->Civs[j].Units[tradeWorkshopID];
            aocDat->Civs[j].Units[newUnitsStartID+i].StandingGraphic.first = graphic1;
            aocDat->Civs[j].Units[newUnitsStartID+i].Bird.ActionWhenDiscoveredID = 0;
            aocDat->Civs[j].Units[newUnitsStartID+i].LanguageDLLName = newStringStartID+i;
            aocDat->Civs[j].Units[newUnitsStartID+i].ID1 = newUnitsStartID+i;
            aocDat->Civs[j].Units[newUnitsStartID+i].ID2 = newUnitsStartID+i;
            aocDat->Civs[j].Units[newUnitsStartID+i].ID3 = newUnitsStartID+i;
            aocDat->Civs[j].Units[newUnitsStartID+i].Class = 1;
            aocDat->Civs[j].Units[newUnitsStartID+i].BlastDefenseLevel = 0;
            aocDat->Civs[j].Units[newUnitsStartID+4+i] = aocDat->Civs[j].Units[tradeWorkshopID];
            aocDat->Civs[j].Units[newUnitsStartID+4+i].StandingGraphic.first = graphic2;
            aocDat->Civs[j].Units[newUnitsStartID+4+i].LanguageDLLName = newStringStartID+4+i;
            aocDat->Civs[j].Units[newUnitsStartID+4+i].ID1 = newUnitsStartID+4+i;
            aocDat->Civs[j].Units[newUnitsStartID+4+i].ID2 = newUnitsStartID+4+i;
            aocDat->Civs[j].Units[newUnitsStartID+4+i].ID3 = newUnitsStartID+4+i;
        }
    }

    int graphic1 = getNewGraphicId(aocDat, 8);
    int graphic2 = getNewGraphicId(aocDat, 9);
    for(int j = 0; j < aocDat->Civs.size(); j++) {
        aocDat->Civs[j].Units[newUnitsStartID+8] = aocDat->Civs[j].Units[tradeWorkshopID];
        aocDat->Civs[j].Units[newUnitsStartID+8].StandingGraphic.first = graphic1;
        aocDat->Civs[j].Units[newUnitsStartID+8].Bird.ActionWhenDiscoveredID = 0;
        aocDat->Civs[j].Units[newUnitsStartID+8].LanguageDLLName = newStringStartID+8;
        aocDat->Civs[j].Units[newUnitsStartID+8].ID1 = newUnitsStartID+8;
        aocDat->Civs[j].Units[newUnitsStartID+8].ID2 = newUnitsStartID+8;
        aocDat->Civs[j].Units[newUnitsStartID+8].ID3 = newUnitsStartID+8;
        aocDat->Civs[j].Units[newUnitsStartID+8].Class = 1;
        aocDat->Civs[j].Units[newUnitsStartID+8].BlastDefenseLevel = 0;
        aocDat->Civs[j].Units[newUnitsStartID+9] = aocDat->Civs[j].Units[tradeWorkshopID];
        aocDat->Civs[j].Units[newUnitsStartID+9].StandingGraphic.first = graphic2;
        aocDat->Civs[j].Units[newUnitsStartID+9].LanguageDLLName = newStringStartID+9;
        aocDat->Civs[j].Units[newUnitsStartID+9].ID1 = newUnitsStartID+9;
        aocDat->Civs[j].Units[newUnitsStartID+9].ID2 = newUnitsStartID+9;
        aocDat->Civs[j].Units[newUnitsStartID+9].ID3 = newUnitsStartID+9;
    }

}

int getNewGraphicId(genie::DatFile *aocDat, int i) {
    size_t const newUnitsSLPStartID = 450;
    size_t const tradeWorkshopGraphicID = 549;
    int newSLP = newUnitsSLPStartID+i;
    genie::Graphic newGraphic = aocDat->Graphics[tradeWorkshopGraphicID];
    newGraphic.ID = aocDat->Graphics.size();
    newGraphic.SLP = newSLP;
    aocDat->Graphics.push_back(newGraphic);
    aocDat->GraphicPointers.push_back(1);
    return newGraphic.ID;
}

DatPatch trickleBuildingFix = {
    &trickleBuildingPatch,
    "Buildings with indivdual resource trickles"
};

}

