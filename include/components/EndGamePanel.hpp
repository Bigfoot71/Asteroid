#ifndef ASTEROID_COMPONENTS_END_GAME_PANEL
#define ASTEROID_COMPONENTS_END_GAME_PANEL

#include <rayflex.hpp>

#include "../entities/Player.hpp"
#include "../ui/Button.hpp"
#include "../settings.hpp"
#include "../utils.hpp"

namespace Asteroid {

    class EndGamePanel
    {
      private:
        static constexpr Rectangle bounds = {
            (BaseWidth - 400) * 0.5f,
            (BaseHeight - 300) * 0.5f,
            400, 300
        };

      private:
        ui::Button          btnRetry{};
        ui::Button          btnTitle{};
        rf::core::App       *app;
        const raylib::Font  *font;
        const Player        &player;
        float               phaseAnim;
        uint32_t            displayedMoney;
        uint32_t            displayedKills;
        uint32_t            displayedTime;
        uint32_t            earnedCoins;
        bool                active;

      private:
        void Enable();

      public:
        EndGamePanel(const Player& player)
        : player(player)
        { }

        void Init(rf::core::App* app);
        void Update(class Game* game, float endGamePhase, float dt);
        void Draw(float endGamePhase);
    };

}

#endif //ASTEROID_COMPONENTS_END_GAME_PANEL
