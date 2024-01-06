#ifndef ASTEROID_STATE_SHOP_HPP
#define ASTEROID_STATE_SHOP_HPP

#include <rayflex.hpp>

#include "../components/MenuBackground.hpp"
#include "../ui/Button.hpp"
#include "../settings.hpp"
#include "../save.hpp"

namespace Asteroid {

    class Shop : public rf::core::State
    {
      private:
        static constexpr Rectangle playerFrame = {
            (BaseWidth - 128) / 2.0f,
            (BaseHeight - 128) / 2.5f,
            128, 128
        };

        static constexpr Vector2 playerPosition = {
            playerFrame.x + playerFrame.width * 0.5f,
            playerFrame.y + playerFrame.height * 0.5f
        };

      private:
        struct StatButton
        {
            ui::Button button{};
            raylib::Text labels[2];

            bool Update(const Vector2& mousePosition)
            {
                return button.Update(mousePosition);
            }

            void Draw(const raylib::Texture* texIcons, const Rectangle& srcIcon, float alpha)
            {
                button.Draw(*texIcons, srcIcon, alpha);
                const auto &label = labels[button.HasFocus() ? 1 : 0];
                label.Draw(button.GetPosition() + Vector2{ 0, button.GetSize().y * 0.85f }, 0, label.MeasureEx() * 0.5f);
            }
        };

      private:
        raylib::Texture *tilesheet;
        raylib::Texture *texIcons;
        raylib::Font *font;
        Save *save;

      private:
        raylib::Text txtMoney;
        raylib::Text txtShipPrice;

      private:
        ui::Button btnAction;           // Title, select or buy
        ui::Button btnSound;
        ui::Button btnFullscreen;
        ui::Button btnArrowLeft;
        ui::Button btnArrowRight;

      private:
        friend struct SB; struct SB
        {
            StatButton acceleration{};
            StatButton maxVelEngine{};
            StatButton bulletPerSec{};
            StatButton damage{};
            StatButton regen{};
            StatButton life{};

            void Update(Shop* shop);
            void Draw(const Shop* shop, float alpha);

            void UpdateLabels(const Shop* shop);
            void SetColor(const Color& color);
            void SetBounds();

            StatButton* begin() { return &acceleration; }
            StatButton* end() { return begin() + 6; }
        }
        stats{};

      private:
        MenuBackground background;

      private:
        int playerSelected;
        float playerRotation;
        float playerPitch;

      private:
        float phaseSelectTransition;
        int previousPlayerSelected;
        bool onSelectTransition;

      private:
        void UpdateLabels();
        void SetAlphaLabels(float alpha);

      public:
        void Enter() override;
        void Update(float dt) override;
        void Draw(const rf::core::Renderer&) override;
    };

    constexpr uint16_t shipsPrice[PlayerCount] = {
        0, 500, 1000,
        1500, 2000, 2500,
        3000, 3500, 4000,
    };

    constexpr PlayerStats statsInc = {
        .acceleration = 5,
        .maxVelEngine = 10,
        .bulletPerSec = 1,
        .damage = 5,
        .regen = 5,
        .life = 10
    };

}

#endif //ASTEROID_STATE_SHOP_HPP
