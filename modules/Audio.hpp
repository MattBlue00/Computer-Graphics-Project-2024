#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <fmod.hpp>
#include <fmod_errors.h>

FMOD::System *audio_system = nullptr;
FMOD_RESULT result;

std::map<std::string, FMOD::Sound*> soundMap;
std::map<std::string, std::vector<FMOD::Channel*>> channelMap;  // Map to keep track of channels

void checkFmodError(FMOD_RESULT result);
void removeChannel(const std::string& soundName, FMOD::Channel* channel);

void initAudio(const json& audioMap) {
    result = FMOD::System_Create(&audio_system);
    checkFmodError(result);
    result = audio_system->init(512, FMOD_INIT_NORMAL, 0);
    checkFmodError(result);

    for (const auto& music : audioMap) {
        std::string name = music["name"];
        std::string path = music["path"];

        FMOD::Sound *sound;
        result = audio_system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);
        checkFmodError(result);

        soundMap[name] = sound;
    }
}

void playSound(const std::string& soundName, float volume, float loopStartSecond = -1, bool allowOverlap = true) {
    FMOD::Channel* channel = nullptr;
    bool isPlaying = false;

    // Check if the sound is already playing
    if (!allowOverlap && channelMap.find(soundName) != channelMap.end()) {
        for (auto& ch : channelMap[soundName]) {
            if (ch) {
                result = ch->isPlaying(&isPlaying);
                checkFmodError(result);
                if (isPlaying) {
                    return;  // If any instance is playing and overlaps are not allowed, return
                }
            }
        }
    }

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

    // Store the channel in the map
    channelMap[soundName].push_back(channel);

    // Attach a callback to remove the channel from channelMap when sound finishes
    result = channel->setCallback([](FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commandData1, void* commandData2) -> FMOD_RESULT {
        if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END) {
            FMOD::Channel* channel = (FMOD::Channel*)channelControl;
            // Find and remove the channel from channelMap
            for (auto& entry : channelMap) {
                auto& channels = entry.second;
                auto it = std::find(channels.begin(), channels.end(), channel);
                if (it != channels.end()) {
                    channels.erase(it);
                    break;  // Exit loop after removing the channel
                }
            }
        }
        return FMOD_OK;
    });
    checkFmodError(result);
}

void stopSound(const std::string& soundName) {
    if (channelMap.find(soundName) != channelMap.end()) {
        auto& channels = channelMap[soundName];
        for (auto& channel : channels) {
            if (channel) {
                bool isPlaying = false;
                result = channel->isPlaying(&isPlaying);
                checkFmodError(result);
                if (isPlaying) {
                    result = channel->stop();
                    checkFmodError(result);
                }
            }
        }
        // Clear the channels for the soundName
        channels.clear();
    }
}

void removeChannel(const std::string& soundName, FMOD::Channel* channel) {
    if (channelMap.find(soundName) != channelMap.end()) {
        auto& channels = channelMap[soundName];
        auto it = std::find(channels.begin(), channels.end(), channel);
        if (it != channels.end()) {
            channels.erase(it);
        }
        if (channels.empty()) {
            channelMap.erase(soundName);
        }
    }
}

void checkFmodError(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
        exit(-1);
    }
}

#endif
