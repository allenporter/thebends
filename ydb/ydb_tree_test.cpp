#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include "assert.h"

#include "ydb_tree.h"

static const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void Random(int length, string* str) {
  int num_letters = sizeof(letters);
  *str = "";
  for (int i = 0; i < length; i++) {
    str->append(&letters[random() % num_letters]);
  }
}

void check_leaf_node(Ydb_Node* node) {
  for (int i = 0; i < node->Size() - 1; ++i) {
    assert(node->GetKey(i) < node->GetKey(i + 1)); 
  }
}

void test_leaf_insert() {
  Ydb_LeafNode* node = new Ydb_LeafNode();

  string key;
  string value;

  // Fill up the leaf node
  for (int i = 0; i < Ydb_LeafNode::kMaxLeafNodeEntries; ++i) {
    Random(10, &key);
    Random(10, &value);
    node->Insert(key, value, NULL);
  }
  assert(node->Size() == Ydb_LeafNode::kMaxLeafNodeEntries);
  check_leaf_node(node);

  Ydb_InsertResult result;

  Random(10, &key);
  Random(10, &value);
  node->Insert(key, value, &result);

  assert(result.split);
  check_leaf_node(node);
  check_leaf_node(result.sibling);
  assert(node->Size() > 0);
  assert(result.sibling->Size() > 0);
  assert(node->Size() + result.sibling->Size() ==
         Ydb_LeafNode::kMaxLeafNodeEntries + 1);
}

void test_leaf_duplicate_insert() {
  Ydb_LeafNode* node = new Ydb_LeafNode();
  for (int i = 0; i < Ydb_LeafNode::kMaxLeafNodeEntries * 2; ++i) {
    node->Insert("test", "value", NULL);
    assert(node->Size() == 1);
  }

  node->Insert("a", "some value", NULL);
  node->Insert("z", "some value", NULL);

  for (int i = 0; i < Ydb_LeafNode::kMaxLeafNodeEntries * 2; ++i) {
    node->Insert("a", "aaaaa", NULL);
    node->Insert("test", "aaaaa", NULL);
    node->Insert("z", "aaaaa", NULL);
    assert(node->Size() == 3);
  }
}

void test_leaf_get() {
  Ydb_LeafNode* node = new Ydb_LeafNode();
  node->Insert("a", "aaa", NULL);
  node->Insert("b", "bbb", NULL);
  node->Insert("c", "ccc", NULL);

  string val;
  assert(node->Get("a", &val));
  assert(val == "aaa");
  assert(node->Get("b", &val));
  assert(val == "bbb");
  assert(node->Get("c", &val));
  assert(val == "ccc");

  // Update an existing value
  node->Insert("c", "ccc++", NULL);
  assert(node->Get("c", &val));
  assert(val == "ccc++");

  // Insert between existing values
  node->Insert("aa", "new value", NULL);
  assert(node->Get("aa", &val));
  assert(val == "new value");
}

void test_leaf_delete() {
  Ydb_LeafNode* node = new Ydb_LeafNode();
  node->Insert("a", "aaa", NULL);
  node->Insert("b", "bbb", NULL);
  node->Insert("c", "ccc", NULL);

  assert(node->Del("b"));
  assert(!node->Del("b"));

  string val;
  assert(node->Get("a", &val));
  assert(val == "aaa");
  assert(!node->Get("b", &val));
  assert(node->Get("c", &val));
  assert(val == "ccc");

  assert(node->Del("a"));
  assert(!node->Del("a"));

  assert(!node->Get("a", &val));
  assert(!node->Get("b", &val));
  assert(node->Get("c", &val));
  assert(val == "ccc");

  assert(node->Del("c"));
  assert(!node->Del("c"));

  assert(!node->Get("a", &val));
  assert(!node->Get("b", &val));
  assert(!node->Get("c", &val));

  node->Insert("z", "zzz", NULL);
  assert(node->Get("z", &val));
  assert(val == "zzz");
  assert(node->Del("z"));
}

int main(int argc, char *argv[]) {
  srandomdev();

  test_leaf_insert();
  test_leaf_duplicate_insert();
  test_leaf_get();
  test_leaf_delete();

  cout << "OK" << endl;
}
