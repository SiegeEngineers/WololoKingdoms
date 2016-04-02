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

#ifndef GENIE_GRAPHIC_H
#define GENIE_GRAPHIC_H
#include "genie/file/ISerializable.h"
#include "GraphicDelta.h"
#include "GraphicAttackSound.h"

namespace genie
{

class Graphic : public ISerializable
{
public:
  Graphic();
  virtual ~Graphic();
  virtual void setGameVersion(GameVersion gv);

  /// Returns the size of Name
  unsigned short getNameSize(void);

  /// These two name strings appear to be equal in all graphics, so the
  /// second one's use is unknown. Like the genie unit's internal name value,
  /// they are heavily abbreviated, such as "ARRG2NNE" meaning "Archery
  /// Range Fuedal Age (orth) European"
  std::string Name = "";

  /// Returns the size of Name2
  unsigned short getName2Size(void);

  /// See Name
  std::string Name2 = "";

  /// SLP resource id
  int32_t SLP = -1;

  //TODO
  int8_t Unknown1 = 0;
  int8_t Unknown2 = 0;

  /// The layer describes order of graphics being rendered.
  /// Possible values: 0 (lowest layer) to 40 (highest layer)
  /// Graphics on a higher layer will be rendered above graphics of a lower
  /// layer. If graphics share the same layer, graphics will be displayed
  /// dependend on their map positions.
  ///
  int8_t Layer = 0;

  /// Setting this to 0 forces the player color to be blue.
  int8_t PlayerColor = -1;
  int8_t Rainbow = -1;

  /// Will the graphic be looped.
  int8_t Replay = 0;

  /// TODO: What kind of coordinates?
  std::vector<int16_t> Coordinates = {0, 0, 0, 0};

  int16_t SoundID = -1;
  int8_t AttackSoundUsed = 0;

  /// Number of frames per angle animation
  uint16_t FrameCount = 0;

  /// Number of angles tored in slp and also the number of extra structures.
  /// If there are more than 1 angle, AngleCount/2 - 1 frames will be
  /// mirrored. That means angles starting from south going clockwise to
  /// north are stored and the others will be mirrored.
  uint16_t AngleCount = 0;

  /// If this is over 0, the speed of the unit will be replaced with this.
  float NewSpeed = 0;

  /// Frame rate in seconds. (Delay between frames)
  float FrameRate = 0;

  /// Time to wait until the animation sequence is started again.
  float ReplayDelay = 0;

  int8_t SequenceType = 0;
  int16_t ID = -1;
  int8_t MirroringMode = 0;
  int8_t Unknown3 = 0;

  std::vector<GraphicDelta> Deltas;
  std::vector<GraphicAttackSound> AttackSounds;

private:
  uint16_t DeltaCount;

  static const unsigned short NAME_SIZE = 21;
  static const unsigned short NAME_SIZE2 = 13;
  static const unsigned short NAME_SIZE_SWGB = 25;

  virtual void serializeObject(void);
};

}

#endif // GENIE_GRAPHIC_H
