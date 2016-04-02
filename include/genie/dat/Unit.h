/*
    geniedat - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011 - 2013  Armin Preiml
    Copyright (C) 2011 - 2016  Mikko "Tapsa" P

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GENIE_UNIT_H
#define GENIE_UNIT_H
#include "genie/file/ISerializable.h"
#include "unit/DamageGraphic.h"
#include "UnitCommand.h"
#include "unit/AttackOrArmor.h"
#include "unit/DeadFish.h"
#include "unit/Bird.h"
#include "unit/Type50.h"
#include "unit/Projectile.h"
#include "unit/Creatable.h"
#include "unit/Building.h"
#include "ResourceUsage.h"

namespace genie
{

/// Available unit types
enum UnitType
{
  /// Basic units like rubble and flares.
  UT_EyeCandy = 10,

  /// Trees, used to be 90?
  UT_Trees = 15,

  /// With Speed but mostly flags.
  UT_Flag = 20,

  /// Only one unit has this type. AOK, DOPL (id 243) same properties as UT_Flag
  UT_25 = 25,

  /// Dead and fish units. It seems to be unused in SWGB as units just explode
  /// and do not leave carcasses.
  UT_Dead_Fish = 30,

  /// Only birds in aoe and ror are of this type.
  UT_Bird = 40,

  /// ???
  UT_Unknown = 50,

  /// Projectiles
  UT_Projectile = 60,

  /// Units that can be created or trained like Army, Villagers and Ships.
  UT_Creatable = 70,

  /// Buildings
  UT_Building = 80,

  /// Trees in aoe and ror are of this type
  UT_AoeTrees = 90
};

//------------------------------------------------------------------------------
/// Stores properties of genie units.
//
class Unit : public ISerializable
{
public:
  Unit();
  virtual ~Unit();
  virtual void setGameVersion(GameVersion gv);

  /// See enum UnitType
  int8_t Type = 10;

  int16_t ID1 = -1;

  /// ID of the name string stored in the language.dll file
  uint16_t LanguageDLLName = 5000;

  /// ID of the name string stored in the language.dll file
  uint16_t LanguageDLLCreation = 6000;

  /// Class that gives certain properties
  int16_t Class = -1;

  /// Default standing graphic
  std::pair<int16_t, int16_t> StandingGraphic = {-1, -1};

  /// Graphic shown while the units is dying.
  std::pair<int16_t, int16_t> DyingGraphic = {-1, -1};

  /// TODO
  int8_t DeathMode = 0;

  /// Unit's hit points
  int16_t HitPoints = 1;

  /// Default line of sight measured in tiles
  float LineOfSight = 2;

  /// Maximum number of units that can be garrisonned at the same time.
  int8_t GarrisonCapacity = 0;

  /// Collision detection area taken by the unit.
  /// No other unit can move into this area except flying units.
  XYZF CollisionSize = {0, 0, 0};

  /// Sound played when the unit is created
  std::pair<int16_t, int16_t> TrainSound = {-1, -1};

  /// ID of the unit to change to when this one dies.
  int16_t DeadUnitID = -1;

  /// TODO
  int8_t PlacementMode = 0;

  /// TODO
  int8_t AirMode = 0;

  /// ID of the icon shown at the bottom when selected or when building
  int16_t IconID = -1;

  /// If true, unit can not be placed in the editor
  int8_t HideInEditor = 0;

  /// TODO Unknown
  int16_t Unknown1 = -1;         // Always -1

  /// TODO
  int8_t Enabled = 1;
  int8_t Disabled = 0;
  std::pair<int16_t, int16_t> PlacementSideTerrain = {-1, -1};
  std::pair<int16_t, int16_t> PlacementTerrain = {-1, -1};
  std::pair<float, float> ClearanceSize = {0, 0};
  int8_t HillMode = 0;
  int8_t VisibleInFog = 0;

  /// ID of terrain restrictions that are imposed on the unit.
  int16_t TerrainRestriction = -1;

  /// Bool which determines wheter the unit can fly.
  int8_t FlyMode = 0;

  /// How much resources this unit is able to carry
  int16_t ResourceCapacity = 0;

  // TODO
  float ResourceDecay = 0;
  int8_t BlastDefenseLevel = 0;
  int8_t SubType = 0;
  int8_t InteractionMode = 0;

  /**
   * \brief Defines if and how a unit is shown on the minimap.
   *
   * Following values are known to work:
   *
   * - 0 does not place a dot on the mini-map. Eye candy, resources,
   *     fish traps, OUTLWs, and farms have this value
   * - 1 places a square dot on the min-map for the unit. When the unit is
   *     selected, the dot will turn white
   * - 2 places a diamond-shaped dot on the mini-map for the unit. When the unit
   *     is selected, the dot will turn white
   * - 3 places a diamond-shaped dot on the mini-map for the unit. When the unit
   *     is selected, the dot remains the player's color.
   * - 4 places a larger black spot on the minimap which does not follow the
   *     unit. Whenever you open the chat box (enter) and close it, the black
   *     spots will move to the unit on the minimap. No blinking occurs when
   *     the unit is attacked. Both allied and enemy units are visible in the
   *     black area on the minimap. CLF units have this value and a brown spot
   *     is placed instead.
   * - 5 does the same thing as “4”
   * - 6 does not place a dot on the minimap
   * - 7-9 does the same thing as “6”
   * - 10 does the same thing as “6”. Hawks, macaws, and flying dogs have
   *      this value.
   */
  int8_t MinimapMode = 0;

  int8_t CommandAttribute = 0;// Page for Build button: 2 = Page 1, 10 = Page 2, ?11 = Page 3?
  float Unknown3A = 0;
  uint8_t MinimapColor = 0;
  int32_t LanguageDLLHelp = 105000;
  int32_t LanguageDLLHotKeyText = 155000;
  int32_t HotKey = 16000;
  int8_t Unselectable = 0;
  int8_t Unknown6 = 0;
  int8_t Unknown7 = 0;
  int8_t Unknown8 = 0;
  int8_t SelectionMask = 0;

  /// values for SelectionShapeType
  /// 0 farm, gate, dead bodies, town center
  /// 2 buildings, gold mine
  /// 3 berserk, flag x
  /// 5 units
  /// 10 mountain(matches selction mask)
  int8_t SelectionShapeType = 0;

  /// 0 square, 1+ circle
  int8_t SelectionShape = 0;
  uint8_t Attribute = 0;
  /// Seems to be used only in SWGB/CC
  int8_t Civilization = 0;
  int16_t Nothing = 0;

  /**
   * 1.HP Bar on, selection shadow (permanent darker outline in editor only, disappears in game)
   * 2.HP Bar on, normal outline
   * 3.HP Bar off, selection shadow
   * 4.HP Bar off, normal outline
   * Note: this doe not work with all units
   */
  int8_t SelectionEffect = 1;
  uint8_t EditorSelectionColour = 52;
  XYZF SelectionShapeSize = {0, 0, 0};

  typedef ResourceUsage<int16_t, float, int8_t> ResourceStorage;

  /// Resource cost of a unit
  std::vector<ResourceStorage> ResourceStorages;

  std::vector<unit::DamageGraphic> DamageGraphics;

  /// Sound that is played when this unit is selected
  int16_t SelectionSound = -1;
  int16_t DyingSound = -1;
  int8_t AttackMode = 0;
  int8_t Unknown10 = 0;
  std::string Name = "";
  std::string Name2 = "";

  /// MinGameVersion: SWGB
  int16_t Unitline = -1;

  /// MinGameVersion: SWGB
  int8_t MinTechLevel = -1;

  int16_t ID2 = -1;
  int16_t ID3 = -1;//not in aoe/ror

//      Type 20+

  float Speed = 0;

  unit::DeadFish DeadFish;

  unit::Bird Bird;

  unit::Type50 Type50;

  unit::Projectile Projectile;

  unit::Creatable Creatable;

  unit::Building Building;

private:
  /// Length of the internal name
  uint16_t NameLength;
  uint16_t NameLength2;
  uint8_t DamageGraphicCount;

protected:
  virtual void serializeObject(void);
};

}

#endif // GENIE_UNIT_H
