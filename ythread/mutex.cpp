#include "mutex.h"
#include <pthread.h>

namespace ythread {

Mutex::Mutex() {
  pthread_mutex_init(&pt_mutex_, NULL);
}

void Mutex::Lock() {
  pthread_mutex_lock(&pt_mutex_);
}

void Mutex::Unlock() {
  pthread_mutex_unlock(&pt_mutex_);
}

MutexLock::MutexLock(Mutex* mutex) : mutex_(mutex) {
  mutex_->Lock();
}

MutexLock::~MutexLock() {
  mutex_->Unlock();
}

}  // namespace ythread
