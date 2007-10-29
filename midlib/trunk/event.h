#ifndef __EVENT_H__
#define __EVENT_H__

namespace midi {

class Buffer;

class MidiEvent {
 public:
  MidiEvent();
  virtual ~MidiEvent();

  bool ParseFrom(Buffer* buffer);
};

class MetaEvent {
 public:
  MetaEvent();
  virtual ~MetaEvent();

  bool ParseFrom(Buffer* buffer);
};

}  // namespace midi

#endif
