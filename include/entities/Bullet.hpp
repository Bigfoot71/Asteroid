#ifndef ASTEROID_ENTITY_BULLET_HPP
#define ASTEROID_ENTITY_BULLET_HPP

#include <cstdint>
#include <rayflex.hpp>
#include "./Enemy.hpp"
#include "./Player.hpp"
#include "./Asteroid.hpp"
#include "../settings.hpp"

namespace Asteroid {

    class Bullet
    {
      public:
        enum class Owner : uint8_t
        {
            Player, Enemy
        };

      public:
        static constexpr float collideRadius = 2.0f;
        static constexpr float collideRadiusSq = collideRadius*collideRadius;

      private:
        static constexpr Rectangle srcPlayer = { 384, 256, 64, 64 };
        static constexpr Rectangle srcEnemy = { 448, 256, 64, 64 };

      private:
        raylib::Vector2 position;
        raylib::Vector2 velocity;
        float           lifeTime;
        uint16_t        damage;
        Owner           owner;
        bool            onScreen;

      private:
        bool PlayerBehavior(Player& player, Enemies& enemies);
        bool EnemyBehavior(Player& player);

      public:
        Bullet() = default;
        Bullet(Owner owner, const Vector2& position, const raylib::Vector2& direction, const Vector2& baseVel, uint16_t damage);

        bool Update(Player& player, Asteroids& asteroids, Enemies& enemies, float dt);
        void Draw(const raylib::Texture* tilesheet, float rotation) const;

      public:
        bool CheckCollisionSqr(const raylib::Vector2& p, float rSq) const
        {
            return p.DistanceSqr(position) <= (collideRadiusSq + rSq);
        }

        bool OnScreen() const { return onScreen; }
    };

    template <uint16_t N>
    class Bullets
    {
      private:
        Bullet bullets[N];
        uint16_t count;

      public:
        Bullets() = default;
        void Init() { count = 0; }

        void Add(Bullet::Owner owner, const Vector2& position, const Vector2& direction, const Vector2& baseVel, uint16_t damage)
        {
            if (count == N) return;
            bullets[count++] = Bullet(owner, position, direction, baseVel, damage);
        }

        void Update(class Player& player, Asteroids& asteroids, class Enemies& enemies, float dt)
        {
            for (int32_t i = count-1; i >= 0; i--)
            {
                Bullet &bullet = bullets[i];

                if (!bullet.Update(player, asteroids, enemies, dt))
                {
                    bullet = bullets[--count];
                    continue;
                }
            }
        }

        void Draw(const raylib::Texture* tilesheet) const
        {
            float rotation = 90.0f * GetTime();

            for (uint16_t i = 0; i < count; i++)
            {
                const auto &bullet = bullets[i];
                if (bullet.OnScreen()) bullet.Draw(tilesheet, rotation);
            }
        }
    };

}

#endif //ASTEROID_ENTITY_PLAYER_HPP
