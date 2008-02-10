// select.cpp
// Author: Allen Porter <allen@thebends.org>

#include "select.h"

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <sysexits.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>

#include <algorithm>
#include <iostream>

using namespace std;

namespace ynet {

static void Add(int fd,
                vector<int>* fds,
                map<int, ReadyCallback*>* callbacks,
                ReadyCallback* cb) {
  if (callbacks->find(fd) != callbacks->end()) {
    errx(EX_SOFTWARE, "Got duplicate read request for fd: %u", fd);
  }
  fds->push_back(fd);
  make_heap(fds->begin(), fds->end());
  (*callbacks)[fd] = cb;
}

static void Remove(int fd,
                   vector<int>* fds,
                   map<int, ReadyCallback*>* callbacks) {
  if (callbacks->find(fd) == callbacks->end()) {
    errx(EX_SOFTWARE, "Removed fd that was not being read: %d\n", fd);
  }
  delete (*callbacks)[fd];
  callbacks->erase(fd);
  for (std::vector<int>::iterator it = fds->begin();
       it != fds->end(); ++it) {
    if (*it == fd) {
      fds->erase(it);
      break;
    }
  }
}

static void GetFdSet(fd_set* fds, vector<int>* fd_list) {
  FD_ZERO(fds);
  for (std::vector<int>::const_iterator it = fd_list->begin();
      it != fd_list->end(); ++it) {
    FD_SET(*it, fds);
  }
}

Select::Select() : looping_(false) { }

Select::~Select() {
  assert(readfds_.size() == 0);
  assert(writefds_.size() == 0);
}

void Select::AddReadFd(int fd, ReadyCallback* cb) {
  Add(fd, &readfds_, &read_callbacks_, cb);
}

void Select::RemoveReadFd(int fd) {
  Remove(fd, &readfds_, &read_callbacks_);
}

void Select::AddWriteFd(int fd, ReadyCallback* cb) {
  Add(fd, &writefds_, &write_callbacks_, cb);
}

void Select::RemoveWriteFd(int fd) {
  Remove(fd, &writefds_, &write_callbacks_);
}

void Select::Start() {
  looping_ = true;
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 1000;
  while (looping_) {
    fd_set readfds;
    GetFdSet(&readfds, &readfds_);
    fd_set writefds;
    GetFdSet(&writefds, &writefds_);
    if (select(nfds(), &readfds, &writefds, NULL, &timeout) == -1) {
      if (errno == EINTR) {
        // Interruped, try again
        break;
      }
      err(EX_OSERR, "select()");
    }
    // We make a copy of the file descriptor list so that we don't invalidate
    // the iterator if one of the callbacks modifies it.
    std::vector<int> fds;
    fds.insert(fds.begin(), readfds_.begin(), readfds_.end());
    fds.insert(fds.begin(), writefds_.begin(), writefds_.end());
    for (std::vector<int>::const_iterator it = fds.begin();
         it != fds.end(); ++it) {
      unsigned int fd = *it;
      if (FD_ISSET(fd, &readfds)) {
        if (read_callbacks_.count(fd) == 0) {
          // We're no longer interested in this socket; Ignoring.
          continue;
        }
        (read_callbacks_[fd])->Execute(fd);
      }
      if (FD_ISSET(fd, &writefds)) {
        if (write_callbacks_.count(fd) == 0) {
          // We're no longer interested in this socket; Ignoring.
          continue;
        }
        (write_callbacks_[fd])->Execute(fd);
      }
    }
    set<ythread::Callback*> callbacks;
    callbacks_mutex_.Lock();
    callbacks = callbacks_;
    callbacks_.clear();
    callbacks_mutex_.Unlock();
    for (set<ythread::Callback*>::const_iterator it = callbacks.begin();
         it != callbacks.end(); ++it) {
      (*it)->Execute();
    }
  }
}

void Select::Stop() {
  looping_ = false;
}

#define max(x, y) (((x) > (y)) ? (x) : (y))

int Select::nfds() const {
  int n = 0;
  if (readfds_.size() > 0) {
    n = max(n, readfds_.front());
  }
  if (writefds_.size() > 0) {
    n = max(n, writefds_.front());
  }
  return n + 1;
}

void Select::AddCallback(ythread::Callback* callback) {
  callbacks_mutex_.Lock();
  callbacks_.insert(callback);
  callbacks_mutex_.Unlock();
}

}  // ynet
