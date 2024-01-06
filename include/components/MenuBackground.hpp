#ifndef ASTEROID_MENU_BACKGROUND
#define ASTEROID_MENU_BACKGROUND

#include <rayflex.hpp>

namespace Asteroid {

    class MenuBackground
    {
      private:
        inline static Shader shader{};
        inline static int locTime{};
        inline static int counter{};

      public:
        MenuBackground();
        ~MenuBackground();

        void Update();
        void Draw();
    };

}

#endif //ASTEROID_MENU_BACKGROUND
