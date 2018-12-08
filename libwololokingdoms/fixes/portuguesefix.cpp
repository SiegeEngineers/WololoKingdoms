#include "portuguesefix.h"
#include "wololo/datPatch.h"

namespace wololo {

void PortuguesePatch(genie::DatFile *aocDat) {
	/*
	 * The imported Portuguese team bonus makes the game crash
	 * so let's replace it by something simpler (and functionally identical ?)
	 */
	size_t const portugueseTBonusTechId = 32; // empty by default
	size_t const portugueseBrokenTBonusTechId = 626; // why is this even used ?
	size_t const caravelId = 1004;
	size_t const eliteCaravelId = 1006;
	size_t const petardId = 440;
	size_t const dockId = 45;
	size_t const castleId = 82;

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

	//This moves the Caraval in the Tech tree to the castle, so that it shows up only for portuguese and doesn't obscure shipwright
	//Delete Caravels from the dock and add them to the castle
	for (std::vector<genie::BuildingConnection>::iterator buildingIt = aocDat->TechTree.BuildingConnections.begin(), end = aocDat->TechTree.BuildingConnections.end(); buildingIt != end; buildingIt++) {
		// Iterate connected units of each age
		if(buildingIt->ID == castleId) {
			for (std::vector<int32_t>::iterator unitIt = buildingIt->Units.begin(), end = buildingIt->Units.end(); unitIt != end; ++unitIt) {
				if(*unitIt == petardId) {
					unitIt = buildingIt->Units.insert(unitIt, caravelId);
					++unitIt;
					unitIt = buildingIt->Units.insert(unitIt, eliteCaravelId);
					break;
				}
			}
		}
		if(buildingIt->ID == dockId) {
			for (std::vector<int32_t>::iterator unitIt = buildingIt->Units.begin(), end = buildingIt->Units.end(); unitIt != end; ++unitIt) {
				if(*unitIt == caravelId || *unitIt == eliteCaravelId) {
					unitIt = buildingIt->Units.erase(unitIt);
				}
			}
		}
	}

	//Change UpperBuilding of Caravels to the Castle
	for (std::vector<genie::UnitConnection>::iterator unitIt = aocDat->TechTree.UnitConnections.begin(), end = aocDat->TechTree.UnitConnections.end(); unitIt != end; ++unitIt) {
		if(unitIt->ID == caravelId || unitIt->ID == eliteCaravelId) {
			unitIt->UpperBuilding = castleId;
		}
	}
}

DatPatch portugueseFix = {
	&PortuguesePatch,
	"Portuguese civ crash fix"
};

}

