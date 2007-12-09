// select.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __YNET_SELECT_H__
#define __YNET_SELECT_H__

#include <map>
#include <vector>
#include <ythread/callback.h>

namespace ynet {

typedef ythread::Callback1<int> ReadyCallback;

// Select is NOT threadsafe
class Select {
 public:
  explicit Select();
  virtual ~Select();

  void AddReadFd(int fd, ReadyCallback*);
  void RemoveReadFd(int fd);

  void AddWriteFd(int fd, ReadyCallback*);
  void RemoveWriteFd(int fd);

  void Start();
  void Stop();

 private:
  int nfds() const;

  std::vector<int> readfds_;
  std::vector<int> writefds_;
  std::map<int, ReadyCallback*> read_callbacks_;
  std::map<int, ReadyCallback*> write_callbacks_;
  bool looping_;
};

}  // namespace ynet

#endif  // __YNET_SELECT_H__
