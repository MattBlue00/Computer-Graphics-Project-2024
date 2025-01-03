#ifndef SIGNALS_HPP
#define SIGNALS_HPP

#include "../modules/engine/pattern/Signal.hpp"
#include "SignalTypes.hpp"

Signal brakeSignal = Signal(BRAKE_SIGNAL);
Signal changeCameraSignal = Signal(CHANGE_CAMERA_SIGNAL);
Signal coinCollectedSignal = Signal(COIN_COLLECTED_SIGNAL);
Signal coinsSignal = Signal(COINS_SIGNAL);
Signal countdownSignal = Signal(COUNTDOWN_SIGNAL);
Signal headlightsChangeSignal = Signal(HEADLIGHTS_CHANGE_SIGNAL);
Signal lapsSignal = Signal(LAPS_SIGNAL);
Signal quitSignal = Signal(QUIT_SIGNAL);
Signal rebuildPipelineSignal = Signal(REBUILD_PIPELINE_SIGNAL);
Signal resetViewSignal = Signal(RESET_VIEW_SIGNAL);
Signal reverseSignal = Signal(REVERSE_SIGNAL);
Signal scoreSignal = Signal(SCORE_SIGNAL);
Signal speedSignal = Signal(SPEED_SIGNAL);
Signal timeSignal = Signal(TIME_SIGNAL);
Signal updateDebounceSignal = Signal(UPDATE_DEBOUNCE_SIGNAL);
Signal updateNextCheckpointSignal = Signal(UPDATE_NEXT_CHECKPOINT_SIGNAL);

#endif
