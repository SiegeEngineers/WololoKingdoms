/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Armin Preiml
    Copyright (C) 2015  Mikko "Tapsa" P

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

#ifndef GENIE_SLPFILE_H
#define GENIE_SLPFILE_H

#include <istream>
#include <vector>

#include "genie/file/IFile.h"
#include "genie/util/Logger.h"
#include "PalFile.h"
#include "SlpFrame.h"

namespace genie
{

//------------------------------------------------------------------------------
/// A slp file stores one or several images encoded using simple commands.
/// The image is stored as 8 bits per pixel, that means only the index of a
/// color in a palette is saved.
//
class SlpFile : public IFile
{

public:
  //----------------------------------------------------------------------------
  /// Constructor
  //
  SlpFile();

  //----------------------------------------------------------------------------
  /// Destructor
  //
  virtual ~SlpFile();

  //----------------------------------------------------------------------------
  /// Frees all content of a slp file.
  //
  void unload(void);

  //----------------------------------------------------------------------------
  /// Check whether the files content is loaded or not.
  //
  bool isLoaded(void) const;

  //----------------------------------------------------------------------------
  /// Return number of frames stored in the file. Available after load.
  ///
  /// @return number of frames
  //
  uint32_t getFrameCount();

  //----------------------------------------------------------------------------
  /// Returns the internal slp id.
  ///
  /// @return id
  //
//   int32_t getId();

  //----------------------------------------------------------------------------
  /// Returns the slp frame at given frame index.
  ///
  /// @param frame frame index
  /// @return SlpFrame
  //
  SlpFramePtr getFrame(uint32_t frame=0);

  std::string version;
  std::string comment;

private:
  static Logger &log;

  bool loaded_ = false;

  uint32_t id_;
  uint32_t len_;

  uint32_t num_frames_ = 0;

  typedef std::vector<SlpFramePtr> FrameVector;
  FrameVector frames_;

  // Used to calculate offsets when saving the SLP.
  uint32_t slp_offset_;

  //----------------------------------------------------------------------------
  virtual void serializeObject(void);

  //----------------------------------------------------------------------------
  /// Loads the file and its frames.
  //
  void loadFile(void);
  void saveFile(void);

  //----------------------------------------------------------------------------
  void serializeHeader(void);
};

typedef std::shared_ptr<SlpFile> SlpFilePtr;

}

#endif // GENIE_SLPFILE_H
