#pragma once

#include <CS2/Classes/QAngle.h>

struct NoRecoilState {
    cs2::QAngle previousPunch{};
    bool hasPreviousPunch{false};
};
