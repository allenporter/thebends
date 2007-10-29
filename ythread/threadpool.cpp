#include "threadpool.h"
#include <assert.h>
#include "callback.h"
#include "condvar.h"
#include "mutex.h"
#include "thread.h"

namespace ythread {

class Worker : public Thread {
 public:
  Worker(ThreadPool* pool)
    : pool_(pool), mutex_(new Mutex()), condvar_(new CondVar(mutex_)),
      shutdown_(false), waiting_(false), cb_(NULL) { }

  void StartTask(Callback* cb) {
    mutex_->Lock();
    assert(waiting_);
    cb_ = cb;
    condvar_->SignalAll();
    mutex_->Unlock();
  }

  void Cancel() {
    mutex_->Lock();
    shutdown_ = true;
    condvar_->SignalAll();
    mutex_->Unlock();
    Join();
  }

  bool IsWaiting() {
    bool waiting;
    mutex_->Lock();
    waiting = waiting_;
    mutex_->Unlock();
    return waiting_;
  }

  void Run() {
    mutex_->Lock();
    while (!shutdown_) {
      waiting_ = true;
      condvar_->Wait();  // wait for a job assignment
      waiting_ = false;
      assert(shutdown_ || cb_ != NULL);
      if (shutdown_) {
        mutex_->Unlock();
        break;
      }
      mutex_->Unlock();
      cb_->Execute();
      delete cb_;
      cb_ = NULL;
      mutex_->Lock();
    }
  }

 private:
  ThreadPool* pool_;
  Mutex* mutex_;
  CondVar* condvar_;
  bool shutdown_;
  bool waiting_;
  Callback* cb_;
};


ThreadPool::ThreadPool(int size) : size_(size) {
  pool_ = new Worker*[size_];
  for (int i = 0; i < size_; ++i) {
    pool_[i] = new Worker(this);
    pool_[i]->Start();
  }
}

ThreadPool::~ThreadPool() {
  for (int i = 0; i < size_; ++i) {
    pool_[i]->Cancel();
  }
  delete [] pool_;
}

bool ThreadPool::TryAdd(Callback* cb) {
  MutexLock l(&mutex_);
  int i;
  for (i = 0; i < size_; ++i) {
    if (pool_[i]->IsWaiting()) {
      break;
    }
  }
  if (i == size_) {
    // no free threads
    return false;
  }
  pool_[i]->StartTask(cb);
  return true;
}

}  // namespace ythread
