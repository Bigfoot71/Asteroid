#include "states/Game.hpp"
#include "raylib.h"
#include "settings.hpp"
#include "utils.hpp"

using namespace rf;

namespace {

    constexpr uint16_t cheat[9] {
        0x0109, 0x0109, 0x0108, 0x0108,
        0x0107, 0x0106, 0x0107, 0x0106,
        0x0101
    };

    void CheatUpdate(Asteroid::Player& player)
    {
        static uint8_t count = 0;
        uint16_t key = GetKeyPressed();

        if (key && key != cheat[count++])
        {
            count = 0;
        }
        else if (count == 9)
        {
            player.Cheat();
            count = 0;
        }
    }

}

/* GAME STATE */

Asteroid::Game::Game()
: ores(gen)
, psys(256)
, enemies(gen, psys, ores)
, asteroids(gen, psys, ores)
, player(psys)
, endPanel(player)
{ }

void Asteroid::Game::Enter()
{
    // Set game playlist
    app->musicManager.SetPlaylist("game");
    app->musicManager.SetLooping(false);
    app->musicManager.Play();

    // Get some asset pointers
    tilesheet = app->assetManager.Get<raylib::Texture>("tilesheet");
    gradient = app->assetManager.Get<raylib::Texture>("gradient");

    // Definition of misc values
    showInstructions = app->saveManager->Get<Save>()->firstLaunch;
    endPhase = 0.0f;
    enter = true;

    // Init particle system
    psys.SetVelocity({ -128, -128 }, { 128, 128 });
    psys.SetLifeTime(2.0f);
    psys.Clear();

    // Init entites
    player.Init(app);
    asteroids.Init();
    enemies.Init();
    bullets.Init();
    ores.Init();

    // Init components
    background.Update(player.GetCameraTarget());    // Send player position to the shader
    if (showInstructions) instructions.Init(app);   // Initializes the display of instructions for the first launch
    endPanel.Init(app);

    // Set default cursor state
    app->SetCursor(app->assetManager.Get<raylib::Texture>("icons"),
        { 64*0.25f, 64*0.25f }, 0.25f,
        { 128, 0, 128, 128 },
        { 255, 255, 255, 191 });

    // Definition of thread values
    isUpdateRequested = false;
    shouldExitThread = false;

    // Initializing the secondary update thread
    // NOTE: Allowed a significant FPS gain with Firefox on my PC
    // NOTE: For some reason, Firefox is much slower than Chrome (emscripten)
    // TODO: The implementation of this thread can be optimized, it will be necessary to carry out tests
    updateThread = std::thread([this]() {

        while (!shouldExitThread)
        {
            // Wait until an update is requested
            {
                std::unique_lock<std::mutex> lock(updateMutex);
                isUpdateRequested = false;
                updateCV.notify_one();

                updateCV.wait(lock, [this] { return isUpdateRequested; });
            }

            float dt = GetFrameTime();

            if (player.IsAlive())
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    app->SetCursor(app->assetManager.Get<raylib::Texture>("icons"),
                        { 64*0.25f, 64*0.25f }, 0.25f,
                        { 256, 0, 128, 128 },
                        { 255, 255, 255, 191 });
                }
                else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    app->SetCursor(app->assetManager.Get<raylib::Texture>("icons"),
                        { 64*0.25f, 64*0.25f }, 0.25f,
                        { 128, 0, 128, 128 },
                        { 255, 255, 255, 191 });
                }

                CheatUpdate(player);
            }

            psys.Update(dt);
            ores.Update(player, dt);
            bullets.Update(player, asteroids, enemies, dt);
        }

    });
}

void Asteroid::Game::Exit()
{
    // Warns the thread that it must stop
    {
        std::unique_lock<std::mutex> lock(updateMutex);
        isUpdateRequested = true;
        shouldExitThread = true;
        updateCV.notify_one();
    }

    // Wait for the thread to end if it still hasn't ended
    if (updateThread.joinable()) updateThread.join();

    // Plays menu music
    app->musicManager.SetLooping(true);
    app->musicManager.Play("Master Select");
}

void Asteroid::Game::Update(float dt)
{
    if (showInstructions)
    {
        if (!instructions.Update(dt))
        {
            showInstructions = false;
            app->saveManager->Get<Save>()->firstLaunch = false;
        }
        return;
    }

    // Notify the update thread to start its update
    {
        std::unique_lock<std::mutex> lock(updateMutex);
        isUpdateRequested = true;
        updateCV.notify_one();
    }

    // Slight music ending transition
    if (enter) enter = app->OnTransition();
    else if (!enter && app->OnTransition())
    {
        app->musicManager.SetVolume(utils::SmoothToZero(app->musicManager.GetVolume(), 0.1875f, dt));
    }

    // Entities update
    asteroids.Update(player, dt);
    enemies.Update(bullets, player, asteroids, dt);
    player.Update(bullets, asteroids, enemies, dt);

    // Components update
    background.Update(player.GetCameraTarget(), 1.0f-player.GetCameraZoom());

    // Update end game state
    if (!player.IsAlive())
    {
        endPhase = std::min(endPhase + 2.0f * dt, 1.0f);
        endPanel.Update(this, endPhase, dt);
    }

    // Wait for the update thread to finish
    {
        std::unique_lock<std::mutex> lock(updateMutex);
        updateCV.wait(lock, [this] { return !isUpdateRequested; });
    }
}

void Asteroid::Game::Draw(const rf::core::Renderer& target)
{
    target.Clear(BLACK);

    background.Draw();

    player.CameraBegin();
        bullets.Draw(tilesheet);
        asteroids.Draw(tilesheet);
        enemies.Draw(tilesheet);
        player.Draw(tilesheet);
        ores.Draw(gradient);
        psys.Draw();
    player.CameraEnd();

    player.DrawHUD(tilesheet, 1.0f-endPhase);

    // Update end game state
    if (!player.IsAlive())
    {
        endPanel.Draw(endPhase);
    }

    if (showInstructions)
    {
        instructions.Draw();
    }

#   ifndef NDEBUG
        DrawFPS(10, 10);
#   endif
}
