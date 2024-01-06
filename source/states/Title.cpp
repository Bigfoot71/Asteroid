#include "states/Title.hpp"
#include "settings.hpp"
#include "save.hpp"
#include "utils.hpp"

using namespace Asteroid;

void Title::Enter()
{
    app->SetCursor(app->assetManager.Get<raylib::Texture>("icons"),
        {}, 0.25f, { 0, 0, 128, 128 },
        { 255, 255, 255, 191 });

    tilesheet = app->assetManager.Get<raylib::Texture>("tilesheet");
    texIcons = app->assetManager.Get<raylib::Texture>("icons");
    font = app->assetManager.Get<raylib::Font>("font");

    btnSound.SetBounds({ 37, 37 }, { 42, 42 });
    btnFullscreen.SetBounds({ BaseWidth - 37, 37 }, { 42, 42 });

    btnGame.SetBounds({ BaseWidth * 0.5f, BaseHeight * 0.625f }, { 142, 42 });
    btnShop.SetBounds({ BaseWidth * 0.5f, BaseHeight * 0.75f }, { 142, 42 });
    btnCredit.SetBounds({ BaseWidth * 0.5f, BaseHeight * 0.875f }, { 142, 42 });

    highScore = app->saveManager->Get<Save>()->hiscore;

    enter = true;
    exit = false;
}

void Title::Exit()
{
    if (app->HasState("game-state-loading"))
    {
        app->FreeState("game-state-loading");
    }
}

void Title::Update(float dt)
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

    else if (btnGame.Update(app->GetMousePosition()))
    {
        app->Transition("game", 1.0f, app->assetManager.Get<raylib::Shader>("shader transition game"));
        app->assetManager.Get<raylib::Sound>("click")->Play();
    }

    else if (btnShop.Update(app->GetMousePosition()))
    {
        app->Transition("shop", 0.5f, app->assetManager.Get<raylib::Shader>("shader transition menu"));
        app->assetManager.Get<raylib::Sound>("click")->Play();
    }

    else if (btnCredit.Update(app->GetMousePosition()))
    {
        app->Transition("credit", 0.5f, app->assetManager.Get<raylib::Shader>("shader transition menu"));
        app->assetManager.Get<raylib::Sound>("click")->Play();
    }

    if (enter)
    {
        app->musicManager.SetVolume(utils::SmoothToTarget(app->musicManager.GetVolume(), DefaultMusicVolume, 0.375f, dt));
        if (!app->OnTransition()) app->musicManager.SetVolume(DefaultMusicVolume), enter = false;
    }
}

void Title::Draw(const rf::core::Renderer& target)
{
    target.Clear(BLACK);

    background.Draw();

    tilesheet->Draw(Rectangle{ 0, 384, 512, 128 }, raylib::Rectangle{ app->GetResolution() * Vector2{ 0.5f, 0.2f }, { 512, 128 } }, { 256, 64 }, 0, WHITE);

    if (highScore > 0)
    {
        raylib::Text text(TextFormat("High-Score: %i", highScore), 20+std::sin(4*GetTime())*0.75f, WHITE, *font, 1);
        text.Draw({ 400, 186 }, 0, text.MeasureEx() * 0.5f);
    }

    btnSound.Draw(*texIcons, { 128, app->saveManager->Get<Save>()->volume ? 128.f : 256.f, 128, 128 });
    btnFullscreen.Draw(*texIcons, { 0, app->window.IsFullscreen() ? 256.f : 128.f, 128, 128 });

    btnGame.Draw("Play", font);
    btnShop.Draw("Shop", font);
    btnCredit.Draw("Credit", font);
}
