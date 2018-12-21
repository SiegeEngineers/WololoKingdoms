#include "tricklebuildingfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void trickleBuildingPatch(genie::DatFile *aocDat) {
    size_t const tradeWorkshopID = 110;
    size_t const newUnitsStartID = 1300;
    size_t const feitoriaID = 1021;
    size_t const sheepID = 594;
    size_t const newStringStartID = 5045;

    aocDat->UnitHeaders[newUnitsStartID+8].TaskList.push_back(aocDat->UnitHeaders[sheepID].TaskList[0]);
    for(size_t i = 0; i < 4; i++) {
        aocDat->UnitHeaders[newUnitsStartID+i].TaskList.push_back(aocDat->UnitHeaders[sheepID].TaskList[0]);
        aocDat->UnitHeaders[newUnitsStartID+i].TaskList.push_back(aocDat->UnitHeaders[feitoriaID].TaskList[i]);
        aocDat->UnitHeaders[newUnitsStartID+i].TaskList[1].WorkValue1 = 1;
        aocDat->UnitHeaders[newUnitsStartID+4+i].TaskList.push_back(aocDat->UnitHeaders[feitoriaID].TaskList[i]);
        aocDat->UnitHeaders[newUnitsStartID+4+i].TaskList[0].WorkValue1 = 1;
        aocDat->UnitHeaders[newUnitsStartID+8].TaskList.push_back(aocDat->UnitHeaders[feitoriaID].TaskList[i]);
        aocDat->UnitHeaders[newUnitsStartID+8].TaskList[1+i].WorkValue1 = 1;
        aocDat->UnitHeaders[newUnitsStartID+9].TaskList.push_back(aocDat->UnitHeaders[feitoriaID].TaskList[i]);
        aocDat->UnitHeaders[newUnitsStartID+9].TaskList[0+i].WorkValue1 = 1;
        int graphic1 = getNewGraphicId(aocDat, i);
        int graphic2 = getNewGraphicId(aocDat, 4+i);
        for(size_t j = 0; j < aocDat->Civs.size(); j++) {
            aocDat->Civs[j].Units[newUnitsStartID+i] = aocDat->Civs[j].Units[tradeWorkshopID];
            aocDat->Civs[j].Units[newUnitsStartID+i].StandingGraphic.first = graphic1;
            aocDat->Civs[j].Units[newUnitsStartID+i].Action.DefaultTaskID = 0;
            aocDat->Civs[j].Units[newUnitsStartID+i].LanguageDLLName = newStringStartID+i;
            aocDat->Civs[j].Units[newUnitsStartID+i].ID = newUnitsStartID+i;
            aocDat->Civs[j].Units[newUnitsStartID+i].CopyID = newUnitsStartID+i;
            aocDat->Civs[j].Units[newUnitsStartID+i].BaseID = newUnitsStartID+i;
            aocDat->Civs[j].Units[newUnitsStartID+i].Class = 1;
            aocDat->Civs[j].Units[newUnitsStartID+i].BlastDefenseLevel = 0;
            aocDat->Civs[j].Units[newUnitsStartID+4+i] = aocDat->Civs[j].Units[tradeWorkshopID];
            aocDat->Civs[j].Units[newUnitsStartID+4+i].StandingGraphic.first = graphic2;
            aocDat->Civs[j].Units[newUnitsStartID+4+i].LanguageDLLName = newStringStartID+4+i;
            aocDat->Civs[j].Units[newUnitsStartID+4+i].ID = newUnitsStartID+4+i;
            aocDat->Civs[j].Units[newUnitsStartID+4+i].CopyID = newUnitsStartID+4+i;
            aocDat->Civs[j].Units[newUnitsStartID+4+i].BaseID = newUnitsStartID+4+i;
        }
    }

    int graphic1 = getNewGraphicId(aocDat, 8);
    int graphic2 = getNewGraphicId(aocDat, 9);
    for(size_t j = 0; j < aocDat->Civs.size(); j++) {
        aocDat->Civs[j].Units[newUnitsStartID+8] = aocDat->Civs[j].Units[tradeWorkshopID];
        aocDat->Civs[j].Units[newUnitsStartID+8].StandingGraphic.first = graphic1;
        aocDat->Civs[j].Units[newUnitsStartID+8].Action.DefaultTaskID = 0;
        aocDat->Civs[j].Units[newUnitsStartID+8].LanguageDLLName = newStringStartID+8;
        aocDat->Civs[j].Units[newUnitsStartID+8].ID = newUnitsStartID+8;
        aocDat->Civs[j].Units[newUnitsStartID+8].CopyID = newUnitsStartID+8;
        aocDat->Civs[j].Units[newUnitsStartID+8].BaseID = newUnitsStartID+8;
        aocDat->Civs[j].Units[newUnitsStartID+8].Class = 1;
        aocDat->Civs[j].Units[newUnitsStartID+8].BlastDefenseLevel = 0;
        aocDat->Civs[j].Units[newUnitsStartID+9] = aocDat->Civs[j].Units[tradeWorkshopID];
        aocDat->Civs[j].Units[newUnitsStartID+9].StandingGraphic.first = graphic2;
        aocDat->Civs[j].Units[newUnitsStartID+9].LanguageDLLName = newStringStartID+9;
        aocDat->Civs[j].Units[newUnitsStartID+9].ID = newUnitsStartID+9;
        aocDat->Civs[j].Units[newUnitsStartID+9].CopyID = newUnitsStartID+9;
        aocDat->Civs[j].Units[newUnitsStartID+9].BaseID = newUnitsStartID+9;
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

