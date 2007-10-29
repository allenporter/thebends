// options.c
// Author: Allen Porter <allen@thebends.org>

#include "options.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

void dump(const uint8_t* buf, uint8_t len) {
  int i;
  for (i = 0; i < len; ++i) {
    printf(" 0x%02x", buf[i]);
  }
}

void dump_text(const char* buf, uint8_t len) {
  int i;
  for (i = 0; i < len; ++i) {
    printf("%c", buf[i]);
  }
}

int parse_options(const uint8_t* buf, int buf_len,
                  struct options* opts) {
  int pos = 0;
  int end = 0;
  while (pos < buf_len) {
    uint8_t code = buf[pos++];
    if (code == DHCP_END) {
        // skip to the end!
        pos = buf_len;
        end = 1;
        break;
    } else if (code == DHCP_PAD || pos + 1 >= buf_len) {
      // Since most packets end with 0xff we probably wont worry about parsing
      // all the padding at the end of the message
      break;
    }
    uint8_t len = buf[pos++];
    switch (code) {
      case DHCP_SUBNET_MASK:
        if (len != 4) {
          warnx("Unexpected DHCP Subnet mask length: %d", len);
          return -1;
        }
        memcpy(&opts->subnet, &buf[pos], 4);
        opts->subnet = ntohl(opts->subnet);
        break;
      case DHCP_ROUTER: {
        if (len >= 4 && (len % 4) != 0) {
          warnx("Unexpected DHCP Router length: %d", len);
          return -1;
        }
        if (len > OPTION_FIELD_MAX) {
          warnx("Unsupported DHCP Router size: %d", len);
          return -1;
        }
        opts->router_num = len / 4;
        int i;
        for (i = 0; i < opts->router_num; i++) {
          memcpy(&opts->router[i], &buf[pos + i * 4], 4);
          opts->router[i] = ntohl(opts->router[i]);
        }
        break;
      }
      case DHCP_DNS: {
        if (len >= 4 && (len % 4) != 0) {
          warnx("Unexpected DHCP DNS Server length: %d", len);
          return -1;
        }
        if (len > OPTION_FIELD_MAX) {
          warnx("Unsupported DHCP DNS Server size: %d", len);
          return -1;
        }
        opts->dns_num = len / 4;
        int i;
        for (i = 0; i < opts->dns_num; i++) {
          memcpy(&opts->dns[i], &buf[pos + i * 4], 4);
          opts->dns[i] = ntohl(opts->dns[i]);
        }
        break;
      }
      case DHCP_DOMAIN:
        if (len < 1) {
          warnx("Unexpected DHCP domain length: %d", len);
          return -1;
        } else if (len > OPTION_FIELD_MAX) {
          warnx("Unsupported DHCP domain length: %d\n", len);
          return -1;
        }
        strncpy(opts->domain, (const char*)(buf + pos), len);
        break;
      case DHCP_MESSAGE_TYPE:
        if (len != 1) {
          warnx("Unexpected DHCP message length: %d", len);
          return -1;
        }
        opts->message_type = buf[pos];
        break;
      case DHCP_MAX_SIZE:
        if (len != 2) {
          warnx("Unexpected Maximum DHCP message size length: %d", len);
          return -1;
        }
        memcpy(&opts->max_size, &buf[pos], 2);
        opts->max_size = ntohs(opts->max_size);
        break; 
      case DHCP_HOST_NAME:
        if (len < 1) {
          warnx("Unexpected DHCP host name length: %d", len);
          return -1;
        } else if (len > OPTION_FIELD_MAX) {
          warnx("Unsupported DHCP host name length: %d\n", len);
          return -1;
        }
        strncpy(opts->hostname, (const char*)(buf + pos), len);
        break;
      case DHCP_REQUESTED_IP:
         if (len != 4) {
          warnx("Unexpected DHCP Requested IP address length: %d", len);
          return -1;
        }
        memcpy(&opts->requested_ip, &buf[pos], 4);
        opts->requested_ip = ntohl(opts->requested_ip);
        break;
      case DHCP_LEASE_TIME:
        if (len != 4) {
          warnx("Unexpected lease time size: %d", len);
          return -1;
        }
        memcpy(&opts->lease_time, &buf[pos], 4);
        opts->lease_time = ntohl(opts->lease_time);
        break;
      case DHCP_OVERLOAD:
        if (len != 1) {
          warnx("Unexpected lease time size: %d", len);
          return -1;
        }
        warnx("Ignoring unsupported OVERLOAD option");
        break;
      case DHCP_SERVER_ID:
         if (len != 4) {
          warnx("Unexpected DHCP Server ID length: %d", len);
          return -1;
        }
        memcpy(&opts->server_id, &buf[pos], 4);
        opts->server_id = ntohl(opts->server_id);
        break;
     case DHCP_CLIENT_ID:
        if (len < 2) {
          warnx("Unexpected DHCP Client ID length: %d", len);
          return -1;
        } else if (len > OPTION_FIELD_MAX) {
          warnx("Unsupported DHCP Client ID length: %d\n", len);
          return -1;
        }
        opts->client_id_type = buf[pos];
        opts->client_id_length = len - 1;
        memcpy(&opts->client_id, &buf[pos+1], len);
        break;
      case DHCP_PARAMETER_LIST:
        if (len < 1) {
          warnx("Unexpected DHCP Parameter List length: %d", len);
          return -1;
        } else if (len > OPTION_FIELD_MAX) {
          warnx("Unsupported DHCP Parameter List length: %d\n", len);
          return -1;
        }
        memcpy(opts->param_list, &buf[pos], len);
        opts->param_size = len;
        break;
      default:
        warnx("Unexpected DHCP option code %d, len %d", code, len);
        break;
    }
    pos += len;
  } 
  if (end != 1)  {
    warnx("Unexpected end of DHCP message");
    return -1;
  }
  if (pos != buf_len) {
    warnx("Unexpected end of message (%d != %d)", pos, buf_len);
    return -1;
  }
  return 0;
}

void write_options(struct options* opts, uint8_t* buf, uint16_t* size) {
  uint16_t max_size = *size;
  uint16_t pos = 0;
  if (opts->message_type > 0) {
    assert(pos + 3 < max_size);
    buf[pos++] = DHCP_MESSAGE_TYPE;
    buf[pos++] = 1;
    buf[pos++] = opts->message_type;
  }
  if (opts->max_size > 0) {
    assert(pos + 4 < max_size);
    buf[pos++] = DHCP_MAX_SIZE;
    buf[pos++] = 2;
    uint16_t max_size = ntohs(opts->max_size);
    memcpy(&buf[pos], &max_size, 2);
    pos += 2;
  }
  int hostname_len = strlen(opts->hostname);
  if (hostname_len > 0) {
    assert(pos + hostname_len + 2 < max_size);
    buf[pos++] = DHCP_HOST_NAME;
    buf[pos++] = hostname_len;
    memcpy(&buf[pos], opts->hostname, hostname_len);
    pos += hostname_len;
  }
  if (opts->requested_ip > 0) {
    assert(pos + 6 < max_size);
    buf[pos++] = DHCP_REQUESTED_IP;
    buf[pos++] = 4;
    uint32_t ip = ntohl(opts->requested_ip);
    memcpy(&buf[pos], &ip, 4);
    pos += 4;
  }
  if (opts->lease_time > 0) {
    assert(pos + 6 < max_size);
    buf[pos++] = DHCP_LEASE_TIME;
    buf[pos++] = 4;
    uint32_t lease = ntohl(opts->lease_time);
    memcpy(&buf[pos], &lease, 4);
    pos += 4;
  }
  if (opts->server_id > 0) {
    assert(pos + 6 < max_size);
    buf[pos++] = DHCP_SERVER_ID;
    buf[pos++] = 4;
    uint32_t ip = ntohl(opts->server_id);
    memcpy(&buf[pos], &ip, 4);
    pos += 4;
  }
  if (opts->subnet > 0) {
    assert(pos + 6 < max_size);
    buf[pos++] = DHCP_SUBNET_MASK;
    buf[pos++] = 4;
    uint32_t subnet = ntohl(opts->subnet);
    memcpy(&buf[pos], &subnet, 4);
    pos += 4;
  }
  if (opts->router_num > 0) {
    assert(pos + 2 + opts->router_num * 4 < max_size);
    buf[pos++] = DHCP_ROUTER;
    buf[pos++] = opts->router_num * 4;
    int i;
    for (i = 0; i < opts->router_num; ++i) {
      uint32_t ip = ntohl(opts->router[i]);
      memcpy(&buf[pos], &ip, 4);
      pos += 4;
    }
  }
  if (opts->dns_num > 0) {
    assert(pos + 2 + opts->dns_num * 4 < max_size);
    buf[pos++] = DHCP_DNS;
    buf[pos++] = opts->dns_num * 4;
    int i;
    for (i = 0; i < opts->dns_num; ++i) {
      uint32_t ip = ntohl(opts->dns[i]);
      memcpy(&buf[pos], &ip, 4);
      pos += 4;
    }
  }
  int domain_len = strlen(opts->domain);
  if (domain_len > 0) {
    assert(pos + domain_len + 2 < max_size);
    buf[pos++] = DHCP_DOMAIN;
    buf[pos++] = domain_len;
    memcpy(&buf[pos], opts->domain, domain_len);
    pos += domain_len;
  }
  if (opts->client_id_length > 0) {
    assert(pos + opts->client_id_length + 3 < max_size);
    buf[pos++] = DHCP_CLIENT_ID;
    buf[pos++] = opts->client_id_length + 1;
    buf[pos++] = opts->client_id_type;
    memcpy(&buf[pos], opts->client_id, opts->client_id_length);
    pos += opts->client_id_length;
  }
  if (opts->param_size > 0) {
    assert(pos + opts->param_size + 2 < max_size);
    buf[pos++] = DHCP_PARAMETER_LIST;
    buf[pos++] = opts->param_size;
    memcpy(&buf[pos], opts->param_list, opts->param_size);
    pos += opts->param_size;
  }
  assert(pos + 1 < max_size);
  buf[pos++] = DHCP_END;
  *size = pos;
}
