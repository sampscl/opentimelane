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
#include <string>
#include "../utils/buffer.hpp"

///
/// @class FifoReader
/// @brief Read from a fifo
///
class FifoReader : public IReader {
public:
  typedef Buffer<char> char_buff_t; //!< Character buffer

  FifoReader();
  virtual ~FifoReader();

  ///
  /// @brief Create a fifo given a path
  /// @param[in] path The path to the fifo; will be created world-writable
  /// @param[in] destroy_on_close @li true causes the fifo to be deleted when
  ///   this instance is done with it @li false the fifo is not deleted, only
  ///   closed when this instance is done with it
  /// @return like errno
  ///
  int fifo_create(const char *path, bool destroy_on_close = true);
  ///
  /// @brief Open fifo given path
  /// @param[in] path The path to the fifo
  /// @param[in] destroy_on_close @li true causes the fifo to be deleted when
  ///   this instance is done with it @li false the fifo is not deleted, only
  ///   closed when this instance is done with it
  /// @return like errno
  ///
  int fifo_open(const char *path, bool destroy_on_close = false);

  ///
  /// @brief Close a created or opened fifo
  /// @return like errno
  ///
  int close(void);

  // Methods inherited from IReader
  virtual int read_data(void) override;
  virtual void init(IMessageHandlerInterface *handler) override;

  ///
  /// @brief Exposed for testing only
  ///
  void process_buffer(void);

private:

  int fd;
  bool destroy_on_close;
  std::string fifo_path;
  char_buff_t line_buffer;
  IMessageHandlerInterface *handler;
  static const size_t MAX_LINE = 2048;

}; // end class FifoReader

#endif // _FIFO_READER_HPP
