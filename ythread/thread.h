// thread.h
// Author: Allen Porter <allen@thebends.org>
//
// YThread: A very simple wrapper of the pthread library, used to make threads
// more friendly.

#ifndef _YTHREAD_THREAD_H_
#define _YTHREAD_THREAD_H_

#include <pthread.h>

// A Thread class should be subclassed to run a task in a thread.
//
// Example:
// class MyThread : public ythread::Thread {
// ...
//   void Run() {
//     // do something
//   }
// }; 
// ...
// MyThread t;
// t.Start();
// ...
// t.Join();

namespace ythread {

class Thread {
 public:
  static void* RunInThread(void *arg);

  Thread();
  virtual ~Thread();

  // Creates a new thread and calls the Run() method in the new threads context.
  void Start();

  // Blocks until the Run() method finishes execution.  If Run() has already
  // returned, then Join() returns immediately.
  void Join();

  // Can be overridden by subclasses to allow a way to stop the thread (usually
  // with some kind of condition variable).
  virtual void Stop() { }

 protected:
  // Method to be run in the context of the created thread.  Subclasses should
  // implement this method.
  virtual void Run() = 0;

 private:
  pthread_t pthread_;
};


} // namespace ythread


#endif // _YTHREAD_THREAD_H_
