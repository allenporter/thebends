#include "ydb_tree.h"

//-----------------------------------------------------------------------------
// Ydb_Tree
//-----------------------------------------------------------------------------

Ydb_Tree::Ydb_Tree() : root_(new Ydb_InnerNode()), depth_(0) {
}

Ydb_Tree::~Ydb_Tree() {
  delete root_;
}

bool Ydb_Tree::Put(const string& key, const string& value) {
  if (key == "") {
    return false;
  }

  Ydb_InsertResult result;
  root_->Insert(key, value, &result);

  if (result.split) {
    // The child split and this is the root, so we need to make a new parent
    // node/ pointing to the two new children.
    Ydb_InnerNode* new_root = new Ydb_InnerNode();
    new_root->InsertNode(root_, NULL);
    new_root->InsertNode(result.sibling, NULL);

    root_ = new_root;
    depth_++;
  }
  return true;
}

bool Ydb_Tree::Get(const string& key, string* value) {
  if (key == "") { // TODO(aporter): support empty key`
    return false;
  }
  return root_->Get(key, value);
}

bool Ydb_Tree::Del(const string& key) {
  if (key == "") { // TODO(aporter): support empty key
    return false;
  }
  return root_->Del(key);
}

//-----------------------------------------------------------------------------
// Ydb_Node
//-----------------------------------------------------------------------------

int Ydb_Node::GetInsertPosition(const string& key) {
  int i = 0;
  while ((i < size_) && (*(GetKey(i)) < key)) {
    ++i;
  }
  return i;
}

//-----------------------------------------------------------------------------
// Ydb_InnerNode
//-----------------------------------------------------------------------------

Ydb_InnerNode::~Ydb_InnerNode() {
  for (int i = 0; i < size_; i++) {
    delete nodes_[i];
  }
}

bool Ydb_InnerNode::IsFull() {
  return (size_ == kMaxInnerNodeEntries);
}

bool Ydb_InnerNode::Get(const string& key, string* value) {
  if (size_ == 0) {
    return false;
  }

  int pos = GetInsertPosition(key);
  if (pos == -1) {
    return false;
  }
  return nodes_[pos]->Get(key, value);
}

void Ydb_InnerNode::Insert(const string& key, const string& value,
                           Ydb_InsertResult* result) {
  if (result == NULL) {
    assert(!IsFull());
  }

  int pos = GetInsertPosition(key);
  assert(size_ < kMaxInnerNodeEntries);
  assert(pos < kMaxInnerNodeEntries);
  assert(pos <= size_);

  // TODO(aporter): What if no inner nodes exist??? Fix

  Ydb_Node* child = nodes_[pos];
  Ydb_InsertResult child_result;
  child->Insert(key, value, &child_result);

  if (child_result.split) {
    // The child split, so add a new child node to the index
    InsertNode(child_result.sibling, result);
  }
}

bool Ydb_InnerNode::Del(const string& key) {
  int pos = GetInsertPosition(key);
  assert(size_ < kMaxInnerNodeEntries);
  assert(pos < kMaxInnerNodeEntries);
  assert(pos <= size_);

  Ydb_Node* child = nodes_[pos];
  return child->Del(key);
}

void Ydb_InnerNode::InsertNode(Ydb_Node* node, Ydb_InsertResult* result) {
  if (result == NULL) {
    assert(!IsFull());
  }

  if (IsFull()) {
    // This node has no more empty index slots, so split a new sibling
    Ydb_InnerNode* sibling = new Ydb_InnerNode();
    ProduceSplit(sibling);
    sibling->InsertNode(node, NULL);
    result->split = true;
    result->sibling = sibling;
  } else {
    nodes_[size_] = node;
    size_++;
  }
}

void Ydb_InnerNode::ProduceSplit(Ydb_InnerNode* sibling) {
  assert(IsFull());
  int threshold = (kMaxInnerNodeEntries + 1) / 2;
  sibling->size_ = size_ - threshold;
  for (int i = 0; i < sibling->size_; ++i) {
    sibling->nodes_[i] = nodes_[threshold + i];
  }
  size_ = threshold;
}

const string* Ydb_InnerNode::GetKey(int i) {
  if (i < size_) {
    return nodes_[i]->GetKey(0);
  } else {
    return NULL;
  }
}

//-----------------------------------------------------------------------------
// Ydb_LeafNode
//-----------------------------------------------------------------------------

bool Ydb_LeafNode::IsFull() {
  return (size_ == kMaxLeafNodeEntries);
}

bool Ydb_LeafNode::Get(const string& key, string* value) {
  if (size_ == 0) {
    return false;
  }

  int pos = GetInsertPosition(key);
  if (pos == -1 || keys_[pos] != key) {
    return false;
  }
 
  *value = values_[pos];
  return true;
}

void Ydb_LeafNode::Insert(const string& key, const string& value,
                          Ydb_InsertResult* result) {
  if (result == NULL) {
    assert(!IsFull());
  }

  if (IsFull()) {
    Ydb_LeafNode* sibling = new Ydb_LeafNode();
    ProduceSplit(sibling);
    result->split = true;
    result->sibling = sibling;

    // If this key should belong to the new sibling then insert there and return
    if (key >= *(sibling->GetKey(0))) {
      sibling->Insert(key, value, NULL);
      return;
    }
  }

  int pos = GetInsertPosition(key);  
  assert(size_ < kMaxLeafNodeEntries);
  assert(pos < kMaxLeafNodeEntries);
  assert(pos <= size_);

  if (keys_[pos] == key) {
    // Update a key that already exists
    values_[pos] = value;
  } else {
    // Insert a new key in order.
    for (int i = size_; i > pos; --i) {
      keys_[i] = keys_[i-1];
      values_[i] = values_[i-1];
    }
    size_++;
    keys_[pos] = key;
    values_[pos] = value;
  }
}

bool Ydb_LeafNode::Del(const string& key) {
  if (size_ == 0) {
    return false;
  }

  int pos = GetInsertPosition(key);
  if (pos == -1 || keys_[pos] != key) {
    return false;
  }

  for (int i = pos + 1; i < size_; i++) {
    keys_[i - 1] = keys_[i];
    values_[i -1] = values_[i];
  } 
  size_--;
  return true;
}

void Ydb_LeafNode::ProduceSplit(Ydb_LeafNode* sibling) {
  assert(IsFull());
  int threshold = (kMaxLeafNodeEntries + 1) / 2;
  sibling->size_ = size_ - threshold;
  for (int i = 0; i < sibling->size_; ++i) {
    sibling->keys_[i] = keys_[threshold + i];
    sibling->values_[i] = values_[threshold + i];
  }
  size_ = threshold;
}

const string* Ydb_LeafNode::GetKey(int i) {
  if (i < size_) {
    return &keys_[i];
  } else {
    return NULL;
  }
}
