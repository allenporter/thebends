#include <iostream>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include "mutex.h"
#include "thread.h"

using namespace std;

class TestThread : public ythread::Thread {
 public:
  static int counter_;
  static ythread::Mutex* mutex_;
  
  string name_; 

  TestThread(string name) : name_(name) { }
  virtual ~TestThread() {}

 protected:
  void Run() {
    while (1) {
      mutex_->Lock();
      bool done = (counter_ >= 10);
      if (done) {
	mutex_->Unlock();
	break;
      }

      counter_++;

      mutex_->Unlock();

      // pthreads are not "fair", so we have to yield if we want the other
      // thread to get a chance to grab the mutex.
      usleep(rand() % 50);
     }
  }
};

int TestThread::counter_;
ythread::Mutex* TestThread::mutex_;

int main(int argc, char *argv[]) {
  srand(time(NULL));

  TestThread t1("t1");
  TestThread t2("t2");

  TestThread::counter_ = 0;
  TestThread::mutex_ = new ythread::Mutex;

  t1.Start();
  t2.Start();
  t1.Join();
  t2.Join();
  assert(TestThread::counter_ == 10);

  cout << "PASS" << endl;
}
