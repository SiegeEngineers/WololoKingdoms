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

#ifndef GENIE_CREATABLE_H
#define GENIE_CREATABLE_H

#include "genie/file/ISerializable.h"
#include "../ResourceUsage.h"

namespace genie
{

namespace unit
{

class Creatable : public ISerializable
{
public:
  Creatable();
  virtual ~Creatable();
  virtual void setGameVersion(GameVersion gv);

  static short getResourceCostsSize();

  typedef ResourceUsage<int16_t, int16_t, int16_t> ResourceCost;

  std::vector<ResourceCost> ResourceCosts;
  int16_t TrainTime = 0;
  int16_t TrainLocationID = -1;
  int8_t ButtonID = 0;
  float Unknown26 = 0;
  float Unknown27 = 0;
  int8_t CreatableType = 0;
  int8_t HeroMode = 0;
  int32_t GarrisonGraphic = -1; 
  float TotalProjectiles = 0;
  int8_t MaxTotalProjectiles = 0;

  /// The area from which projectiles spawn. Size and randomness.
  std::vector<float> ProjectileSpawningArea = {0, 0, 0};

  int32_t SecondaryProjectileUnit = -1;
  int32_t SpecialGraphic = -1;

  //TODO:
  /*
   * 1.Determines adjacent unit graphics
    2.Enter 0 or 1
    3.If “1”, the building will have the capability to change graphics depending
    on the buildings adjacent to it. For instance, a single wall will be a
    wall end piece, but if a wall is placed between two other walls, it will
    have a different graphic.
   */
  int8_t SpecialAbility = 0;
  int16_t DisplayedPierceArmour = 0;

protected:
  virtual void serializeObject(void);

};

}

}

#endif // GENIE_CREATABLE_H
