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
#include <memory>
#include <string>
#include <map>

///
/// @class ReaderManager
/// @brief Manages readers
///
class ReaderManager {
  typedef std::map<std::string, std::shared_ptr<IReader> > reader_map_t; //!< Type for the reader map
public:
  ReaderManager();
  virtual ~ReaderManager();

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
  /// @note This does not block of there are no readers in the database
  /// @param[in] timeout_ms The maximum time to wait for data in ms
  ///
  void poll_readers(int timeout_ms);

private:
  reader_map_t reader_map; //!< The readers by name
}; // end class ReaderManager

#endif // _READER_MANAGER_HPP
