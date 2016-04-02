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

#ifndef GENIE_SCN_RESOURCE_H
#define GENIE_SCN_RESOURCE_H

#include "genie/file/ISerializable.h"

#include <stdint.h>

namespace genie
{

class ScnUnit : public ISerializable
{
public:
  ScnUnit();
  virtual ~ScnUnit();

  float positionX;
  float positionY;
  float positionZ;
  uint32_t spawnID;
  uint16_t objectID;
  uint8_t state;
  float rotation;
  uint16_t initAnimationFrame;
  uint32_t garrisonedInID;

private:
  virtual void serializeObject(void);
};

class ScnPlayerResources : public ISerializable
{
public:
  ScnPlayerResources();
  virtual ~ScnPlayerResources();

  float food = 0;
  float wood = 0;
  float gold = 0;
  float stone = 0;
  float ore = 0;
  float goods = 0;
  float popLimit = 75;

private:
  virtual void serializeObject(void);
};

class ScnPlayerUnits : public ISerializable
{
public:
  ScnPlayerUnits();
  virtual ~ScnPlayerUnits();

  std::vector<ScnUnit> units;

private:
  uint32_t unitCount_;
  virtual void serializeObject(void);
};

}

#endif // GENIE_SCN_RESOURCE_H
