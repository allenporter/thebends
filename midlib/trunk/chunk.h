#ifndef __CHUNK_H__
#define __CHUNK_H__

namespace midi {

class Buffer;

class ChunkHeader {
 public:
  ChunkHeader();
  virtual ~ChunkHeader();

  // Returns true if the chunk is parseable.
  bool ParseFrom(Buffer* buffer);

  // Returns true if the chunk has the specified id.  id must be at least 4
  // bytes long, but does not need to be NULL terminated.
  bool HasID(const char* id) const;

  // Returns length of this chunk
  unsigned long Length() const { return length_; }

 protected:
  char id_[4];
  unsigned long length_; 
};

class MThdChunk {
 public:
  MThdChunk();
  virtual ~MThdChunk();

  // Returns true if the chunk header matches a MThd Chunk
  static bool HasHeader(const ChunkHeader& header);

  // Returns true if the MThd Chunk was parseable from Buffer
  bool ParseFrom(Buffer* buffer);

  unsigned short Format() const { return format_; }
  unsigned short NumTracks() const { return num_tracks_; }
  unsigned short Division() const { return division_; }

 protected:
  unsigned short format_;
  unsigned short num_tracks_;
  unsigned short division_;
};

class MTrkChunk {
 public:
  MTrkChunk();
  virtual ~MTrkChunk();

  static bool HasHeader(const ChunkHeader& header);

  bool ParseFrom(Buffer* buffer, const ChunkHeader& header);

 protected:


};


};

#endif
