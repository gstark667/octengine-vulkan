#ifndef H_AUDIO
#define H_AUDIO

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include "gameobject.h"


struct audio_world_t
{
    ALCdevice *device = NULL;
    ALCcontext *context = NULL;
    ALfloat listenerPos[3];
    ALfloat listenerVel[3];
    ALfloat listenerOri[6];
};

void audio_world_init(audio_world_t *audio);
void audio_world_update_listener(audio_world_t *audio, gameobject_t *listener, float delta);
void audio_world_cleanup(audio_world_t *audio);

#endif
