#ifndef ASTEROID_STATE_CREDIT_HPP
#define ASTEROID_STATE_CREDIT_HPP

#include <rayflex.hpp>
#include <cstring>

#include "../components/MenuBackground.hpp"
#include "../ui/Button.hpp"
#include "../settings.hpp"

namespace _ {

#ifdef PLATFORM_WEB

    // emscripten's std::array implementation does not
    // seem to allow array initialization with constexpr

    template <typename T, std::size_t N>
    class array
    {
      private:
        T arr[N]{};

      public:
        constexpr array() = default;

        template <typename... Args>
        constexpr array(Args... args) : arr{args...} {}

        constexpr std::size_t size() const noexcept { return N; }

        constexpr T& operator[](std::size_t i) { return arr[i]; }
        constexpr const T& operator[](std::size_t i) const { return arr[i]; }
    };

#else

    template <typename T, std::size_t N>
    using array = std::array<T, N>;

#endif

}

namespace Asteroid {

    class Credit : public rf::core::State
    {
      private:
        struct CreditLine
        {
            const char title[32]{};
            const char content[32]{};

            void Draw(raylib::Vector2 position, const Font& font, float fontSize, float spacing, const Color& c1, const Color& c2) const
            {
                raylib::Vector2 sizeTitle = MeasureTextEx(font, title, fontSize, spacing);
                raylib::Vector2 sizeTotal = sizeTitle + MeasureTextEx(font, content, fontSize, spacing);

                position -= (sizeTotal * 0.5f);
                ::DrawTextEx(font, title, position, fontSize, spacing, c1);

                position.x += sizeTitle.x;
                ::DrawTextEx(font, content, position, fontSize, spacing, c2);
            }
        };

      private:
        static constexpr _::array<CreditLine, 11> credits = {
            CreditLine{ "Programming: ", "Bigfoot71" },
            CreditLine{ "Musics: ", "Drozerix" },
            CreditLine{ "Textures: ", "Kenney" },
            CreditLine{ "Font: ", "GGBotNet" },
            CreditLine{ "Icons: ", "Kenney" },
            CreditLine{ "Icons: ", "Konstantin Filatov" },
            CreditLine{ "Icons: ", "Solar Icons" },
            CreditLine{ "Icons: ", "thewolfkit" },
            CreditLine{ "Icons: ", "Bigfoot71" },
            CreditLine{ "Sounds: ", "Muncheybobo" },
            CreditLine{ "Sounds: ", "Kenney" }
        };

        static constexpr Rectangle bounds = {
            (BaseWidth - 494) / 2.f,
            (BaseHeight - 225) * .575f,
            494, 225
        };

        static constexpr float fontSize = 18;
        static constexpr float creditScrollBaseVelY = 2 * fontSize;
        static constexpr int spaceTextHeight = static_cast<int>(fontSize * 0.65f + 0.5f);
        static constexpr int totalTextHeight = credits.size() * (fontSize + spaceTextHeight);

      private:
        raylib::Texture *tilesheet;
        raylib::Texture *texIcons;
        raylib::Font *font;

      private:
        ui::Button btnSound;
        ui::Button btnFullscreen;
        ui::Button btnTitle;

      private:
        float nameHue;
        float creditScale;
        float creditScrollY;
        float creditScrollVelY;
        MenuBackground background;
        bool clickFocusCredits;
        bool enter, exit;

      public:
        void Enter() override;
        void Update(float dt) override;
        void Draw(const rf::core::Renderer&) override;
    };

}

#endif //ASTEROID_STATE_CREDIT_HPP
