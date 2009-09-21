#import <AudioToolbox/AudioToolbox.h>
#import <CoreAudio/CoreAudio.h>

#define NUM_BUFFERS 3

static const float kD = 1.0313897683787216;
static const float kC = 3235.4938901792257;

static const float kPI = 3.1415926535897931;

struct PlayerState {
    AudioStreamBasicDescription     mDataFormat;
    AudioQueueRef                   mQueue;
    AudioQueueBufferRef             mBuffers[NUM_BUFFERS];
    AudioStreamPacketDescription    *mPacketDescs;
    int                             mIsRunning;
    UInt32                          mFrequency;
    UInt32                          mSample;
};

static void HandleOutputBuffer (void *inUserData,
                                AudioQueueRef inAQ,
                                AudioQueueBufferRef inBuffer) {
    printf("HandleOutputBuffer\n");
    struct PlayerState* state = (struct PlayerState*)inUserData;
    if (!state->mIsRunning) {
        AudioQueueStop (state->mQueue, false);
        AudioQueueDispose(state->mQueue, false);
        
        return;
    }
   
    inBuffer->mAudioDataByteSize = 1024 * state->mDataFormat.mBytesPerFrame;
    int numPackets = inBuffer->mAudioDataByteSize / sizeof(float);
    float* data = (float*)inBuffer->mAudioData;
    for (int i = 0; i < numPackets; ++i) {
        state->mSample = (state->mSample + 1) % (int)state->mDataFormat.mSampleRate;
        int t = state->mSample;
        data[i] = sinf((2 * kPI * state->mFrequency * t) / state->mDataFormat.mSampleRate);
    }
    inBuffer->mAudioDataByteSize = sizeof(float) * numPackets;
    
   printf("Enqueue\n"); 
   OSStatus status = AudioQueueEnqueueBuffer(state->mQueue,
                            inBuffer,
                            (state->mPacketDescs ? numPackets : 0),
                            state->mPacketDescs);
   printf("Enqueue done: %d\n", (int)status); 
}

int main(int argc, char* argv[]) {
	struct PlayerState* state;
	state = (struct PlayerState*)malloc(sizeof(struct PlayerState));
	bzero(state, sizeof(struct PlayerState));
	state->mIsRunning = 1;
	state->mDataFormat.mSampleRate = 44100;
	state->mDataFormat.mFormatID = kAudioFormatLinearPCM;
	state->mDataFormat.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;// | kLinearPCMFormatFlagIsNonInterleaved;
	state->mDataFormat.mBytesPerPacket = sizeof(float);
	state->mDataFormat.mFramesPerPacket = 1;
	state->mDataFormat.mBytesPerFrame =
		state->mDataFormat.mBytesPerPacket *  state->mDataFormat.mFramesPerPacket;
	state->mDataFormat.mChannelsPerFrame = 1;
	state->mDataFormat.mBitsPerChannel = sizeof(float) * 8;
	state->mPacketDescs = NULL;
	state->mSample = 0;

	OSType status =
  	  AudioQueueNewOutput(
     		&state->mDataFormat, HandleOutputBuffer, state,
	 	CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0,
                &state->mQueue);
	assert(status == 0);

	AudioQueueSetParameter (state->mQueue, kAudioQueueParam_Volume, 1.0);

	int bufferByteSize = 1024 * state->mDataFormat.mBytesPerFrame;

	for (int i = 0; i < NUM_BUFFERS; ++i) {
printf("Buf: %d\n", i);
		OSStatus status =
			AudioQueueAllocateBuffer(state->mQueue,
					bufferByteSize,
					&state->mBuffers[i]);
printf("status=%d\n", (int)status);
		assert(status == 0);
HandleOutputBuffer(state, state->mQueue, state->mBuffers[i]);
	}

        int frequency = 500; 

	state->mIsRunning = 1;
	state->mFrequency = frequency;

printf("Starting queue\n");
	status = AudioQueueStart(state->mQueue, NULL);
        assert(status == 0);

     	CFRunLoopRun();

}



