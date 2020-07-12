#include <AL/al.h>
#include <AL/alut.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>

#define MAX_SOUNDBUFFERS 0
#define MAX_SOUNDSOURCES 0

fstream data_file;

void InitOpenAL() {
	alutInit(NULL, 0);  // init OpenAL
	alGetError();      // checking for errors
} // end InitOpenAL()

void alLoadSound(ALuint &SoundBuffer, ALuint &SoundSource, char *filename) {
	alGenBuffers(1, &SoundBuffer);
	alGenSources(1, &SoundSource);

	ALboolean bLooping;
	ALsizei size, freq;
	ALenum format;
	ALvoid *data;

	alutLoadWAVFile((ALbyte*) filename, &format, &data, &size, &freq, &bLooping);
	alBufferData(SoundBuffer, format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
	alSourcei(SoundSource, AL_BUFFER, SoundBuffer); // attach the buffer to the sound source
}

ALuint g_SoundBuffers[MAX_SOUNDBUFFERS];
ALuint g_SoundSources[MAX_SOUNDSOURCES];

ALfloat listenerPos[] = {0.0, 0.0, 4.0};
ALfloat listenerVel[] = {0.0, 0.0, 0.0};
ALfloat listenerOri[] = {0.0, 0.0, 1.0, 0.0, 1.0, 0.0};

ALfloat source0Pos[] = { 2.0, 0.0, 0.0};
ALfloat source0Vel[] = { 0.0, 0.0, 0.0};

void init() {
	InitOpenAL();

	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);

	alLoadSound(g_SoundBuffers[0], g_SoundSources[0], "test.wav");
		alSourcei(g_SoundSources[0], AL_LOOPING, AL_FALSE);
		alSourcef(g_SoundSources[0], AL_PITCH, 1.0);
		alSourcef(g_SoundSources[0], AL_GAIN, 1.0);
		alSourcef(g_SoundSources[0], AL_MAX_DISTANCE, 13.0);
		alSourcefv(g_SoundSources[0], AL_POSITION, source0Pos);
		alSourcefv(g_SoundSources[0], AL_VELOCITY, source0Vel);

	alSourcePlay(g_SoundSources[0]);

}// end Init()

int main(void) {
	init();
	while(1);
	return 0;
} // end main()
