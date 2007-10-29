// ythreadpool.h
// Author: Allen Porter <allen@thebends.org>

#ifndef _YTHREADPOOL_H_
#define _YTHREADPOOL_H_

#include "mutex.h"

namespace ythread {

class Worker;
class Callback;

class ThreadPool {
 public:
  ThreadPool(int size);
  ~ThreadPool();

  bool TryAdd(Callback* cb);

 private:
  Mutex mutex_;
  Worker **pool_;  // array of worker threads objects
  int size_;
};

}  // namespace ythread

#endif // _YTHREADPOOL_H_
