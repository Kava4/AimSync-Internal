#pragma once

struct TriggerbotState {
    bool waitingToFire{false};
    bool justFired{false};
    bool forcedAttack{false};
    int holdFramesLeft{0};
    float fireTime{0.0f};
};
