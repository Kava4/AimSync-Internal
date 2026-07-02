#pragma once

#include <CS2/Classes/QAngle.h>
#include <CS2/Classes/Vector.h>
#include <Utils/FastMath.h>

namespace combat_math
{

constexpr float kPi = 3.14159265f;
constexpr float kRadToDeg = 180.0f / kPi;

[[nodiscard]] inline float angleDiff(float a, float b) noexcept
{
    float delta = a - b;
    while (delta > 180.0f)
        delta -= 360.0f;
    while (delta < -180.0f)
        delta += 360.0f;
    return delta;
}

[[nodiscard]] inline cs2::QAngle calcAngle(const cs2::Vector& source, const cs2::Vector& destination) noexcept
{
    const cs2::Vector delta{destination.x - source.x, destination.y - source.y, destination.z - source.z};
    const float hyp = fast_math::sqrt(delta.x * delta.x + delta.y * delta.y);
    return cs2::QAngle{
        .pitch = fast_math::atan2(-delta.z, hyp) * kRadToDeg,
        .yaw = fast_math::atan2(delta.y, delta.x) * kRadToDeg,
        .roll = 0.0f
    };
}

[[nodiscard]] inline float fovDistance(const cs2::QAngle& current, const cs2::QAngle& target) noexcept
{
    const float dPitch = angleDiff(target.pitch, current.pitch);
    const float dYaw = angleDiff(target.yaw, current.yaw);
    return fast_math::sqrt(dPitch * dPitch + dYaw * dYaw);
}

[[nodiscard]] inline cs2::QAngle normalizeAngles(cs2::QAngle angles) noexcept
{
    while (angles.yaw > 180.0f)
        angles.yaw -= 360.0f;
    while (angles.yaw < -180.0f)
        angles.yaw += 360.0f;
    if (angles.pitch > 89.0f)
        angles.pitch = 89.0f;
    if (angles.pitch < -89.0f)
        angles.pitch = -89.0f;
    angles.roll = 0.0f;
    return angles;
}

[[nodiscard]] inline cs2::QAngle smoothAngles(const cs2::QAngle& current, const cs2::QAngle& target, float smoothness) noexcept
{
    if (smoothness < 1.0f)
        smoothness = 1.0f;
    return normalizeAngles(cs2::QAngle{
        .pitch = current.pitch + angleDiff(target.pitch, current.pitch) / smoothness,
        .yaw = current.yaw + angleDiff(target.yaw, current.yaw) / smoothness,
        .roll = 0.0f
    });
}

}
