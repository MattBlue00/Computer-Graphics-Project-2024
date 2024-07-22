#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <fmod.hpp>
#include <fmod_errors.h>

struct AudioManager : public Observer {
    FMOD::System* audio_system = nullptr;
    FMOD_RESULT result;
    std::map<std::string, FMOD::Sound*> soundMap;
    std::map<std::string, std::vector<FMOD::Channel*>> channelMap;  // Map to keep track of channels

    // Initialize the audio system
    void initAudio(const json& audioMap) {
        result = FMOD::System_Create(&audio_system);
        checkFmodError(result);
        result = audio_system->init(1024, FMOD_INIT_NORMAL, 0);  // Increased number of channels
        checkFmodError(result);

        for (const auto& music : audioMap) {
            std::string name = music["name"];
            std::string path = music["path"];

            FMOD::Sound* sound;
            result = audio_system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);
            checkFmodError(result);

            soundMap[name] = sound;
        }
    }

    // Play a sound with optional looping
    void playSound(const std::string& soundName, float volume, float loopStartSecond = -1, float speed = 1) {
        FMOD::Channel* channel = nullptr;

        // Set loop mode and loop points before playing the sound
        if (loopStartSecond >= 0) {
            unsigned int loopStartMillis = loopStartSecond * 1000;

            unsigned int soundLength = 0;
            result = soundMap[soundName]->getLength(&soundLength, FMOD_TIMEUNIT_MS);
            checkFmodError(result);

            result = soundMap[soundName]->setMode(FMOD_LOOP_NORMAL);
            checkFmodError(result);
            result = soundMap[soundName]->setLoopPoints(loopStartMillis, FMOD_TIMEUNIT_MS, soundLength, FMOD_TIMEUNIT_MS);
            checkFmodError(result);
        } else {
            result = soundMap[soundName]->setMode(FMOD_LOOP_OFF);
            checkFmodError(result);
        }

        // Play the sound
        result = audio_system->playSound(soundMap[soundName], 0, false, &channel);
        checkFmodError(result);

        // Set volume
        result = channel->setVolume(volume);
        checkFmodError(result);
        
        // Set playback speed using pitch
        result = channel->setPitch(speed);
        checkFmodError(result);

        // Store the channel in the map
        channelMap[soundName].push_back(channel);

        if (loopStartSecond < 0) {
            // Attach a callback to remove the channel from channelMap when sound finishes
            result = channel->setCallback([](FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commandData1, void* commandData2) -> FMOD_RESULT {
                if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END) {
                    FMOD::Channel* channel = (FMOD::Channel*)channelControl;
                    // Find and remove the channel from channelMap
                    for (auto& entry : AudioManager().channelMap) {
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
    }
    
    // Stop a sound
    void stopAudio(const std::string& soundName) {
        auto it = channelMap.find(soundName);
        if (it != channelMap.end()) {
            for (FMOD::Channel* channel : it->second) {
                bool isPlaying;
                result = channel->isPlaying(&isPlaying);
                checkFmodError(result);
                if (isPlaying) {
                    result = channel->stop();
                    checkFmodError(result);
                }
            }
            it->second.clear();  // Clear the channel list for this sound
        }
    }

    // Update the audio system
    void updateAudioSystem() {
        result = audio_system->update();
        checkFmodError(result);
    }

    // Check for FMOD errors
    void checkFmodError(FMOD_RESULT result) {
        if (result != FMOD_OK) {
            std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
            exit(-1);
        }
    }

    // Clean up the audio system
    void cleanupAudio() {
        for (auto& entry : soundMap) {
            result = entry.second->release();
            checkFmodError(result);
        }
        if (audio_system) {
            result = audio_system->close();
            checkFmodError(result);
            result = audio_system->release();
            checkFmodError(result);
            audio_system = nullptr;
        }
    }
    
    
    // -------Observer methods--------
    void onCoinCollected(int collectedCoins) override {
        playSound("COIN_SFX", 1.0f);
    }
    
    void onStartSemaphore(int countDownValue) override {
        if(countDownValue > 1 && countDownValue < 5){
            playSound("COUNTDOWN_SFX", 0.15f);
        }else if (countDownValue == 1){
            playSound("START_SFX", 0.15f);
            // plays the race music
            playSound("RACE_MUSIC", 0.1f, 7);
        }
    };
    
    int lapsLabel = 1;
    void onCheckLaps(int lapsDone) override {
        if(lapsLabel > 3) return;

        lapsLabel += lapsDone;
        if(lapsLabel == 2){
            playSound("FINAL_SFX", 1.0f);
            stopAudio("RACE_MUSIC");
            playSound("RACE_MUSIC", 0.1f, 7, 2);
        }
        else if(lapsLabel == 3){
            stopAudio("RACE_MUSIC");
            playSound("END_SFX", 1.0f);
        }
    }
};

#endif

