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

/*
 *  TODO: Config file for logger (loglevel, log method)
 */

#ifndef GENIE_LOGGER_H
#define GENIE_LOGGER_H

#include <string>
#include <stdarg.h>

namespace genie
{

//------------------------------------------------------------------------------
/// Simple logging class
//
class Logger {

public:
  enum LogLevel 
  {
    L_INFO    = 0, // Informational messages that highlight the process
    L_DEBUG   = 1, // Messages that are most useful for debugging
    L_WARNING = 2, // Messages that designate potential harmful situations
    L_ERROR   = 3, // Malfunctions which allows the application to continue 
    L_FATAL   = 4, // Malfunctions that break the application
    L_OFF     = 5  // No messages at all
  };
  
  //----------------------------------------------------------------------------
  /// Returns the static root logger.
  ///
  /// @return root logger
  //
  static Logger& getRootLogger(void);
  
  //----------------------------------------------------------------------------
  /// Returns specific logger that prints additionally its name before log info
  /// TODO
  ///
  /// @return specific logger
  //
  static Logger& getLogger(std::string name);

  //----------------------------------------------------------------------------
  /// Set the overall loglevel to print. All lower log levels will be shown.
  /// E.g. if loglevel is L_WARNING, then messages with level L_ERROR and
  /// L_FATAL will also be shown.
  ///
  /// @param loglevel minimum level to log
  //
  static void setLogLevel(LogLevel loglevel);
  
  static std::ostream* getGlobalOutputStream(void);
  static void setGlobalOutputStream(std::ostream &ostream);

  //----------------------------------------------------------------------------
  /// Logs messages which aren't very important, but they highlight the 
  /// program status.
  ///
  /// @param msg message to print
  //
  void info(const char *msg, ...);
  
  //----------------------------------------------------------------------------
  /// Messages crucial for debugging can be printed using this method.
  ///
  /// @param msg message to print
  //
  void debug(const char *msg, ...);
  
  //----------------------------------------------------------------------------
  /// Message that could interfere the programms behaviour should be printed
  /// using this method.
  ///
  /// @param msg message to print
  //
  void warn(const char *msg, ...);
  
  //----------------------------------------------------------------------------
  /// Method for printing information about a malfunction which are serious
  /// but lets the programm continue.
  ///
  /// @param msg message to print
  //
  void error(const char *msg, ...);
  
  //----------------------------------------------------------------------------
  /// Prints messages that crashes the program.
  ///
  /// @param msg message to print
  //
  void fatal(const char *msg, ...);

protected:
  void log(LogLevel loglevel, va_list vlist, const char *msg);
  void log(LogLevel loglevel, const char *msg, ...);
  
  std::string getLogLevelName(LogLevel loglevel);

private:
  Logger();
  virtual ~Logger();
  
  static std::ostream *global_out_;
  std::ostream *out_;

private:
  static LogLevel LOG_LEVEL;

};

}

#endif // GENIE_LOGGER_H
