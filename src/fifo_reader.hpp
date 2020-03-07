//
//  fifo_reader.hpp
//  opentimelane
//
//  Created by Clay Sampson on 3/5/2020.
//  Copyright 3/5/2020 Clay Sampson. All rights reserved.
//
#ifndef _FIFO_READER_HPP
#define _FIFO_READER_HPP

#include "reader_interface.hpp"

///
/// @class FifoReader
/// @brief Read from a fifo
/// @details
///
class FifoReader : public IReader {
public:
  FifoReader();
  virtual ~FifoReader();

  // Methods inherited from IReader
  virtual int on_data(const void *data, size_t len) override;

}; // end class FifoReader

#endif // _FIFO_READER_HPP
