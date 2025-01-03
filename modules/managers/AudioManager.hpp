#ifndef AUDIO_MANAGER_HPP
#define AUDIO_MANAGER_HPP

#include <fmod.hpp>
#include <fmod_errors.h>
#include "../modules/engine/pattern/Receiver.hpp"
#include <any>

class AudioManager : public Manager, public Receiver {
    
protected:
    
    FMOD::System* audio_system = nullptr;
    FMOD_RESULT result;
    std::map<std::string, FMOD::Sound*> soundMap;
    std::map<std::string, std::vector<FMOD::Channel*>> channelMap;
    std::map<FMOD::Channel*, std::function<void()>> endCallbacks;
    
    static FMOD_RESULT F_CALLBACK channelEndCallback(FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commandData1, void* commandData2) {
            if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END) {
                FMOD::Channel* channel = (FMOD::Channel*)channelControl;
                AudioManager* audioManager;
                channel->getUserData((void**)&audioManager);

                // Find and remove the channel from channelMap
                for (auto& entry : audioManager->channelMap) {
                    auto& channels = entry.second;
                    auto it = std::find(channels.begin(), channels.end(), channel);
                    if (it != channels.end()) {
                        channels.erase(it);
                        break;  // Exit loop after removing the channel
                    }
                }

                // If there's an end callback, call it
                if (audioManager->endCallbacks.find(channel) != audioManager->endCallbacks.end()) {
                    audioManager->endCallbacks[channel]();
                    audioManager->endCallbacks.erase(channel);
                }
            }
            return FMOD_OK;
        }
    
    // plays a sound with various options
    void playSound(const std::string& soundName, float volume, float loopStartSecond = -1, float speed = 1, std::function<void()> onSoundEnd = nullptr) {
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

        // Retrieve the default frequency of the sound
        float defaultFrequency;
        result = channel->getFrequency(&defaultFrequency);
        checkFmodError(result);

        // Set playback speed by modifying the frequency
        result = channel->setFrequency(defaultFrequency * speed);
        checkFmodError(result);

        // Store the channel in the map
        channelMap[soundName].push_back(channel);

        // Attach the AudioManager instance to the channel's user data
        result = channel->setUserData(this);
        checkFmodError(result);

        // Attach a callback to remove the channel from channelMap when sound finishes
        if (loopStartSecond < 0 || onSoundEnd != nullptr) {
            result = channel->setCallback(channelEndCallback);
            checkFmodError(result);

            if (onSoundEnd) {
                endCallbacks[channel] = onSoundEnd;
            }
        }
    }
    
    // Stop a sound
    void stopSound(const std::string& soundName) {
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
    
    // Check for FMOD errors
    void checkFmodError(FMOD_RESULT result) {
        if (result != FMOD_OK) {
            std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
            exit(-1);
        }
    }
    
    void onCoinCollected() {
        playSound("COIN_SFX", 1.0f);
    }
    
    void onCountdown(int countdownValue) {
        if (countdownValue == 7) {
            playSound("INTRO_SFX", 0.15f);
        } else if(countdownValue > 1 && countdownValue < 5){
            playSound("COUNTDOWN_SFX", 0.15f);
        } else if (countdownValue == 1){
            playSound("START_SFX", 0.15f);
            playSound("RACE_MUSIC", 0.2f, 7);
            
        }
    };
    
    void onLapChanged(int currentLap) {
        if (currentLap == 2) {
            stopSound("RACE_MUSIC");
            playSound("FINAL_SFX", 0.5f, -1, 1.0f, [this]() {
                playSound("RACE_MUSIC", 0.2f, 7, 1.25f);
            });
        } else if (currentLap == 0) {
            stopSound("RACE_MUSIC");
            playSound("END_SFX", 0.5f);
        }
    }

public:
    
    // Initialize the audio system
    void init() override {
        
        result = FMOD::System_Create(&audio_system);
        checkFmodError(result);
        result = audio_system->init(1024, FMOD_INIT_NORMAL, 0);  // Increased number of channels
        checkFmodError(result);

        for (const auto& music : audioData) {
            std::string name = music["name"];
            std::string path = music["path"];

            FMOD::Sound* sound;
            result = audio_system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);
            checkFmodError(result);

            soundMap[name] = sound;
        }
    }

    // Update the audio system
    void update() override {
        result = audio_system->update();
        checkFmodError(result);
    }

    // Clean up the audio system
    void cleanup() override {
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
    
    void onSignal(std::string id, std::any data) override {
        if (id == COINS_SIGNAL) {
            onCoinCollected();
        } else if (id == COUNTDOWN_SIGNAL) {
            onCountdown(std::any_cast<int>(data));
        } else if (id == LAPS_SIGNAL) {
            onLapChanged(std::any_cast<int>(data));
        }
        else {
            std::cerr << "Unknown signal type: " << id << std::endl;
        }
    }
    
};

#endif
