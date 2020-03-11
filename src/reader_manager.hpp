//
//  reader_manager.hpp
//  opentimelane
//
//  Created by Clay Sampson on 3/9/2020.
//  Copyright 3/9/2020 Clay Sampson. All rights reserved.
//
#ifndef _READER_MANAGER_HPP
#define _READER_MANAGER_HPP

#include "readers/reader_interface.hpp"
#include "utils/rwlock_interface.hpp"
#include <stdint.h>
#include <memory>
#include <string>
#include <map>

/// @brief Reader state bit mask
enum READER_MANAGER_READER_STATE {
  RS_FD_BAD    = 0x01, //!< Reader file descriptor is bad (<0)
  RS_READ_FAIL = 0x02, //!< Reader failed reading when it should have succeeded
  RS_HAD_DATA  = 0x04  //!< Reader had data available and processed it
};

/// @brief Storage for reader state
struct READER_STATE {
  READER_STATE() : current_state(0), accumulated_state(0) {}
  ~READER_STATE(){}
  uint8_t current_state; //!< The current state;
  uint8_t accumulated_state; //!< The accumulated state; bits set are never cleared
};

///
/// @class ReaderManager
/// @brief Manages readers
///
class ReaderManager {
public:
  /// @brief Construct
  ReaderManager();

  /// @brief Destroy
  virtual ~ReaderManager();

  typedef std::map<std::string, std::shared_ptr<IReader> > reader_map_t; //!< Type for the reader map
  typedef std::map<std::string, READER_STATE> reader_state_map_t; //!< Type for reader to state map

  ///
  /// @brief Add a reader
  /// @param[in] name The name to associate with reader; must be unique
  /// @param[in] reader The reader to add; must be initialized and ready to use
  ///   and its fd should not change for the lifetime of the reader
  /// @return @li true on success @li false if name is already taken or reader is null
  ///
  bool add_reader(const std::string& name, std::shared_ptr<IReader> reader);

  ///
  /// @brief Delete a reader
  /// @param[in] name The name of the reader to delete
  /// @return @li true on success @li false if named reader did not exist
  ///
  bool delete_reader(const std::string& name);

  ///
  /// @brief Poll all readers and read from them if they have data pending
  /// @param[in] timeout_ms The maximum time to wait for data in ms
  /// @param[out] before_poll_states If not null, will contain a copy of all
  ///   reader states before any polling or data processing occurs.
  ///
  void poll_readers(int timeout_ms, reader_state_map_t* before_poll_states = nullptr);

private:

  void clear_current_states(void);

  PosixRWLockInterface<reader_map_t> reader_map_locked; //!< The readers by name; LOCK FIRST
  PosixRWLockInterface<reader_state_map_t> state_map_locked; //!< The state map; LOCK SECOND
}; // end class ReaderManager

#endif // _READER_MANAGER_HPP
