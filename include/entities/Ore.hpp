#ifndef ASTEROID_ENTITY_ORE_HPP
#define ASTEROID_ENTITY_ORE_HPP

#include <rayflex.hpp>

namespace Asteroid {

    class Ores;

    class Ore
    {
      private:
        friend class Ores;

      public:
        static constexpr Color levelColors[7] = {
            { 200, 200, 200, 255 },
            { 0, 0, 255, 255 },
            { 0, 255, 0, 255 },
            { 0, 255, 127, 255 },
            { 255, 0, 0, 255 },
            { 0, 255, 255, 255 },
            { 160, 32, 240, 255 }
        };

        static constexpr float collideRadius = 2;
        static constexpr float collideRadiusSq = collideRadius*collideRadius;

      private:
        raylib::Vector2 position;
        raylib::Vector2 velocity;
        float speed, lifeTime;
        uint8_t level;
        bool onScreen;

      public:
        Ore() = default;

        Ore(rf::core::RandomGenerator& gen, const Vector2& position)
        : position(position), speed(gen.Random(64.0f, 256.0f)), lifeTime(3.0f)
        {
            float th = gen.Random(0.0f, 2*PI);
            velocity = Vector2{ std::cos(th), std::sin(th) };

            std::discrete_distribution<int> d({ 0.3, 0.2, 0.15, 0.15, 0.1, 0.075, 0.025 });
            level = gen(d);
        }

        //bool CheckCollisionSqr(const raylib::Vector2& p, float rSq) const
        //{
        //    return p.DistanceSqr(position) <= (collideRadiusSq + rSq);
        //}

        bool Update(class Player& player, float dt);

        void Draw(const raylib::Texture* gradient) const
        {
            gradient->Draw(Rectangle{ 0, 0, static_cast<float>(gradient->width), static_cast<float>(gradient->height) },
                { position.x, position.y, 8, 8 }, { 4, 4 }, 0, ColorAlpha(levelColors[level], std::min(lifeTime, 1.0f)));
        }
    };

    class Ores
    {
      private:
        static constexpr uint16_t max = 512;
        rf::core::RandomGenerator &gen;

      private:
        Ore ores[max];
        uint16_t count;

      public:
        Ores(rf::core::RandomGenerator& gen)
        : gen(gen)
        { }

        void Init();
        void Spawn(const Vector2& position, uint8_t num);

        void Update(class Player& player, float dt);
        void Draw(const raylib::Texture* gradient);
    };

}

#endif //ASTEROID_ENTITY_ORE_HPP