#include "states/RaySplash.hpp"
#include "settings.hpp"

void Asteroid::RaySplash::Enter()
{
    logoPosition = app->GetResolution() * 0.5f - raylib::Vector2{ 128, 128 };

    framesCounter = 0;
    lettersCount = 0;

    topSideRecWidth = 16;
    leftSideRecHeight = 16;
    bottomSideRecWidth = 16;
    rightSideRecHeight = 16;

    state = 0;
    alpha = 1.0f;
}

void Asteroid::RaySplash::Update(float dt)
{
    if (alphaMask > 0.0f)
    {
        alphaMask -= 4 * dt;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        state = 3;
        lettersCount = 10;
        topSideRecWidth = 256;
        leftSideRecHeight = 256;
        bottomSideRecWidth = 256;
        rightSideRecHeight = 256;
    }

    switch (state)
    {
        case 0: // Small box blinking
        {
            framesCounter++;
            if (framesCounter == 120)
            {
                framesCounter = 0;      // Reset counter... will be used later...
                state++;
            }
        }
        break;

        case 1: // Top and left bars growing
        {
            topSideRecWidth += 4;
            leftSideRecHeight += 4;
            if (topSideRecWidth == 256) state++;
        }
        break;

        case 2: // Bottom and right bars growing
        {
            bottomSideRecWidth += 4;
            rightSideRecHeight += 4;
            if (bottomSideRecWidth == 256) state++;
        }
        break;

        case 3: // Letters appearing (one by one)
        {
            framesCounter++;
            if (framesCounter/12)       // Every 12 frames, one more letter!
            {
                lettersCount++;
                framesCounter = 0;
            }

            if (lettersCount >= 10)     // When all letters have appeared, just fade out everything
            {
                alpha -= 0.02f;

                if (alpha <= 0.0f)
                {
                    alpha = 0.0f;
                    state++;
                }
            }
        }
        break;

        case 4:
        {
            app->Transition("splash");
            state++;
        }
        break;

        default:
            break;
    }
}

void Asteroid::RaySplash::Draw(const rf::core::Renderer& target)
{
    target.Clear(RAYWHITE);

    switch (state)
    {
        case 0: // Small box blinking
        {
            if ((framesCounter/15)%2) logoPosition.DrawRectangle({ 16, 16 }, BLACK);
        }
        break;

        case 1: // Top and left bars growing
        {
            logoPosition.DrawRectangle({ static_cast<float>(topSideRecWidth), 16 });
            logoPosition.DrawRectangle({ 16, static_cast<float>(leftSideRecHeight) });
        }
        break;

        case 2: // Bottom and right bars growing
        {
            logoPosition.DrawRectangle({ static_cast<float>(topSideRecWidth), 16 });
            logoPosition.DrawRectangle({ 16, static_cast<float>(leftSideRecHeight) });

            DrawRectangle(logoPosition.x + 240, logoPosition.y, 16, rightSideRecHeight, BLACK);
            DrawRectangle(logoPosition.x, logoPosition.y + 240, bottomSideRecWidth, 16, BLACK);
        }
        break;

        default: // Letters appearing (one by one)
        {
            const Color fadeBlack = Fade(BLACK, alpha);

            logoPosition.DrawRectangle({ static_cast<float>(topSideRecWidth), 16 }, fadeBlack);
            DrawRectangle(logoPosition.x, logoPosition.y + 16, 16, leftSideRecHeight - 32, fadeBlack);

            DrawRectangle(logoPosition.x + 240, logoPosition.y + 16, 16, rightSideRecHeight - 32, fadeBlack);
            DrawRectangle(logoPosition.x, logoPosition.y + 240, bottomSideRecWidth, 16, fadeBlack);

            DrawText(TextSubtext("raylib", 0, lettersCount), BaseWidth/2 - 44, BaseHeight/2 + 48, 50, fadeBlack);
        }
        break;
    }

    if (alphaMask > 0.0f)
    {
        DrawRectangle(0, 0, BaseWidth, BaseHeight, ColorAlpha(BLACK, alphaMask));
    }
}
