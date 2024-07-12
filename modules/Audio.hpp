#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <fmod.hpp>
#include <fmod_errors.h>

FMOD::System *audio_system = nullptr;
FMOD_RESULT result;

std::map<std::string, FMOD::Sound*> soundMap;

std::map<std::string, const char*> soundLoadingMap = {
    {"RACE_MUSIC", "assets\audio\music\RaceBackgroundMusic.mp3"}
};

void checkFmodError(FMOD_RESULT result);

void initAudio(const std::string& projectPath){
    result = FMOD::System_Create(&audio_system);
    checkFmodError(result);
    result = audio_system->init(512, FMOD_INIT_NORMAL, 0);
    checkFmodError(result);
    
    for (auto it = soundLoadingMap.begin(); it != soundLoadingMap.end(); ++it) {
        FMOD::Sound *sound;

        std::stringstream fullPathStream;
        fullPathStream << projectPath << it->second;
        std::string fullPath = fullPathStream.str();

        FMOD_RESULT result = audio_system->createSound(fullPath.c_str(), FMOD_DEFAULT, 0, &sound);
        checkFmodError(result);

        soundMap[it->first] = sound;
    }
}

void playSound(const std::string& soundName, float volume, int loopStartSecond = -1) {
    FMOD::Channel* channel = nullptr;

    // Set loop mode and loop points before playing the sound
    if(loopStartSecond >= 0) {
        // Convert to milliseconds
        unsigned int loopStartMillis = loopStartSecond * 1000;

        // Get sound length in milliseconds
        unsigned int soundLength = 0;
        result = soundMap[soundName]->getLength(&soundLength, FMOD_TIMEUNIT_MS);
        checkFmodError(result);

        // Set loop mode and loop starting point
        result = soundMap[soundName]->setMode(FMOD_LOOP_NORMAL);
        checkFmodError(result);
        result = soundMap[soundName]->setLoopPoints(loopStartMillis, FMOD_TIMEUNIT_MS, soundLength, FMOD_TIMEUNIT_MS);
        checkFmodError(result);
    }

    // Play the sound
    result = audio_system->playSound(soundMap[soundName], 0, false, &channel);
    checkFmodError(result);

    // Set volume
    result = channel->setVolume(volume);
    checkFmodError(result);
}

void checkFmodError(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
        exit(-1);
    }
}

#endif
