# client.swig

%module client

# For handling C++ strings
%include "std_string.i"

%apply int { mach_msg_id_t };

%exception {
  char* err;
  machrpc::ClearException();
  $action
  if ((err = machrpc::CheckException())) {
    PyErr_SetString(PyExc_IOError, err);
    return NULL;
  }
}

%{
#include "client.h"
%}

%include "client.h"
