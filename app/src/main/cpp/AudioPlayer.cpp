//
// Created by patri on 4/17/2018.
//
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

#include "AudioPlayer.h"

AudioPlayer::AudioPlayer() {
    SLresult result;

    // Create engine
    result = slCreateEngine(&audioEngineObject, 0, NULL, 0, NULL, NULL);
    if(result != SL_RESULT_SUCCESS)
        return;
    (void)result;

    // Realize engine
    result = (*audioEngineObject)->Realize(audioEngineObject, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS)
        return;
    (void)result;

    // Get engine interface, to create other objects (HIGHLY NEEDED)
    result = (*audioEngineObject)->GetInterface(audioEngineObject, SL_IID_ENGINE, &audioEngine);
    if(result != SL_RESULT_SUCCESS)
        return;
    (void)result;

    // Create output mix, (with environmental reverb specified as a non-required interface)
//    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
//    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*audioEngine)->CreateOutputMix(audioEngine, &outputMixObject, 0, NULL, NULL);
    if(result != SL_RESULT_SUCCESS)
        return;
    (void)result;

    // Realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS)
        return;
    (void)result;
}

void AudioPlayer::printHelloWorld() {
    LOGI("Hello Happy World!");
}

bool AudioPlayer::createAssetAudioPlayer(AAssetManager* assetManager, const std::string &path) {

    AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_UNKNOWN);

    if(asset == NULL)
        return false;

    // Get file descriptor
    off_t start;
    off_t length;
    int fileDescriptor = AAsset_openFileDescriptor(asset, &start, &length);
    AAsset_close(asset);

    // Configure audio source
    SLDataLocator_AndroidFD locatorFileDescriptor = {
            SL_DATALOCATOR_ANDROIDFD,
            fileDescriptor,
            start,
            length};
    SLDataFormat_MIME formatMime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_MP3}; //SL_CONTAINERTYPE_UNSPECIFIED
    SLDataSource audioSource = {&locatorFileDescriptor, &formatMime};

    // Configure audio sink
    SLDataLocator_OutputMix locatorOutputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSink = {&locatorOutputMix, NULL};



    SLresult result;

    // Create audio player
    const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*audioEngine)->CreateAudioPlayer(
            audioEngine,
            &audioPlayerObject,
            &audioSource,
            &audioSink,
            3,
            ids,
            req);
    if(result != SL_RESULT_SUCCESS)
        return false;
    (void)result;

    // Realize audio player
    result = (*audioPlayerObject)->Realize(audioPlayerObject, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS)
        return false;
    (void)result;

    // Get play interface
    result = (*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_PLAY, &audioPlayer);
    if(result != SL_RESULT_SUCCESS)
        return false;
    (void)result;

    // Get the seek interface
    result = (*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_SEEK, &audioPlayerSeek);
    if(result != SL_RESULT_SUCCESS)
        return false;
    (void)result;

    // Get the mute/solo interface
    result = (*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_MUTESOLO, &audioPlayerMuteSolo);
    if(result != SL_RESULT_SUCCESS)
        return false;
    (void)result;

    // Get the volume interface
    result = (*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_VOLUME, &audioPlayerVolume);
    if(result != SL_RESULT_SUCCESS)
        return false;
    (void)result;

    // Enable whole file looping
    result = (*audioPlayerSeek)->SetLoop(audioPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
    return result == SL_RESULT_SUCCESS;
}

bool AudioPlayer::play()
{
    if(audioPlayer != NULL)
    {
        return (*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_PLAYING) == SL_RESULT_SUCCESS;
    }

    return false;
}

bool AudioPlayer::play(const SLmillisecond& position)
{
    if(audioPlayer != NULL)
    {
        SLresult result;

        result = (*audioPlayerSeek)->SetPosition(audioPlayerSeek, position, SL_SEEKMODE_FAST);
        if(result != SL_RESULT_SUCCESS)
            return false;
        (void)result;

        return (*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_PLAYING) == SL_RESULT_SUCCESS;
    }

    return false;
}

SLmillisecond AudioPlayer::getPosition()
{
    if(audioPlayer != NULL) {
        SLresult result;

        SLmillisecond position = NULL;

        result = (*audioPlayer)->GetPosition(audioPlayer, &position);
        if(result == SL_RESULT_SUCCESS)
            return position;
    }

    return (SLmillisecond)0;
}

SLmillibel AudioPlayer::getVolume() {
    if(audioPlayer != NULL) {
        SLresult result;

        SLmillibel volume = NULL;

        result = (*audioPlayerVolume)->GetVolumeLevel(audioPlayerVolume, &volume);
        if(result == SL_RESULT_SUCCESS)
            return volume;
    }

    return (SLmillibel)0;
}

bool AudioPlayer::setPosition(const SLmillisecond &position) {
    if(audioPlayer != NULL) {
        return (*audioPlayerSeek)->SetPosition(audioPlayerSeek, position, SL_SEEKMODE_FAST) == SL_RESULT_SUCCESS;
    }
    return false;
}

bool AudioPlayer::setVolume(const SLmillibel &volume) {
    if(audioPlayer != NULL) {
        return (*audioPlayerVolume)->SetVolumeLevel(audioPlayerVolume, volume) == SL_RESULT_SUCCESS;
    }

    return false;
}

