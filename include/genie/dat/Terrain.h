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

#ifndef GENIE_TERRAIN_H
#define GENIE_TERRAIN_H
#include "genie/file/ISerializable.h"
#include "genie/dat/TerrainCommon.h"

namespace genie
{

class Terrain : public SharedTerrain
{
public:
  Terrain();
  virtual ~Terrain();
  virtual void setGameVersion(GameVersion gv);
  static unsigned short getTerrainCount(GameVersion gv);
  static void setTerrainCount(unsigned short cnt);

  int16_t Unknown1 = 0;

  unsigned short getNameSize(void);

  int32_t BlendPriority = -1;//not in aoe/ror
  int32_t BlendType = -1; //not in aoe/ror

  std::pair<uint8_t, uint8_t> CliffColors = {0, 0};
  int8_t PassableTerrain = -1;
  int8_t ImpassableTerrain = -1;

  std::vector<FrameData> ElevationGraphics;

  int16_t TerrainToDraw = 0;
  std::pair<int16_t, int16_t> TerrainDimensions = {0, 0}; // rows + cols

  /// These refer to terrain borders, which are actually used only in AoE and RoR.
  std::vector<int16_t> Borders;

  static const unsigned short TERRAIN_UNITS_SIZE = 30;
  std::vector<int16_t> TerrainUnitID;
  std::vector<int16_t> TerrainUnitDensity;

  /// If two terrain units are to be placed on same spot, this selects which one will prevail others.
  /// 1 = prevails, others don't.
  std::vector<int8_t> TerrainUnitPriority;

  int16_t NumberOfTerrainUnitsUsed = 0;

private:
  virtual void serializeObject(void);

  static unsigned short terrain_count;
};

}

#endif // GENIE_TERRAIN_H
