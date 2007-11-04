// messages_test.c
// Author: Allen Porter <allen@thebends.org>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "messages.h"
#include "options.h"
#include "test_macros.h"

// Tests creating a DHCPDISCOVER message
void test1() {
  u_char source_mac[] = { 0x00, 0x0c, 0x15, 0xfe, 0x78, 0xd5 };
  const char* hostname = "lozenge";

  uint8_t buf[1024];
  uint16_t size = 1024;
  build_dhcp_discover(source_mac, hostname, buf, &size);
  ASSERT_TRUE(size > sizeof(struct dhcp_message));

  struct dhcp_message* msg = (struct dhcp_message*)buf;
  ASSERT_EQ(BOOTREQUEST, msg->op);
  ASSERT_EQ(1, msg->htype);
  ASSERT_EQ(6, msg->hlen);
  ASSERT_EQ(0, msg->hops);
  ASSERT_NE(0, msg->xid);
  ASSERT_EQ(0, msg->secs);
  ASSERT_EQ(0, msg->flags);
  ASSERT_EQ(0, msg->ciaddr);
  ASSERT_EQ(0, msg->yiaddr);
  ASSERT_EQ(0, msg->siaddr);
  ASSERT_EQ(0, msg->giaddr);

  ASSERT_ARRAY_EQ(source_mac, msg->chaddr, 6);

  // sname and file are empty
  int i;
  for (i = 0; i < 16; ++i) {
    ASSERT_EQ(0, msg->sname[i]);
  }
  for (i = 0; i < 128; ++i) {
    ASSERT_EQ(0, msg->file[i]);
  }

  ASSERT_EQ(99, msg->options_signature[0]);
  ASSERT_EQ(130, msg->options_signature[1]);
  ASSERT_EQ(83, msg->options_signature[2]);
  ASSERT_EQ(99, msg->options_signature[3]);

  // Verify correct options (much duplicated from options_test)

  int options_size = size - sizeof(struct dhcp_message);
  struct options opts;
  memset(&opts, 0, sizeof(struct options));
 
  int ret = parse_options(buf + sizeof(struct dhcp_message), options_size,
                          &opts);
  ASSERT_EQ(0, ret);
  ASSERT_EQ(MESSAGE_DHCPDISCOVER, opts.message_type);
  ASSERT_EQ(1500, opts.max_size);
  ASSERT_EQ(0, strcmp(hostname, opts.hostname));
  ASSERT_EQ(7776000, opts.lease_time);
  ASSERT_EQ(1, opts.client_id_type);
  ASSERT_EQ(6, opts.client_id_length);
  ASSERT_EQ(0, memcmp(source_mac, opts.client_id, 6));
  uint8_t params[] = { DHCP_SUBNET_MASK, DHCP_ROUTER, DHCP_DNS, DHCP_DOMAIN };
  ASSERT_EQ(4, opts.param_size);
  ASSERT_EQ(0, memcmp(params, opts.param_list, 4));

  // Options not in the outgoing message
  ASSERT_EQ(0, opts.server_id);
  ASSERT_EQ(0, opts.subnet);
  ASSERT_EQ(0, opts.router_num);
  ASSERT_EQ(0, opts.dns_num);
  ASSERT_EQ(0, strlen(opts.domain));
}

int main() {
  test1();
  printf("PASS\n");
  return 0;
}
