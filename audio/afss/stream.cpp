// stream.cpp
// Author: Allen Porter <allen@thebends.org>
//
// Audio File Stream Services API is available only in Mac OS X v10.5 and later.
// TODO(aporter): Make this work

#include <iostream>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/AudioFileStream.h>

using namespace std;

void PropertyListener(void *inClientData,
                      AudioFileStreamID inAudioFileStream,
                      AudioFileStreamPropertyID inPropertyID,
                      UInt32 *ioFlags) {
  cout << "PropertyListener" << endl;
}

void PacketsProc(void *inClientData,
                 UInt32 inNumberBytes,
                 UInt32 inNumberPackets,
                 const void *inInputData,
                 AudioStreamPacketDescription *inPacketDescriptions) {

  cout << "PacketsProc" << endl;
}

int main(int argc, char* argv[]) {
  AudioFileStreamID stream;
  OSStatus ret = AudioFileStreamOpen(NULL,
                                     &PropertyListener,
                                     &Packets,
                                     AudioFileStreamClose,
                                     &stream);
  cout << "AudioFileStreamOpen: " << ret << endl;
  for (int i = 0; i < 10; i++) {
    ret = AudioFileStreamParseBytes(stream, inDataByteSize, data, 0);
    cout << "AudioFileStreamParseBytes: " << ret << endl;
  }
  ret = AudioFileStreamClose(stream);
  cout << "AudioFileStreamClose: " << ret << endl;
}
