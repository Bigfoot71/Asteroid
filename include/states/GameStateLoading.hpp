#ifndef ASTEROID_STATE_GAME_STATE_LOADING_HPP
#define ASTEROID_STATE_GAME_STATE_LOADING_HPP

#include <rayflex.hpp>

#include "components/RadarEffect.hpp"
#include "states/Credit.hpp"
#include "states/Title.hpp"
#include "states/Shop.hpp"
#include "states/Game.hpp"

namespace Asteroid {

    class GameStateLoading : public rf::core::State
    {
        void Update(float dt) override
        {
            if (!app->OnTransition()) // This is only done on a single frame
            {
                RadarEffect _tmp1; (void)_tmp1;     ///< NOTE 1: Temporary instances to avoid unloading/reloading the static shaders which will be reused
                GameBackground _tmp2; (void)_tmp2;  ///< NOTE 2: The addition of `(void)X;` is only there in the hypothesis where the compiler could try to optimize this (simple precaution, no idea if this could be useful)

                app->FreeState("ray-splash");
                app->FreeState("splash");

                app->AddState<Credit>("credit");
                app->AddState<Title>("title");
                app->AddState<Game>("game");
                app->AddState<Shop>("shop");

                app->Transition("title", 0.75f, nullptr);

                app->musicManager.Play("Master Select");
            }
        }

        void Draw(const rf::core::Renderer& target) override
        {
            target.Clear(BLACK);
        }
    };

}

#endif //ASTEROID_STATE_GAME_STATE_LOADING_HPP