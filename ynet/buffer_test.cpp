// buffer_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <assert.h>
#include <iostream>
#include <string>
#include "buffer.h"

using namespace std;

string MakeString(uint8_t size) {
  string data;
  size_t i;
  for (i = 0; i < size; i++) {
    const char c = 'A' + i;
    data += c;
  }
  return data;
}

void Offset(ynet::Buffer* buf, int offset) {
  assert(offset > 0);
  assert(buf->Size() == 0);
  const string& data = MakeString(offset);
  assert(buf->Write(data.data(), data.size()));
  assert(buf->Size() == offset);
  char buffer[offset];
  assert(buf->Read(buffer, offset));
  assert(buf->Size() == 0);
}

void Fill(ynet::Buffer* buf, int step) {
  assert(step > 0);
  assert(buf->Size() == 0);
  assert(buf->SizeLeft() > 0);
  const char* data = MakeString(buf->SizeLeft()).c_str();
  int i = 0;
  while (buf->SizeLeft() > step) {
    assert(buf->Write(data + i, step));
    i += step;
  }
  if (buf->SizeLeft() > 0) {
    assert(buf->Write(data + i, buf->SizeLeft()));
  }
  assert(buf->SizeLeft() == 0);
}

void Verify(ynet::Buffer* buf) {
  const string& data = MakeString(buf->Size());
  int len = buf->Size();
  char buf_data[len + 1];
  buf->Read(buf_data, len);
  buf_data[len] = '\0';
  assert(data == string(buf_data));
}

void test1() {
  ynet::Buffer buf(5);
  int i;
  for (i = 0; i < 5; i++) {
    assert(buf.Size() == 0);
    assert(buf.SizeLeft() == 5);
    assert(buf.Write("abc", 3)); 
    assert(buf.Size() == 3);
    assert(buf.SizeLeft() == 2);
    char data[5];
    assert(buf.Peek(data, 1));
    assert(buf.Peek(data, 2));
    assert(buf.Peek(data, 3));
    assert(!buf.Peek(data, 4));
    assert(buf.Advance(3));
    assert(buf.Size() == 0);
    assert(buf.SizeLeft() == 5);
    assert(buf.Unadvance(1));
    assert(buf.Size() == 1);
    assert(buf.SizeLeft() == 4);
    assert(buf.Advance(1));
  }
}

void test2() {
  int bufsize;
  int offset;
  int step;
  for (bufsize = 1; bufsize < 10; bufsize++) {
    for (offset = 0; offset < bufsize; offset++) {
      for (step = 1; step < bufsize; step++) {
        ynet::Buffer buf(bufsize);
        if (offset > 0) {
          Offset(&buf, offset);
        }
        assert(buf.Size() == 0);
        Fill(&buf, step);
        Verify(&buf);
      }
    }
  }
}

int main(int argc, char* argv[]) {
  test1();
  test2();
  cout << "PASS" << endl;
}
