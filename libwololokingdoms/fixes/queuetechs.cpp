#include "queuetechs.h"
#include "wololo/datPatch.h"
#include <cassert>
#include <unordered_map>
#include <cassert>

using std::unordered_map;

namespace wololo {

// Tech ID of the Horse Collar tech.
const int TECH_ID_HORSE_COLLAR = 14;

// Tech ID of the Heavy Plow tech.
const int TECH_ID_HEAVY_PLOW = 13;

// Tech ID of the Crop Rotation tech.
const int TECH_ID_CROP_ROTATION = 12;

// Tech ID of the Spies/Treason tech.
const int TECH_ID_SPIES_TREASON = 408;

// Civilization ID of the gaia civilization.
const int CIV_ID_GAIA = 0;

// Unit ID of the "New Unit" unit.
const int UNIT_ID_NEW = 1401;

// Button ID value to disable a technology (remove it from the interface menu).
const int DISABLED_BUTTON = -1;

// First integer representing the unique resource for each technology.
// WololoKingdoms 5.8 ends at 211; Aoe2 DE ends at 217.
const int RESOURCE_START = 220;

// The number of resource IDs that are reserved consecutively starting at
// `RESOURCE_START`.
const int RESOURCE_ID_NUM_RESERVED = 4;

// The resource ID of Castle unique units.
const int RESOURCE_ID_UU = RESOURCE_START;

// The resource ID of unique ships.
const int RESOURCE_ID_US = RESOURCE_START + 1;

// The resource ID of Castle Age unique technologies.
const int RESOURCE_ID_CT = RESOURCE_START + 2;

// The resource ID of Imperial Age unique technologies.
const int RESOURCE_ID_IT = RESOURCE_START + 3;

// Location ID of the castle, for technologies researched there.
const int LOCATION_CASTLE = 82;

// Location ID of the dock, for technologies researched there.
const int LOCATION_DOCK = 45;

// Button ID of the unique unit elite upgrade button.
const int BUTTON_ID_ELITE_UU = 6;

// Button ID of the Castle Age unique technologies.
const int BUTTON_ID_CASTLE_UT = 7;

// Button ID of the Imperial Age unique technologies.
const int BUTTON_ID_IMP_UT = 8;

// Button ID of the elite unique ship upgrades at the dock.
const int BUTTON_ID_ELITE_SHIP = 29;

// The number of unit icons, plus a small buffer.
const int ICON_ID_OFFSET = 258;

// An ID that is not linked to any SLP, used to make invisible units.
const int GRAPHICS_INVISIBLE_UNIT = 6401;

// Constant for the 2nd value of standing graphics when only the first value is
// used.
const int GRAPHICS_UNUSED = -1;

// Effect ID of a "New Effect" effect.
const int EFFECT_ID_NEW = 798;

// Tech ID of a "New Research" technology.
const int TECH_ID_NEW = 801;

// Tech ID of the Elite Genitour upgrade.
const int TECH_ID_ELITE_GENITOUR = 599;

// Effect ID of the Genitour (Disable) effect.
const int EFFECT_ID_GENITOUR_DISABLE = 632;

// Tech ID of the Imperial Skirmisher upgrade.
const int TECH_ID_IMPERIAL_SKIRMISHER = 655;

// Effect ID of the Elite Genitour (Disable) effect.
const int EFFECT_ID_IMPERIAL_SKIRMISHER_DISABLE = 692;

// Mode value for Effect Commands of type Tech Cost Modifier that set a cost.
const int MODE_SET_COST = 0;

// Enum representing food costs for technologies.
const int TECH_FOOD_COST = 0;

// Enum representing wood costs for technologies.
const int TECH_WOOD_COST = 1;

// Enum representing stone costs for technologies.
const int TECH_STONE_COST = 2;

// Enum representing gold costs for technologies.
const int TECH_GOLD_COST = 3;

// The number of ages.
const int NUM_AGES = 4;

// Tech ID of the Feudal Age Chinese technology discount.
const int TECH_ID_CHI_F = 350;

// Tech ID of the Castle Age Chinese technology discount.
const int TECH_ID_CHI_C = 351;

// Tech ID of the Imperial Age Chinese technology discount.
const int TECH_ID_CHI_I = 352;

// Tech IDs of the Chinese technology discounts for Feudal, Castle, and Imperial Ages.
const int TECH_IDS_CHI[] = {TECH_ID_CHI_F, TECH_ID_CHI_C, TECH_ID_CHI_I};

// Unit ID of a packet trebuchet.
const int UNIT_ID_TREBUCHET_PACKED = 331;

// Multiplier values for adjusting Chinese unit class 51 (Packed Unit) based on
// Age. Ages are Feudal, Castle, and Imperial, in that order.
const float MULTIPLIERS_PACKED_UNIT[] = {0.9f, 0.94444f, 0.94118f};

// Multiplier values for adjusting Chinese unit 331 (Trebuchet Packed) costs
// based on Age. Ages are Feudal, Castle, and Imperial, in that order.
const float MULTIPLIERS_PACKED_TREB[] = {1.11111f, 1.05887f, 1.06250f};

// The effect command attributes representing resource costs.
const int16_t RESOURCE_ATTRIBUTES[] = {
    genie::EffectCommand::Attributes::FoodCosts,
    genie::EffectCommand::Attributes::WoodCosts,
    genie::EffectCommand::Attributes::GoldCosts,
    genie::EffectCommand::Attributes::StoneCosts
};

// Flag to indicate that this unit should not be shown in the editor.
const int8_t HIDE_IN_EDITOR_FLAG = 1;
// Flag to indicate that this unit is a queueable tech dummy.
const int8_t QUEUEABLE_TECH_FLAG = 2;

// Returns `true` if `techId` represents an age, `false` otherwise.
// Ages are given by ids:
//   * Dark Age     - 101
//   * Feudal Age   - 102
//   * Castle Age   - 103
//   * Imperial Age - 104
// Making the age advancement technologies queuable currently removes the Age
// progress bar at the top of the screen. Hence the need to filter out these
// techs.
// @param techId the technology id to check
bool isAge(int techId) { return 101 <= techId && techId <= 104; }

/// Returns `true` if `techId` represents a mill technology (farm upgrade),
/// `false` otherwise.
bool isMillTech(int techId) {
  return techId == TECH_ID_HORSE_COLLAR || techId == TECH_ID_HEAVY_PLOW ||
         techId == TECH_ID_CROP_ROTATION;
}

// Returns `true` if `techId` represents the Spies/Treason tech, `false`
// otherwise.
bool isSpiesTreason(int techId) { return techId == TECH_ID_SPIES_TREASON; }

// Returns `true` if `techId` represents a tech that should be skipped.
// Age upgrades, mill upgrades (to prevent interference with farm queuing), and
// Spies/Treason should be skipped.
bool skipId(int techId) {
  return isAge(techId) || isMillTech(techId) || isSpiesTreason(techId);
}

// Returns `true` if `locationId` represents a location for which the
// technology should be skipped. Technologies with research locations of `-1`
// and `0` are researched automatically.
// @param locationId the location to filter
bool filterLocation(int locationId) { return locationId == -1 || locationId == 0; }

// Returns `true` if `tech` is the Elite upgrade for a unique unit at the
// Castle.
// @param tech the technology to check
bool isEliteUniqueUnitUpgrade(genie::Tech& tech) {
  return tech.ResearchLocation == LOCATION_CASTLE &&
         tech.ButtonID == BUTTON_ID_ELITE_UU;
}

// Returns `true` if `tech` is the Elite upgrade for Caravels, Longboats, or
// Turtle Ships.
// @param tech the technology to check
bool isEliteShipUpgrade(genie::Tech& tech) {
  return tech.ResearchLocation == LOCATION_DOCK &&
         tech.ButtonID == BUTTON_ID_ELITE_SHIP;
}

// Returns `true` if `tech` is a Castle Age unique technology.
// @param tech the technology to check
bool isUniqueCastleTech(genie::Tech& tech) {
  return tech.ResearchLocation == LOCATION_CASTLE &&
         tech.ButtonID == BUTTON_ID_CASTLE_UT;
}

// Returns `true` if `tech` is an Imperial Age unique technology.
// @param tech the technology to check
bool isUniqueImpTech(genie::Tech& tech) {
  return tech.ResearchLocation == LOCATION_CASTLE &&
         tech.ButtonID == BUTTON_ID_IMP_UT;
}

// Returns the id of the resource assigned to `tech`, along with whether or not
// the ID is a reserved index. The returned id is given by `RESOURCE_START +
// techId`, unless the technology is a unique unit elite upgrade, unique ship
// elite upgrage, or unique techonology. In those cases, all researches in each
// of those classes share the same resource id, which is given by
// `RESOURCE_START` plus the min index of a technology in the class.
// @param reserveIds `true` if specific IDs should be reserved to map multiple
// techs to a single ID,
//     e.g. mapping all Elite Unit upgrades to a single ID, `false` not to
//     reserve
// @param techId the technology id of `tech`
// @param tech the technology for which to return the research id
// @param numSkipped the number of "skipped" IDs that are already included as
// one of the reserved IDs or are already filtered
std::tuple<int, bool> resourceId(bool reserveIds, int techId, genie::Tech& tech, int numSkipped) {
  auto numReserved = 0;
  if (reserveIds) {
    numReserved = RESOURCE_ID_NUM_RESERVED;
    if (isEliteUniqueUnitUpgrade(tech)) { return {RESOURCE_ID_UU, true}; }
    if (isEliteShipUpgrade(tech))       { return {RESOURCE_ID_US, true}; }
    if (isUniqueCastleTech(tech))       { return {RESOURCE_ID_CT, true}; }
    if (isUniqueImpTech(tech))          { return {RESOURCE_ID_IT, true}; }
  }
  return {RESOURCE_START + numReserved + techId - numSkipped, false};
}

// Mutates `unit` to represent `tech`.
// @param unit a copy of the new unit, mutated by this method to represent
// `tech`
// @param tech the technology to use for initializing the `unit`
// @param resId the resource ID for the unit to cost
void initializeUnit(genie::Unit& unit, int resId, genie::Tech& tech) {
  unit.Type = genie::Unit::UnitType::BuildingType;
  unit.Class = genie::Unit::UnitClass::PackedUnit;
  unit.StandingGraphic = {GRAPHICS_INVISIBLE_UNIT, GRAPHICS_UNUSED};
  unit.HitPoints = 0;
  unit.LineOfSight = 0;
  unit.Enabled = 0;
  unit.IconID = tech.IconID + ICON_ID_OFFSET;
  unit.LanguageDLLName = tech.LanguageDLLName;
  unit.LanguageDLLHelp = tech.LanguageDLLHelp;
  unit.LanguageDLLCreation = tech.LanguageDLLDescription;
  unit.TerrainRestriction = 0;
  unit.Name = tech.Name + " (Queueable Dummy)";
  unit.HideInEditor = HIDE_IN_EDITOR_FLAG | QUEUEABLE_TECH_FLAG;

  unit.Creatable.TrainTime = tech.ResearchTime;
  unit.Creatable.TrainLocationID = tech.ResearchLocation;
  unit.Creatable.ButtonID = tech.ButtonID;

  // Copies the first two resources, uses the new resource for the third.
  unit.Creatable.ResourceCosts[0].Amount = tech.ResourceCosts[0].Amount;
  unit.Creatable.ResourceCosts[0].Paid = tech.ResourceCosts[0].Paid;
  unit.Creatable.ResourceCosts[0].Type = tech.ResourceCosts[0].Type;
  unit.Creatable.ResourceCosts[1].Amount = tech.ResourceCosts[1].Amount;
  unit.Creatable.ResourceCosts[1].Paid = tech.ResourceCosts[1].Paid;
  unit.Creatable.ResourceCosts[1].Type = tech.ResourceCosts[1].Type;
  unit.Creatable.ResourceCosts[2].Amount = 1;
  unit.Creatable.ResourceCosts[2].Paid = 1;
  unit.Creatable.ResourceCosts[2].Type = resId;
}

// Initializes the technology and effect that enable the dummy unit.
// @param df the dat file to modify
// @param techId the technology id, which is the index of the technology in
// `df.Techs`
// @param unitId the ID of the dummy unit
// @param techIdEnablers maps a `techId` to the id of the technology that
// enables its dummy unit, mutated by calling this method
void researchEnabler(genie::DatFile * df, int techId, int unitId,
                     unordered_map<int, int>& techIdEnablers) {
  auto tech = &df->Techs[techId];
  auto researchEnabler = df->Effects[EFFECT_ID_NEW];
  researchEnabler.Name = "Research Enabler";
  auto researchEnablerUnitCmd = genie::EffectCommand();
  researchEnablerUnitCmd.Type = genie::EffectCommand::EffectType::EnableUnit;
  researchEnablerUnitCmd.TargetUnit = unitId;
  researchEnablerUnitCmd.UnitClassID = 1;
  researchEnablerUnitCmd.AttributeID = -1;
  researchEnablerUnitCmd.Amount = 0;
  researchEnabler.EffectCommands.push_back(researchEnablerUnitCmd);
  auto techEnabler = df->Techs[TECH_ID_NEW];
  techEnabler.LanguageDLLName = tech->LanguageDLLName;
  techEnabler.LanguageDLLDescription = tech->LanguageDLLDescription;
  techEnabler.Name = "Enabler";
  techEnabler.RequiredTechs = tech->RequiredTechs;
  techEnabler.RequiredTechCount = tech->RequiredTechCount;
  techEnabler.Civ = tech->Civ;
  techEnabler.FullTechMode = tech->FullTechMode;
  techEnabler.EffectID = static_cast<int16_t>(df->Effects.size());
  df->Effects.push_back(researchEnabler);
  auto newTechId = df->Techs.size();
  techIdEnablers[techId] = newTechId;
  df->Techs.push_back(techEnabler);

  if (techId == TECH_ID_ELITE_GENITOUR) {
    auto disableEliteGenitour = genie::EffectCommand();
    disableEliteGenitour.Type = genie::EffectCommand::EffectType::DisableTech;
    disableEliteGenitour.Amount = newTechId;
    df->Effects[EFFECT_ID_GENITOUR_DISABLE].EffectCommands.push_back(disableEliteGenitour);
  } else if (techId == TECH_ID_IMPERIAL_SKIRMISHER) {
    auto disableImpSkirm = genie::EffectCommand();
    disableImpSkirm.Type = genie::EffectCommand::EffectType::DisableTech;
    disableImpSkirm.Amount = newTechId;
    df->Effects[EFFECT_ID_IMPERIAL_SKIRMISHER_DISABLE].EffectCommands.push_back(disableImpSkirm);
  }
}

// Initializes the technology and effect that set the research time of the
// technology 0 so it researches automatically.
// @param df the dat file to modify
// @param techId the technology id, which is the index of the technology in
// `df.Techs`
// @param unit the dummy unit
void researchModifier(genie::DatFile * df, int techId, genie::Unit& unit) {
  auto tech = &df->Techs[techId];
  auto researchModifier = df->Effects[EFFECT_ID_NEW];
  researchModifier.Name = "Research Modifier";
  auto removeTimeTech = genie::EffectCommand();
  removeTimeTech.Type = 8; // 8 is a UP 1.5 command, it's not an enum value in
                           // the genieutils library currently.
  removeTimeTech.TargetUnit = techId;
  removeTimeTech.UnitClassID = -1;
  removeTimeTech.AttributeID = -1;
  removeTimeTech.Amount = 0;
  researchModifier.EffectCommands.push_back(removeTimeTech);
  auto techModifier = df->Techs[TECH_ID_NEW];
  techModifier.Name = "Modifier";
  techModifier.RequiredTechCount =
      1; // Prevents the tech from firing without the unit.
  techModifier.FullTechMode = 1;
  techModifier.LanguageDLLName = tech->LanguageDLLName;
  techModifier.LanguageDLLDescription = tech->LanguageDLLDescription;
  techModifier.EffectID = static_cast<int16_t>(df->Effects.size());
  techModifier.Civ = tech->Civ;
  techModifier.FullTechMode = tech->FullTechMode;
  df->Effects.push_back(researchModifier);
  unit.Building.TechID = static_cast<int16_t>(df->Techs.size()); // Sets unit's initiate technology
                                                                 // to the tech modifier tech.
  df->Techs.push_back(techModifier);
}

// Initializes the technology and effect that disable the dummy unit when the
// technology is researched.
// @param df the dat file to modify
// @param techId the technology id, which is the index of the technology in
// `df.Techs`
// @param unitId the ID of the dummy unit
void researchDisabler(genie::DatFile * df, int techId, int unitId) {
  auto tech = &df->Techs[techId];
  auto researchDisabler = df->Effects[EFFECT_ID_NEW];
  researchDisabler.Name = "Research Disabler";
  auto researchDisablerUnit = genie::EffectCommand();
  researchDisablerUnit.Type =
      genie::EffectCommand::EffectType::EnableUnit; // Also Disable Unit, not
                                                    // just Enable Unit.
  researchDisablerUnit.TargetUnit = unitId;
  researchDisablerUnit.UnitClassID = 0;
  researchDisablerUnit.AttributeID = -1;
  researchDisablerUnit.Amount = 0;
  researchDisabler.EffectCommands.push_back(researchDisablerUnit);
  auto techDisabler = df->Techs[EFFECT_ID_NEW];
  techDisabler.Name = "Disabler";
  techDisabler.RequiredTechs[0] = techId;
  techDisabler.RequiredTechCount = 1;
  techDisabler.FullTechMode = 1;
  techDisabler.LanguageDLLName = tech->LanguageDLLName;
  techDisabler.LanguageDLLDescription = tech->LanguageDLLDescription;
  techDisabler.EffectID = static_cast<int16_t>(df->Effects.size());
  techDisabler.Civ = tech->Civ;
  techDisabler.FullTechMode = tech->FullTechMode;
  df->Effects.push_back(researchDisabler);
  df->Techs.push_back(techDisabler);
}

// Initializes the technologies and effects.
// @param df the dat file to modify
// @param techId the technology id, which is the index of the technology in
// `df.Techs`
// @param unitId the ID of the dummy unit
// @param unit the dummy unit
// @param techIdEnablers maps a `techId` to the id of the technology that
// enables its dummy unit, mutated by calling this method
void initializeTechs(genie::DatFile * df, int techId, int unitId, genie::Unit& unit, 
					 unordered_map<int, int>& techIdEnablers) {
  researchEnabler(df, techId, unitId, techIdEnablers);
  researchModifier(df, techId, unit);
  researchDisabler(df, techId, unitId);
}

// Modifies a single tech of the datfile `df` given by index `techId`.
// Requires that `techId` is a valid technology index.
// @param df the dat file to modify
// @param techId the technology id, which is the index of the technology in
// `df.Techs`
// @param reserveIds `true` if specific IDs should be reserved to map multiple
// techs to a single ID,
//     e.g. mapping all Elite Unit upgrades to a single ID, `false` not to
//     reserve
// @param numSkipped the number of "skipped" IDs that are already included as
// one of the reserved IDs
// @param techIdToUnitId maps a `techId` the the id of the dummy unit created
// to represent it, mutated by calling this method
// @param techIdEnablers maps a `techId` to the id of the technology that
// enables its dummy unit, mutated by calling this method
// @return `true` if this technology's id is skipped (i.e. this technology uses
// a reserved ID or is filtered), `false` otherwise
int processTech(genie::DatFile * df, int techId, bool reserveIds, int numSkipped,
                unordered_map<int, int>& techIdToUnitId,
                unordered_map<int, int>& techIdEnablers) {
  auto tech = &df->Techs[techId];
  if (filterLocation(tech->ResearchLocation)) { return true; }
  auto [resId, isSkipped] = resourceId(reserveIds, techId, *tech, numSkipped);

  int unitId = df->UnitHeaders.size();
  techIdToUnitId[techId] = unitId;
  df->UnitHeaders.push_back(df->UnitHeaders[UNIT_ID_NEW]); // Copies the "New Unit" header.
  auto unit = df->Civs[CIV_ID_GAIA].Units[UNIT_ID_NEW]; // Copies the "new unit" unit from the gaia civ.
  initializeUnit(unit, resId, *tech);

  tech->ButtonID = DISABLED_BUTTON; // Disables the button after copying it to the unit.
  initializeTechs(df, techId, unitId, unit, techIdEnablers);

  // Appends the unit to every civilization.
  for (auto& civ : df->Civs) {
    civ.Units.push_back(unit);
    civ.UnitPointers.push_back(1); // All Unit Pointers are 1.
  }
  return isSkipped;
}

// Sets settings that need to be set for each civilization.
//
// Adds resources for each new dummy unit.
// Disables dummy units for civs without access to them.
//
// @param df the dat file to modify
// @param numAdded the total number of technology ID added during the
// modification, excluding skipped ids
// @param techIdEnablers maps a `techId` to the id of the technology that
// enables its dummy unit, mutated by calling this method
void setCivSettings(genie::DatFile * df, int numAdded, unordered_map<int, int>& techIdEnablers) {
  for (auto& civ : df->Civs) {
    // Appends extra resources for a buffer before the new resources start.
    for (int i = civ.Resources.size(); i != RESOURCE_START; ++i) { civ.Resources.push_back(0); }
    // Appends 1 of each of the extra resources to each civ.
    for (int i = 0; i != numAdded; ++i) { civ.Resources.push_back(1); }

    auto techTreeEffect = &df->Effects[civ.TechTreeID];
    auto numTechs = techTreeEffect->EffectCommands.size();
    for (int i = 0; i != numTechs; ++i) {
      auto cmd = techTreeEffect->EffectCommands[i]; // Copies the effect command.
      // Filters out effect comands other than disable tech
      if (cmd.Type != genie::EffectCommand::DisableTech) { continue; }
      int techId = int(cmd.Amount);
      int enablerId = techIdEnablers[techId];
      if (enablerId == 0) { continue; }
      cmd.Amount = static_cast<float>(enablerId);
      techTreeEffect->EffectCommands.push_back(cmd);
    }
  }
}

// Returns `true` if the final effect command with type `TechTimeModifier` with
// target unit `techId` sets the reserach time to 0. Returns `false` if
// `effectCommands` does not contain such an effect command, or the final such
// effect command sets the research time to a nonzero value.
// @param effectCommands the list of effect commands to check
// @param tech the technology to check
bool researchTimeChangesToZero(std::vector<genie::EffectCommand>& effectCommands, int techId) {
  // Loops backwards, as if multiple Effect Commands set the research time, the
  // final one in the list is the one that determines the value.
  for (int i = effectCommands.size() - 1; i != -1; --i) {
    auto ec = effectCommands[i];
    if (ec.Type != genie::EffectCommand::EffectType::TechTimeModifier) { continue; }
    if (ec.TargetUnit != techId) { continue; }
    return ec.Amount == 0;
  }
  return false;
}

// Sets the discount for the Burmese Monastery technology bonus,
// the Italian Dock technology bonus, and the Spanish Blacksmith technology
// bonus.
// @param df the dat file to modify
// @param maps a `techId` the the id of the dummy unit that represent it
void setDiscounts(genie::DatFile * df, unordered_map<int, int>& techIdToUnitId) {
  for (auto civ : df->Civs) {
    auto techTreeId = civ.TechTreeID;
    auto techTreeEffect = &df->Effects[techTreeId];
    auto numCmdsStart = techTreeEffect->EffectCommands.size();
    for (int i = 0; i != numCmdsStart; ++i) {
      auto cmd = &techTreeEffect->EffectCommands[i];
      // Filters out effect commands that are not tech cost modifiers.
      if (cmd->Type != genie::EffectCommand::EffectType::TechCostModifier) { continue; }

      auto techId = cmd->TargetUnit;
      // Filters out the Age technologies for the Italians, since Ages cannot currently be queued
      if (isAge(techId)) { continue; }
      // Filters out technologies that are "free" with 0 research time
      if (&df->Techs[cmd->TargetUnit].ResearchTime == 0
			|| researchTimeChangesToZero(techTreeEffect->EffectCommands, cmd->TargetUnit)) {
        continue;
      }

      auto cmdNew = genie::EffectCommand();

      cmdNew.Type = cmd->AttributeID == MODE_SET_COST
              ? (int) genie::EffectCommand::EffectType::AbsoluteAttributeModifier
              : (int) genie::EffectCommand::EffectType::RelativeAttributeModifier;
      cmdNew.TargetUnit = techIdToUnitId[techId];
      switch (cmd->UnitClassID) {
      case TECH_FOOD_COST:
        cmdNew.AttributeID = genie::EffectCommand::Attributes::FoodCosts;
        break;
      case TECH_WOOD_COST:
        cmdNew.AttributeID = genie::EffectCommand::Attributes::WoodCosts;
        break;
      case TECH_STONE_COST:
        cmdNew.AttributeID = genie::EffectCommand::Attributes::StoneCosts;
        break;
      case TECH_GOLD_COST:
        cmdNew.AttributeID = genie::EffectCommand::Attributes::GoldCosts;
        break;
      default:
        // Unit Class ID must be between 0 and 3, inclusive, to represent a
        // resource cost.
        assert(false);
        break;
      }
      cmdNew.Amount = cmd->Amount;

      techTreeEffect->EffectCommands.push_back(cmdNew);
    }
  }
}

// Sets the discount for the Chinese technology bonus.
// @param df the dat file to modify
void setChineseDiscount(genie::DatFile * df) {
  // Uses `NUM_AGES - 1` to account for no bonus in the Dark Age.
  for (int age = 0; age != NUM_AGES - 1; ++age) {
    auto tech = df->Techs[TECH_IDS_CHI[age]];
    auto effect = df->Effects[EFFECT_ID_NEW];
    effect.Name = "Chinese Tech Bonus Fix";
    for (auto res : RESOURCE_ATTRIBUTES) {
      auto ecPackedUnit = genie::EffectCommand();
      ecPackedUnit.Type = genie::EffectCommand::EffectType::AttributeMultiplier;
      ecPackedUnit.UnitClassID = genie::Unit::UnitClass::PackedUnit;
      ecPackedUnit.AttributeID = res;
      ecPackedUnit.Amount = MULTIPLIERS_PACKED_UNIT[age];
      effect.EffectCommands.push_back(ecPackedUnit);

      auto ecPackedTreb = genie::EffectCommand();
      ecPackedTreb.Type = genie::EffectCommand::EffectType::AttributeMultiplier;
      ecPackedTreb.TargetUnit = UNIT_ID_TREBUCHET_PACKED;
      ecPackedTreb.AttributeID = res;
      ecPackedTreb.Amount = MULTIPLIERS_PACKED_TREB[age];
      effect.EffectCommands.push_back(ecPackedTreb);
    }
    tech.EffectID = static_cast<int16_t>(df->Effects.size());
    df->Effects.push_back(effect);
    df->Techs.push_back(tech);
  }
}

// Performs a conversion for some technologies as a step
// towards allowing technologies and units to be queued together.
//
// The basic algorithm proceeds as follows.
// For each technology, create a new unit with the same train location, cost,
// time, etc. as the technology. This unit additionally costs 1 of a unique
// resource, of which each player has 1 to spend. That way the player can queue
// only 1 of the research. When this unit is created, it researches the
// associated technology. The unit is available to create at the same time as
// the tech is available. The unit is disabled when the tech is researched,
// removing it from the menus.
void queueTechsPatch(genie::DatFile* aocDat) {
  bool reserveIds = false;
  int endId = 700; // There are no valid techs after 700, only ones added by WK.
  int numSkipped = 0;
  unordered_map<int, int> techIdToUnitId;
  unordered_map<int, int> techIdEnablers;
  for (auto techId = 0; techId != endId; ++techId) {
    auto idIsSkipped = skipId(techId) || processTech(aocDat, techId, reserveIds, numSkipped, techIdToUnitId, techIdEnablers);
    if (idIsSkipped) { ++numSkipped; }
  }
  auto numAdded = endId - numSkipped;
  if (reserveIds) { numAdded += RESOURCE_ID_NUM_RESERVED; }
  setCivSettings(aocDat, numAdded, techIdEnablers);
  setDiscounts(aocDat, techIdToUnitId);
  setChineseDiscount(aocDat);
}

DatPatch queueTechs = {&queueTechsPatch, "Creates queueable technologies."};

} // namespace wololo
