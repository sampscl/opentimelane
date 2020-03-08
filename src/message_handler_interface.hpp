//
//  message_handler_interface.hpp
//  opentimelane
//
//  Created by Clay Sampson on 3/5/2020.
//  Copyright 3/5/2020 Clay Sampson. All rights reserved.
//
#ifndef _MESSAGE_HANDLER_INTERFACE_HPP
#define _MESSAGE_HANDLER_INTERFACE_HPP

#include <string>

///
/// @class IMessageHandlerInterface
/// @brief Interface to line-oriented message handlers
///
class IMessageHandlerInterface {
public:
  IMessageHandlerInterface(){}
  virtual ~IMessageHandlerInterface(){}

  ///
  /// @brief Handle a message
  /// @param[in] line A line of text, not including \n
  ///
  virtual void process_message(const std::string& line) = 0;

}; // end class IMessageHandlerInterface

#endif // _MESSAGE_HANDLER_INTERFACE_HPP
