#include "audio.h"

void audio_world_init(audio_world_t *audio)
{
    audio->device = alcOpenDevice(NULL);
    audio->context = alcCreateContext(audio->device, NULL);
    alcMakeContextCurrent(audio->context);

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    ALuint  buffer;
    ALuint  source;
    ALuint  environment;

    char al_bool;

    ALsizei size,freq;
    ALenum format;
    ALvoid *data;

    audio->listenerPos[0] = 0.0;
    audio->listenerPos[1] = 0.0;
    audio->listenerPos[2] = 0.0;
    audio->listenerVel[0] = 0.0;
    audio->listenerVel[1] = 0.0;
    audio->listenerVel[2] = 0.0;
    audio->listenerOri[0] = 0.0;
    audio->listenerOri[1] = 0.0;
    audio->listenerOri[2] = 1.0;
    audio->listenerOri[3] = 0.0;
    audio->listenerOri[4] = 1.0;
    audio->listenerOri[5] = 0.0;

    ALfloat source0Pos[]={0.0, 0.0, 0.0};
    ALfloat source0Vel[]={0.0, 0.0, 0.0};

    alListenerfv(AL_POSITION, audio->listenerPos);
    alListenerfv(AL_VELOCITY, audio->listenerVel);
    alListenerfv(AL_ORIENTATION, audio->listenerOri);

    alGenBuffers(1, &buffer);
    alGenSources(1, &source);

    alutLoadWAVFile((ALbyte*)"c.wav", &format, &data, &size, &freq, &al_bool);
    alBufferData(buffer, format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);

    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 0.9f);
    alSourcefv(source, AL_POSITION, source0Pos);
    alSourcefv(source, AL_VELOCITY, source0Vel);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, AL_TRUE);
    alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);
    alSourcef(source, AL_MAX_DISTANCE, 100.0f);
    alSourcef(source, AL_REFERENCE_DISTANCE, 1.0f);

    alSourcePlay(source);
}

void audio_world_update_listener(audio_world_t *audio, gameobject_t *listener, float delta)
{
    audio->listenerVel[0] = (listener->globalPos.x - audio->listenerPos[0]) / delta;
    audio->listenerVel[1] = (listener->globalPos.y - audio->listenerPos[1]) / delta;
    audio->listenerVel[2] = (listener->globalPos.z - audio->listenerPos[2]) / delta;

    std::cout << "audio position: " << audio->listenerPos[0] << ":" << audio->listenerPos[1] << ":" << audio->listenerPos[2] << std::endl;

    audio->listenerPos[0] = listener->globalPos.x;
    audio->listenerPos[1] = listener->globalPos.y;
    audio->listenerPos[2] = listener->globalPos.z;

    glm::vec3 forward = gameobject_rotate_vector(listener, glm::vec3(0.0f, 0.0f, 1.0f));
    audio->listenerOri[0] = forward.x;
    audio->listenerOri[1] = forward.y;
    audio->listenerOri[2] = forward.z;

    alListenerfv(AL_POSITION, audio->listenerPos);
    alListenerfv(AL_VELOCITY, audio->listenerVel);
    alListenerfv(AL_ORIENTATION, audio->listenerOri);
}

void audio_world_cleanup(audio_world_t *audio)
{
    alcDestroyContext(audio->context);
    alcCloseDevice(audio->device);
}
