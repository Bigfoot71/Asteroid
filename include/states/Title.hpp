#ifndef ASTEROID_STATE_TITLE_HPP
#define ASTEROID_STATE_TITLE_HPP

#include <rayflex.hpp>
#include "../ui/Button.hpp"
#include "../components/MenuBackground.hpp"

namespace Asteroid {

    class Title : public rf::core::State
    {
      private:
        raylib::Texture *tilesheet;
        raylib::Texture *texIcons;
        raylib::Font *font;

      private:
        ui::Button btnGame;
        ui::Button btnShop;
        ui::Button btnCredit;
        ui::Button btnSound;
        ui::Button btnFullscreen;

      private:
        MenuBackground background;
        int highScore;
        bool enter;
        bool exit;

      public:
        void Enter() override;
        void Exit() override;

        void Update(float dt) override;
        void Draw(const rf::core::Renderer&) override;
    };

}

#endif //ASTEROID_STATE_TITLE_HPP
