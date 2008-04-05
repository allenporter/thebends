// service.cpp
// Author: Allen Porter <allen@thebends.org>

#include <map>
#include <string>
#include <err.h>
#include <dns_sd.h>
#include "service.h"

using namespace std;

namespace btunnel {

Service::Service(const string& name,
                 const string& type,
                 const string& domain,
                 const string& host,
                 uint16_t port,
                 const string& txt)
    : name_(name), type_(type), domain_(domain),
      host_(host), port_(port), txt_(txt) {
  const char* txt_data = txt.data();
  uint16_t txt_len = txt.size();
  int i = 0;
  while (true) {
    char key[BUFSIZ];
    char *value;
    uint8_t value_len;
    DNSServiceErrorType result =
      TXTRecordGetItemAtIndex(txt_len, txt_data, i, BUFSIZ, key,
                              &value_len, (const void**)&value);
    if (result == kDNSServiceErr_Invalid) {
      break;
    }
    if (result != kDNSServiceErr_NoError) {
      warn("TXTRecordGetItemAtIndex failed: %d", result);
      return;
    }
    string val = string(value, value_len);
    txt_records_[key] = val;
    i++;
  }
}

Service::Service(const string& name,
                 const string& type,
                 const string& domain,
                 const string& host,
                 uint16_t port,
                 const map<string, string>& txt_records)
    : name_(name), type_(type), domain_(domain),
      host_(host), port_(port), txt_records_(txt_records) {
  TXTRecordRef record;
  TXTRecordCreate(&record, 0, NULL);
  for (map<string, string>::const_iterator it = txt_records_.begin();
       it != txt_records_.end(); ++it) {
    const string& key = it->first;
    const string& value = it->second;
    DNSServiceErrorType result = TXTRecordSetValue(&record, key.c_str(),
                                                   value.size(), value.data());
    if (result != kDNSServiceErr_NoError) {
      warn("TXTRecordSetValue failed: %d", result);
      break;
    }
  }
  txt_ = string((const char*)TXTRecordGetBytesPtr(&record),
                TXTRecordGetLength(&record));
  TXTRecordDeallocate(&record);
}

Service::~Service() { }

const string& Service::name() const { return name_; }

const string& Service::type() const { return type_; }

const string& Service::domain() const { return domain_; }

const string& Service::host() const { return host_; }

uint16_t Service::port() const { return port_; }

const std::string& Service::txt() const { return txt_; }

const map<string, string>& Service::txt_records() const {
  return txt_records_;
}

}  // namespace btunnel
