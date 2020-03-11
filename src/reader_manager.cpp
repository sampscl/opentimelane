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

  return reader_map_locked.wrlock<bool>([&] (reader_map_t& reader_map) -> bool {

    if(reader_map.find(name) != reader_map.end()) {
      return false;
    }

    reader_map[name] = reader;
    state_map_locked.wrlock<bool>([&] (reader_state_map_t& state_map) -> bool {
      state_map[name] = READER_STATE();
      return true;
    }); // end scope state_map_locked.wrlock

    return true;
  }); // end scope reader_map_locked.wrlock
} // end ReaderManager::add_reader

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::delete_reader
////////////////////////////////////////////////////////////////////////////////
bool ReaderManager::delete_reader(const std::string& name) {
  return reader_map_locked.wrlock([&] (reader_map_t& reader_map) -> bool {
    if(reader_map.find(name) == reader_map.end()) {
      return false;
    }
    reader_map.erase(name);

    state_map_locked.wrlock([&] (reader_state_map_t& state_map) -> bool {
      state_map.erase(name);
      return true;
    });

    return true;
  });
} // end ReaderManager::delete_reader

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::poll_readers
////////////////////////////////////////////////////////////////////////////////
void ReaderManager::poll_readers(int timeout_ms, reader_state_map_t* before_poll_states) {

  if(before_poll_states) {
    state_map_locked.rdlock([&] (reader_state_map_t& state_map) -> bool {
    *before_poll_states = state_map;
    return true;
    });
  }

  clear_current_states();

  struct timeval t;
  t.tv_sec = timeout_ms / 1000;
  t.tv_usec = (timeout_ms - (t.tv_sec*1000)) * 1000;

  fd_set read_fds;
  FD_ZERO(&read_fds);
  int nfds = 0;
  reader_map_locked.rdlock([&] (reader_map_t& reader_map) -> bool {
    for(auto reader : reader_map) {
      const int fd = reader.second->get_fd();
      if(fd >= 0) {
        FD_SET(fd, &read_fds);
        nfds = std::max(nfds, fd);
      }
    } // end for all readers
    return true;
  });
  ++nfds;

  int fd_count = select(nfds, &read_fds, nullptr, nullptr, &t);
  if(fd_count <= 0) { // timeout or error; could be signal delivered; try again later
    return;
  }

  reader_map_locked.wrlock([&] (reader_map_t& reader_map) -> bool {
    for(auto reader : reader_map) {
      if(FD_ISSET(reader.second->get_fd(), &read_fds)) {
        const int read_result = reader.second->read_data();
        state_map_locked.wrlock([&] (reader_state_map_t& state_map) -> bool {
          READER_STATE& state = state_map[reader.first];

          // clear current state then OR in new state bits
          state.current_state = 0;

          if(reader_map[reader.first]->get_fd() < 0) {
            state.current_state |= RS_FD_BAD;
          }

          if(read_result) {
            state.current_state |= RS_READ_FAIL;
          }
          else {
            // reader state 0 here means that the reader's fd was selected and read_data()
            // was called, therefore there was some data; if read_result is zero,
            // then data was processed
            state.current_state |= RS_HAD_DATA;
          }

          // accumulate state
          state.accumulated_state |= state.current_state;

          return true;
        }); // end scope state_map_locked.wrlock
      } // end if reader has data
    } // end for all readers
    return true;
  }); // end scope reader_map_locked.wrlock
} // end ReaderManager::poll_readers

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::clear_current_states
////////////////////////////////////////////////////////////////////////////////
void ReaderManager::clear_current_states(void) {
  state_map_locked.wrlock([&] (reader_state_map_t& state_map) -> bool {
    for(auto reader_state : state_map) {
      reader_state.second.current_state = 0;
    }
    return true;
  });
} // end ReaderManager::clear_current_states
