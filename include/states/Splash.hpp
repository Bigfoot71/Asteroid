#ifndef ASTEROID_STATE_SPLASH_HPP
#define ASTEROID_STATE_SPLASH_HPP

#include <rayflex.hpp>
#include <cstdint>

#include "../components/GameBackground.hpp"
#include "../components/SplashEntity.hpp"
#include "../components/SplashTitle.hpp"
#include "settings.hpp"
#include "utils.hpp"

namespace Asteroid {

    class Splash : public rf::core::State
    {
      private:
        const raylib::Texture *tilesheet;
        const raylib::Font * font;

      private:
        rf::gfx2d::ParticleSystem psys;
        GameBackground background;
        raylib::Vector2 bgPos;
        float bgSpeed;

      private:
        SplashTitle title;
        SplashEntities entities;

      private:
        float phaseFlash;       // Phase d'opacité du flash ( -1 -> 1 )
        float phaseTitle17;
        float phaseTitle18;
        float phaseSubTitle18;
        uint8_t state;
        bool exit;

      public:
        Splash() : psys(192)
        {
            psys.SetVelocity({ -256, -256 }, { 256, 256 });
        }

        void Enter() override;
        void Update(float dt) override;
        void Draw(const rf::core::Renderer&) override;
    };

}

#endif //ASTEROID_STATE_SPLASH_HPP
