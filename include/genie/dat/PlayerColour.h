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

#ifndef GENIE_PLAYERCOLOUR_H
#define GENIE_PLAYERCOLOUR_H
#include "genie/file/ISerializable.h"

namespace genie
{

class PlayerColour : public ISerializable
{
public:
  PlayerColour();
  virtual ~PlayerColour();
  virtual void setGameVersion(GameVersion gv);

  static const unsigned short NAME_SIZE = 30;
  std::string Name = "";
  int32_t ID = -1;

  int32_t Palette = 0;
  int32_t Colour = 0;
  int32_t Unknown1 = 0;
  int32_t Unknown2 = 0;
  int32_t MinimapColour = 0;
  int32_t Unknown3 = 0;
  int32_t Unknown4 = 0;
  int32_t StatisticsText = 0;

private:
  virtual void serializeObject(void);
};

}

#endif // GENIE_PLAYERCOLOUR_H
