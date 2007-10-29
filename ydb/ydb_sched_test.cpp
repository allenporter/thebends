#include <assert.h>
#include <unistd.h>
#include <ythread/condvar.h>
#include <ythread/mutex.h>
#include <ythread/thread.h>
#include "ydb_base.h"
#include "ydb_map.h"
#include "ydb_sched.h"

#define assertOk(x) assert(YDB_OK == (x))
#define assertNotFound(x) assert(YDB_NOT_FOUND == (x))
#define assertDeadlock(x) assert(YDB_DEADLOCK == (x))

static int thread_count_ = 0;

class TestThread : public ythread::Thread {
 public:
  TestThread(Ydb_Scheduler* sched, ythread::Mutex* mutex,
             ythread::CondVar* condvar)
    :  thread_num_(++thread_count_), sched_(sched), mutex_(mutex),
       condvar_(condvar), is_blocked_(false) { }

  ~TestThread() { }

  void WaitForPause() {
    bool blocked = false;
    do {
      usleep(100);

      mutex_->Lock();
      blocked = is_blocked_;
      mutex_->Unlock();
    } while (!blocked);
  }

  void Continue() {
    condvar_->SignalAll();
  }

 protected:
  void Pause() {
    mutex_->Lock();
    is_blocked_ = true;
    condvar_->Wait();
    is_blocked_ = false;
    mutex_->Unlock();
  }

  int thread_num_;
  Ydb_Scheduler* sched_;
  ythread::Mutex* mutex_;
  ythread::CondVar* condvar_;
  bool is_blocked_;
};

class TestThread1 : public TestThread {
 public:
  TestThread1(Ydb_Scheduler* sched, ythread::Mutex* mutex,
              ythread::CondVar* condvar)
    : TestThread(sched, mutex, condvar) { }

 protected:
  virtual void Run() {
    Ydb_Trans* t1 = sched_->OpenTransaction();
    Pause();  // resumed by main thread

    Ydb_WriteOp* op1 = new Ydb_WriteOp("keyA", "t1");
    assertOk(sched_->AddOp(t1, op1)); 
    Pause();  // resumed by main thread

    Ydb_WriteOp* op2 = new Ydb_WriteOp("keyB", "t1");
    assertOk(sched_->AddOp(t1, op2));

    sched_->Commit(t1);
  }
};

class TestThread2 : public TestThread {
 public:
  TestThread2(Ydb_Scheduler* sched, ythread::Mutex* mutex,
              ythread::CondVar* condvar)
    : TestThread(sched, mutex, condvar) { }

 protected:
  virtual void Run() {
    Ydb_Trans* t2 = sched_->OpenTransaction();
    Pause();  // resumed by main thread

    Ydb_WriteOp* op1 = new Ydb_WriteOp("keyB", "t2");
    assertOk(sched_->AddOp(t2, op1)); 
    Pause();  // resumed by main thread

    Ydb_WriteOp* op2 = new Ydb_WriteOp("keyA", "t2");
    assertDeadlock(sched_->AddOp(t2, op2));

    t2->Abort();
  }
};

static void CheckResults(Ydb_Scheduler* sched, Ydb_Map* map) {
  Ydb_Trans* checkTrans = sched->OpenTransaction();

  string keyA;
  Ydb_ReadOp* op1 = new Ydb_ReadOp("keyA", &keyA);
  assertOk(sched->AddOp(checkTrans, op1)); 
  assert(keyA == "t1");

  string keyB; 
  Ydb_ReadOp* op2 = new Ydb_ReadOp("keyB", &keyB);
  assertOk(sched->AddOp(checkTrans, op2)); 
  assert(keyB == "t1");

  checkTrans->Commit();
}

int main(int argc, char *argv[]) {
  ythread::Mutex mutex1;
  ythread::CondVar condvar1(&mutex1);
  ythread::Mutex mutex2;
  ythread::CondVar condvar2(&mutex2);

  Ydb_Map *map = Ydb_Map::Open("/dev/null");
  assert(map != NULL);
  Ydb_Scheduler sched(map);

  TestThread1 t1(&sched, &mutex1, &condvar1);
  t1.Start();

  TestThread2 t2(&sched, &mutex2, &condvar2);
  t2.Start();

  // t1 writes keyA then waits
  t1.WaitForPause();
  t1.Continue();
  t1.WaitForPause();

  // t2 writes keyB then waits
  t2.WaitForPause();
  t2.Continue();
  t2.WaitForPause();

  // t1 attempts to write keyB and has to block on t2 then exits
  t1.Continue();

  // t2 attempts to write keyA but causes a deadlock so it fails and exits
  t2.Continue();

  // threads are done
  t1.Join();
  t2.Join();

  // Verify that t1 succeeded
  CheckResults(&sched, map);
 
  cout << "OK\n";
}
