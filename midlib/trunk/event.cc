#include "event.h"

#include <iostream>
#include <string.h>
#include "buffer.h"

using namespace std;

namespace midi {

MidiEvent::MidiEvent() { }

MidiEvent::~MidiEvent() { }

const char* notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A",
                        "A#", "B" };

static void print_note(unsigned char note) {
  int octave = -1 + (note / 12);
  int letter = note % 12;
  cerr << "NOTE: " << notes[letter] << ", octave=" << dec << octave << endl; 
}

bool MidiEvent::ParseFrom(Buffer* buffer) {
  unsigned char status;
  if (!buffer->Read(&status) && (status & 0x80) == 0x80) {
    cerr << "Unable to read midi event status" << endl;
    return false;
  }
  unsigned int high = (status & (0xF0)) >> 4;
  unsigned int low = (status & 0x0F);


  unsigned char c = 0x00;
  while ((c & 0x80) == 0 && !buffer->Eof()) {
    cerr << "High=" << hex << (int)high << ", Low=" << hex << (int)low << ", Status=" << hex << (int)status << endl;

    const unsigned char* data = (const unsigned char*)buffer->data_;
    int p = buffer->Position();
    cerr << "[" << (int)data[p+0] << "," << (int)data[p+1] << "," << (int)data[p+2] << "," << (int)data[p+3] << "," << (int)data[p+4] << "," << (int)data[p+5] << "," << (int)data[p+6] << "]" << endl;

  switch (high) {
    case 0x8:
    case 0x9: {
      unsigned char note;
      if (!buffer->Read(&note) && (note & 0x80) == 0) {
        cerr << "Unable to read midi note" << endl;
        return false;
      }

      unsigned char velocity;
      if (!buffer->Read(&velocity) && (note & 0x80) == 0) {
        cerr << "Unable to read midi velocity" << endl;
        return false;
      }
      cerr << "EVENT: Note " << dec << ((high == 0x8 || velocity == 0) ? "off" : "on") << ", note=" << (int)note << ", velocity=" << (int)velocity << endl;
      print_note(note);
      break;
    }
    case 0xA:
      cerr << "AfterTouch: " << low << endl;
      return false;
      break;
    case 0xB: {
      unsigned char controller;
      if (!buffer->Read(&controller) && (controller & 0x80) == 0) {
        cerr << "Unable to read control/mode change command" << endl;
        return false;
      }
      unsigned char value;
      if (!buffer->Read(&value) && (value && 0x80) == 0) {
        cerr << "Unable to read control/mode change value" << endl;
        return false;
      }
      cerr << "EVENT: Control/Mode change, channel=" << dec << low << ", controller=0x" << hex << (int)controller << ", value=0x" << hex << (int)value << endl;
      break;
    }
    case 0xC: {
      unsigned char number;
      if (!buffer->Read(&number) && (number && 0x80) == 0) {
        cerr << "Unable to read patch change" << endl;
        return false;
      }
      cerr << "EVENT: Program (patch) change, program=" << (int)number << endl;
      break;
    }
    case 0xD:
      cerr << "EVENT: Channel Pressure: " << low << endl;
      return false;
      break;
    case 0xE:
      cerr << "EVENT: Pitch Wheel: " << low << endl;
      return false;
      break;
    default:
      cerr << "High bit unknown: " << low << endl;
      return false;
      break;
  }
  return true;
  c = buffer->Peek();
  cerr << "c=0x" << hex << (int)c << endl;
  }
  return true;
}

MetaEvent::MetaEvent() { }

MetaEvent::~MetaEvent() { }

bool MetaEvent::ParseFrom(Buffer* buffer) {
  unsigned int event_type;
  if (!buffer->ReadVar(&event_type)) {
    cerr << "Unable to read meta even type" << endl;
    return false;
  }
  if (event_type <= 0x07) {
    unsigned int length;
    if (!buffer->ReadVar(&length)) {
      cerr << "Unable to read meta event length" << endl;
      return false;
    }
    char text[length+1];
    if (!buffer->Read(text, length)) {
      cerr << "Unable to read text of length " << length << endl;
      return false;
    }
    text[length] = '\0';
    cerr << "Meta (" << event_type << "): " << text << endl;
  } else if (event_type == 0x20) {
    unsigned int one;
    if (!buffer->ReadVar(&one) || one != 1) {
      cerr << "No one byte found in midi port event" << endl;
      return false;
    }
    unsigned int channel;
    if (!buffer->ReadVar(&channel)) {
      cerr << "Unable to parse midi channel" << endl;
      return false;
    }
    cerr << "Midi Channel: " << channel << endl;
  } else if (event_type == 0x21) {
    unsigned int one;
    if (!buffer->ReadVar(&one) || one != 1) {
      cerr << "No one byte found in midi port event" << endl;
      return false;
    }
    unsigned int port;
    if (!buffer->ReadVar(&port)) {
      cerr << "Unable to parse midi port" << endl;
      return false;
    }
    cerr << "Midi port: " << port << endl; 
  } else if (event_type == 0x2f) {
    unsigned int null_byte;
    if (!buffer->ReadVar(&null_byte) || null_byte != 0) {
      cerr << "No null byte found in end of track event" << endl;
      return false;
    }
    cerr << "End of track found" << endl;
  } else if (event_type == 0x7f) {
    unsigned int length;
    if (!buffer->ReadVar(&length)) {
      cerr << "Unable to read sequencer event length" << endl;
      return false;
    }
    char data[length];
    if (!buffer->Read(data, length)) {
      cerr << "Unable to read sequencer data of length " << length << endl;
      return false;
    }
    cerr << "Sequencer specific meta event" << endl;
  } else if (event_type == 0x51) {
    unsigned int three;
    if (!buffer->ReadVar(&three) || three != 0x03) {
      cerr << "Unable to read signature three" << endl;
      return false;
    }

    unsigned char tempo[3];
    if (!buffer->Read((char*)tempo, 3)) {
      cerr << "Unable to read signature three" << endl;
      return false;
    }

    unsigned int tempo_ms = (tempo[0] << 16) + (tempo[1] << 8) + tempo[2];
    cerr << "Tempo: " << tempo_ms << "(ms)" << endl;
  } else if (event_type == 0x58) {
    unsigned int four;
    if (!buffer->ReadVar(&four) || four != 0x04) {
      cerr << "Unable to read signature four" << endl;
      return false;
    }
    unsigned int nn;
    if (!buffer->ReadVar(&nn)) {
      cerr << "Unable to read signature numerator" << endl;
      return false;
    }

    unsigned int dd;
    if (!buffer->ReadVar(&dd)) {
      cerr << "Unable to read signature denominator" << endl;
      return false;
    }

    unsigned int cc;
    if (!buffer->ReadVar(&cc)) {
      cerr << "Unable to read metronome click" << endl;
      return false;
    }

    unsigned int bb;
    if (!buffer->ReadVar(&bb)) {
      cerr << "Unable to read quarter" << endl;
      return false;
    }
    cerr << "Time Signature: " << nn << "/" << dd << " (" << cc << ", "
         << bb << ")" << endl;
  } else if (event_type == 0x59) {
    unsigned int two;
    if (!buffer->ReadVar(&two) || two != 0x02) {
      cerr << "Unable to read signature two" << endl;
      return false;
    }
    unsigned int sf;
    if (!buffer->ReadVar(&sf)) {
      cerr << "Unable to read key signature sf" << endl;
      return false;
    }
    unsigned int mi;
    if (!buffer->ReadVar(&mi)) {
      cerr << "Unable to read key signature mi" << endl;
      return false;
    }
    cerr << "Key Signature: sf=" << sf << ", mi=" << mi << endl; 
  } else {
    cerr << "Unknown event type 0x" << hex << event_type << endl;
  }
  return true;
}

}  // namespace
