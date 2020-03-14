//
//  event_listener.hpp
//  web
//
//  Created by Clay Sampson on 3/12/2020.
//  Copyright 3/12/2020 Clay Sampson. All rights reserved.
//
#ifndef _EVENT_LISTENER_HPP
#define _EVENT_LISTENER_HPP

#include "../interfaces/event_listener_interface.hpp"
#include "mongoose.h"

///
/// @class EventListener; not thread-safe
/// @brief Implement event listener for web destinations
/// @details
///
class EventListener : public IEventListener {
public:
  EventListener();
  virtual ~EventListener();

  ///
  /// @brief Initialize; must call before any other method
  /// @param[in] mgr the mongoose manager used for sending events
  /// @return @li true on success @li false on failure
  ///
  bool init(mg_mgr *mgr);

  // Methods inherited from IEventListener
  virtual void state_change(const std::string& name, const READER_STATE& state) override;
  virtual void timeline_event(const std::string& name, const std::string& json_text) override;

private:

  void broadcast_msg(const std::string& msg);

  mg_mgr *mgr;

}; // end class EventListener

#endif // _EVENT_LISTENER_HPP
