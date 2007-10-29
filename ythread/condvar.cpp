#include "condvar.h"
#include <pthread.h>
#include "mutex.h"

namespace ythread {

CondVar::CondVar(Mutex* mutex) : mutex_(mutex) {
  pthread_cond_init(&pt_cond_, NULL);
}

CondVar::~CondVar() {
  pthread_cond_destroy(&pt_cond_);
}

void CondVar::Signal() {
  pthread_cond_signal(&pt_cond_);
}

void CondVar::SignalAll() {
  pthread_cond_broadcast(&pt_cond_);
}

void CondVar::Wait() {
  pthread_cond_wait(&pt_cond_, &mutex_->pt_mutex_);
}

}  // namespace ythread
