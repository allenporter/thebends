#undef NDEBUG
#include <iostream>
#include <stdlib.h>
#include "dir.h"
#include "file.h"

using namespace std;

void test_tree() {
  ramdisk::Dir root;

  std::string rest;
  assert(&root == GetNodeParent(&root, "/", &rest));
  assert(rest == "");

  assert(NULL == root.GetNode("a"));
  assert(NULL == root.GetFile("a"));
  assert(NULL == root.GetDir("a"));

  root.Mkdir("a");

  ramdisk::Dir* a = root.GetDir("a");
  assert(a != NULL);

  ramdisk::Dir* b = a->Mkdir("b");
  assert(b != NULL);
  assert(b == a->GetDir("b"));

  ramdisk::Dir* parent = GetNodeParent(&root, "/a", &rest);
  assert(parent == &root);
  assert(rest == "a");

  parent = GetNodeParent(&root, "/a/b", &rest);
  assert(parent == a);
  assert(rest == "b");
}

void test_getnode() {
  ramdisk::Dir root;
  assert(&root == GetDir(&root, "/"));
  assert(NULL == GetDir(&root, "/a"));

  ramdisk::Dir* node = root.Mkdir("a");
  assert(node == GetNode(&root, "/a"));

  ramdisk::Node* node2 = root.CreateFile("b");
  assert(node2 == GetNode(&root, "/b"));

  ramdisk::Dir* node3 = node->Mkdir("b");
  assert(node3 == GetNode(&root, "/a/b"));
}

void test_create() {
  ramdisk::Dir root;
  assert(root.Size() == 2);

  assert(root.GetFile("a") == NULL);
  assert(root.GetDir("a") == NULL);
  assert(root.GetNode("a") == NULL);

  ramdisk::File* file = root.CreateFile("a");
  assert(file != NULL);
  assert(root.GetFile("a") == file);
  assert(root.Mkdir("a") == NULL);
  assert(root.GetDir("a") == NULL);
  
  ramdisk::Dir* dir = root.Mkdir("b");
  assert(dir != NULL);
  assert(root.GetDir("b") == dir);
  assert(root.CreateFile("b") == NULL);
  assert(root.GetFile("b") == NULL);

  ramdisk::Dir* dir1 = root.Mkdir("c");
  assert(dir1 != NULL);

  assert(root.Size() == 5);
}

void test_readdir() {
  ramdisk::Dir root;
  root.CreateFile("a");
  root.Mkdir("b");
  root.CreateFile("c");

  vector<string> files;
  root.Readdir(&files);
  assert(files.size() == 5);
  assert(files[0] == ".");
  assert(files[1] == "..");
  assert(files[2] == "b");
  assert(files[3] == "a");
  assert(files[4] == "c");
}

void test_getattr() {
  struct stat st;
  ramdisk::Dir dir;
  dir.GetAttr(&st);
  assert((st.st_mode & S_IFMT) == S_IFDIR);

  dir.Chmod(0765);;
  dir.GetAttr(&st);
  assert((st.st_mode & S_IFMT) == S_IFDIR);
  assert((st.st_mode & 0777) == 0765);

  dir.Chmod(0111);;
  dir.GetAttr(&st);
  assert((st.st_mode & S_IFMT) == S_IFDIR);
  assert((st.st_mode & 0777) == 0111);
}

int main(int argc, char* argv[]) {
  test_tree();
  test_getnode();
  test_create();
  test_readdir();
  test_getattr();
  printf("PASS\n");
  exit(0);
}
