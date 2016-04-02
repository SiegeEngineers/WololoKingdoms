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

#ifndef GENIE_UNITCOMMAND_H
#define GENIE_UNITCOMMAND_H
#include "genie/file/ISerializable.h"

namespace genie
{

class UnitCommand : public ISerializable
{
public:
  UnitCommand();
  virtual ~UnitCommand();
  virtual void setGameVersion(GameVersion gv);

  int16_t Enabled = 1;
  int16_t ID = -1;
  int8_t Unknown1 = 0;
  int16_t Type = -1;
  int16_t ClassID = -1;
  int16_t UnitID = -1;
  int16_t TerrainID = -1;
  int16_t ResourceIn = -1;
  int16_t ResourceProductivityMultiplier = -1;
  int16_t ResourceOut = -1;
  int16_t Resource = -1;
  float Quantity = 0;
  float ExecutionRadius = 0;
  float ExtraRange = 0;
  int8_t Unknown4 = 1;
  float Unknown5 = 3;
  int8_t SelectionEnabler = 1;
  int8_t Unknown7 = 1;
  int16_t PlunderSource = 1;
  int16_t Unknown9 = 0;
  int8_t SelectionMode = 5;
  int8_t RightClickMode = 0;
  int8_t Unknown12 = 0;
  int16_t ToolGraphicID = -1;
  int16_t ProceedingGraphicID = -1;
  int16_t ActionGraphicID = -1;
  int16_t CarryingGraphicID = -1;
  int16_t ExecutionSoundID = -1;
  int16_t ResourceDepositSoundID = -1;

private:
  virtual void serializeObject(void);
};

}

#endif // GENIE_UNITCOMMAND_H
