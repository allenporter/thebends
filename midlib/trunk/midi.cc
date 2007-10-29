#include "midi.h"

#include <string>
#include <iostream>
#include "buffer.h"
#include "chunk.h"

using namespace std;

namespace midi {

Midi::Midi() { }

Midi::~Midi() { }

bool Midi::ParseFrom(Buffer* buffer) {
  ChunkHeader header;
  int chunk = 0;
  while (buffer->BytesLeft() > 0) {
    cerr << "Buffer at offset: " << buffer->Position() << endl;
    if (!header.ParseFrom(buffer)) {
      cerr << "Unable to parse chunk header" << endl;
      return false;
    }
    chunk++;
    cerr << "Chunk #" << chunk << ": ";

    if (MThdChunk::HasHeader(header)) {
      cout << "MThd" << endl;
      MThdChunk mthd;
      if (!mthd.ParseFrom(buffer)) {
        cerr << "Unable to parse MThd chunk" << endl;
        return false;
      }

      cerr << "Format: " << mthd.Format() << endl;
      cerr << "NumTracks: " << mthd.NumTracks() << endl;
      cerr << "Division: " << mthd.Division() << endl;
    } else if (MTrkChunk::HasHeader(header)) {
      cout << "MTrk" << endl;
      MTrkChunk mtrk;
      if (!mtrk.ParseFrom(buffer, header)) {
        cerr << "Unable to parse MTrk chunk" << endl;
        return false;
      }

      cerr << "Track has " << header.Length() << " bytes";

    } else {
      cout << "Unknown" << endl;
      return false;
    }
    cerr << endl;
  }
  return true;
}

}  // namespace midi
