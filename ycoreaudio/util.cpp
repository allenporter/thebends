// util.cpp
// Author: Allen Porter <allen@thebends.org>

#include "util.h"
#include <iostream>

using namespace std;

namespace ycoreaudio {

bool GetDevices(vector<AudioDeviceID>* devices) {
  OSStatus ret;
  UInt32 size;
  Boolean ignored;
  ret = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices,
                                     &size,
                                     &ignored);
  if (ret != 0 || size == 0) {
    return false;
  }
  int num_devices = size / sizeof(AudioDeviceID);
  AudioDeviceID* devices_buffer = new AudioDeviceID[num_devices];
  ret = AudioHardwareGetProperty(kAudioHardwarePropertyDevices,
                                 &size,
                                 devices_buffer);
  for (int i = 0 ; i < num_devices; ++i) {
    devices->push_back(devices_buffer[i]);
  }
  delete [] devices_buffer;
  return true;
}

AudioDeviceID GetDefaultOutputDevice() {
  AudioDeviceID device;
  UInt32 size = sizeof(AudioDeviceID);
  OSStatus ret;
  ret = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,
                                 &size,
                                 &device);
  assert(ret == 0);
  return device;
}

AudioStreamID GetStreamForDevice(AudioDeviceID device, bool input) {
  OSStatus ret;
  UInt32 size;
  ret = AudioDeviceGetPropertyInfo(device,
                                   0, 
                                   input,
                                   kAudioDevicePropertyStreams,
                                   &size,
                                   NULL); 
  assert(ret == 0);
  assert(size > 0);
  int num_streams = size / sizeof(AudioStreamID);
  AudioStreamID* streams = new AudioStreamID[num_streams];
  ret = AudioDeviceGetProperty(device,
                               0,
                               input,
                               kAudioDevicePropertyStreams,
                               &size,
                               streams);
  AudioStreamID stream = streams[0];
  delete [] streams;
  return stream;
}

AudioStreamID GetDefaultOutputStream() {
  return GetStreamForDevice(GetDefaultOutputDevice(), false);
}

bool GetStreamDescription(AudioStreamID stream_id,
                          AudioStreamBasicDescription* description) {
  OSStatus ret;
  UInt32 size = sizeof(AudioStreamBasicDescription);;
  ret = AudioStreamGetProperty(stream_id,
                               0, 
                               kAudioStreamPropertyPhysicalFormat,
                               &size,
                               description);
  return (ret == 0);
}

bool GetDeviceInfo(AudioDeviceID device, struct DeviceInfo* info) {
  OSStatus ret;
  CFStringRef device_uid = NULL;
  UInt32 size = sizeof(CFStringRef);
  ret = AudioDeviceGetProperty(device,
      0,
      false,
      kAudioDevicePropertyDeviceUID,
      &size,
      &device_uid);
  if (ret != 0) {
    return false;
  }
  char device_name[256];
  size = 256;
  ret = AudioDeviceGetProperty(device,
      0,
      false,
      kAudioDevicePropertyDeviceName,
      &size,
      &device_name);
  if (ret != 0) {
    return false;
  }
  info->uid = string(CFStringGetCStringPtr(device_uid,
                                           CFStringGetSystemEncoding()));
  info->name = string(device_name); 
  return true;
}

bool HasInputStream(AudioDeviceID device) {
  OSStatus ret;
  UInt32 size;
  ret = AudioDeviceGetPropertyInfo(device,
                                   0,
                                   true,
                                   kAudioDevicePropertyStreams,
                                   &size,
                                   NULL);
  return (ret == 0 && size > 0);
}

}  // namespace ycoreaudio
