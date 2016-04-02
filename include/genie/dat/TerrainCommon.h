/*
    geniedat - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2015 - 2016  Mikko "Tapsa" P

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

#ifndef GENIE_TERRAINCOMMON_H
#define GENIE_TERRAINCOMMON_H
#include "genie/file/ISerializable.h"

namespace genie
{

class FrameData : public ISerializable
{
public:
  FrameData();
  virtual ~FrameData();
  virtual void setGameVersion(GameVersion gv);

  int16_t FrameCount = 0;
  int16_t AngleCount = 0;
  int16_t ShapeID = 0;

private:
  virtual void serializeObject(void);
};

class SharedTerrain : public ISerializable
{
public:
  static const unsigned short TILE_TYPE_COUNT = 19;

  int8_t Enabled = 0; //must be one or the game will crash
  int8_t Random = 0;

  virtual unsigned short getNameSize(void)=0;

  /// Internal long name
  std::string Name = "";
  /// Internal SLP name
  std::string Name2 = "";
  int32_t SLP = -1;
  int32_t Unknown3 = 0;
  int32_t SoundID = -1;

  std::vector<uint8_t> Colors = {0, 0, 0};

  int8_t IsAnimated = 0;
  int16_t AnimationFrames = 0; // # of frames to animate through
  int16_t PauseFames = 0; // # of frames to pause animation after last frame is drawn
  float Interval = 0; // time between frames
  float PauseBetweenLoops = 0; // time to pause after last frame
  int16_t Frame = 0; // the current frame (includes animation & pause frames)
  int16_t DrawFrame = 0; // the current frame to draw
  float AnimateLast = 0; // last time animation frame was changed
  int8_t FrameChanged = 0; // has the DrawFrame changed since terrain was drawn?
  int8_t Drawn = 0;
};

}

#endif // GENIE_TERRAINCOMMON_H
