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

#ifndef GENIE_PROJECTILE_H
#define GENIE_PROJECTILE_H

#include "genie/file/ISerializable.h"

namespace genie
{

namespace unit
{

class Projectile : public ISerializable
{
public:
  Projectile();
  virtual ~Projectile();
  virtual void setGameVersion(GameVersion gv);

  int8_t StretchMode = 0;
  int8_t SmartMode = 0;
  int8_t DropAnimationMode = 0;

  /// Affects the graphics so that they pass through the target instead of stopping
  /// 1 allows the projectile to pass through, a value of 0 stops the projectile.
  /// Only affects graphic not pass through damage.
  int8_t PenetrationMode = 0;

  int8_t Unknown24 = 0;

  /// Determines the arc a projectile follows.
  /// Enter a non-negative value.
  /// The higher the value, the higher the arc will be that the projectile travels.
  /// The graphics do not change, so a scorpion bolt will still always point forwards,
  /// even if it has a very high arc.
  float ProjectileArc = 0;

protected:
  virtual void serializeObject(void);
};

}

}

#endif // GENIE_PROJECTILE_H
