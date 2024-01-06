#include <rayflex.hpp>

#include "states/GameStateLoading.hpp"
#include "states/RaySplash.hpp"
#include "states/Loading.hpp"
#include "states/Splash.hpp"
#include "settings.hpp"
#include "save.hpp"

using namespace rf;

int main()
{
    core::App app("Asteroid", Asteroid::BaseWidth, Asteroid::BaseHeight, FLAG_WINDOW_RESIZABLE);

#   ifdef PLATFORM_DESKTOP
        app.window.SetTargetFPS(20);    // Limit FPS before Run() for loading screen
#   endif

    app.InitSaveManager(Asteroid::Save{});

#   ifndef PLATFORM_WEB
        app.saveManager->Load<Asteroid::Save>("save.dat");
#   endif

    app.Loading<Asteroid::Loading>();

    app.audio.SetVolume(app.saveManager->Get<Asteroid::Save>()->volume);
    app.musicManager.SetVolume(Asteroid::DefaultMusicVolume);
    app.musicManager.RandomizePlaylist(true);

    app.AddState<Asteroid::GameStateLoading>("game-state-loading");
    app.AddState<Asteroid::RaySplash>("ray-splash");
    app.AddState<Asteroid::Splash>("splash");

    // NOTE: Unloading splash states as well as loading
    //       game states is done in GameStateLoading state

    int ret = app.Run("ray-splash", Asteroid::TargetFPS);

#   ifndef PLATFORM_WEB
        if (ret == 0) app.saveManager->Write<Asteroid::Save>("save.dat");
#   endif

    return ret;
}
