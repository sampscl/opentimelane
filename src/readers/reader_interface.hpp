//
//  reader_interface.hpp
//  opentimelane
//
//  Created by Clay Sampson on 3/5/2020.
//  Copyright 3/5/2020 Clay Sampson. All rights reserved.
//
#ifndef _READER_INTERFACE
#define _READER_INTERFACE

#include "../message_handler_interface.hpp"

///
/// @class IReader
/// @brief Interrface to data readers
///
class IReader {
public:
  IReader(){}
  virtual ~IReader(){}

  ///
  /// @brief Initialize
  /// @param[in] handler The message handler for this reader
  ///
  virtual void init(IMessageHandlerInterface *handler) = 0;

  ///
  /// @brief Get a file descriptor for this reader that can be waited on
  /// @return File descriptor for this reader; -1 for no fd
  ///
  virtual int get_fd(void) const = 0;

  ///
  /// @brief Read data; called when there is definitely data available
  /// @return like errno
  ///
  virtual int read_data(void) = 0;
}; // end class IReader

#endif // _READER_INTERFACE
