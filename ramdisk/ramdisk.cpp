// TODO: File locking?
// TODO: Update # of free bytes
// TODO: respect file open flags
#include <iostream>
#include <errno.h>
#include "ramdisk.h"
#include "dir.h"
#include "file.h"

using namespace std;

namespace ramdisk {

static Dir* root = NULL;
static int const kBlockSize = 1024;
static int disk_size;

static int ramfs_getattr(const char *path, struct stat *stbuf) {
#ifndef NDEBUG
  cerr << "ramfs_getattr(" << path << ")" << endl;
#endif

  Node* node = GetNode(root, path);
  if (node == NULL) {
    return -ENOENT;
  }
  node->GetAttr(stbuf);
  return 0;
}

static int ramfs_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info* fi) {
#ifndef NDEBUG
  cerr << "ramfs_readdir(" << path << ")" << endl;
#endif

  Dir* dir = GetDir(root, path);
  if (dir == NULL) {
    return -ENOENT;
  }

  vector<string> files;
  dir->Readdir(&files);
  for (vector<string>::const_iterator it = files.begin(); it != files.end();
       ++it) {
#ifndef NDEBUG
    if (*it != "." && *it != "..") {
      assert(dir->GetNode(it->c_str()) != NULL);
    }
#endif
    filler(buf, it->c_str(), NULL, 0);
  }
  return 0;
}

static int ramfs_open(const char* path, struct fuse_file_info* fi) {
#ifndef NDEBUG
  cerr << "ramfs_open(" << path << ")" << endl;
#endif

  string filename;
  Dir* parent = GetNodeParent(root, path, &filename);
  if (parent == NULL) {
    return -ENOENT;
  }
  Node* file = parent->GetFile(filename);
  if (file == NULL) {
    return -ENOENT;
  }
  fi->fh = (uint64_t)file;
  return 0;
}

static int ramfs_create(const char* path, mode_t mode,
                        struct fuse_file_info* fi) {
#ifndef NDEBUG
  cerr << "ramfs_create(" << path << ", 0" << oct << mode << dec << ")"
       << endl;
#endif

  string filename;
  Dir* parent = GetNodeParent(root, path, &filename);
  if (parent == NULL) {
    return -ENOENT;
  }
  Node* file = parent->CreateFile(filename, mode);
  if (file == NULL) {
    return -ENOENT;
  }
  fi->fh = (uint64_t)file;
  return 0;
}

static int ramfs_release(const char *path, struct fuse_file_info *fi) {
#ifndef NDEBUG
  cerr << "ramfs_release(" << path << ")" << endl;
#endif

  return 0;
}

static int ramfs_unlink(const char* path) {
#ifndef NDEBUG
  cerr << "ramfs_unlink(" << path << ")" << endl;
#endif

  string filename;
  Dir* parent = GetNodeParent(root, path, &filename);
  if (!parent->Unlink(filename)) {
    return -ENOENT;
  }
  return 0;
}

static int ramfs_rename(const char* from, const char* to) {
#ifndef NDEBUG
  cerr << "ramfs_rename(" << from << ", " << to << ")" << endl;
#endif
  string filename1;
  Dir* parent1 = GetNodeParent(root, from, &filename1);
  string filename2;
  Dir* parent2 = GetNodeParent(root, to, &filename2);
  if (parent1 == NULL || parent2 == NULL) {
    return -ENOENT;
  }

  File* f = parent1->ReleaseFileNode(filename1);
  if (f != NULL) {
    parent2->InsertFileNode(filename2, f);
    return 0;
  } else {
    Dir* d = parent1->ReleaseDirNode(filename1);
    if (d != NULL) {
      parent2->InsertDirNode(filename2, d);
      return 0;
    }
  }
cout << "f1=" << filename1;
cout << "f2=" << filename2;
  return -ENOENT;
}

static int ramfs_chmod(const char* path, mode_t mode) {
#ifndef NDEBUG
  cerr << "ramfs_chmod(" << path << ", 0" << oct << mode << dec << ")" << endl;
#endif

  Node* node = GetNode(root, path);
  if (node == NULL) {
    return -ENOENT;
  }
  node->Chmod(mode);
  return 0;
}

static int ramfs_chown(const char* path, uid_t uid, gid_t gid) {
#ifndef NDEBUG
  cerr << "ramfs_chown(" << path << ")" << endl;
#endif

  Node* node = GetNode(root, path);
  if (node == NULL) {
    return -ENOENT;
  }
  node->Chown(uid, gid);
  return 0;
}

static int ramfs_truncate(const char* path, off_t offset) {
#ifndef NDEBUG
  cerr << "ramfs_truncate(" << path << ", " << offset << ")" << endl;
#endif

  File* file = GetFile(root, path);
  if (file == NULL) {
    return -ENOENT;
  }
  file->Truncate(offset);
  return 0;
}

static int ramfs_read(const char* path, char* buf, size_t size,
                      off_t offset, struct fuse_file_info* fi) {
#ifndef NDEBUG
  cerr << "ramfs_read(" << path << ", " << size << ")" << endl;
#endif

  assert(fi->fh != 0);
  File* file = (File*)fi->fh;
  return file->Read(buf, size, offset);
}

static int ramfs_write(const char* path, const char* buf, size_t size,
                       off_t offset, struct fuse_file_info* fi) {
#ifndef NDEBUG
  cerr << "ramfs_write(" << path << ", " << size << ")" << endl;
#endif

  assert(fi->fh != 0);
  File* file = (File*)fi->fh;
  return file->Write(buf, size, offset);
}

static int ramfs_utimens(const char* path, const struct timespec tv[2]) {
#ifndef NDEBUG
  cerr << "ramfs_utimens(" << path << ")" << endl;
#endif

  return 0;
}

static int ramfs_mkdir(const char* path, mode_t mode) {
#ifndef NDEBUG
  cerr << "ramfs_mkdir(" << path << ", 0" << oct << mode << dec << ")"
       << endl;
#endif

  string filename;
  Dir* parent = GetNodeParent(root, path, &filename);
  if (parent == NULL) {
    return -ENOENT;
  }

  Dir* dir = parent->Mkdir(filename, mode);
  if (dir == NULL) {
    return -ENOENT;
  }
  return 0;
}

static int ramfs_rmdir(const char* path) {
#ifndef NDEBUG
  cerr << "ramfs_rmdir(" << path << ")" << endl;
#endif

  string filename;
  Dir* parent = GetNodeParent(root, path, &filename);
  if (parent == NULL) {
    return -ENOENT;
  }
  return parent->Rmdir(filename) ? 0 : -ENOENT;
}

static int ramfs_statfs(const char* path, struct statvfs* vfs) {
#ifndef NDEBUG
  cerr << "ramfs_statfs(" << path << ")" << endl;
#endif

  assert(string(path) == "/");
  int blockSize = kBlockSize;
  unsigned long long totalBytes = disk_size;
  unsigned long long freeBytes = disk_size - root->DiskUsed();
  memset(vfs, 0, sizeof(struct statvfs));
  vfs->f_namemax = 255;
  vfs->f_bsize = blockSize;
  vfs->f_frsize = vfs->f_bsize;
  vfs->f_frsize = vfs->f_bsize;
  vfs->f_blocks = totalBytes / blockSize;
  vfs->f_bfree = vfs->f_bavail = freeBytes / blockSize;
  vfs->f_files = 110000;
  vfs->f_ffree = vfs->f_files - 10000;
  return 0;
}

void NewRamdisk(struct fuse_operations* oper, size_t bytes) {
  if (root != NULL) {
    delete root;
  }
  root = new Dir;
  disk_size = bytes;

  oper->getattr = ramfs_getattr;
  oper->readdir = ramfs_readdir;
  oper->open = ramfs_open;
  oper->create = ramfs_create;
  oper->release = ramfs_release;
  oper->read = ramfs_read;
  oper->write = ramfs_write;
  oper->truncate = ramfs_truncate;
  oper->unlink = ramfs_unlink;
  oper->rename = ramfs_rename;
  oper->mkdir = ramfs_mkdir;
  oper->rmdir = ramfs_rmdir;
  oper->statfs = ramfs_statfs;
  oper->chmod = ramfs_chmod;
  oper->chown = ramfs_chown;
  oper->utimens = ramfs_utimens;
}

}  // namespace ramdisk
