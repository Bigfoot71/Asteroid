#ifndef ASTEROID_COMPONENT_GAME_INSTRUCTIONS_HPP
#define ASTEROID_COMPONENT_GAME_INSTRUCTIONS_HPP

#include <rayflex.hpp>
#include "./TextBox.hpp"
#include "../settings.hpp"
#include "raylib.h"

namespace Asteroid {

    class GameInstructions
    {
      private:
        static constexpr char texts[13][192] = {
            "Welcome to the world of Asteroid!",
            "Allow me to explain the situation to you.",
            "In this world, humanity has reached the technology necessary to send asteroid mining expeditions.",
            "However, extraterrestrial civilizations, who remained discreet as long as humanity stayed in its corner, have decided to take action.",
            "They certainly won't let newcomers easily enter the mining sector...",
            "So, you'll have to do your best to gather as much ore as possible without being shot down!",
            "The collected ore will be bought from you and will be used to upgrade your ship or even buy better ones.",
            "The one you currently have is an old wreck. The company you work for wouldn't risk entrusting a Millennium Falcon to a newbie like you!",
            "So, it is highly recommended to upgrade your ship as soon as possible with the money you've earned.",
            "I almost forgot, you'll be entitled to an additional risk bonus for the time spent in space.",
            "And of course, if you shoot down some of those damn aliens, you'll also get a bonus of 10 bucks per alien ship shot down.",
            "You might also be able to recover some ore they might have collected before attacking you.",
            "Well, good luck to you, comrade!"
        };

        static constexpr Rectangle boundsText = { (BaseWidth-400)/2.f, (BaseHeight-225)/2.f, 400, 225 };
        static constexpr Rectangle boundsControls = { (BaseWidth-416)/2.f, (BaseHeight-416)/2.f, 416, 416 };

      private:
        raylib::Texture *controls;
        raylib::Font *font;
        TextBox textBox;
        float timeAnim;
        float timeEnd;
        uint8_t state;

      public:
        void Init(rf::core::App *app)
        {
            state = 0;
            timeAnim = 0.0f, timeEnd = 1.0f;
            font = app->assetManager.Get<raylib::Font>("font");
            controls = app->assetManager.Get<raylib::Texture>("controls");
            textBox.Init(boundsText, texts[state], font, 24, 1);
        }

        bool Update(float dt)
        {
            if (state < 13)
            {
                textBox.Update(dt);

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                {
                    if (textBox.IsFinish() && (++state) < 13)
                    {
                        textBox.SetText(texts[state], {}, font, state == 12 ? 24 : 18, 1);
                    }
                    else
                    {
                        textBox.Finish();
                    }
                }
            }
            else if (state == 13)
            {
                if (timeAnim == 1.0f)
                {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    {
                        state++;
                    }
                }
                else
                {
                    timeAnim = std::min(timeAnim + 2.0f * dt, 1.0f);
                }
            }
            else
            {
                timeEnd -= 4.0f * dt;
            }

            return state < 13 || timeEnd > 0.0f;
        }

        void Draw()
        {
            DrawRectangle(0, 0, BaseWidth, BaseHeight, { 0, 0, 0, static_cast<uint8_t>(63*timeEnd) });

            if (state < 13)
            {
                textBox.DrawCentered();
            }
            else if (state == 13)
            {
                float v = timeAnim * timeAnim * (3.0f - 2.0f * timeAnim);

                Rectangle bounds = {
                    boundsText.x + (boundsControls.x - boundsText.x) * v,
                    boundsText.y + (boundsControls.y - boundsText.y) * v,
                    boundsText.width + (boundsControls.width - boundsText.width) * v,
                    boundsText.height + (boundsControls.height - boundsText.height) * v
                };

                DrawRectangleRounded(bounds, 0.25f, 9, { 0, 0, 0, 95 });
                DrawRectangleRoundedLines(bounds, 0.25f, 9, 3, WHITE);
                controls->Draw(Rectangle{ 0, 0, 512, 512 }, { 400, 225, 512*v, 512*v }, { 256*v, 256*v }, 0, ColorAlpha(WHITE, v));
            }
            else
            {
                DrawRectangleRounded(boundsControls, 0.25f, 9, { 0, 0, 0, static_cast<uint8_t>(95*timeEnd) });
                DrawRectangleRoundedLines(boundsControls, 0.25f, 9, 3, ColorAlpha(WHITE, timeEnd));
                controls->Draw(Rectangle{ 0, 0, 512, 512 }, { 400, 225, 512, 512 }, { 256, 256 }, 0, ColorAlpha(WHITE, timeEnd));
            }
        }
    };

}

#endif //ASTEROID_COMPONENT_GAME_INSTRUCTIONS_HPP