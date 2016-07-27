#include "demoshipfix.h"
#include "wololo/datPatch.h"

namespace wololo {

void demoshipPatch(genie::DatFile *aocDat, std::map<int, std::string> *langReplacement) {
	/*
	 * The way self-destruct units work in AOE2 is harcoded into the .exe :
	 * a set of units id (demo ships, petards...) will always suicide as they attack.
	 * The problem is that the demolition raft is not part of that list in AOC,
	 * however, I found a workaround for this :
	 * There is a Hero/Cheat unit called "Saboteur" that is basically a stronger petard
	 * The way this fix works is by replacing this unit entierly by
	 * the newly imported demolition raft, and adjust the tech tree accordingly.
	 */

	size_t const demoRaftUnitId = 1104;
	size_t const saboteurUnitId = 706;
	size_t const demoShipUnitId = 527;
	size_t const heavyDemoShipUnitId = 528;
	size_t const demoRaftTechId = 631;
	size_t const demoRaftAvailabiltyEffectIdx = 0;
	size_t const warGalleyTechId = 155;
	size_t const heavyDemoShipTechId = 233;
	size_t const feudalAgeId = 1;
	size_t const demoRaftIdxInFeudal = 2;
	size_t const dockBuildingConnectionId = 1;
	size_t const demoRaftIdxInDock = 3;
	size_t const demoRaftIdxInUnitConnections = 119;
	size_t const feudalAgeIdInResearchConnections = 1;
	size_t const demoRaftIdxInFeudalResearchConnection = 5;

	for (size_t civIndex = 0; civIndex < aocDat->Civs.size(); civIndex++) {
		// Copies the demoraft attributes to the saboteur unit
		aocDat->Civs[civIndex].Units[saboteurUnitId] = aocDat->Civs[civIndex].Units[demoRaftUnitId];
		aocDat->Civs[civIndex].Units[saboteurUnitId].ID1 = saboteurUnitId; // restores the proper ID
		aocDat->Civs[civIndex].Units[demoRaftUnitId].HideInEditor = 1; // Prevents having 2 demo rafts in the scenario editor
	}
	// Change the hidden tech that makes demo ships available
	aocDat->Techages[demoRaftTechId].Effects[demoRaftAvailabiltyEffectIdx].A = saboteurUnitId;
	// Makes the war galley and the heavy demolition ship techs upgrade the newly modified unit
	genie::TechageEffect effect;
	effect.Type = 3; // upgrade unit
	effect.A = saboteurUnitId; // source unit
	effect.B = demoShipUnitId; // destination unit
	aocDat->Techages[warGalleyTechId].Effects.push_back(effect);
	effect.B = heavyDemoShipUnitId;
	aocDat->Techages[heavyDemoShipTechId].Effects.push_back(effect);

	// Replaces the demo raft by the newly modified unit in the feudal age
	aocDat->TechTree.TechTreeAges[feudalAgeId].Units[demoRaftIdxInFeudal] = saboteurUnitId;
	// Replaces the demo raft by the newly modified unit in the dock
	aocDat->TechTree.BuildingConnections[dockBuildingConnectionId].Units[demoRaftIdxInDock] = saboteurUnitId;
	// Replaces the demo raft by the newly modified unit in the tech tree units list
	aocDat->TechTree.UnitConnections[demoRaftIdxInUnitConnections].ID = saboteurUnitId;
	// Replaces the demo raft by the newly modified unit in the tech tree research list
	aocDat->TechTree.ResearchConnections[feudalAgeIdInResearchConnections].Units[demoRaftIdxInFeudalResearchConnection] = saboteurUnitId;
}

DatPatch demoShipFix = {
	&demoshipPatch,
	"Demolition ships not exploding fix"
};

}
