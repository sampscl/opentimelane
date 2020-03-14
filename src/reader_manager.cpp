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
  return reader_map_locked.wrlock<bool>([&] (reader_map_t& reader_map) -> bool {
    if(reader_map.find(name) == reader_map.end()) {
      return false;
    }
    reader_map.erase(name);

    state_map_locked.wrlock<bool>([&] (reader_state_map_t& state_map) -> bool {
      state_map.erase(name);
      return true;
    });

    return true;
  });
} // end ReaderManager::delete_reader

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::delete_all_readers
////////////////////////////////////////////////////////////////////////////////
void ReaderManager::delete_all_readers(void) {
  reader_map_locked.wrlock<bool>([&] (reader_map_t& reader_map) -> bool {
    reader_map.clear();
    return state_map_locked.wrlock<bool>([&] (reader_state_map_t& state_map) -> bool {
      state_map.clear();
      return true;
    });
  });
} // end ReaderManager::delete_all_readers

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::poll_readers
////////////////////////////////////////////////////////////////////////////////
void ReaderManager::poll_readers(int timeout_ms, reader_state_map_t* before_poll_states) {

  if(before_poll_states) {
    state_map_locked.rdlock<bool>([&] (reader_state_map_t& state_map) -> bool {
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
  reader_map_locked.rdlock<bool>([&] (reader_map_t& reader_map) -> bool {
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

  std::list<std::string> updated_state_readers;

  reader_map_locked.wrlock<bool>([&] (reader_map_t& reader_map) -> bool {
    for(auto reader : reader_map) {
      if(FD_ISSET(reader.second->get_fd(), &read_fds)) {
        const int read_result = reader.second->read_data();
        state_map_locked.wrlock<bool>([&] (reader_state_map_t& state_map) -> bool {
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

          // remember that this one got updated state
          updated_state_readers.push_back(reader.first);

          return true;
        }); // end scope state_map_locked.wrlock
      } // end if reader has data
    } // end for all readers
    return true;
  }); // end scope reader_map_locked.wrlock

  // tell listeners about state changes
  state_map_locked.rdlock<bool>([&] (reader_state_map_t& state_map) -> bool {
    return listener_map_locked.rdlock<bool>([&] (listener_map_t& listener_map) -> bool {
      for(std::string& name : updated_state_readers) {
        const READER_STATE& state = state_map[name];
        for(auto listener : listener_map) {
          listener.second->state_change(name, state);
        } // end for all listeners
      } // end end for all updated state readers
      return true;
    });
  });
} // end ReaderManager::poll_readers

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::add_listener
////////////////////////////////////////////////////////////////////////////////
bool ReaderManager::add_listener(const std::string& name, std::shared_ptr<IEventListener> listener) {

  if(!listener) {
    return false;
  }

  // add listener to map
  bool added = listener_map_locked.wrlock<bool>([&] (listener_map_t& listener_map) -> bool {
    if(listener_map.find(name) != listener_map.end()) {
      return false;
    }
    listener_map[name] = listener;
    return true;
  });

  if(!added) {
    return false;
  }

  // update listener with recent history
  recent_messages_locked.rdlock<bool>([&] (recent_messages_list_t& recent_messages) -> bool {
    for(auto item : recent_messages) {
      listener->timeline_event(item.first, item.second);
    }
    return true;
  });

  return true;
} // end ReaderManager::add_listener

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::delete_listener
////////////////////////////////////////////////////////////////////////////////
bool ReaderManager::delete_listener(const std::string& name) {
  return listener_map_locked.wrlock<bool>([&] (listener_map_t& listener_map) -> bool {
    if(listener_map.find(name) == listener_map.end()) {
      return false;
    }
    listener_map.erase(name);
    return true;
  });
} // end ReaderManager::delete_listener

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::process_message
////////////////////////////////////////////////////////////////////////////////
void ReaderManager::process_message(const std::string& name, const std::string& line) {

  recent_messages_locked.wrlock<bool>([&] (recent_messages_list_t& recent_messages) -> bool {
    recent_messages.push_front(std::make_pair(name, line));
    while(recent_messages.size() > 1000) {
      recent_messages.pop_back();
    }
    return true;
  });

  listener_map_locked.rdlock<bool>([&] (listener_map_t& listener_map) -> bool {
    for(auto listener : listener_map) {
      listener.second->timeline_event(name, line);
    } // end for all listeners
    return true;
  });

} // end ReaderManager::process_message

////////////////////////////////////////////////////////////////////////////////
// ReaderManager::clear_current_states
////////////////////////////////////////////////////////////////////////////////
void ReaderManager::clear_current_states(void) {
  state_map_locked.wrlock<bool>([&] (reader_state_map_t& state_map) -> bool {
    for(auto reader_state : state_map) {
      reader_state.second.current_state = 0;
    }
    return true;
  });
} // end ReaderManager::clear_current_states
