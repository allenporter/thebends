// util.h
// Author: Allen Porter <allen@thebends.org>
#include "util.h"
#include <string.h>

void ntohs(uint16_t* value) {
  uint16_t tmp_a = (*value & 0xff00) >> 8;
  uint16_t tmp_b = (*value & 0x00ff) << 8;
  *value = tmp_b | tmp_a;
}

void ntohl(uint32_t* value) {
  uint32_t tmp_a = (*value & 0xff000000) >> 24;
  uint32_t tmp_b = (*value & 0x00ff0000) >> 8;
  uint32_t tmp_c = (*value & 0x0000ff00) << 8 ;
  uint32_t tmp_d = (*value & 0x000000ff) << 24;
  *value = tmp_d | tmp_c |tmp_b | tmp_a;
}

static uint8_t power(uint8_t x, uint8_t y) {
  uint8_t sum = 1;
  uint8_t i;
  for (i = 0; i < y; i++) {
    sum *= x;
  }
  return sum;
}

int inet_aton(const char* cp, uint32_t* ip) {
  uint8_t len = strlen(cp);
  if (len > 16) {
    return -1;
  }
  char buf[17];
  memcpy(buf, cp, len);
  buf[len] = '\0';

  uint32_t ip_buf = 0;
  uint32_t shift = 24;
  char *left = buf;
  int i;
  for (i = 0; i < 4; i++) {
    char *value = strsep(&left, ".");
    if (value == NULL || *value == '\0') {
      return -1;
    }
    int value_len = strlen(value);
    if (value_len > 3) {
      return -1;
    }
    uint32_t octet_value = 0;
    int j;
    for (j = 0; j < value_len; j++) {
      char c = value[j];
      if (c < '0' || c > '9') {
        return -1;
      }
      c = c - '0';
      uint8_t p = value_len - j - 1;
      octet_value += power(10, p) * c;
    } 
    ip_buf |= (octet_value << shift);
    if (i < 3) { 
      if (left == NULL) {
        return -1;
      }
      shift -= 8;
    }
  }
  *ip = ip_buf;
  return 0;
}
