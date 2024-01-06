#ifndef ASTEROID_STATE_GAME_HPP
#define ASTEROID_STATE_GAME_HPP

#include <rayflex.hpp>

#include <mutex>
#include <condition_variable>

#include "../entities/Ore.hpp"
#include "../entities/Enemy.hpp"
#include "../entities/Player.hpp"
#include "../entities/Bullet.hpp"
#include "../entities/Asteroid.hpp"
#include "../components/EndGamePanel.hpp"
#include "../components/GameBackground.hpp"
#include "../components/GameInstructions.hpp"

namespace Asteroid {

    class Game : public rf::core::State
    {
      private:
        Ores                        ores;
        Bullets<MaxBullets>         bullets;
        rf::gfx2d::ParticleSystem   psys;
        rf::core::RandomGenerator   gen;
        Enemies                     enemies;
        Asteroids                   asteroids;
        Player                      player;
        EndGamePanel                endPanel;
        GameInstructions            instructions;
        std::condition_variable     updateCV;
        std::mutex                  updateMutex;
        std::thread                 updateThread;
        bool                        isUpdateRequested;
        bool                        shouldExitThread;
        const raylib::Texture       *tilesheet;
        const raylib::Texture       *gradient;
        float                       endPhase;
        GameBackground              background;
        bool                        showInstructions;
        bool                        enter;

      public:
        Game();

        void Enter() override;
        void Exit() override;

        void Update(float dt) override;
        void Draw(const rf::core::Renderer&) override;
    };

}

#endif //ASTEROID_STATE_GAME_HPP
