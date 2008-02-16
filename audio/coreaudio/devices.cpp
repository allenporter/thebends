#include <CoreAudio/CoreAudio.h>
#include <assert.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  OSStatus ret;
  UInt32 size;
  Boolean ignored;
  ret = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices,
                                     &size,
                                     &ignored);
  assert(ret == 0);
  assert(size > 0);
  int num_devices = size / sizeof(AudioDeviceID);
  AudioDeviceID* devices = new AudioDeviceID[num_devices];
  ret = AudioHardwareGetProperty(kAudioHardwarePropertyDevices,
                                 &size,
                                 devices);
  for (int i = 0 ; i < num_devices; i++) {
    cout << "DeviceID: " << devices[i] << endl;
    CFStringRef device_uid = NULL;
    size = sizeof(CFStringRef);
    ret = AudioDeviceGetProperty(devices[i],
                                 0,
                                 false,
                                 kAudioDevicePropertyDeviceUID,
                                 &size,
                                 &device_uid);
    assert(ret == 0);
    cout << "  UID: "
         << CFStringGetCStringPtr(device_uid, CFStringGetSystemEncoding())
         << endl;

    char device_name[256];
    size = 256;
    ret = AudioDeviceGetProperty(devices[i],
                                 0,
                                 false,
                                 kAudioDevicePropertyDeviceName,
                                 &size,
                                 &device_name);
    assert(ret == 0);
    cout << "  Name: " << device_name << endl;
  }
  delete [] devices;
  
}
