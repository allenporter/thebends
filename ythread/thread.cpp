#include "thread.h"
#include <pthread.h>
#include <iostream>

namespace ythread {

Thread::Thread() {
}

Thread::~Thread() {
  Join();
}

void Thread::Start() {
  pthread_create(&pthread_, NULL, &Thread::RunInThread, this); 
}

void* Thread::RunInThread(void *arg) {
  Thread* thread = (Thread*)arg;
  thread->Run();
  return NULL;
}

void Thread::Join() {
  pthread_join(pthread_, NULL);
}

}  // namespace ythread
