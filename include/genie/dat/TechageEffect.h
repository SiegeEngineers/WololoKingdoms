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

#ifndef GENIE_TECHAGEEFFECT_H
#define GENIE_TECHAGEEFFECT_H
#include "genie/file/ISerializable.h"

namespace genie
{

/// Stores the effect type and its parameters
/// See http://www.digitization.org/wiki/index.php/Genie_technologies#Effects
/// for more details
class TechageEffect : public ISerializable
{
public:
  TechageEffect();
  virtual ~TechageEffect();
  virtual void setGameVersion(GameVersion gv);

  int8_t Type = -1;
  int16_t A = -1;
  int16_t B = -1;
  int16_t C = -1;
  float D = 0;

private:
  virtual void serializeObject(void);
};

}

#endif // GENIE_TECHAGEEFFECT_H
