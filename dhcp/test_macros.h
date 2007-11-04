// test_macros.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __TEST_MACROS_H__
#define __TEST_MACROS_H__

#include <err.h>
#include <sysexits.h>

#define ASSERT_TRUE(x) \
  if (!x) { \
    errx(EX_DATAERR, #x " != true"); \
  }

#define ASSERT_FALSE(x) \
  if (x) { \
    errx(EX_DATAERR, #x " != false"); \
  }

#define ASSERT_EQ(x, y) \
  if (x != y) { \
    errx(EX_DATAERR, #x " != " #y " (%u != %u)", x, y); \
  }

#define ASSERT_NE(x, y) \
  if (x == y) { \
    errx(EX_DATAERR, #x " == " #y " (%u == %u)", x, y); \
  }


#define ASSERT_ARRAY_EQ(a1, a2, size) \
  { int i; \
    for (i = 0; i < size; ++i) { \
      if (a1[i] != a2[i]) { \
        errx(EX_DATAERR, #a1 "[%d] != " #a2 "[%d] (0x%02x != 0x%02x)", i, i, \
             a1[i], a2[i]); \
       } \
    } }

#endif  // __TEST_MACROS_H__
