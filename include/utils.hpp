#ifndef ASTEROID_UTILS_HPP
#define ASTEROID_UTILS_HPP

#include "Vector2.hpp"
#include "raymath.h"
#include <rayflex.hpp>

namespace utils {

    inline constexpr Rectangle ScaleRectangleFromCenter(const Rectangle& rectangle, float scale)
    {
        float sw = rectangle.width * scale;
        float sh = rectangle.height * scale;

        return {
            rectangle.x + (rectangle.width - sw) * 0.5f,
            rectangle.y + (rectangle.height - sh) * 0.5f,
            sw, sh
        };
    }

    inline constexpr float GetDeltaRad(float th0, float th1)
    {
        const float c0 = std::cos(th0), s0 = std::sin(th0);
        const float c1 = std::cos(th1), s1 = std::sin(th1);
        return std::atan2(c0*s1-c1*s0, c0*c1+s1*s0);
    }

    inline constexpr float SmoothToZero(float value, float time, float dt)
    {
        return value * std::exp(-dt / time);
    }

    inline constexpr float SmoothToTarget(float value, float targetValue, float time, float dt)
    {
        return (value - targetValue) * std::exp(-dt / time) + targetValue;
    }

    inline const char* FormatTime(float secondes)
    {
        static char str[6]; // "MM:SS\0"
        std::snprintf(str, sizeof(str), "%02d:%02d",
            static_cast<int>(secondes)/60, static_cast<int>(secondes)%60);
        return str;
    }

    inline constexpr Vector2 Project3D(const Vector3& vertex, const Vector2 position)
    {
        float scaleFactor = 1.0f / (1.0f + vertex.z / 1000.0f);
        return { position.x + vertex.x * scaleFactor, position.y + vertex.y * scaleFactor };
    }

    bool CheckCollisionLineCircle(const Vector2& a, const Vector2& b, const Vector2& c, float r);

    // Takes radians for rotation and sidePitch
    void DrawPerspective(const raylib::Texture* texture, const Rectangle& source, const Vector2& position, float rotation, float sidePitch, float scale = 1.0f, const Color& tint = WHITE);

}

#endif //ASTEROID_UTILS_HPP
