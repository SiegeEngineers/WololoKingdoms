/*
    geniedat - A library for reading and writing data files of genie
               engine games.
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

#ifndef GENIE_UNKNOWN_H
#define GENIE_UNKNOWN_H
#include "genie/file/ISerializable.h"

namespace genie
{

class MapUnknown : public ISerializable
{
public:
  MapUnknown();
  virtual ~MapUnknown();
  virtual void setGameVersion(GameVersion gv);

  int32_t Unknown1 = 0;
  int32_t Unknown2 = 0;
  int32_t Unknown3 = 0;
  int32_t Unknown4 = 0;
  int32_t Unknown5 = 0;
  int32_t Unknown6 = 0;

private:
  virtual void serializeObject(void);
};

class MapUnit : public ISerializable
{
public:
  MapUnit();
  virtual ~MapUnit();
  virtual void setGameVersion(GameVersion gv);

  int32_t Unit = -1;
  int32_t HostTerrain = -1;
  std::vector<int8_t> Unknown3 = {0, 0, 0, 0};
  int32_t ObjectsPerGroup = 1;
  int32_t Fluctuation = 0;
  int32_t GroupsPerPlayer = 1;
  int32_t GroupRadius = 1;
  int32_t OwnAtStart = 0;
  int32_t SetPlaceForAllPlayers = 1;
  int32_t MinDistanceToPlayers = 2;
  int32_t MaxDistanceToPlayers = 6;

private:
  virtual void serializeObject(void);
};

class MapTerrain : public ISerializable
{
public:
  MapTerrain();
  virtual ~MapTerrain();
  virtual void setGameVersion(GameVersion gv);

  int32_t Proportion;
  int32_t Terrain;
  int32_t NumberOfClumps;
  int32_t SpacingToOtherTerrains;
  int32_t PlacementZone;
  int32_t Unknown6;

private:
  virtual void serializeObject(void);
};

class BaseZone : public ISerializable
{
public:
  BaseZone();
  virtual ~BaseZone();
  virtual void setGameVersion(GameVersion gv);

  int32_t Unknown1 = 1;
  int32_t BaseTerrain = -1;
  int32_t SpacingBetweenPlayers = 2;
  int32_t Unknown4 = 7;
  std::vector<int8_t> Unknown5 = {0, 1, 0, 0};
  int32_t Unknown6 = 0;
  int32_t Unknown7 = 0;
  std::vector<int8_t> Unknown8 = {100, 1, 0, 0};
  int32_t StartAreaRadius = 10;
  int32_t Unknown10 = 25;
  int32_t Unknown11 = 8;

private:
  virtual void serializeObject(void);
};

class Map : public ISerializable
{
public:
  Map();
  virtual ~Map();
  virtual void setGameVersion(GameVersion gv);

  int32_t BorderSouthWest = 0;
  int32_t BorderNorthWest = 0;
  int32_t BorderNorthEast = 0;
  int32_t BorderSouthEast = 0;
  int32_t BorderUsage = 0;
  int32_t WaterShape = 10;
  int32_t NonBaseTerrain = -1;
  int32_t BaseZoneCoverage = 80;
  int32_t Unknown9 = 0;

  int32_t BaseZonePointer = 0;
  std::vector<BaseZone> BaseZones;
  int32_t MapTerrainPointer = 0;
  std::vector<MapTerrain> MapTerrains;
  int32_t MapUnitPointer = 0;
  std::vector<MapUnit> MapUnits;
  int32_t MapUnknownPointer = 0;
  std::vector<MapUnknown> MapUnknowns;

private:
  uint32_t BaseZoneCount;
  uint32_t MapTerrainCount;
  uint32_t MapUnitCount;
  uint32_t MapUnknownCount;

  virtual void serializeObject(void);
};

class MapHeader : public ISerializable
{
public:
  MapHeader();
  virtual ~MapHeader();
  virtual void setGameVersion(GameVersion gv);

  int32_t ScriptNumber = -1;
  int32_t BorderSouthWest = 0;
  int32_t BorderNorthWest = 0;
  int32_t BorderNorthEast = 0;
  int32_t BorderSouthEast = 0;
  int32_t BorderUsage = 0;
  int32_t WaterShape = 10;
  int32_t NonBaseTerrain = -1;
  int32_t BaseZoneCoverage = 80;
  int32_t Unknown9 = 0;

  int32_t BaseZonePointer = 0;
  int32_t MapTerrainPointer = 0;
  int32_t MapUnitPointer = 0;
  int32_t MapUnknownPointer = 0;

  /// These *HAVE* to be same as Map class counterparts!
  uint32_t BaseZoneCount;
  uint32_t MapTerrainCount;
  uint32_t MapUnitCount;
  uint32_t MapUnknownCount;

private:
  virtual void serializeObject(void);
};

class RandomMaps : public ISerializable
{
public:
  RandomMaps();
  virtual ~RandomMaps();
  virtual void setGameVersion(GameVersion gv);

  int32_t RandomMapPointer = 0;

  std::vector<MapHeader> MapHeaders;
  std::vector<Map> Maps;

private:
  uint32_t total_randommaps_count;
  std::vector<std::vector<int32_t>> OldMaps;

  virtual void serializeObject(void);
};

}

#endif // GENIE_UNKNOWN_H
