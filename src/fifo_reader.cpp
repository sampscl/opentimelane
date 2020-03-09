//
//  fifo_reader.cpp
//  opentimelane
//
//  Created by Clay Sampson on 3/5/2020.
//  Copyright 3/5/2020 Clay Sampson. All rights reserved.
//
#include "fifo_reader.hpp"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <algorithm>

#include "utils/debug.hpp"

////////////////////////////////////////////////////////////////////////////////
// FifoReader::FifoReader
////////////////////////////////////////////////////////////////////////////////
FifoReader::FifoReader() :
  fd(-1), destroy_on_close(false), fifo_path(), line_buffer(MAX_LINE), handler(nullptr) {
} // end FifoReader::FifoReader

////////////////////////////////////////////////////////////////////////////////
// FifoReader::~FifoReader
////////////////////////////////////////////////////////////////////////////////
FifoReader::~FifoReader() {
  close();
} // end FifoReader::~FifoReader

////////////////////////////////////////////////////////////////////////////////
// FifoReader::on_data
////////////////////////////////////////////////////////////////////////////////
int FifoReader::read_data(void) {
  size_t fill_line = line_buffer.size();
  if(fill_line >= MAX_LINE) {
    dpr("Line buffer full\n");
    return EFBIG;
  }

  // make space in the buffer and remember how much "free space" we added
  line_buffer.resize(MAX_LINE);
  size_t available = MAX_LINE - fill_line;

  // read as much as possible, up to the max size of the buffer
  ssize_t amount = read(fd, line_buffer.arrayptr(fill_line), available);
  if(0 == amount) { // end of file
    line_buffer.resize(fill_line);
    dpr("End of file on what should be a non-empty fifo\n");
    return EAGAIN;
  }
  if(amount < 0) { // error
    int result = errno;
    dpr("Error reading %s: %s\n", fifo_path.c_str(), strerror(result));
    line_buffer.resize(fill_line);
    return result;
  }

  // resize to be the amount actually read
  line_buffer.resize(fill_line + amount);

  // process all full lines in the buffer
  process_buffer();

  return 0;
} // end FifoReader::read_data

////////////////////////////////////////////////////////////////////////////////
// FifoReader::init
////////////////////////////////////////////////////////////////////////////////
void FifoReader::init(IMessageHandlerInterface *handler) {
  this->handler = handler;
} // end FifoReader::init

////////////////////////////////////////////////////////////////////////////////
//  FifoReader::fifo_create
////////////////////////////////////////////////////////////////////////////////
int FifoReader::fifo_create(const char *path, bool destroy_on_close) {
  dpr("Creating %s\n", path);
  if(mkfifo(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH)) {
    return errno;
  }
  return fifo_open(path, destroy_on_close);
} // end FifoReader::fifo_create

////////////////////////////////////////////////////////////////////////////////
//  FifoReader::fifo_open
////////////////////////////////////////////////////////////////////////////////
int FifoReader::fifo_open(const char *path, bool destroy_on_close) {
  dpr("Opening %s\n", path);
  this->fifo_path = path;
  this->destroy_on_close = destroy_on_close;
  this->fd = open(path, O_RDONLY|O_NONBLOCK);
  return fd < 0 ? errno : 0;
} // end FifoReader::fifo_open

////////////////////////////////////////////////////////////////////////////////
//  FifoReader::close
////////////////////////////////////////////////////////////////////////////////
int FifoReader::close(void) {
  if(fd < 0) {
    return 0;
  }
  dpr("Closing %s\n", fifo_path.c_str());
  if(::close(fd)) {
    return errno;
  }
  if(destroy_on_close) {
    dpr("Destroying %s\n", fifo_path.c_str());
  }
  if(destroy_on_close && unlink(fifo_path.c_str())) {
    return errno;
  }
  fd = -1;
  return 0;
} // end FifoReader::close

////////////////////////////////////////////////////////////////////////////////
// FifoReader::process_buffer
////////////////////////////////////////////////////////////////////////////////
void FifoReader::process_buffer(void) {
  while(!line_buffer.empty()) {
    char_buff_t::const_iterator newline = std::find(line_buffer.begin(), line_buffer.end(), '\n');
    if(newline == line_buffer.end()) { // have some data but not a whole line (yet)
      dpr("Partial line\n");
      return;
    }

    // a full line exists between line_buffer.begin() and newline
    std::string line;
    for(char_buff_t::const_iterator ch=line_buffer.begin(); ch!=newline; ++ch) {
      line += (*ch);
    }

    dpr("Got line: \"%s\"\n", line.c_str());

    // realign to 1-past-the-newline
    line_buffer.realign(newline - line_buffer.begin() + 1);

    dpr("New buffer size: %zu\n", line_buffer.size());

    // process a line
    handler->process_message(line);
  } // end while
} // end FifoReader::process_buffer
