//
//  event_listener.cpp
//  web
//
//  Created by Clay Sampson on 3/12/2020.
//  Copyright 3/12/2020 Clay Sampson. All rights reserved.
//

#include "event_listener.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/debug.hpp"

////////////////////////////////////////////////////////////////////////////////
// EventListener::EventListener
////////////////////////////////////////////////////////////////////////////////
EventListener::EventListener() {
  mgr = nullptr;
} // end EventListener::EventListener

////////////////////////////////////////////////////////////////////////////////
// EventListener::~EventListener
////////////////////////////////////////////////////////////////////////////////
EventListener::~EventListener() {
} // end EventListener::~EventListener

////////////////////////////////////////////////////////////////////////////////
// EventListener::init
////////////////////////////////////////////////////////////////////////////////
bool EventListener::init(mg_mgr *mgr) {
  this->mgr = mgr;
  mgr->user_data = this;
  dpr("Using mgr %p\n", mgr);
  return true;
} // end EventListener::init

////////////////////////////////////////////////////////////////////////////////
// EventListener::state_change
////////////////////////////////////////////////////////////////////////////////
void EventListener::state_change(const std::string& name, const READER_STATE& state) {
  std::string msg = util_sprintf("{\"event\": \"state_change\","
    "\"source\": \"%s\", "
    "\"current_state\": %u, "
    "\"accumulated_state\": %u "
    "}", name.c_str(), state.current_state, state.accumulated_state);

  broadcast_msg(msg);
} // end EventListener::state_change

////////////////////////////////////////////////////////////////////////////////
// EventListener::timeline_event
////////////////////////////////////////////////////////////////////////////////
void EventListener::timeline_event(const std::string& name, const std::string& json_text) {
  std::string msg = util_sprintf("{\"event\": \"timeline_event\","
    "\"source\": \"%s\", "
    "\"message\": %s, "
    "}", name.c_str(), json_text.c_str());

  broadcast_msg(msg);
} // end EventListener::timeline_event

////////////////////////////////////////////////////////////////////////////////
// EventListener::broadcast_msg
////////////////////////////////////////////////////////////////////////////////
void EventListener::broadcast_msg(const std::string& msg) {
  dpr("Broadcasting message: %s\n", msg.c_str());
  struct mg_connection *conn = mgr->active_connections;
  while(conn) {
    dpr("Recepient %p (sock %d)\n", conn, conn->sock);
    iodpr(false, msg.c_str(), msg.size());
    mg_send(conn, msg.c_str(), msg.size());
    conn = conn->next;
  }
} // end EventListener::broadcast_msg
