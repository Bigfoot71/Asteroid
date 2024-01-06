#ifndef ASTEROID_LOADING_STATE_HPP
#define ASTEROID_LOADING_STATE_HPP

#include <rres.h>
#include <rayflex.hpp>
#include "settings.hpp"

namespace Asteroid {

    class Loading : public rf::core::LoadingState
    {
      private:
        const char *assetPath;
        rresCentralDir dir;

      private:
        int frameCounter;
        int dotLen;

      public:
        void Enter() override;
        void Exit() override;

        void Task() override;
        void PostTask() override;

        void Update(float dt) override;
        void Draw(const rf::core::Renderer& target) override;
    };

}

#endif //ASTEROID_LOADING_STATE_HPP