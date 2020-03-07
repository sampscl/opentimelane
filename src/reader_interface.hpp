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
  /// @brief Read data; called when there is definitely data available
  /// @return like errno; if an error is reported this reader will be deleted
  ///
  virtual int read_data(void) = 0;
}; // end class IReader

#endif // _READER_INTERFACE
