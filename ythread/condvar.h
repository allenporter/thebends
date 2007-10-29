// condvar.h
// Author: Allen Porter <allen@thebends.org>
//
// CondVar, a wrapper for pthread condition variables.  Allows threads to
// suspend execution and wait until another thread signals it to resume.

#ifndef _YTHREAD_CONDVAR_H_
#define _YTHREAD_CONDVAR_H_

#include <pthread.h>

namespace ythread {

class Mutex;

// CondVar should be initialized with a pointer to an existing YMutex.  Before
// calling any public methods (Wait, Signal, SignalAll) the mutex passed should
// be locked.  For example:
//
//   Mutex mutex;
//   CondVar condvar(&mutex);
//   ...
//   mutex.Lock();
//   condvar.Wait();
//   mutex.Unlock();
//   ...
//   mutex.Lock();
//   condvar.Signal();
//   mutex.Unlock();
//   ...
//
class CondVar {
 public:
  CondVar(Mutex* mutex);
  ~CondVar();

  // Blocks and waits for the condition variable to be signaled.  The mutex
  // passed to the constructor should be held before calling Wait(), and will
  // be atomically released at the same time the thread waits.
  void Wait();

  // Signals a waiting thread to wake.  If no threads are waiting, then nothing
  // happens.
  void Signal();

  // Signals all waiting threads to wake up.  If no threads are waiting, then
  // nothing happens.
  void SignalAll();

 private:
  Mutex* mutex_;
  pthread_cond_t pt_cond_;
};

}  // namespace ythread

#endif // _YTHREAD_CONDVAR_H_
