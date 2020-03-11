//
//  rwlock_interface.hpp
//  utils
//
//  Created by Clay Sampson on 3/9/2020.
//  Copyright 3/9/2020 Clay Sampson. All rights reserved.
//
#ifndef _RWLOCK_INTERFACE_HPP
#define _RWLOCK_INTERFACE_HPP

#include <pthread.h>
#include <functional>

///
/// @class PosixRWLockInterface
/// @tparam UNDERLYING_T The underlying type to mediate access to
/// @brief Mediate access to an underlying object with a POSIX R/W Lock
///
template<class UNDERLYING_T>
class PosixRWLockInterface {
public:
  PosixRWLockInterface() {
    pthread_rwlockattr_init(&attr);
    pthread_rwlock_init(&l, &attr);
  }

  ~PosixRWLockInterface() {
    pthread_rwlock_destroy(&l);
    pthread_rwlockattr_destroy(&attr);
  }

  ///
  /// @brief Acquire a read lock
  /// @param[in] fun The function to call when the lock is acquired
  /// @return The result of calling fun on the underlying object
  ///
  template<class RETURN_T> RETURN_T rdlock(std::function<RETURN_T(UNDERLYING_T&)> fun) {
    pthread_rwlock_rdlock(&l);
    try {
      RETURN_T result = fun(t);
      pthread_rwlock_unlock(&l);
      return result;
    }
    catch(...) {
      pthread_rwlock_unlock(&l);
      throw;
    }
  }

  ///
  /// @brief Acquire a write lock
  /// @param[in] fun The function to call when the lock is acquired
  /// @return The result of calling fun on the underlying object
  ///
  template<class RETURN_T> RETURN_T wrlock(std::function<RETURN_T(UNDERLYING_T&)> fun) {
    pthread_rwlock_wrlock(&l);
    try {
      RETURN_T result = fun(t);
      pthread_rwlock_unlock(&l);
      return result;
    }
    catch(...) {
      pthread_rwlock_unlock(&l);
      throw;
    }
  }

  ///
  /// @brief Acquire a read lock (const)
  /// @param[in] fun The function to call when the lock is acquired
  /// @return The result of calling fun on the underlying object
  ///
  template<class RETURN_T> RETURN_T rdlock(std::function<RETURN_T(const UNDERLYING_T&)> fun) const {
    pthread_rwlock_rdlock(&l);
    try {
      RETURN_T result = fun(t);
      pthread_rwlock_unlock(&l);
      return result;
    }
    catch(...) {
      pthread_rwlock_unlock(&l);
      throw;
    }
  }

  ///
  /// @brief Acquire a write lock (const)
  /// @param[in] fun The function to call when the lock is acquired
  /// @return The result of calling fun on the underlying object
  ///
  template<class RETURN_T> RETURN_T wrlock(std::function<RETURN_T(const UNDERLYING_T&)> fun) const {
    pthread_rwlock_wrlock(&l);
    try {
      RETURN_T result = fun(t);
      pthread_rwlock_unlock(&l);
      return result;
    }
    catch(...) {
      pthread_rwlock_unlock(&l);
      throw;
    }
  }

private:
  UNDERLYING_T t;
  pthread_rwlock_t l;
  pthread_rwlockattr_t attr;
}; // end class PosixRWLockInterface

#endif // _RWLOCK_INTERFACE_HPP
