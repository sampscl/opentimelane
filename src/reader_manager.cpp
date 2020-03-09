//
//  reader_manager.cpp
//  opentimelane
//
//  Created by Clay Sampson on 3/9/2020.
//  Copyright 3/9/2020 Clay Sampson. All rights reserved.
//

#include "reader_manager.hpp"
#include <sys/select.h>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::ReaderManager
////////////////////////////////////////////////////////////////////////////////
ReaderManager::ReaderManager() {
} // end ReaderManager::ReaderManager

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::~ReaderManager
////////////////////////////////////////////////////////////////////////////////
ReaderManager::~ReaderManager() {
} // end ReaderManager::~ReaderManager

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::add_reader
////////////////////////////////////////////////////////////////////////////////
bool ReaderManager::add_reader(const std::string& name, std::shared_ptr<IReader> reader) {
  if(!reader) {
    return false;
  }

  if(reader_map.find(name) != reader_map.end()) {
    return false;
  }

  reader_map[name] = reader;
  return true;
} // end ReaderManager::add_reader

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::delete_reader
////////////////////////////////////////////////////////////////////////////////
bool ReaderManager::delete_reader(const std::string& name) {
  if(reader_map.find(name) == reader_map.end()) {
    return false;
  }

  reader_map.erase(name);

  return true;
} // end ReaderManager::delete_reader

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::poll_readers
////////////////////////////////////////////////////////////////////////////////
void ReaderManager::poll_readers(int timeout_ms) {
  if(reader_map.empty()) {
    return;
  }

  struct timeval t;
  t.tv_sec = timeout_ms / 1000;
  t.tv_usec = (timeout_ms - t.tv_sec) * 1000;

  fd_set read_fds;
  FD_ZERO(&read_fds);
  int nfds = 0;
  for(auto reader : reader_map) {
    FD_SET(reader.second->get_fd(), &read_fds);
    nfds = std::max(nfds, reader.second->get_fd());
  }
  ++nfds;

  int fd_count = select(nfds, &read_fds, nullptr, nullptr, &t);
  if(fd_count <= 0) {
    // timeout or error; could be signal delivered; try again later
    return;
  }

  for(auto reader : reader_map) {
    if(FD_ISSET(reader.second->get_fd(), &read_fds)) {
      reader.second->read_data();
    }
  }
} // end ReaderManager::poll_readers
