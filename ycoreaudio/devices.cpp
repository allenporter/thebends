#include <CoreAudio/CoreAudio.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

#include "util.h"

using namespace std;

int main(int argc, char* argv[]) {
  vector<AudioDeviceID> devices;
  if (!coreaudio::GetDevices(&devices)) {
    cerr << "coreaudio::GetDevices() failed" << endl;
    return 1;
  }
  for (vector<AudioDeviceID>::const_iterator it = devices.begin();
       it != devices.end(); ++it) {
    struct coreaudio::DeviceInfo info;
    if (!coreaudio::GetDeviceInfo(*it, &info)) {
      cerr << "coreaudio::GetDeviceInfo() failed" << endl;
      return 1;
    }
    cout << "DeviceID: " << *it << endl;
    cout << "     UID: " << info.uid << endl;
    cout << "    Name: " << info.name << endl;
    bool input = coreaudio::HasInputStream(*it);
    AudioStreamID stream = coreaudio::GetStreamForDevice(*it, input);
    cout << "  Stream: " << stream << " ("
         << (input ? "Input" : "Output" ) << ")" << endl;

    AudioObjectShow(*it);
    AudioObjectShow(stream);

    AudioStreamBasicDescription desc;
    if (!coreaudio::GetStreamDescription(stream, &desc)) {
      cerr << "coreaudio::GetStreamDescription() failed" << endl;
      return 1;
    }

    cout << "SampleRate: " << desc.mSampleRate << endl;
    cout << "Format ID: " << desc.mFormatID << endl;
    cout << "Bytes Per Packet: " << desc.mBytesPerPacket << endl;
    cout << "Frames per packet: " << desc.mFramesPerPacket << endl;
    cout << "Bytes Per Frame: " << desc.mBytesPerFrame << endl;

    cout << endl;
  }
}
