#ifndef ASTEROID_ENTITY_ASTEROID_HPP
#define ASTEROID_ENTITY_ASTEROID_HPP

#include "Texture.hpp"
#include <rayflex.hpp>

namespace Asteroid {

    class Asteroid
    {
      private:
        friend class Asteroids;

      public:
        enum class RetLife { None, LevelChange, Destroyed };

      public:
        static constexpr float collideRadius[2] = { 14, 20 };
        static constexpr float collideRadiusSq[2] = { 14*14, 20*20 };

        static constexpr Rectangle sources[2][4] = {
            {
                { 0, 192, 64, 64 }, { 64, 192, 64, 64 },
                { 128, 192, 64, 64 }, { 192, 192, 64, 64 },
            },
            {
                { 256, 192, 64, 64 }, { 320, 192, 64, 64 },
                { 384, 192, 64, 64 }, { 448, 192, 64, 64 },
            },
        };

      private:
        raylib::Vector2 position;
        raylib::Vector2 velocity;
        float rotation, speed;
        float killTimer;
        int16_t lifeState;          // Vie de l'asteroid (par tranche de 100 pour chaque taille, 200 max)
        uint8_t level;              // Niveau par rapport à la vie => ceil(life/100-1)
        uint8_t ix;                 // Index de 0 ou 3 aleatoire, pour les deux types d'asteroid
        bool alive;

      private:
        void UpdateLevel()
        {
            level = static_cast<uint8_t>(lifeState*0.01f-EPSILON);
        }

        RetLife RemLife(uint16_t value)
        {
            uint8_t prevLevel = level;
            alive = (lifeState -= value) > 0;

            UpdateLevel();

            return (!alive ? RetLife::Destroyed : prevLevel != level ? RetLife::LevelChange : RetLife::None);
        }

      public:
        Asteroid() = default;

        Asteroid(rf::core::RandomGenerator& gen, const Vector2& position, const Vector2& velocity, float speed)
        : position(position), velocity(velocity), rotation(gen.Random(0.f, 360.f)), speed(speed)
        , killTimer(1.0f), lifeState(gen.Random(50, 200)), ix(gen.Random(0, 3)), alive(true)
        {
            UpdateLevel();
        }

        bool Update(float dt)
        {
            rotation -= (velocity.x > velocity.y ? velocity.x : velocity.y) * speed * dt;
            position += velocity * speed * dt;
            if (!alive) killTimer -= 8 * dt;
            return killTimer > 0.0f;
        }

        void Draw(const raylib::Texture* texture) const
        {
            texture->Draw(sources[level][ix],
                { position.x, position.y, 64*killTimer, 64*killTimer },
                { 32*killTimer, 32*killTimer }, rotation, ColorAlpha(WHITE, killTimer));
        }

      public:
        float CheckCollision(const raylib::Vector2& p, float r) const
        {
            //float d = p.DistanceSqr(position) - (collideRadiusSq[level] + rSq);
            //return d < 0 ? sqrt(-d) : 0;

            float d = p.Distance(position) - (collideRadius[level] + r);
            return d < 0 ? -d : 0;
        }

        float CheckCollision(const Asteroid& other) const
        {
            return CheckCollision(other.position, collideRadius[other.level]);
        }

        bool IsAlive() const { return alive; }
        raylib::Vector2 GetPosition() const { return position; }
        float GetCollisionRadius() const { return collideRadiusSq[level]; }
        float GetCollisionRadiusSqr() const { return collideRadiusSq[level]; }
    };

    class Asteroids
    {
      private:
        static constexpr uint8_t max = 24;

      private:
        Asteroid                    asteroids[max];
        rf::gfx2d::ParticleSystem   &psys;
        rf::core::RandomGenerator   &gen;
        class Ores                  &ores;
        float                       maxSpawnerTimer;
        float                       spawnerTimer;
        uint8_t                     count;

      private:
        void Spawner(const class Player& player, float dt);

      public:
        Asteroids(rf::core::RandomGenerator& gen, rf::gfx2d::ParticleSystem& psys, class Ores& ores)
        : psys(psys), gen(gen), ores(ores)
        { }

        void Init();

        // Return true if asteroid is destroyed
        bool Impact(Asteroid& asteroid, uint16_t damage);
        bool BulletImpact(Asteroid& asteroid, const Vector2& bulletPos, uint16_t damage);

        void Update(class Player& player, float dt);
        void Draw(const raylib::Texture* tilesheet);

        Asteroid* begin() { return asteroids; }
        Asteroid* end() { return asteroids + count; }
    };

}

#endif //ASTEROID_ENTITY_ASTEROID_HPP