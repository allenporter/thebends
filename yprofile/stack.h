// stack.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __STACK_H__
#define __STACK_H__

namespace profile {

// Returns true if 'address' is a function below the stack frame, starting at
// 'start_frame' ie, was the function at 'address' called before the function at
// address 'start_frame'
bool IsBelowStackFrame(void *start_frame, void *address);

}

#endif  // __STACK_H__
