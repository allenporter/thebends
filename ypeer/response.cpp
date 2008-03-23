// response.cpp
// Author: Allen Porter <allen@thebends.org>

#include "response.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "bencode.h"

using namespace std;

namespace ypeer {

bool ParseDictPeer(be_dict* dict, Peer* peer) {
  for (int i = 0; dict[i].val; ++i) {
    be_node* n = dict[i].val;
    be_str* str = dict[i].key;
    if (str == NULL) {
      return false;
    }
    const string& key = str->s;
    if (key == "peer id") {
      if (n->type != BE_STR || n->val.s == NULL) {
        cerr << "Invalid format for peer id" << endl;
        return false;
      }
      peer->peer_id = n->val.s->s;
    } else if (key == "ip") {
      if (n->type != BE_STR || n->val.s == NULL) {
        cerr << "Invalid format for ip" << endl;
        return false;
      }
      peer->ip = n->val.s->s;
    } else if (key == "port") {
      if (n->type != BE_INT) {
        cerr << "Invalid format for port" << endl;
        return false;
      }
      peer->port = n->val.i;
    }
  }
  return true;
}

bool ParseListPeers(be_node* node, vector<Peer>* peers) {
  for (int i = 0; node->val.l[i]; ++i) {
    be_node* n = node->val.l[i];
    if (n->type != BE_DICT) {
      cerr << "peers entry was not a dict" << endl;
      peers->clear();
      return false;
    }
    Peer peer;
    if (!ParseDictPeer(n->val.d, &peer)) {
      cerr << "couldn't parse peer dict" << endl;
      peers->clear();
      return false;
    }
    peers->push_back(peer);
  }
  return true;
}

bool ParseBinaryPeers(be_str* str,
                      vector<Peer>* peers) {
  const string& data = string(str->s, str->len);
  if (data.size() % 6 != 0) {
    cerr << "peer data invalid size (" << data.size() << ")" << endl;
    return false;
  }
  int count = data.size() / 6;
  for (int i = 0; i < count; ++i) {
    const string& ip = data.substr(i * 6, 4);
    const string& port = data.substr(i * 6 + 4, 2);
    in_addr addr;
    memcpy(&addr, ip.data(), 4);
    Peer peer;
    peer.ip = inet_ntoa(addr);
    peer.port = ntohs((port[1] << 8) + port[0]);
    peers->push_back(peer);
  }
  return true;
}

bool ParseResponseMessage(const string& message,
                          ResponseMessage* response) {
  be_node* node = be_decode(message.data());
  if (node == NULL) {
    cerr << "Can't decode message" << endl;
    return false;
  }
  if (node->type != BE_DICT) {
    cerr << "Message was wrong type" << endl;
    be_free(node);
    return false;
  }
  be_dict* dict = node->val.d;
  assert(dict != NULL);
  for (int i = 0; dict[i].val; ++i) {
    be_node* n = dict[i].val;
    const string& key = dict[i].key->s;
    if (key == "failure reason") {
      if (n->type != BE_STR || n->val.s == NULL) {
        cerr << "Invalid format for failure reason" << endl;
        be_free(node);
        return false;
      }
      response->failure_reason = n->val.s->s;
      be_free(node);
      return true;
    } else if (key == "warning message") {
      if (n->type != BE_STR || n->val.s == NULL) {
        cerr << "Invalid format for warning message" << endl;
        be_free(node);
        return false;
      }
      response->warning_message = n->val.s->s;
    } else if (key == "interval") {
      if (n->type != BE_INT) {
        cerr << "Invalid format for interval" << endl;
        be_free(node);
        return false;
      }
      response->interval = n->val.i;
    } else if (key == "min interval") {
      if (n->type != BE_INT) {
        cerr << "Invalid format for min interval" << endl;
        be_free(node);
        return false;
      }
      response->min_interval = n->val.i;
    } else if (key == "tracker id") {
      if (n->type != BE_STR || n->val.s == NULL) {
        cerr << "Invalid format for tracker id" << endl;
        be_free(node);
        return false;
      }
      response->tracker_id = n->val.s->s;
    } else if (key == "complete") {
      if (n->type != BE_INT) {
        cerr << "Invalid format for interval" << endl;
        be_free(node);
        return false;
      }
      response->complete = n->val.i;
    } else if (key == "incomplete") {
      if (n->type != BE_INT) {
        cerr << "Invalid format for interval" << endl;
        be_free(node);
        return false;
      }
      response->incomplete = n->val.i;
    } else if (key == "peers") {
      if (n->type == BE_STR || n->val.s == NULL) {
        if (!ParseBinaryPeers(n->val.s, &response->peers)) {
          cerr << "Can't parse binary peers list" << endl;
          be_free(node);
          return false;
        }
      } else if (n->type == BE_LIST) {
        if (!ParseListPeers(n, &response->peers)) {
          cerr << "Can't parse peers list" << endl;
          be_free(node);
          return false;
        }
      } else {
        cerr << "Invalid type for peers" << endl;
        be_free(node);
        return false;
      }
    }
  }
  be_free(node);
  return true;
}

}  // namespace ypeer
