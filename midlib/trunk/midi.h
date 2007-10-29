#ifndef __MIDI_H__
#define __MIDI_H__

namespace midi {

class Buffer;

class Midi {
 public:
  Midi();
  virtual ~Midi();

  bool ParseFrom(Buffer* buffer);
};

}

#endif
