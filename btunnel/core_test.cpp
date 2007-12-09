// core_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <ythread/callback-inl.h>
#include <arpa/inet.h>
#include <err.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ynet/select.h>
#include <ynet/util.h>
#include <iostream>
#include "peer_message.h"
#include "service.h"
#include "peer.h"
#include "core.h"

using namespace std;

static int kBasePort = 15000;

void create_pair(int* client_sock, int* server_sock) {
  int pair[2];
  int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, pair);
  if (ret == -1) {
    err(1, "socketpair");
  }
  assert(ret == 0);
  *client_sock = pair[0];
  *server_sock = pair[1];
  ynet::SetNonBlocking(*client_sock);
  ynet::SetNonBlocking(*server_sock);
}

static void test1() {
  int client_sock;
  int server_sock;
  create_pair(&client_sock, &server_sock);

  ynet::Select select;
  vector<btunnel::Service*> empty;
  btunnel::Core core(&select, server_sock, &empty, NULL);
  assert(core.GetRemoteServiceCount() == 0);
  assert(core.GetLocalServiceCount() == 0);
  // Sanity check; do nothing and delete
}

static void StopCallback(ynet::Select* select) {
  select->Stop();
}

static string Name(int i) {
  string name = "service";
  char c = '0' + i;
  name.append(&c, 1);
  return name;
}

class RegisterStub : public btunnel::Peer {
 public:
  RegisterStub() : count_(0) { }
  virtual ~RegisterStub() { }
 
  virtual bool Register(int sock, const btunnel::RegisterRequest* request) {
    count_++;
    request_.service_id = request->service_id;
    request_.name = request->name;
    request_.type = request->type;
    request_.txt_records = request->txt_records;
    return true;
  }

  void Clear() {
    request_.service_id = 0;
    request_.name.clear();
    request_.type.clear();
    request_.txt_records.clear();
  }

  int count_;
  btunnel::RegisterRequest request_;
};

static void AllocateServices(int count, vector<btunnel::Service*>* services) {
  for (int i = 0; i < count; i++) {
    btunnel::Service* service =
      new btunnel::Service(Name(i), "some type", "some domain", "127.0.0.1",
                           kBasePort + i, "some record");
    services->push_back(service);
  }
}

static void FreeServices(vector<btunnel::Service*>* services) {
  int count = services->size();
  for (int i = 0; i < count; i++) {
    delete (*services)[i];
  }
  services->clear();
}

static void CheckRegisteredServices(ynet::Select* select, int client_sock,
                                    int count) {
  ynet::BufferedReader reader(
    select, client_sock,
    ythread::NewCallback(&StopCallback, select));
  select->Start();

  RegisterStub stub;
  btunnel::MessageReader msg_reader(&stub);
  assert(stub.count_ == 0);
  for (int i = 0; i < count; i++) {
    int nbytes = msg_reader.Read(client_sock, &reader);
    string name = "service";
    char c = '0' + i;
    name.append(&c, 1);
    assert(nbytes > 0);
    assert(stub.count_ == i + 1);
    assert(stub.request_.service_id > 0);
    assert(stub.request_.name == Name(i));
  }
  // Empty
  char c;
  assert(!reader.Peek(&c, 1));
}

static void test2() {
  int client_sock;
  int server_sock;
  create_pair(&client_sock, &server_sock);

  ynet::Select select;
  vector<btunnel::Service*> services;
  AllocateServices(5, &services);

  // Test exporting of registration messages on startup
  btunnel::Core *core = new btunnel::Core(&select, server_sock, &services,
                                          NULL);
  assert(core->GetRemoteServiceCount() == 0);
  assert(core->GetLocalServiceCount() == 5);

  CheckRegisteredServices(&select, client_sock, 5);

  delete core;
  FreeServices(&services);
}

int main(int argc, char* argv[]) {
  srandom(time(NULL));
  test1();
  test2();
  cout << "PASS" << endl;
}
