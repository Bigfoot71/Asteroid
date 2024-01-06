#ifndef ASTEROID_COMPONENT_RADAR_EFFECT_HPP
#define ASTEROID_COMPONENT_RADAR_EFFECT_HPP

#include <rayflex.hpp>

/**
  NOTE: This class was intended to be used for an enemy radar in the game, but it
        proved to be more cumbersome than helpful. Therefore, the class has been
        "recycled" to add a focus effect to buttons.
  
  TODO: It might be wise in the future to optimize the shader for the button effect
        to perform it in a single pass with a double ray.
 */

namespace Asteroid {

    class RadarEffect
    {
      private:
        static uint16_t Counter;
        static ::Shader Shader;
        static int LocStrength;
        static int LocCenter;
        static int LocRadius;
        static int LocSpeed;
        static int LocDrag;
        static int LocTime;

      public:
        Vector2 position{};
        float strength = 0;
        float radius = 0;
        float speed = 0;
        float drag = 0;

      public:
        RadarEffect();
        ~RadarEffect();

        void SendToShader();

        void BeginMode(float time, bool sendToShader = true);
        void EndMode();
    };

}

#endif //ASTEROID_COMPONENT_RADAR_EFFECT_HPP
