#ifndef ASTEROID_GUI_BUTTON_HPP
#define ASTEROID_GUI_BUTTON_HPP

#include <rayflex.hpp>

#include "../components/RadarEffect.hpp"
#include "../utils.hpp"

namespace ui {

    class Button
    {
      private:
        Asteroid::RadarEffect focusEffect{};
        raylib::Rectangle bounds{};
        bool isPressed = false;
        bool focus = false;

      public:
        raylib::Vector2 GetPosition() const { return bounds.GetPosition() + bounds.GetSize() * 0.5f; }
        raylib::Vector2 GetSize() const { return bounds.GetSize(); }
        bool HasFocus() const { return focus; }

        void SetBounds(const raylib::Vector2& center, const raylib::Vector2& size);

        bool Update(const Vector2& mousePosition);

        void Draw(float alpha = 1.0f);
        void Draw(const std::string& text, const Font* font = nullptr, float alpha = 1.0f);
        void Draw(const raylib::Texture& texture, const raylib::Rectangle& source, float alpha = 1.0f, bool drawFrame = true);
    };

}

#endif //ASTEROID_GUI_BUTTON_HPP
