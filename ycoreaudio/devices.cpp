#include <CoreAudio/CoreAudio.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

#include "util.h"

using namespace std;

int main(int argc, char* argv[]) {
  vector<AudioDeviceID> devices;
  if (!ycoreaudio::GetDevices(&devices)) {
    cerr << "ycoreaudio::GetDevices() failed" << endl;
    return 1;
  }
  for (vector<AudioDeviceID>::const_iterator it = devices.begin();
       it != devices.end(); ++it) {
    struct ycoreaudio::DeviceInfo info;
    if (!ycoreaudio::GetDeviceInfo(*it, &info)) {
      cerr << "ycoreaudio::GetDeviceInfo() failed" << endl;
      return 1;
    }
    cout << "DeviceID: " << *it << endl;
    cout << "     UID: " << info.uid << endl;
    cout << "    Name: " << info.name << endl;
    bool input = ycoreaudio::HasInputStream(*it);
    AudioStreamID stream = ycoreaudio::GetStreamForDevice(*it, input);
    cout << "  Stream: " << stream << " ("
         << (input ? "Input" : "Output" ) << ")" << endl;

    AudioObjectShow(*it);
    AudioObjectShow(stream);

    AudioStreamBasicDescription desc;
    if (!ycoreaudio::GetStreamDescription(stream, &desc)) {
      cerr << "ycoreaudio::GetStreamDescription() failed" << endl;
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
