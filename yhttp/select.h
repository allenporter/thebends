// select.h
// Author: Allen Porter <allen@thebends.org>

#ifndef _SELECT_H_
#define _SELECT_H_

#include <map>
#include <vector>
#include <ythread/callback.h>

namespace yhttpserver {

// Select is NOT threadsafe
class Select {
 public:
  explicit Select();
  virtual ~Select();

  typedef ythread::Callback1<int> AcceptCallback;

  void AddFd(int fd, AcceptCallback*);
  void RemoveFd(int fd);

  void Start();

 private:
  std::vector<int> nfds_;
  std::map<int, AcceptCallback*> read_callbacks_;
};

}  // namespace yhttpserver

#endif  // _SELECT_H_
