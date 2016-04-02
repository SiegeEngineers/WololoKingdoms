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

#ifndef GENIE_DRSFILE_H
#define GENIE_DRSFILE_H

#include <vector>
#include <list>
#include <map>
#include <stdint.h>

#include "genie/file/IFile.h"
#include "SlpFile.h"
#include "BinaFile.h"

namespace genie
{

class Logger;

//------------------------------------------------------------------------------
/// Base class for .drs files
//
class DrsFile : public IFile
{

public:
  //----------------------------------------------------------------------------
  /// Default Constructor.
  //
  DrsFile();

  //----------------------------------------------------------------------------
  /// Destructor
  //
  virtual ~DrsFile();

  //----------------------------------------------------------------------------
  /// Get a shared pointer to a slp file.
  ///
  /// @param id resource id
  /// @return slp file pointer or "empty" shared pointer if not found
  //
  SlpFilePtr getSlpFile(uint32_t id);

  //----------------------------------------------------------------------------
  /// Get a shared pointer to a color palette file.
  ///
  /// @param id resource id
  /// @return bina file pointer or "empty" shared pointer if not found
  //
  PalFilePtr getPalFile(uint32_t id);

  unsigned char* getWavPtr(uint32_t id);

private:
  static Logger &log;

  bool header_loaded_ = false;

  uint32_t num_of_tables_;
  uint32_t header_offset_;

  std::vector<std::string> table_types_;
  std::vector<uint32_t> table_num_of_files_;
  std::list<uint32_t> loaded_slp_ids_;
  std::map<uint32_t, uint32_t> wav_offsets_;
  std::vector<uint8_t> wav_file_;

  typedef std::map<uint32_t, SlpFilePtr> SlpMap;
  SlpMap slp_map_;

  typedef std::map<uint32_t, BinaFilePtr> BinaMap;
  BinaMap bina_map_;

  unsigned int getCopyRightHeaderSize(void) const;

  std::string getSlpTableHeader(void) const;
  std::string getBinaryTableHeader(void) const;
  std::string getSoundTableHeader(void) const;

   //----------------------------------------------------------------------------
  /// Loads table and resource headers.
  //
  void loadHeader();

  virtual void serializeObject(void);
};

}

#endif // GENIE_DRSFILE_H
