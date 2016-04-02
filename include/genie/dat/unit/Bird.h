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

#ifndef GENIE_BIRD_H
#define GENIE_BIRD_H

#include "genie/file/ISerializable.h"
#include "../UnitCommand.h"

namespace genie
{

namespace unit
{

class Bird : public ISerializable
{
public:
  Bird();
  virtual ~Bird();
  virtual void setGameVersion(GameVersion gv);

  int16_t ActionWhenDiscoveredID = -1;
  float SearchRadius = 0;
  float WorkRate = 0;
  std::pair<int16_t, int16_t> DropSite = {-1, -1};

  /// If activated unit switches villager types
  int8_t TaskSwapID = 0;
  int16_t AttackSound = -1;
  int16_t MoveSound = -1;
  /// Some animals have this.
  int8_t AnimalMode = 0;
  std::vector<UnitCommand> Commands;//only in aoe/ror

private:
  uint16_t CommandCount;//only in aoe/ror

protected:
  virtual void serializeObject(void);
};

}

}

#endif // GENIE_BIRD_H
