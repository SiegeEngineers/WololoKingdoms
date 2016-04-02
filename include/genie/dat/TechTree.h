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

#ifndef GENIE_TECHTREE_H
#define GENIE_TECHTREE_H
#include "genie/file/ISerializable.h"

namespace genie
{

class TechTreeAge;
class BuildingConnection;
class UnitConnection;
class ResearchConnection;

class TechTree : public ISerializable
{
public:
  TechTree();
  virtual ~TechTree();
  virtual void setGameVersion(GameVersion gv);

  int32_t Unknown2 = 1; //1

  std::vector<TechTreeAge> TechTreeAges;

  std::vector<BuildingConnection> BuildingConnections;

  std::vector<UnitConnection> UnitConnections;

  std::vector<ResearchConnection> ResearchConnections;

  static unsigned short getCount();

private:
  uint8_t age_count_;
  uint8_t total_building_count_;
  uint16_t total_unit_count_; // int16_t in >= swgb
  uint8_t total_research_count_;

  virtual void serializeObject(void);

};

namespace techtree
{

class Common : public ISerializable
{
public:
  Common() {}

  virtual ~Common() {}

  virtual void setGameVersion(GameVersion gv)
  {
    ISerializable::setGameVersion(gv);
    UnitResearch.resize(getSlots());
    Mode.resize(getSlots());
  }

  int32_t SlotsUsed = 0;

  /// Connection lines when selected
  std::vector<int32_t> UnitResearch;

  /// 0 Age/Tech-level, 1 Building, 2 Unit, 3 Research.
  std::vector<int32_t> Mode;

  unsigned short getSlots()
  {
    if (getGameVersion() >= genie::GV_SWGB)
      return 20;
    else if (getGameVersion() >= genie::GV_AoKB)
      return 10;
    else
      return 5;
  }

private:
  virtual void serializeObject(void) // 84 bytes, 164 in SWGB
  {
    serialize<int32_t>(SlotsUsed);
    serialize<int32_t>(UnitResearch, getSlots());
    serialize<int32_t>(Mode, getSlots());
  }
};

}

/// Contains all items of an age in techtree
class TechTreeAge : public ISerializable
{
public:
  TechTreeAge();
  virtual ~TechTreeAge();
  virtual void setGameVersion(GameVersion gv);

  int32_t ID = -1; //Age ID?
  int8_t Unknown2 = 2; //Always 2

  std::vector<int32_t> Buildings;
  std::vector<int32_t> Units;
  std::vector<int32_t> Researches;

  techtree::Common Common;
  int8_t SlotsUsed = 0;
  std::vector<int8_t> Unknown4;
  std::vector<int8_t> Unknown5;
  int8_t Unknown6 = 0;
  int32_t LineMode = 0;

  unsigned short getU4Size();

private:
  uint8_t building_count_;
  uint8_t unit_count_;
  uint8_t research_count_;

  virtual void serializeObject(void);
};

class BuildingConnection : public ISerializable
{
public:
  BuildingConnection();
  virtual ~BuildingConnection();
  virtual void setGameVersion(GameVersion gv);

  int32_t ID = -1;
  int8_t Unknown1 = 2; //always 2

  std::vector<int32_t> Buildings;
  std::vector<int32_t> Units;
  std::vector<int32_t> Researches;

  techtree::Common Common;

  static const unsigned short AGES = 5;
  int8_t LocationInAge = 0;
  std::vector<int8_t> UnitsTechsTotal;
  std::vector<int8_t> UnitsTechsFirst;

  /// 5 One or more connections, 6 No connections.
  int32_t LineMode = 0;
  /// Makes available. Used by buildings, which need a research to be available.
  int32_t EnablingResearch = 0;

private:
  uint8_t building_count_;
  uint8_t unit_count_;
  uint8_t research_count_;

  virtual void serializeObject(void);
};

class UnitConnection : public ISerializable
{
public:
  UnitConnection();
  virtual ~UnitConnection();
  virtual void setGameVersion(GameVersion gv);

  int32_t ID = -1;
  int8_t Unknown1 = 2; //always 2
  int32_t UpperBuilding = -1;

  techtree::Common Common;

  int32_t VerticalLine = -1;

  std::vector<int32_t> Units;

  /// 1 First, 2 Second.
  int32_t LocationInAge = 0;
  /// Upgrades unit. Used by units, which aren't first in upgrade line.
  int32_t RequiredResearch = -1;
  /// 2 First in vertical line. 3 Not first.
  int32_t LineMode = 0;
  /// Makes available. Used by units, which need a research to be available.
  int32_t EnablingResearch = -1;

private:
  uint8_t unit_count_;

  virtual void serializeObject(void);

};

class ResearchConnection : public ISerializable
{
public:
  ResearchConnection();
  virtual ~ResearchConnection();
  virtual void setGameVersion(GameVersion gv);

  int32_t ID = -1;
  int8_t Unknown1 = 2; //always 2
  int32_t UpperBuilding = -1;

  std::vector<int32_t> Buildings;
  std::vector<int32_t> Units;
  std::vector<int32_t> Researches;

  techtree::Common Common;

  int32_t VerticalLine = 0;
  /// 0 Hidden, 1 First, 2 Second.
  int32_t LocationInAge = 0;
  /// 0 First Age. Others.
  int32_t LineMode = 0;

private:
  uint8_t building_count_;
  uint8_t unit_count_;
  uint8_t research_count_;

  virtual void serializeObject(void);
};

}

#endif // GENIE_TECHTREE_H
