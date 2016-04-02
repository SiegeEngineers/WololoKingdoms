/*
    genieutils - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011 - 2013  Armin Preiml
    Copyright (C) 2015  Mikko "Tapsa" P
    Copyright (C) 2015  JustTesting1234

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

#ifndef GENIE_SCNFILE_H
#define GENIE_SCNFILE_H

#include <stdint.h>

#include "genie/file/IFile.h"
#include "genie/file/Compressor.h"

#include "scn/ScnPlayerData.h"
#include "scn/MapDescription.h"
#include "scn/Trigger.h"

namespace genie
{

class ScnIncludedFile : public ISerializable
{
public:
  ScnIncludedFile();
  virtual ~ScnIncludedFile();

  std::string perFileName;
  std::string someString;

private:
  virtual void serializeObject(void);
};

//------------------------------------------------------------------------------
/// Class to read and write scenario files (.scn and .scx). The version of the
/// game will be auto-detected on read.
//
class ScnFile : public IFile
{
public:
  ScnFile();
  virtual ~ScnFile();

  //----------------------------------------------------------------------------
  /// Extracts a scenario (for debugging purpose).
  //
  void extractRaw(const char *from, const char *to);

  static uint32_t getSeparator(void);

  std::string version;

  // Uncompressed Header:

  int32_t saveType;

  /// Timestamp of last save
  uint32_t lastSaveTime;

  std::string scenarioInstructions;

  uint32_t victoryType;

  uint32_t playerCount;

  // Compressed header:

  /// aokts description: "Next unit ID to place" ??
  uint32_t nextUnitID;

  ScnMainPlayerData playerData;

  ScnMap map;

  std::vector<ScnPlayerResources> playerResources;
  std::vector<ScnPlayerUnits> playerUnits;

  std::vector<ScnMorePlayerData> players;

  double triggerVersion;
  int8_t objectivesStartingState;
  std::vector<Trigger> triggers;
  std::vector<int32_t> triggerDisplayOrder;

  uint32_t includeFiles;
  uint32_t perErrorIncluded;
  std::vector<uint32_t> perError;
  std::vector<ScnIncludedFile> includedFiles;

private:
  uint32_t headerLength_; //starting after this
  uint32_t playerCount1_;
  uint32_t playerCount2_;
  uint32_t numTriggers_;
  uint32_t fileCount_;

  Compressor compressor_;

  virtual void serializeObject(void);

  void serializeVersion(void);
};

}

#endif // GENIE_SCNFILE_H
