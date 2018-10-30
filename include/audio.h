#ifndef H_AUDIO
#define H_AUDIO

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include <AL/alut.h>

#include <vector>

#include "export.h"
#include "gameobject.h"


struct audio_source_t
{
    ALfloat pos[3];
    ALfloat vel[3];
    gameobject_t *object;
    ALuint source;
};

struct audio_world_t
{
    gameobject_t *listener = NULL;
    ALCdevice *device = NULL;
    ALCcontext *context = NULL;
    ALfloat listenerPos[3];
    ALfloat listenerVel[3];
    ALfloat listenerOri[6];

    std::map<std::string, ALuint> buffers;
    std::vector<audio_source_t> sources;
};

extern "C" {
void EXPORT audio_world_init(audio_world_t *audio);
void EXPORT audio_world_update(audio_world_t *audio, float delta);
void EXPORT audio_world_cleanup(audio_world_t *audio);

ALuint EXPORT audio_source_create(audio_world_t *audio, gameobject_t *object, std::string path);
}

#endif
