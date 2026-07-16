#pragma once

#include <CS2/Classes/Vector.h>

struct Trajectory {
    int pointsCount{0};
    cs2::Vector points[500];
    int bouncesCount{0};
    cs2::Vector bounces[20];
    cs2::Vector endPos;
    bool valid;
    bool validLanding{true};
};
