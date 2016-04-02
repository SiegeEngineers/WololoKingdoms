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

#ifndef GENIE_TYPE50_H
#define GENIE_TYPE50_H

#include "genie/file/ISerializable.h"
#include "AttackOrArmor.h"

namespace genie
{

namespace unit
{

class Type50 : public ISerializable
{
public:
  Type50();
  virtual ~Type50();
  virtual void setGameVersion(GameVersion gv);

  int16_t DefaultArmor = 1000; // uint8_t below TC
  std::vector<unit::AttackOrArmor> Attacks;
  std::vector<unit::AttackOrArmor> Armours;
  int16_t TerRestrictionForDmgMultiplying = -1;
  float MaxRange = 0;
  float BlastWidth = 0;
  float ReloadTime = 0;
  int16_t ProjectileUnitID = -1;

  /// Percentage value determining the probability of an attack hiting
  int16_t AccuracyPercent = 0;

  /// Used in AoE/RoR for towers.
  int8_t TowerMode = 0;
  int16_t FrameDelay = 0;

  std::vector<float> GraphicDisplacement = {0, 0, 0};

  int8_t BlastAttackLevel = 0;
  float MinRange = 0;
  float AccuracyDispersion = 0;
  int16_t AttackGraphic = -1;
  int16_t DisplayedMeleeArmour = 0;
  int16_t DisplayedAttack = 0;
  float DisplayedRange = 0;
  float DisplayedReloadTime = 0;

private:
  uint16_t AttackCount;
  uint16_t ArmourCount;

protected:
  virtual void serializeObject(void);
};

}

}

#endif // GENIE_TYPE50_H
