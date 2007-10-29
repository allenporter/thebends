#ifndef _YTHREAD_CALLBACK_INL_H_
#define _YTHREAD_CALLBACK_INL_H_
#include "callback.h"

namespace ythread {

template <class ARG>
class CallbackFuncArg1Impl : public Callback {
 public:
  CallbackFuncArg1Impl(void(*func)(ARG), ARG arg)
    : func_(func), arg_(arg) { }
  virtual ~CallbackFuncArg1Impl() { }

  virtual void Execute() {
    (*func_)(arg_);
  }

 private:
  void(*func_)(ARG);
  ARG arg_;
};

template <class ARG>
Callback* NewCallback(void(*func)(ARG), ARG arg) {
  return new CallbackFuncArg1Impl<ARG>(func, arg);
}

template <class CLASS>
class CallbackImpl : public Callback {
 public:
  CallbackImpl(CLASS* object, void(CLASS::*func)())
    : object_(object), func_(func) { }
  virtual ~CallbackImpl() { }

  virtual void Execute() {
    (object_->*func_)();
  }

 private:
  CLASS* object_;
  void(CLASS::*func_)();
};

template <class CLASS>
Callback* NewCallback(CLASS* object, void(CLASS::*func)()) {
  return new CallbackImpl<CLASS>(object, func);
}

template <class CLASS, class ARG>
class CallbackArg1Impl : public Callback {
 public:
  CallbackArg1Impl(CLASS* object, void(CLASS::*func)(ARG), ARG arg)
    : object_(object), func_(func), arg_(arg) { }
  virtual ~CallbackArg1Impl() { }

  virtual void Execute() {
    (object_->*func_)(arg_);
  }

 private:
  CLASS* object_;
  void(CLASS::*func_)(ARG);
  ARG arg_;
};

template <class CLASS, class ARG>
Callback* NewCallback(CLASS* object, void(CLASS::*func)(ARG), ARG arg) {
  return new CallbackArg1Impl<CLASS, ARG>(object, func, arg);
}

template <class CLASS, class ARG>
class Callback1Impl : public Callback1<ARG> {
 public:
  Callback1Impl(CLASS* object, void(CLASS::*func)(ARG))
    : object_(object), func_(func) { }
  virtual ~Callback1Impl() { }

  virtual void Execute(ARG arg) {
    (object_->*func_)(arg);
  }

 private:
  CLASS* object_;
  void(CLASS::*func_)(ARG arg);
};

template <class CLASS, class ARG>
Callback1<ARG>* NewCallback(CLASS* object, void(CLASS::*func)(ARG)) {
  return new Callback1Impl<CLASS, ARG>(object, func);
}

template <class CLASS, class ARG1, class ARG2>
class Callback2Impl : public Callback2<ARG1, ARG2> {
 public:
  Callback2Impl(CLASS* object, void(CLASS::*func)(ARG1, ARG2))
    : object_(object), func_(func) { }
  virtual ~Callback2Impl() { }

  virtual void Execute(ARG1 arg1, ARG2 arg2) {
    (object_->*func_)(arg1, arg2);
  }

 private:
  CLASS* object_;
  void(CLASS::*func_)(ARG1 arg1, ARG2 arg2);
};

template <class CLASS, class ARG1, class ARG2>
Callback2<ARG1, ARG2>* NewCallback(CLASS* object,
                                   void(CLASS::*func)(ARG1, ARG2)) {
  return new Callback2Impl<CLASS, ARG1, ARG2>(object, func);
}

}  // namespace ythread

#endif  // _YTHREAD_CALLBACK_INL_H_
