#ifndef ASTEROID_COMPONENT_TRAIL_EFFECT_HPP
#define ASTEROID_COMPONENT_TRAIL_EFFECT_HPP

#include <rayflex.hpp>
#include <array>

namespace Asteroid {

    class TrailEffect
    {
      private:
        static constexpr uint8_t SplineSegDiv = 12;//24;

      public:
        std::array<Vector2, 16> points;
        uint8_t count;

      public:
        void Init() { count = 0; }
        void Update(const Vector2& position, float strength);
        void Draw(float thick, const Color& color) const;
    };

}

#endif //ASTEROID_COMPONENT_TRAIL_EFFECT_HPP
