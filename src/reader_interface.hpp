//
//  reader_interface.hpp
//  opentimelane
//
//  Created by Clay Sampson on 3/5/2020.
//  Copyright 3/5/2020 Clay Sampson. All rights reserved.
//
#ifndef _READER_INTERFACE
#define _READER_INTERFACE

///
/// @class IReader
/// @brief Interrface to data readers
///
class IReader {
public:
  IReader(){}
  virtual ~IReader(){}

  ///
  /// @brief Handle receipt of data
  /// @param[in] data Pointer to data, cannot be null unless len is 0
  /// @param[in] len Length, bytes, of data; can be 0
  /// @return like errno
  ///
  virtual int on_data(const void *data, size_t len) = 0;
}; // end class IReader

#endif // _READER_INTERFACE
