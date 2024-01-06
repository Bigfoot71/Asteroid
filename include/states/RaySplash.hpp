#ifndef ASTEROID_STATE_RAY_SPLASH_HPP
#define ASTEROID_STATE_RAY_SPLASH_HPP

#include <rayflex.hpp>

namespace Asteroid {

    class RaySplash : public rf::core::State
    {
      private:
        raylib::Vector2 logoPosition;

        int framesCounter;
        int lettersCount;

        int topSideRecWidth;
        int leftSideRecHeight;
        int bottomSideRecWidth;
        int rightSideRecHeight;

        int state = 0;                  // Tracking animation states (State Machine)
        float alpha = 1.0f;             // Useful for fading
        float alphaMask = 1.0f;

      public:
        void Enter() override;
        void Update(float dt) override;
        void Draw(const rf::core::Renderer&) override;
    };

}

#endif //ASTEROID_STATE_RAY_SPLASH_HPP
