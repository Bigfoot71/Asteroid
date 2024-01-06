#include "states/Credit.hpp"
#include "settings.hpp"
#include "save.hpp"
#include <limits>

using namespace Asteroid;

/* CREDIT STATE */

void Credit::Enter()
{
    tilesheet = app->assetManager.Get<raylib::Texture>("tilesheet");
    texIcons = app->assetManager.Get<raylib::Texture>("icons");
    font = app->assetManager.Get<raylib::Font>("font");

    btnSound.SetBounds({ 37, 37 }, { 42, 42 });
    btnFullscreen.SetBounds({ BaseWidth - 37, 37 }, { 42, 42 });
    btnTitle.SetBounds({ BaseWidth * 0.5f, BaseHeight * 0.875f }, { 142, 42 });

    nameHue = GetRandomValue(0, 360);
    creditScrollVelY = creditScrollBaseVelY;
    creditScrollY = bounds.y;
    creditScale = 0.0f;
    enter = true;
    exit = false;
}

void Credit::Update(float dt)
{
    background.Update();

    if (btnFullscreen.Update(app->GetMousePosition()))
    {
        app->assetManager.Get<raylib::Sound>("click")->Play();
        app->ToggleFullscreen();
    }

    else if (btnSound.Update(app->GetMousePosition()))
    {
        app->assetManager.Get<raylib::Sound>("click")->Play();
        ToggleVolume(app);
    }

    else if (btnTitle.Update(app->GetMousePosition()))
    {
        app->Transition("title", 0.5f, app->assetManager.Get<raylib::Shader>("shader transition menu"));
        app->assetManager.Get<raylib::Sound>("click")->Play();
        exit = true;
    }

    /* UPADTE CREDIT SCROLL */

    if (clickFocusCredits)
    {
        clickFocusCredits = !IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        creditScrollVelY = -60 * GetMouseDelta().y;
    }
    else
    {
        clickFocusCredits = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && app->GetMousePosition().CheckCollision(bounds);

        float diff = creditScrollBaseVelY - creditScrollVelY;
        float rate = diff / 0.33f;                                                      ///< (diff/seconds) Calculate rate of change per second to reach creditScrollBaseVelY in X seconds

        creditScrollVelY += rate * dt;                                                  ///< Update vel based on rate of change and elapsed time (dt)
        creditScrollVelY = Lerp(creditScrollVelY, creditScrollBaseVelY, dt / 0.33f);    ///< Use linear interpolation to make the transition smoother (recheck that)
    }

    creditScrollY -= creditScrollVelY * dt;

    if (creditScrollY + totalTextHeight < bounds.y)
    {
        creditScrollY += totalTextHeight;
    }
    else if (creditScrollY > bounds.y)
    {
        creditScrollY -= totalTextHeight;
    }

    /* MISC UPDATES */

    nameHue += 180 * dt;

    if (enter)
    {
        creditScale += 2 * dt;
        if (creditScale >= 1.0f) enter = false, creditScale = 1.0f;
    }
    else if (exit)
    {
        creditScale -= 2 * dt;
    }
    
}

void Credit::Draw(const rf::core::Renderer& target)
{
    target.Clear(BLACK);

    background.Draw();
    tilesheet->Draw(Rectangle{ 0, 384, 512, 128 }, raylib::Rectangle{ app->GetResolution() * Vector2{ 0.5f, 0.2f }, { 512, 128 } }, { 256, 64 }, 0, WHITE);

    btnSound.Draw(*texIcons, { 128, app->saveManager->Get<Save>()->volume ? 128.f : 256.f, 128, 128 });
    btnFullscreen.Draw(*texIcons, { 0, app->window.IsFullscreen() ? 256.f : 128.f, 128, 128 });

    rlPushMatrix();

        float s = creditScale * creditScale * (3.0f - 2.0f * creditScale);
        float is = (1.0f - s) * 0.5f;

        rlTranslatef(BaseWidth/2.f, BaseHeight/2.f, 0);
        rlScalef(s, s, 1.0f);
        rlTranslatef(-BaseWidth/2.f, -BaseHeight/2.f, 0);

        DrawRectangleRounded(bounds, 0.25f, 9, { 0, 0, 0, 91 });
        DrawRectangleRoundedLines(bounds, 0.25f, 9, 3, WHITE);

        BeginScissorMode(bounds.x + bounds.width * is, bounds.y + bounds.height * is, bounds.width * s, bounds.height * s);

            for (uint8_t j = 0; j < 2; j++)
            {
                for (uint8_t i = 0; i < credits.size(); i++)
                {
                    Color nameTint = ColorFromHSV(nameHue + i * 45, 1, 1);
                    credits[i].Draw({ BaseWidth / 2.f, (creditScrollY + i * (fontSize + spaceTextHeight)) + j * totalTextHeight }, *font, fontSize, 1, WHITE, nameTint);
                }
            }

        EndScissorMode();

    rlPopMatrix();

    btnTitle.Draw("Title", font);
}
