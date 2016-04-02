/*
    genieutils - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011 - 2013  Armin Preiml
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

#ifndef GENIE_IFILE_H
#define GENIE_IFILE_H

#include "ISerializable.h"

#include <boost/utility.hpp>
#include <fstream>

namespace genie
{

//------------------------------------------------------------------------------
/// Interface providing file loading and saving for ISerializable objects.
//
class IFile : public ISerializable, boost::noncopyable
{
public:

  //----------------------------------------------------------------------------
  IFile();

  //----------------------------------------------------------------------------
  virtual ~IFile();
  void freelock(void);

  //----------------------------------------------------------------------------
  /// Sets name of file to work with.
  ///
  /// @param fileName.
  //
  void setFileName(const char *fileName);

  //----------------------------------------------------------------------------
  /// Get name of file worked on.
  ///
  /// @return fileName.
  //
  const char *getFileName(void) const;

  //----------------------------------------------------------------------------
  /// Loads the object from file. Can only be called if fileName is already set.
  ///
  /// @exception std::ios_base::failure thrown if file can't be read (file
  ///                                   doesn't exist, insufficient rights...)
  ///
  //
  void load(void ) throw (std::ios_base::failure);

  //----------------------------------------------------------------------------
  /// Loads the object from file.
  ///
  /// @param fileName file name
  /// @exception std::ios_base::failure thrown if file can't be read (file
  ///                                   doesn't exist, insufficient rights...)
  //
  virtual void load(const char *fileName) throw (std::ios_base::failure);

  //----------------------------------------------------------------------------
  /// Saves data to file. Can only be called if fileName is set through set
  /// method or the file was loaded using the load method.
  ///
  /// @exception std::ios_base::failure thrown if file can't be written (
  ///                                   insufficient rights...)
  //
  void save(void ) throw (std::ios_base::failure);

  //----------------------------------------------------------------------------
  /// Saves data to a different file.
  ///
  /// @param fileName file name
  /// @exception std::ios_base::failure thrown if file can't be written (
  ///                                   insufficient rights...)
  //
  virtual void saveAs(const char *fileName) throw (std::ios_base::failure);

protected:

  //----------------------------------------------------------------------------
  /// The unload method will be called before loading an object, but only
  /// if the object was already loaded once.
  //
  virtual void unload(void);

private:
  std::string fileName_;

  std::ifstream fileIn_;

  bool loaded_ = false;
};

}

#endif // GENIE_IFILE_H