/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Armin Preiml

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

#ifndef GENIE_BINAFILE_H
#define GENIE_BINAFILE_H

#include "genie/file/ISerializable.h"
#include "PalFile.h"

namespace genie
{

//------------------------------------------------------------------------------
/// Class for loading "bina" files of a drs file.
//
class BinaFile : public ISerializable
{
public:
  BinaFile();
  virtual ~BinaFile();
  
  PalFilePtr readPalFile(std::istream *istr);
  
private:
  virtual void serializeObject(void);
};

typedef std::shared_ptr<BinaFile> BinaFilePtr;

}

#endif // GENIE_BINAFILE_H