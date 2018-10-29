#include "audio.h"

void audio_world_init(audio_world_t *audio)
{
    audio->device = alcOpenDevice(NULL);
    audio->context = alcCreateContext(audio->device, NULL);
    alcMakeContextCurrent(audio->context);

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    audio->listenerPos[0] = 0.0;
    audio->listenerPos[1] = 0.0;
    audio->listenerPos[2] = 0.0;
    audio->listenerVel[0] = 0.0;
    audio->listenerVel[1] = 0.0;
    audio->listenerVel[2] = 0.0;
    audio->listenerOri[0] = 0.0;
    audio->listenerOri[1] = 0.0;
    audio->listenerOri[2] = -1.0;
    audio->listenerOri[3] = 0.0;
    audio->listenerOri[4] = 1.0;
    audio->listenerOri[5] = 0.0;

    alListenerfv(AL_POSITION, audio->listenerPos);
    alListenerfv(AL_VELOCITY, audio->listenerVel);
    alListenerfv(AL_ORIENTATION, audio->listenerOri);
}

void audio_world_update(audio_world_t *audio, float delta)
{
    if (audio->listener)
    {
        audio->listenerVel[0] = (-audio->listener->globalPos.x - audio->listenerPos[0]) / delta;
        audio->listenerVel[1] = (audio->listener->globalPos.y - audio->listenerPos[1]) / delta;
        audio->listenerVel[2] = (-audio->listener->globalPos.z - audio->listenerPos[2]) / delta;

        audio->listenerPos[0] = -audio->listener->globalPos.x;
        audio->listenerPos[1] = audio->listener->globalPos.y;
        audio->listenerPos[2] = -audio->listener->globalPos.z;

        glm::vec3 forward = gameobject_rotate_vector(audio->listener, glm::vec3(0.0f, 0.0f, 1.0f));
        audio->listenerOri[0] = forward.x;
        audio->listenerOri[1] = forward.y;
        audio->listenerOri[2] = forward.z;

        alListenerfv(AL_POSITION, audio->listenerPos);
        alListenerfv(AL_VELOCITY, audio->listenerVel);
        alListenerfv(AL_ORIENTATION, audio->listenerOri);
    }

    for (auto it = audio->sources.begin(); it != audio->sources.end(); ++it)
    {
        it->vel[0] = (-it->object->globalPos.x - it->pos[0]) / delta;
        it->vel[1] = (it->object->globalPos.y - it->pos[1]) / delta;
        it->vel[2] = (-it->object->globalPos.z - it->pos[2]) / delta;
        
        it->pos[0] = -it->object->globalPos.x;
        it->pos[1] = it->object->globalPos.y;
        it->pos[2] = -it->object->globalPos.z;

        alSourcefv(it->source, AL_POSITION, it->pos);
        alSourcefv(it->source, AL_VELOCITY, it->vel);
    }
}

void audio_world_cleanup(audio_world_t *audio)
{
    alcDestroyContext(audio->context);
    alcCloseDevice(audio->device);
}

ALuint audio_source_create(audio_world_t *audio, gameobject_t *object, std::string path)
{
    return -1;
    if (audio->buffers.find(path) == audio->buffers.end())
    {
        alGenBuffers(1, &audio->buffers[path]);
        ALsizei size, freq;
        ALenum format;
        ALvoid *data;
#ifdef __APPLE__
        //alutLoadWAVFile((ALbyte*)path.c_str(), &format, &data, &size, &freq);
#else
        char alBool;
        //alutLoadWAVFile((ALbyte*)path.c_str(), &format, &data, &size, &freq, &alBool);
#endif
        alBufferData(audio->buffers[path], format, data, size, freq);
        //alutUnloadWAV(format, data, size, freq);
    }


    audio_source_t source;
    source.object = object;
    source.pos[0] = -object->globalPos.x;
    source.pos[1] = object->globalPos.y;
    source.pos[2] = -object->globalPos.z;
    source.vel[0] = 0.0f;
    source.vel[1] = 0.0f;
    source.vel[2] = 0.0f;

    alGenSources(1, &source.source);
    alSourcef(source.source, AL_PITCH, 1.0f);
    alSourcef(source.source, AL_GAIN, 0.9f);
    alSourcefv(source.source, AL_POSITION, source.pos);
    alSourcefv(source.source, AL_VELOCITY, source.vel);
    alSourcei(source.source, AL_BUFFER, audio->buffers[path]);
    alSourcei(source.source, AL_LOOPING, AL_TRUE);
    alSourcef(source.source, AL_ROLLOFF_FACTOR, 1.0f);
    alSourcef(source.source, AL_MAX_DISTANCE, 100.0f);
    alSourcef(source.source, AL_REFERENCE_DISTANCE, 1.0f);

    audio->sources.push_back(source);
   
    return source.source;
}
