//
//  event_listener_interface.hpp
//  opentimelane
//
//  Created by Clay Sampson on 3/11/2020.
//  Copyright 3/11/2020 Clay Sampson. All rights reserved.
//
#ifndef _EVENT_LISTENER_INTERFACE_HPP
#define _EVENT_LISTENER_INTERFACE_HPP

#include <string>

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
/// @class IEventListener
/// @brief Interface to listen to timelane events
///
class IEventListener {
public:
  IEventListener(){}
  virtual ~IEventListener(){}

  ///
  /// @brief Inform the listener of a state change
  /// @param[in] name The name of the reader
  /// @param[in] state The current state of the reader
  ///
  virtual void state_change(const std::string& name, const READER_STATE& state) = 0;

  ///
  /// @brief Inform the listener of a timeline event
  /// @param[in] name The name of the reader
  /// @param[in] json_text The json text for the event. Format:
  ///
  virtual void timeline_event(const std::string& name, const std::string& json_text) = 0;


}; // end class IEventListener

#endif // _EVENT_LISTENER_INTERFACE_HPP
