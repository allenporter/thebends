#include "chunk.h"

#include <iostream>
#include <string.h>
#include "buffer.h"
#include "event.h"

using namespace std;

namespace midi {

ChunkHeader::ChunkHeader() {
  memset(id_, 0, 4);
  length_ = 0;
}

ChunkHeader::~ChunkHeader() { }

bool ChunkHeader::ParseFrom(Buffer* buffer) {
  if (!buffer->Read(id_, 4) ||
      !buffer->Read(&length_)) {
    return false;
  }
  return true;
}

bool ChunkHeader::HasID(const char* id) const {
  return (strncmp(id_, id, 4) == 0);
}

MThdChunk::MThdChunk() { }
MThdChunk::~MThdChunk() { }

bool MThdChunk::HasHeader(const ChunkHeader& header) {
  return (header.HasID("MThd") && (header.Length() == 6));
}

bool MThdChunk::ParseFrom(Buffer* buffer) {
  return (buffer->Read(&format_) &&
          buffer->Read(&num_tracks_) &&
          buffer->Read(&division_));
}

MTrkChunk::MTrkChunk() { }
MTrkChunk::~MTrkChunk() { }

bool MTrkChunk::HasHeader(const ChunkHeader& header) { 
  return (header.HasID("MTrk") && (header.Length() > 0));
}

bool MTrkChunk::ParseFrom(Buffer* buffer, const ChunkHeader& header) {
  if (header.Length() <= 0) {
    return false;
  }
  unsigned char data[header.Length()];
  if (!buffer->Read((char*)data, header.Length())) {
    return false;
  }
 
  Buffer event_buffer((char*)data, header.Length());
  while (!event_buffer.Eof()) {
    cerr << "Chunk progress: " << dec << event_buffer.Position() << "/" << header.Length() << endl;

    int p = event_buffer.Position();
    cerr << hex << "[" << (int)data[p+0] << "," << (int)data[p+1] << "," << (int)data[p+2] << "," << (int)data[p+3] << "," << (int)data[p+4] << "," << (int)data[p+5] << "," << (int)data[p+6] << "]" << endl;

    unsigned int delta;
    if (!event_buffer.ReadVar(&delta)) {
      cerr << "Unable to read delta" << endl;
      return false;
    }
    cerr << "Delta: " << delta << endl;

    unsigned char c = (char)event_buffer.Peek();
    switch (c) {
      case 0xff: {
        if (!event_buffer.Seek(event_buffer.Position() + 1)) {
          cerr << "Unexepected seek error" << endl;
          return false;
        }
        MetaEvent event;
        if (!event.ParseFrom(&event_buffer)) {
          cerr << "Unable to process meta event" << endl;
          return false;
        }
        break;
      }
      case 0xf0: {
        cerr << "Sysex event found" << endl;
        return false;
      }
      default: {
        if (c & 0x80 != 0x80) {
          cerr << "Bit 7 not set in midi event!" << endl;
          return false;
        }
          MidiEvent event;
          if (!event.ParseFrom(&event_buffer)) {
            cerr << "Unable to process event" << endl;
            return false;
          }
        break;
      }
    }
  }
  return true;
}

}  // namespace midi
