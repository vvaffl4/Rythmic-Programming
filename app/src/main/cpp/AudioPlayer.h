//
// Created by patri on 4/17/2018.
//

#ifndef NATIVE_ACTIVITY_AUDIOPLAYER_H
#define NATIVE_ACTIVITY_AUDIOPLAYER_H

#include <android/log.h>
#include <string>

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

class AudioPlayer
{
    SLObjectItf outputMixObject = NULL;

    SLObjectItf audioEngineObject = NULL;
    SLEngineItf audioEngine;

    SLObjectItf audioPlayerObject = NULL;
    SLPlayItf audioPlayer;
    SLSeekItf audioPlayerSeek;
    SLMuteSoloItf audioPlayerMuteSolo;
    SLVolumeItf audioPlayerVolume;

public:
    AudioPlayer();

    void printHelloWorld();
    bool createAssetAudioPlayer(AAssetManager* assetManager, const std::string& path);
    bool play();
    bool play(const SLmillisecond &position);

    bool setPosition(const SLmillisecond &position);
    SLmillisecond getPosition();

    bool setVolume(const SLmillibel &volume);
    SLmillibel getVolume();
};


#endif //NATIVE_ACTIVITY_AUDIOPLAYER_H
