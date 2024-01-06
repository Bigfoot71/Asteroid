#ifndef ASTEROID_GAME_BACKGROUND
#define ASTEROID_GAME_BACKGROUND

#include <rayflex.hpp>

namespace Asteroid {

    class GameBackground
    {
      private:
        inline static Shader shader{};
        inline static int locPosition{};
        inline static int locTime{};
        inline static int locZoom{};
        inline static int counter{};

      public:
        GameBackground();
        ~GameBackground();

        void Update(const Vector2& position, float zoom = 0.0f); // TODO: Review zoom, its not a zoom
        void Draw();
    };

}

#endif //ASTEROID_GAME_BACKGROUND
