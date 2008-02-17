// sample.cpp
// Author: Allen Porter <allen@thebends.org>
//
// Generates a random linear PCM audio signal using the CoreAudio API.

#include <CoreAudio/CoreAudio.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

#include "util.h"

#define NDEBUG

using namespace std;

struct SoundState {
  int counter;
  float multiplier;
};

OSStatus Proc(AudioDeviceID inDevice, 
              const AudioTimeStamp* inNow,
              const AudioBufferList* inInputData,
              const AudioTimeStamp* inInputTime,
              AudioBufferList* outOutputData,
              const AudioTimeStamp* inOutputTime,
              void* inClientData) {
  assert(outOutputData->mNumberBuffers > 0);
  AudioBuffer* buf = &outOutputData->mBuffers[0];
  int units = buf->mDataByteSize / sizeof(float);

  SoundState* state = (SoundState*)inClientData;
  state->counter++;
  if ((state->counter % 20) == 0) {
    state->multiplier = 0.5 * ((random() % 100) / 100.0) + 0.1;
  }

#ifndef NDEBUG
  cout << "Proc called (" << state->multiplier << ", " << state->counter
       << ")" << endl;
#endif

  float* data = (float*)buf->mData;
  for (int i = 0; i < units; ++i) {
    data[i] = sinf(state->multiplier * i);
  }
  return 0;
}

int main(int argc, char* argv[]) {
  srandom(time(NULL));
  AudioDeviceID device = coreaudio::GetDefaultOutputDevice();

  SoundState state;
  state.counter = 0;
  state.multiplier = 1;

  OSStatus status;
  status = AudioDeviceAddIOProc(device, &Proc, &state);
  if (status) {
    cerr << "AudioDeviceAddIOProc: " << status;
    return 1;
  }
  status = AudioDeviceStart(device, &Proc);
  if (status) {
    cerr << "AudioDeviceAddIOProc: " << status;
    return 1;
  }
  CFRunLoopRun();
  AudioDeviceStop(device, &Proc);
}
