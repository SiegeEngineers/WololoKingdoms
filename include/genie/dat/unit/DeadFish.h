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

#ifndef GENIE_DEADFISH_H
#define GENIE_DEADFISH_H

#include "genie/file/ISerializable.h"

namespace genie
{

namespace unit
{

/// Attributes for Unit of type >= DeadFish.
class DeadFish : public ISerializable
{
public:
  DeadFish();
  virtual ~DeadFish();
  virtual void setGameVersion(GameVersion gv);

  std::pair<int16_t, int16_t> WalkingGraphic = {-1, -1};
  float RotationSpeed = 0;
  int8_t Unknown11 = 0;
  int16_t TrackingUnit = -1;
  int8_t TrackingUnitUsed = 0;
  float TrackingUnitDensity = 0;
  int8_t Unknown16 = 0;

  std::vector<float> RotationAngles = {0, 3.40282347e+38, 3.40282347e+38, 0, 3.40282347e+38};

protected:
  virtual void serializeObject(void);
};

}

}

#endif // GENIE_DEADFISH_H
