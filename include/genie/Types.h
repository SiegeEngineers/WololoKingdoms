/*
    genieutils - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011 - 2013  Armin Preiml
    Copyright (C) 2014 - 2016  Mikko "Tapsa" P

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

#ifndef GENIE_TYPES_H
#define GENIE_TYPES_H

namespace genie
{

enum GameVersion
{
  GV_None = 0, //Game version not set
  GV_TEST, // ?
  GV_MIK, // ?
  GV_DAVE, // ?
  GV_MATT, // ?
  GV_AoEB, // 7.04 - 7.11
  GV_AoE, // 7.2
  GV_RoR, // 7.24
  GV_AoKE3, // 9.36
  GV_AoKA, // 10.19
  GV_AoKB, // 11.05
  GV_AoK, // 11.5
  GV_TC, // 11.76 - 12.0
  GV_Cysion, // 12.0
  GV_SWGB, // 1.0
  GV_CC // 1.1
};

struct XYZF
{
  float x, y, z;
};

}

#endif //GENIE_TYPES_H
