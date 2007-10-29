#include <vector>
#include <iostream>
#include "dir.h"
#include "file.h"

using namespace std;

namespace ramdisk {

static void assertNoSubdirs(const string& path) {
  assert(path.find('/') == string::npos);
}

Dir::Dir() : Node(S_IFDIR | DEFAULT_PERMS), parent_(NULL), disk_used_(0) {
  stat_.st_nlink = 2;
  stat_.st_size = 2;  // . and ..
}

Dir::Dir(Dir* parent, mode_t mode) : Node(S_IFDIR | (PMASK & mode)),
                                     parent_(parent),
                                     disk_used_(0) {
  assert((mode & S_IFMT) == S_IFDIR ||
         (mode & S_IFMT) == 0);
  stat_.st_nlink = 2;
  stat_.st_size = 2;  // . and ..
}

Dir::~Dir() {
  for (DirMap::const_iterator iter = dirs_.begin();
       iter != dirs_.end(); ++iter) {
    delete iter->second;
  }
  for (FileMap::const_iterator iter = files_.begin();
       iter != files_.end(); ++iter) {
    delete iter->second;
  }
  dirs_.clear();
  files_.clear();
}

void Dir::UpdateDiskUsed() {
  disk_used_ = 0;
  for (DirMap::const_iterator iter = dirs_.begin();
       iter != dirs_.end(); ++iter) {
    disk_used_ += iter->second->DiskUsed();
  }
  for (FileMap::const_iterator iter = files_.begin();
       iter != files_.end(); ++iter) {
    disk_used_ += iter->second->Size();
  }
  if (parent_ != NULL) {
    parent_->UpdateDiskUsed();
  }
}

size_t Dir::DiskUsed() {
  return disk_used_;
}

File* Dir::GetFile(const std::string& path) {
  assertNoSubdirs(path);
  FileMap::const_iterator iter = files_.find(path);  
  return ((iter == files_.end()) ? NULL : iter->second);
}

Dir* Dir::GetDir(const std::string& path) {
  assertNoSubdirs(path);
  DirMap::const_iterator iter = dirs_.find(path);  
  return ((iter == dirs_.end()) ? NULL : iter->second);
}

Node* Dir::GetNode(const std::string& path) {
  Node* node = GetFile(path);
  if (node != NULL) {
    return node;
  }
  node = GetDir(path);
  return node;
}

bool Dir::Unlink(const std::string& path) {
  assertNoSubdirs(path);
  FileMap::iterator iter = files_.find(path);
  if (iter != files_.end()) {
    delete iter->second;
    files_.erase(iter);
    stat_.st_size--;
    return true;
  }
  return false;
}

bool Dir::Rmdir(const std::string& path) {
  assertNoSubdirs(path);
  DirMap::iterator iter = dirs_.find(path);
  if (iter != dirs_.end()) {
    delete iter->second;
    dirs_.erase(iter);
    stat_.st_size--;
    return true;
  }
  return false;
}

void Dir::Readdir(vector<string>* files) {
  files->push_back(".");
  files->push_back("..");
  for (DirMap::const_iterator iter = dirs_.begin();
       iter != dirs_.end(); ++iter) {
    files->push_back(iter->first.c_str());
  }
  for (FileMap::const_iterator iter = files_.begin();
       iter != files_.end(); ++iter) {
    files->push_back(iter->first.c_str());
  }
}

File* Dir::CreateFile(const std::string& path) {
  return CreateFile(path, DEFAULT_PERMS);
}

File* Dir::CreateFile(const std::string& path, mode_t mode) {
  assertNoSubdirs(path);
  Node* exist = GetNode(path);
  if (exist != NULL) return NULL;

  File* file = new File(this, mode);
  files_[path] = file;
  stat_.st_size++;
  return file;
}

Dir* Dir::Mkdir(const std::string& path) {
  return Mkdir(path, DEFAULT_PERMS);
}

Dir* Dir::Mkdir(const std::string& path, mode_t mode) {
  assertNoSubdirs(path);
  Node* exist = GetNode(path);
  if (exist != NULL) return NULL;
  Dir* dir = new Dir(this, mode);
  dirs_[path] = dir;
  stat_.st_size++;
  return dir;
}

File* Dir::ReleaseFileNode(const std::string& path) {
  FileMap::iterator file_iter = files_.find(path);
  if (file_iter != files_.end()) {
    File* node = file_iter->second;
    files_.erase(file_iter);
    UpdateDiskUsed();
    return node;
  }
  return NULL;
}

Dir* Dir::ReleaseDirNode(const std::string& path) {
  DirMap::iterator dir_iter = dirs_.find(path);
  if (dir_iter != dirs_.end()) {
    Dir* node = dir_iter->second;
    dirs_.erase(dir_iter);
    UpdateDiskUsed();
    return node;
  }
  return NULL;
}

void Dir::InsertDirNode(const std::string& path, Dir* node) {
  assert(GetNode(path) == NULL);
  dirs_[path] = node;
  UpdateDiskUsed();
}

void Dir::InsertFileNode(const std::string& path, File* node) {
  File* exist = ReleaseFileNode(path);
  if (exist != NULL) {
    delete exist;
  }
  assert(GetNode(path) == NULL);
  files_[path] = node;
  UpdateDiskUsed();
}

Dir* GetNodeParent(Dir* root, const std::string& path, std::string* rest) {
  assert(path.size() > 0 && path[0] == '/');

  size_t pos = path.find('/', 1);
  if (pos != string::npos) {
    const std::string& next_dir_name = path.substr(1, pos - 1);
    Dir* next = root->GetDir(next_dir_name);
    if (next == NULL) {
      rest->clear();
      return NULL;
    }
    return GetNodeParent(next, path.substr(pos), rest);
  }
  *rest = path.substr(1);
  return root;
}

Dir* GetDir(Dir* root, const std::string& path) {
  if (path == "/") {
    return root;
  }
  string rest;
  Dir* parent = GetNodeParent(root, path, &rest);
  assert(rest.size() > 0);
  Dir* foo = parent->GetDir(rest);
  return foo;
}

File* GetFile(Dir* root, const std::string& path) {
  assert(path != "/");
  string rest;
  Dir* parent = GetNodeParent(root, path, &rest);
  assert(rest.size() > 0);
  return parent->GetFile(rest);
}


Node* GetNode(Dir* root, const std::string& path) {
  if (path == "/") {
    return root;
  }
  string rest;
  Dir* parent = GetNodeParent(root, path, &rest);
  assert(rest.size() > 0);
  return parent->GetNode(rest);
}

}  // namespace ramdisk
