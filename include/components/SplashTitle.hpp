#ifndef ASTEROID_COMPONENT_SPLASH_TITLE_HPP
#define ASTEROID_COMPONENT_SPLASH_TITLE_HPP

#include <rayflex.hpp>
#include <utility>

namespace Asteroid {

    class SplashTitle
    {
      private:
        inline static Shader shaderWave{};
        inline static int locTime{};
        inline static int count{};

      private:
        raylib::Vector2 start{};
        raylib::Vector2 end{};
        float t{};

      public:
        SplashTitle();
        ~SplashTitle();

        void Init(const Vector2& start, const Vector2& end, float t);

        bool Update(float dt);
        void Draw(const raylib::Texture* texture, const raylib::Font* font) const;
        static void DrawWave(const raylib::Texture* texture, const Vector2& position, float scale = 1.0f);
    };

}

#endif //ASTEROID_COMPONENT_SPLASH_TITLE_HPP