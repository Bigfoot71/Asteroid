#ifndef ASTEROID_ENTITIES_ENEMY_HPP
#define ASTEROID_ENTITIES_ENEMY_HPP

#include <rayflex.hpp>

#include "../components/TrailEffect.hpp"
#include "../settings.hpp"
#include "../utils.hpp"

namespace Asteroid {

    class Player;
    class Asteroids;
    template <uint16_t N> class Bullets;

    struct EnemyStats
    {
        Rectangle source;           // Source sur la texture
        float collideRadius;        // Rayon de collision
        float collideRadiusSq;      // Rayon de collision au carré
        float acceleration;         // Acceleration du moteur
        float maxVelEngine;         // Velocité max du moteur
        float bulletPerSec;         // Nombre de bullet par secondes
        int16_t life;               // Vie de base du vaisseau
        uint16_t damage;            // Degats de base d'une bullet
        uint8_t blasterNum;         // Nombre de blaster sur le vaisseau
        Vector2 blasterPos[4];      // Local blaster position (avec le centre pour origine sur 64x64)
    };

    class Enemy : private EnemyStats
    {
      private:
        friend class Enemies;

      private:
        enum class State : uint8_t
        {
            Repositioning,
            Pursuit,
            Shoot
        };

      private:
        TrailEffect trail;

      private:
        raylib::Vector2 position;       ///< Position du vaisseau
        raylib::Vector2 velocity;       ///< Velocité du mouvment (dir*speed)
        float rotation;                 ///< Rotation du vaisseau (en radian)
        float sidePitch;                ///< Tanguage de gauche à droite (de -1 à 1)

      private:
        float velEngine;                ///< Velocité de la poussée du moteur
        float fireTimer;                ///< Timer gérant la cadence de tir
        float fireRate;                 ///< Cadence de tir du cannon, egal à l'inverse du nombre de balles par secondes (1.0f/bPerSec)
        float killTimer;
        uint8_t level;
        State state;
        bool alive;

      private:
        void UpdateRotation(Player& player, float dt);
        void UpdateMovement(Player& player, float dt);
        void UpdateCollision(const Enemy* enemies, uint8_t currentIndex, Player& player, Asteroids& asteroids, rf::gfx2d::ParticleSystem& psys, float dt);
        void UpdateBlaster(Bullets<MaxBullets>& bullets, Asteroids& asteroids, Player& player, float dt);

      private:
        bool RemLife(uint16_t value)
        {
            alive = (life -= value) > 0;
            return alive;
        }

      public:
        Enemy() = default;
        Enemy(uint8_t level, const Vector2& position, float rotation);

        bool Update(Bullets<MaxBullets>& bullets, const Enemy* enemies, uint8_t currentIndex, Player& player, Asteroids& asteroids, rf::gfx2d::ParticleSystem& psys, float dt);
        void Draw(const raylib::Texture* tilesheet);

      public:
        float CheckCollision(const raylib::Vector2& p, float r) const
        {
            float d = p.Distance(position) - (collideRadius + r);
            return d < 0 ? -d : 0;
        }

        float CheckCollision(const Enemy& other) const
        {
            return CheckCollision(other.position, other.collideRadius);
        }

        bool CheckCollisionSqr(const raylib::Vector2& p, float rSq) const
        {
            return p.DistanceSqr(position) <= (collideRadiusSq + rSq);
        }

        bool IsAlive() const { return alive; }
        Vector2 GetPosition() const { return position; }
        float GetCollisionRadiusSqr() const { return collideRadiusSq; }
    };

    class Enemies
    {
      private:
        static constexpr uint8_t max = 8;

      private:
        Enemy                       enemies[max];
        rf::gfx2d::ParticleSystem   &psys;
        rf::core::RandomGenerator   &gen;
        class Ores                  &ores;
        float                       maxSpawnerTimer;
        float                       spawnerTimer;
        uint8_t                     count;

      private:
        uint8_t GetEnemyIndex(const class Player& player) const;
        void Spawner(const class Player& player, float dt);

      public:
        Enemies(rf::core::RandomGenerator& gen, rf::gfx2d::ParticleSystem& psys, class Ores& ores)
        : psys(psys), gen(gen), ores(ores)
        { }

        void Init();

        // Return true if enemy is destroyed
        bool Impact(Enemy& enemy, uint16_t damage);
        bool BulletImpact(Enemy& enemy, const Vector2& bulletPos, uint16_t damage);

        void Update(Bullets<MaxBullets>& bullets, Player& player, Asteroids& asteroids, float dt);
        void Draw(const raylib::Texture* tilesheet);

        Enemy* begin() { return enemies; }
        Enemy* end() { return enemies + count; }
    };


    /* ENEMIES STATS */

    constexpr EnemyStats enemyStats[15] = {
        {
            .source = { 192, 0, 64, 64 },
            .collideRadius = 16.0f,
            .collideRadiusSq = 16.0f*16.0f,
            .acceleration = 150,
            .maxVelEngine = 150,
            .bulletPerSec = 1,
            .life = 30,
            .damage = 10,
            .blasterNum = 1,
            .blasterPos = {
                { 0, 0 }
            }
        },

        {
            .source = { 256, 0, 64, 64 },
            .collideRadius = 16.0f,
            .collideRadiusSq = 16.0f*16.0f,
            .acceleration = 175,
            .maxVelEngine = 175,
            .bulletPerSec = 1,
            .life = 50,
            .damage = 10,
            .blasterNum = 1,
            .blasterPos = {
                { 0, 0 }
            }
        },

        {
            .source = { 320, 0, 64, 64 },
            .collideRadius = 16.0f,
            .collideRadiusSq = 16.0f*16.0f,
            .acceleration = 175,
            .maxVelEngine = 175,
            .bulletPerSec = 2,
            .life = 50,
            .damage = 10,
            .blasterNum = 1,
            .blasterPos = {
                { 0, 0 }
            }
        },

        {
            .source = { 384, 0, 64, 64 },
            .collideRadius = 16.0f,
            .collideRadiusSq = 16.0f*16.0f,
            .acceleration = 200,
            .maxVelEngine = 200,
            .bulletPerSec = 2,
            .life = 60,
            .damage = 10,
            .blasterNum = 2,
            .blasterPos = {
                { -14, 14 },
                { 14, 14 }
            }
        },

        {
            .source = { 448, 0, 64, 64 },
            .collideRadius = 16.0f,
            .collideRadiusSq = 16.0f*16.0f,
            .acceleration = 200,
            .maxVelEngine = 200,
            .bulletPerSec = 2,
            .life = 80,
            .damage = 10,
            .blasterNum = 2,
            .blasterPos = {
                { -14, 6 },
                { 14, 6 }
            }
        },

        {
            .source = { 192, 64, 64, 64 },
            .collideRadius = 16.0f,
            .collideRadiusSq = 16.0f*16.0f,
            .acceleration = 200,
            .maxVelEngine = 200,
            .bulletPerSec = 2,
            .life = 80,
            .damage = 15,
            .blasterNum = 2,
            .blasterPos = {
                { -16, 16 },
                { 16, 16 }
            }
        },

        {
            .source = { 256, 64, 64, 64 },
            .collideRadius = 16.0f,
            .collideRadiusSq = 16.0f*16.0f,
            .acceleration = 225,
            .maxVelEngine = 225,
            .bulletPerSec = 2,
            .life = 80,
            .damage = 15,
            .blasterNum = 2,
            .blasterPos = {
                { -16, 0 },
                { 16, 0 }
            }
        },

        {
            .source = { 320, 64, 64, 64 },
            .collideRadius = 16.0f,
            .collideRadiusSq = 16.0f*16.0f,
            .acceleration = 200,
            .maxVelEngine = 200,
            .bulletPerSec = 2,
            .life = 100,
            .damage = 20,
            .blasterNum = 2,
            .blasterPos = {
                { -19, 0 },
                { 19, 0 }
            }
        },

        {
            .source = { 384, 64, 64, 64 },
            .collideRadius = 24.0f,
            .collideRadiusSq = 24.0f*24.0f,
            .acceleration = 250,
            .maxVelEngine = 250,
            .bulletPerSec = 1,
            .life = 100,
            .damage = 10,
            .blasterNum = 4,
            .blasterPos = {
                { -16, -16 },
                { -16, 16 },
                { 16, -16 },
                { 16, 16 }
            },
        },

        {
            .source = { 448, 64, 64, 64 },
            .collideRadius = 24.0f,
            .collideRadiusSq = 24.0f*24.0f,
            .acceleration = 250,
            .maxVelEngine = 250,
            .bulletPerSec = 1,
            .life = 120,
            .damage = 20,
            .blasterNum = 4,
            .blasterPos = {
                { -16, -16 },
                { -16, 16 },
                { 16, -16 },
                { 16, 16 }
            }
        },

        {
            .source = { 192, 128, 64, 64 },
            .collideRadius = 24.0f,
            .collideRadiusSq = 24.0f*24.0f,
            .acceleration = 300,
            .maxVelEngine = 300,
            .bulletPerSec = 2,
            .life = 120,
            .damage = 20,
            .blasterNum = 1,
            .blasterPos = {
                { -16, 0 },
                { 16, 0 }
            }
        },

        {
            .source = { 256, 128, 64, 64 },
            .collideRadius = 16.0f,
            .collideRadiusSq = 16.0f*16.0f,
            .acceleration = 350,
            .maxVelEngine = 350,
            .bulletPerSec = 4,
            .life = 100,
            .damage = 10,
            .blasterNum = 2,
            .blasterPos = {
                { -16, 0 },
                { 16, 0 }
            }
        },

        {
            .source = { 320, 128, 64, 64 },
            .collideRadius = 24.0f,
            .collideRadiusSq = 24.0f*24.0f,
            .acceleration = 300,
            .maxVelEngine = 300,
            .bulletPerSec = 3,
            .life = 150,
            .damage = 10,
            .blasterNum = 4,
            .blasterPos = {
                { -16, -16 },
                { -16, 16 },
                { 16, -16 },
                { 16, 16 }
            }
        },

        {
            .source = { 384, 128, 64, 64 },
            .collideRadius = 24.0f,
            .collideRadiusSq = 24.0f*24.0f,
            .acceleration = 350,
            .maxVelEngine = 350,
            .bulletPerSec = 3,
            .life = 150,
            .damage = 15,
            .blasterNum = 4,
            .blasterPos = {
                { -16, -16 },
                { -16, 16 },
                { 16, -16 },
                { 16, 16 }
            }
        },

        {
            .source = { 448, 128, 64, 64 },
            .collideRadius = 24.0f,
            .collideRadiusSq = 24.0f*24.0f,
            .acceleration = 400,
            .maxVelEngine = 400,
            .bulletPerSec = 3,
            .life = 200,
            .damage = 20,
            .blasterNum = 4,
            .blasterPos = {
                { -16, -16 },
                { -16, 16 },
                { 16, -16 },
                { 16, 16 }
            }
        },

    };
}

#endif //ASTEROID_ENTITIES_ENEMY_HPP
