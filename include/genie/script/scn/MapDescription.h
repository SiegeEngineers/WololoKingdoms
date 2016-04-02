/*
    genieutils - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011 - 2013  Armin Preiml
    Copyright (C) 2015 - 2016  Mikko "Tapsa" P
    Copyright (C) 2015  JustTesting1234

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

#ifndef GENIE_MAP_DESCRIPTION_H
#define GENIE_MAP_DESCRIPTION_H

#include "genie/file/ISerializable.h"
#include <stdint.h>

namespace genie
{

class MapTile : public ISerializable
{
public:
  MapTile();
  virtual ~MapTile();

  uint8_t terrainID = 0;
  uint8_t elevation = 0;

  /// always 0
  uint8_t unused = 0;

private:
  virtual void serializeObject(void);
};

/// Naming it ScnMap because it may be used elsewhere
class ScnMap : public ISerializable
{
public:
  ScnMap();
  virtual ~ScnMap();

  /// AoK caps at 256
  uint32_t width = 0;

  /// AoK caps at 256
  uint32_t height = 0;

  std::vector<MapTile> tiles;

private:
  virtual void serializeObject(void);
};

}

#endif // GENIE_MAP_DESCRIPTION_H
