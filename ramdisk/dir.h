// Written by Allen Porter <allen@thebends.org>

#ifndef __RAMDISK_DIR_H__
#define __RAMDISK_DIR_H__

#include <map>
#include <string>
#include <vector>
#include <fuse.h>
#include "node.h"

namespace ramdisk {

class File;

class Dir : public Node {
 public:
  typedef std::map<std::string, Dir*> DirMap;
  typedef std::map<std::string, File*> FileMap;

  Dir();
  Dir(Dir* parent, mode_t mode);
  virtual ~Dir();

  void UpdateDiskUsed();
  size_t DiskUsed();

  File* CreateFile(const std::string& path);
  File* CreateFile(const std::string& path, mode_t mode);

  File* GetFile(const std::string& path);
  bool Unlink(const std::string& path);

  Dir* Mkdir(const std::string& path);
  Dir* Mkdir(const std::string& path, mode_t mode);

  Dir* GetDir(const std::string& path);
  bool Rmdir(const std::string& path);

  Node* GetNode(const std::string& path);

  void Readdir(std::vector<std::string>* files);

  // For rename operation
  File* ReleaseFileNode(const std::string& path);
  Dir* ReleaseDirNode(const std::string& path);
  void InsertFileNode(const std::string& path, File* node);
  void InsertDirNode(const std::string& path, Dir* node);

 private:
  Dir* parent_;
  size_t disk_used_;
  DirMap dirs_;
  FileMap files_;
};

Dir* GetNodeParent(Dir* root, const std::string& path, std::string* filename);

Dir* GetDir(Dir* root, const std::string& path);

File* GetFile(Dir* root, const std::string& path);

Node* GetNode(Dir* root, const std::string& path);

}  // namespace ramdisk

#endif  // __RAMDISK_DIR_H__
