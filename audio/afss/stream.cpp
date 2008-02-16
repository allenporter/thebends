// stream.cpp
// Author: Allen Porter <allen@thebends.org>
//
// Audio File Stream Services API is available only in Mac OS X v10.5 and later.
// TODO(aporter): Make this work

#include <iostream>
#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

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
                                     &PacketsProc,
                                     kAudioFileWAVEType,
                                     &stream);
  if (ret != 0) {
  }
  cout << "AudioFileStreamOpen: " << ret << endl;
  for (int i = 0; i < 10; i++) {
    int size = 100;
    char data[100];
    bzero(data, 100);
    ret = AudioFileStreamParseBytes(stream, size, data, 0);
    cout << "AudioFileStreamParseBytes: " << ret << endl;
  }
  ret = AudioFileStreamClose(stream);
  cout << "AudioFileStreamClose: " << ret << endl;
}
