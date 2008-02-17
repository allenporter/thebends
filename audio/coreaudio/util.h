// util.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __COREAUDIO_UTIL_H__
#define __COREAUDIO_UTIL_H__

#include <CoreAudio/CoreAudio.h>
#include <vector>
#include <string>

namespace coreaudio {

bool GetDevices(std::vector<AudioDeviceID>* devices);

struct DeviceInfo {
  std::string uid;
  std::string name;
};

bool GetDeviceInfo(AudioDeviceID device, struct DeviceInfo* info);

AudioDeviceID GetDefaultOutputDevice();

bool HasInputStream(AudioDeviceID device);

AudioStreamID GetStreamForDevice(AudioDeviceID device_id, bool input);

AudioStreamID GetDefaultOutputStream();

bool GetStreamDescription(AudioStreamID stream,
                          AudioStreamBasicDescription* description);

}  // namespace coreaudio

#endif  // __COREAUDIO_UTIL_H__
