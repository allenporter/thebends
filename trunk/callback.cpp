#include "callback-inl.h"

namespace ythread {

class CallbackFunc : public Callback {
 public:
  CallbackFunc(void(*func)())
    : func_(func) { }
  virtual ~CallbackFunc() { }

  virtual void Execute() {
    (*func_)();
  }

 private:
  void(*func_)();
};

Callback* NewCallback(void (*func)()) {
  return new CallbackFunc(func);
}

}  // namespace ythread
