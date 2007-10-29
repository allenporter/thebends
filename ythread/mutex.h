// ymutex.h
// Author: Allen Porter <allen@thebends.org>
//
// Mutex, a very simple wrapper of pthread mutexes.  The Mutex and MutexLocks
// are intended to provide familiar semantics to make using pthreads easier in
// C++.

#ifndef _YTHREAD_MUTEX_H_
#define _YTHREAD_MUTEX_H_

#include <pthread.h>

namespace ythread {

class CondVar;

class Mutex {
 public:
  Mutex();

  // Locks the mutex.  If the mutex is already held by another thread, then it
  // will block until the mutex has been unlocked by the holder.
  void Lock();

  // Unlocks the mutex.
  void Unlock();

 private:
  friend class CondVar;
  pthread_mutex_t pt_mutex_;
};

// YMutexLock will lock a mutex and release it when it goes out of scope.  For
// example:
// 
// YMutex mutex;
// ...
// {
//   YMutexLock m(&mutex);  // Lock held
//   ..
// }  // m goes out of scope, mutex is released
//
class MutexLock {
 public:
  MutexLock(Mutex* mutex);
  ~MutexLock();

 private:
  Mutex* mutex_;
};

}  // namespace ythread

#endif // _YTHREAD_MUTEX_H_
