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

//#define NDEBUG
#define PI 3.14159265 

using namespace std;

struct SoundState {
  int sample;
  int freq;
  float multiplier;
  AudioStreamBasicDescription description;
};

int note = 0;
int notes[] = {
 659, 587, 523, 587, 659, 659, 659, 587, 587, 587, 659, 698, 698, 0
//  523, 493, 440, 0,
//  261, 277, 293, 311, 329, 349, 369, 392, 0
};

OSStatus Proc(AudioDeviceID inDevice, 
              const AudioTimeStamp* inNow,
              const AudioBufferList* inInputData,
              const AudioTimeStamp* inInputTime,
              AudioBufferList* outOutputData,
              const AudioTimeStamp* inOutputTime,
              void* inClientData) {
  assert(outOutputData->mNumberBuffers > 0);
  assert(inOutputTime->mSampleTime != 0);

  SoundState* state = (SoundState*)inClientData;

  AudioBuffer* buf = &outOutputData->mBuffers[0];
  int units = buf->mDataByteSize / sizeof(float);
  float* data = (float*)buf->mData;
  for (int i = 0; i < units; ++i) {
    if (state->sample == 0) {
      state->freq = notes[note++];
      if (state->freq == 0) {
        note = 0;
        state->freq = notes[0];
      }
      cout << state->freq << endl;
    }
    state->sample = (state->sample + 1) % (int)state->description.mSampleRate;
    int t = state->sample;
    data[i] = sinf((2 * PI * state->freq * t) / state->description.mSampleRate);
  }
  return 0;
}

int main(int argc, char* argv[]) {
  srandom(time(NULL));
  AudioDeviceID device = coreaudio::GetDefaultOutputDevice();

  SoundState state;
  state.sample = 0;
  state.multiplier = 1;
  state.freq = notes[0];

  if (!coreaudio::GetStreamDescription(coreaudio::GetDefaultOutputStream(),
                                       &state.description)) {
    cerr << "GetStreamDescription failed" << endl;
    return 1;
  }
  assert(state.description.mFormatID == kAudioFormatLinearPCM);
                            

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
