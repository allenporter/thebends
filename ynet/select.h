// select.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __YNET_SELECT_H__
#define __YNET_SELECT_H__

#include <map>
#include <set>
#include <vector>
#include <ythread/callback.h>
#include <ythread/mutex.h>

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

  // Threadsafe methods
  void AddCallback(ythread::Callback* callback);

 private:
  int nfds() const;

  std::vector<int> readfds_;
  std::vector<int> writefds_;
  std::map<int, ReadyCallback*> read_callbacks_;
  std::map<int, ReadyCallback*> write_callbacks_;
  bool looping_;

  ythread::Mutex callbacks_mutex_;  // protects callbacks_
  std::set<ythread::Callback*> callbacks_;
};

}  // namespace ynet

#endif  // __YNET_SELECT_H__
