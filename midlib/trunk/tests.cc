void tests() {
  {
    char* a = "\x00";
    midi::Buffer buffer(a, 1);
    unsigned int i;
    assert(buffer.ReadVar(&i));
    assert(i == 0);
  }

  {
    char* a = "\x40";
    midi::Buffer buffer(a, strlen(a));
    unsigned int i;
    assert(buffer.ReadVar(&i));
    assert(i == 0x40);
  }

  {
    char* a = "\x81\x00";
    midi::Buffer buffer(a, 2);
    unsigned int i;
    assert(buffer.ReadVar(&i));
    assert(i == 0x80);
  }

  {
    char* a = "\xc0\x00";
    midi::Buffer buffer(a, 2);
    unsigned int i;
    assert(buffer.ReadVar(&i));
    assert(i == 0x2000);
  }

  {
    char* a = "\xff\xff\xff\x7f";
    midi::Buffer buffer(a, 4);
    unsigned int i;
    assert(buffer.ReadVar(&i));
    assert(i == 0x0fffffff);
  }
}

