#pragma once

struct TriggerbotState {
    bool waitingToFire = false;
    bool justFired = false;
    float fireTime = 0.f;
    int holdFramesLeft = 0;
};
