//
//  main.cpp
//  opentimelane
//
//  Created by Clay Sampson on 3/5/2020.
//  Copyright 3/5/2020 Clay Sampson. All rights reserved.
//
#include <stdlib.h>
#include <thread>
#include "web/mongoose.h"
#include "utils/debug.hpp"
#include "reader_manager.hpp"

////////////////////////////////////////////////////////////////////////////////
// rm The reader manager
////////////////////////////////////////////////////////////////////////////////
static ReaderManager rm;

////////////////////////////////////////////////////////////////////////////////
// Stop when this is false
////////////////////////////////////////////////////////////////////////////////
static bool run = true;

////////////////////////////////////////////////////////////////////////////////
// Re-read schedule when this is true
////////////////////////////////////////////////////////////////////////////////
static bool hup = false;

////////////////////////////////////////////////////////////////////////////////
// on_sigint
////////////////////////////////////////////////////////////////////////////////
static void on_sigint(int signal) {
  (void)signal; // not used
  if(!run) { exit(-1); }
  run = false;
} // end on_sigint

////////////////////////////////////////////////////////////////////////////////
// on_sigterm
////////////////////////////////////////////////////////////////////////////////
static void on_sigterm(int signal) {
  (void)signal; // not used
  if(!run) { exit(-1); }
  run = false;
} // end on_sigterm

static void on_sighup(int signal) {
  (void)signal; // not used
  hup = true;
} // end on_sighup

////////////////////////////////////////////////////////////////////////////////
// setup_signals
////////////////////////////////////////////////////////////////////////////////
static void setup_signals(void) {
  struct sigaction action;

  memset(&action, 0, sizeof(action));
  action.sa_handler = on_sigint;
  sigaction(SIGINT, &action, nullptr);

  memset(&action, 0, sizeof(action));
  action.sa_handler = on_sigterm;
  sigaction(SIGTERM, &action, nullptr);

  memset(&action, 0, sizeof(action));
  action.sa_handler = on_sighup;
  sigaction(SIGHUP, &action, nullptr);

  memset(&action, 0, sizeof(action));
  action.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &action, nullptr);

} // end setup_signals

////////////////////////////////////////////////////////////////////////////////
// run_readers
////////////////////////////////////////////////////////////////////////////////
static void run_readers(void) {
  dpr("Starting\n");
  while(run) {
    rm.poll_readers(1000);
  }
  dpr("Exiting\n");
} // end run_readers

////////////////////////////////////////////////////////////////////////////////
// ev_handler
////////////////////////////////////////////////////////////////////////////////
static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if(ev != MG_EV_HTTP_REQUEST) {
    dpr("c: %p, ev: %d, user_data: %p\n", c, ev, c->user_data);
    return;
  }

  struct http_message *hm = (struct http_message *) p;
  c->user_data = (void*)0xDEADBEEF;
  iodpr(true, hm->message.p, hm->message.len);
  dpr("method        : %.*s\n", int(hm->method.len), hm->method.p);
  dpr("uri           : %.*s\n", int(hm->uri.len), hm->uri.p);
  dpr("proto         : %.*s\n", int(hm->proto.len), hm->proto.p);
  dpr("query_string  : %.*s\n", int(hm->query_string.len), hm->query_string.p);
  dpr("user_data     : %p\n", c->user_data);

  // We have received an HTTP request. Parsed request is contained in `hm`.
  // Send HTTP reply to the client which shows full original request.
  mg_send_head(c, 200, hm->message.len, "Content-Type: text/plain");
  mg_printf(c, "%.*s", (int)hm->message.len, hm->message.p);
} // end ev_handler

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
  dpr("Startup\n");

  setup_signals();
  std::thread readers_thread(run_readers);

  struct mg_mgr mgr;
  struct mg_connection *c;

  mg_mgr_init(&mgr, NULL);
  c = mg_bind(&mgr, "8080", ev_handler);
  mg_set_protocol_http_websocket(c);

  while(run) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  readers_thread.join();

  dpr("Cleanup\n");
  return EXIT_SUCCESS;
} // end main
