// response_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <iostream>
#include <string>
#include "response.h"
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;
using ypeer::ResponseMessage;
using ypeer::ParseResponseMessage;

static void test1() {
  ResponseMessage msg;
  assert(!ParseResponseMessage("", &msg));

  assert(ParseResponseMessage("d14:failure reason3:fooe", &msg));
  assert("foo" == msg.failure_reason);

  assert(ParseResponseMessage("d15:warning message3:bar8:intervali3e8:completei10e10:incompletei11ee", &msg));
  assert("bar" == msg.warning_message);
  assert(3 == msg.interval);
  assert(10 == msg.complete);
  assert(11 == msg.incomplete);
}

static void test2() {
  ResponseMessage msg;
  assert(ParseResponseMessage("d5:peerslee", &msg));
  assert(0 == msg.peers.size());
  assert(ParseResponseMessage("d5:peers0:e", &msg));
  assert(0 == msg.peers.size());
}

static void test3() {
  ResponseMessage msg;
  assert(ParseResponseMessage("d5:peersld7:peer id1:a2:ip9:127.0.0.14:porti3030eeee", &msg));
  assert(1 == msg.peers.size());
  assert(msg.peers[0].peer_id == "a");
  assert(msg.peers[0].ip == "127.0.0.1");
  assert(msg.peers[0].port == 3030);
}

static void test4() {
  ResponseMessage msg;
  assert(ParseResponseMessage("d5:peersld7:peer id1:a2:ip9:127.0.0.14:porti3030eed7:peer id1:b2:ip9:127.0.0.24:porti3031eeeee", &msg));
  assert(2 == msg.peers.size());
  assert(msg.peers[0].peer_id == "a");
  assert(msg.peers[0].ip == "127.0.0.1");
  assert(msg.peers[0].port == 3030);
  assert(msg.peers[1].peer_id == "b");
  assert(msg.peers[1].ip == "127.0.0.2");
  assert(msg.peers[1].port == 3031);
}

static void test5() {
  ResponseMessage msg;
  assert(!ParseResponseMessage("d5:peers1:ae", &msg));
  assert(!ParseResponseMessage("d5:peers7:aaaaaaae", &msg));
  assert(ParseResponseMessage("d5:peers0:e", &msg));
}

struct host_port {
  in_addr addr;
  int16_t port;
};

static void test6() {
  ResponseMessage msg;
  std::string message;
  message.append("d5:peers");
  message.append("6:\x0a\x00\x00\x01\x04\xd2", 8);
  message.append("e");
  assert(ParseResponseMessage(message, &msg));
  assert(1 == msg.peers.size());
  assert(msg.peers[0].peer_id.empty());
  assert(msg.peers[0].ip == "10.0.0.1");
  assert(msg.peers[0].port == 1234);
}

static void test7() {
  ResponseMessage msg;
  std::string message;
  message.append("d5:peers");
  message.append("12:");
  message.append("\x0a\x00\x00\x01\x04\xd2", 6);
  message.append("\x0a\x00\x00\x02\x04\xd3", 6);
  message.append("e");
  assert(ParseResponseMessage(message, &msg));
  assert(2 == msg.peers.size());
  assert(msg.peers[0].peer_id.empty());
  assert(msg.peers[0].ip == "10.0.0.1");
  assert(msg.peers[0].port == 1234);
  assert(msg.peers[1].peer_id.empty());
  assert(msg.peers[1].ip == "10.0.0.2");
  assert(msg.peers[1].port == 1235);
}

int main(int argc, char* argv) {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
}
