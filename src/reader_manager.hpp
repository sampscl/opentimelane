//
//  reader_manager.hpp
//  opentimelane
//
//  Created by Clay Sampson on 3/9/2020.
//  Copyright 3/9/2020 Clay Sampson. All rights reserved.
//
#ifndef _READER_MANAGER_HPP
#define _READER_MANAGER_HPP

#include "interfaces/reader_interface.hpp"
#include "utils/rwlock_interface.hpp"
#include "interfaces/event_listener_interface.hpp"
#include "interfaces/message_handler_interface.hpp"
#include <stdint.h>
#include <memory>
#include <string>
#include <map>
#include <list>

///
/// @class ReaderManager
/// @brief Manages readers
///
class ReaderManager : public IMessageHandlerInterface {
public:
  /// @brief Construct
  ReaderManager();

  /// @brief Destroy
  virtual ~ReaderManager();

  typedef std::map<std::string, std::shared_ptr<IReader> > reader_map_t; //!< Type for the reader map
  typedef std::map<std::string, READER_STATE> reader_state_map_t; //!< Type for reader to state map
  typedef std::map<std::string, std::shared_ptr<IEventListener> > listener_map_t; //!< Type for listener map
  typedef std::list<std::pair<std::string, std::string> > recent_messages_list_t;

  /** @name Reader API
   *  API Dealing with readers
   */
  /// @{
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
  /// @}

  /** @name Listener API
   *  API Dealing with listeners
   */
  /// @{
  ///
  /// @brief Register a listener
  /// @param[in] name The name to associate with listener; must be unique
  /// @param[in] listener The listener
  /// @return @li true on success @li false if listener is null or name already exists
  ///
  bool add_listener(const std::string& name, std::shared_ptr<IEventListener> listener);

  ///
  /// @brief Delete a listener
  /// @param[in] name The name of the listener to delete
  /// @return @li true on success @li false if named listener did not exist
  ///
  bool delete_listener(const std::string& name);
  /// @}

  // Methods inherited from IMessageHandlerInterface
  virtual void process_message(const std::string& name, const std::string& line) override;

private:

  void clear_current_states(void);

  PosixRWLockInterface<reader_map_t> reader_map_locked; //!< The readers by name; LOCK FIRST
  PosixRWLockInterface<reader_state_map_t> state_map_locked; //!< The state map; LOCK SECOND
  PosixRWLockInterface<listener_map_t> listener_map_locked; //!< The listener map; LOCK THIRD
  PosixRWLockInterface<recent_messages_list_t> recent_messages_locked; //!< The list of recent messages; lock FOURTH
}; // end class ReaderManager

#endif // _READER_MANAGER_HPP
