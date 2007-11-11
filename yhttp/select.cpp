#include "select.h"
#include <algorithm>
#include <err.h>
#include <sysexits.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>

namespace yhttpserver {

Select::Select() : nfds_(0), looping_(false) { }

Select::~Select() {
  // TODO(aporter): delete accept callbacks?
}

void Select::AddFd(int fd, AcceptCallback* cb) {
  if (read_callbacks_.find(fd) != read_callbacks_.end()) {
    errx(EX_SOFTWARE, "Got duplicate read request for fd: %u", fd);
  }
  nfds_.push_back(fd);
  make_heap(nfds_.begin(), nfds_.end());
  read_callbacks_[fd] = cb;
}

void Select::RemoveFd(int fd) {
  if (read_callbacks_.find(fd) == read_callbacks_.end()) {
    errx(EX_SOFTWARE, "Removed fd that was not being read: %d\n", fd);
  }
  delete read_callbacks_[fd];
  read_callbacks_.erase(fd);
  for (std::vector<int>::iterator it = nfds_.begin();
       it != nfds_.end(); ++it) {
    if (*it == fd) {
      nfds_.erase(it);
      break;
    }
  }
}

void Select::Start() {
  looping_ = true;
  while (looping_) {
    fd_set readfds;
    FD_ZERO(&readfds);
    int nfds = nfds_.front() + 1;
    std::vector<int> fds;
    fds.insert(fds.begin(), nfds_.begin(), nfds_.end());
    for (std::vector<int>::const_iterator it = fds.begin();
         it != fds.end(); ++it) {
      FD_SET(*it, &readfds);
    }
    if (select(nfds, &readfds, NULL, NULL, NULL) == -1) {
      err(EX_OSERR, "select()");
    }
    for (std::vector<int>::const_iterator it = fds.begin();
         it != fds.end(); ++it) {
      unsigned int fd = *it;
      if (FD_ISSET(fd, &readfds)) {
        (read_callbacks_[fd])->Execute(fd);
      }
    }
  }
}

void Select::Stop() {
  looping_ = false;
}

}  // yhttpserver
