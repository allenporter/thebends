#include <iostream>
#include <string>
#include <unistd.h>
#include "assert.h"

#include "ydb_map.h"

using namespace std;

void test_basic() {
  Ydb_Map *map = Ydb_Map::Open("/dev/null");
  assert(map != NULL);

  string val;
  assert(!map->Get("a", &val));
  map->Put("a", "1");

  string key = "";
  key.append("a");
  assert(map->Get(key, &val));

  string one;
  one.append("1");
  assert(val == one);

  key.append("b");
  assert(!map->Get(key, &val));

}

int main(int argc, char *argv[]) {
  test_basic();

  cout << endl << "OK" << endl;
}
