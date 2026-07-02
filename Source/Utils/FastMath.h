#pragma once

namespace fast_math
{

[[nodiscard]] inline float sqrt(float x) noexcept
{
    if (x <= 0.0f)
        return 0.0f;
    float result = x;
    for (int i = 0; i < 8; ++i)
        result = 0.5f * (result + x / result);
    return result;
}

[[nodiscard]] inline float atan(float z) noexcept
{
    const float z2 = z * z;
    return z * (1.0f - z2 * (0.333333f - z2 * 0.2f));
}

[[nodiscard]] inline float atan2(float y, float x) noexcept
{
    constexpr float kPi = 3.14159265f;
    if (x == 0.0f) {
        if (y > 0.0f)
            return kPi / 2.0f;
        if (y < 0.0f)
            return -kPi / 2.0f;
        return 0.0f;
    }
    const float a = atan(y / x);
    if (x > 0.0f)
        return a;
    if (y >= 0.0f)
        return a + kPi;
    return a - kPi;
}

}
