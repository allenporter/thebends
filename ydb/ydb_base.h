// ydb_base.h
// Author: Allen Porter <allen@thebends.org>
//

#ifndef _YDB_UTIL_H_
#define _YDB_UTIL_H_

#include <string>
using namespace std;

#ifdef NDEBUG
#define YDEBUG(x) ((void)0)(x)
#else
#include <sstream> 
#include <fstream> 
#include <iostream> 
#define YDEBUG(x) do { using namespace std; \
                       stringstream z_; \
                       z_ << "|" << __FILE__ << ":" << __LINE__ << "| " \
                          << x; \
                       cerr << z_.str () << endl; \
                       } while (false)
#endif

// Return code from a Ydb operation
enum Ydb_RetCode {
  // OK : Operation completed successfullly
  YDB_OK = 0,

  // NOT_FOUND : The requested key was not found.
  YDB_NOT_FOUND,

  // DEADLOCK : The operation was aborted because it would have resulted in
  // a deadlock.  The operation may succeed if it is retried.
  YDB_DEADLOCK,
};

#endif // _YDB_UTIL_H_
