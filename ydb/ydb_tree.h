// ydb_tree.h
// Author: Allen Porter <allen@thebends.org>
//
// Ydb B+Tree implementation.  All data is saved in the leaves.  Internal nodes
// contain only keys and tree pointers.  All leaves are at the same lowest
// level.  Leaf nodes are also linked together as a linked list to make range
// queries easy.
//
// Deleting an entry does not change the structure of the tree.

#ifndef _YDB_BTREE_H_
#define _YDB_BTREE_H_

#include "ydb_base.h"

class Ydb_Node;

class Ydb_Tree {
 public:
  Ydb_Tree();
  ~Ydb_Tree();

  bool Put(const string& key, const string& value);
  bool Get(const string& key, string* value);
  bool Del(const string& key);

 private:
  Ydb_Node* root_;
  int depth_;
};

struct Ydb_InsertResult {
  bool split;
  Ydb_Node* sibling;
};

class Ydb_Node {
 public:
  virtual ~Ydb_Node() { };

  virtual int Size() { return size_; }
 
  virtual bool Get(const string& key, string* value) = 0;

  // Inserts the specified key and value.  If the key already exists, it will
  // replace the existing key.  Insertion will fail if the Leaf node is full.
  // TODO(aporter): result
  virtual void Insert(const string& key, const string& value,
                      Ydb_InsertResult* result) = 0;

  virtual bool Del(const string& key) = 0;

  virtual const string* GetKey(int i) = 0;

 protected:
  // Ydb_Node cannot be instantiated directly
  Ydb_Node() : size_(0) { };

  // Returns the index position that the specific key should be inserted into,
  // keeping the leaf node in sorted order.
  int GetInsertPosition(const string& key);

  int size_;
};

class Ydb_InnerNode : public Ydb_Node {
 public:
  static const int kMaxInnerNodeEntries = 10;

  Ydb_InnerNode() : Ydb_Node() { }
  virtual ~Ydb_InnerNode();

  virtual bool Get(const string& key, string* value);

  virtual void Insert(const string& key, const string& value,
                      Ydb_InsertResult* result);

  void InsertNode(Ydb_Node* node, Ydb_InsertResult* result);

  virtual bool Del(const string& key);

  // Returns the first key of the leaf node by index
  virtual const string* GetKey(int i);

 private:
  bool IsFull();

  void ProduceSplit(Ydb_InnerNode *sibling);

  Ydb_Node* nodes_[kMaxInnerNodeEntries];
};

class Ydb_LeafNode : public Ydb_Node {
 public:
  static const int kMaxLeafNodeEntries = 100;

  Ydb_LeafNode() : Ydb_Node() { }
  virtual ~Ydb_LeafNode() { }

  virtual bool Get(const string& key, string* value);

  // Inserts the specified key and value.  If the key already exists, it will
  // replace the existing key.  Insertion will fail if the Leaf node is full.
  virtual void Insert(const string& key, const string& value,
                      Ydb_InsertResult* result);

  virtual bool Del(const string& key);

  virtual const string* GetKey(int i);

 protected:
  bool IsFull();

  // Copies half of the keys in this node to the specified sibling leaf node.
  // This call fails if this Leaf is not full.
  void ProduceSplit(Ydb_LeafNode *sibling);

  string keys_[kMaxLeafNodeEntries];
  string values_[kMaxLeafNodeEntries];
};

#endif  // _YDB_TREE_H_
