#ifndef ASTEROID_ENTITY_PLAYER_HPP
#define ASTEROID_ENTITY_PLAYER_HPP

#include <rayflex.hpp>

#include "../components/TrailEffect.hpp"
#include "../settings.hpp"
#include "../save.hpp"

namespace Asteroid {

    class Enemies;
    class Asteroids;
    template <uint16_t N> class Bullets;

    struct PlayerConstants
    {
        const Rectangle source;         // Source sur la texture
        const float collideRadius;      // Rayon de collision
        const float collideRadiusSq;    // Rayon de collision au carré
        const Vector2 blasterPos[2];    // Local blaster position (avec le centre pour origine sur 64x64)
        const uint8_t blasterNum;       // Nombre de blaster sur le vaisseau
    };

    class Player
    {
      private:
        inline static Shader shaderLifeBar{};
        inline static int locNormLife{};
        inline static int locTime{};

      private:
        TrailEffect                 trail;              // Engine trail
        raylib::Camera2D            camera;             // Camera that targets the player
        raylib::Rectangle           source;             // Texture source
        rf::gfx2d::ParticleSystem   &psys;              // Particle system reference
        rf::core::App               *app;               // App pointer
        raylib::Vector2             blasterPos[2];      // Local position of blasters
        raylib::Vector2             position;           // Spaceship position
        raylib::Vector2             velocity;           // Movement velocity of the spaceshipt (dir*speed)
        float                       rotation;           // Spaceship rotation (radian)
        float                       sidePitch;          // Pitching from left to right (-PI to PI)
        float                       collideRadius;      // Collide radius of the spaceship
        float                       collideRadiusSq;    // Collide square radius of the spaceship
        float                       gameTimer;          // Global playing time timer
        float                       velEngine;          // Engine thrust velocity
        float                       maxVelEngine;       // Maximum engine power stat
        float                       acceleration;       // Acceleration stat
        float                       fireTimer;          // Timer managing the rate of fire
        float                       fireRate;           // Rate of fire of the cannon, equal to the inverse of the number of bullets per second (1.0f/bPerSec)
        float                       life;               // Total ship health bar
        float                       maxLife;            // Max value of life
        float                       regenLifeTime;      // Timer before life regeneration begins
        float                       shakeCameraTime;    // Camera shake timer during a big impact
        uint32_t                    money;              // Total money raised
        uint32_t                    displayedMoney;     // Money collected displayed on the screen
        uint32_t                    asteroidsCounter;   // Number of asteroids destroyed
        uint32_t                    enemiesCounter;     // Number of enemies destroyed
        uint16_t                    regenLife;          // Life point regenerated per second
        uint16_t                    bulletDamage;       // Dégat fait par les bullets tirés
        uint8_t                     blasterNum;         // Number of blasters on the ship
        bool                        cheated;

      private:
        void UpdateMovement(float dt);
        void UpdateRotation(float dt);
        void UpdateCamera(float dt);
        void UpdateBlaster(Bullets<MaxBullets>& bullets, Asteroids& asteroids, Enemies& enemies, float dt);

      public:
        Player(rf::gfx2d::ParticleSystem& psys);
        ~Player();

        void Init(rf::core::App* app);
        void Update(Bullets<MaxBullets>& bullets, Asteroids& asteroids, Enemies& enemies, float dt);
        void Draw(const raylib::Texture* tilesheet) const;

        void DrawHUD(const raylib::Texture* tilesheet, float nAlpha) const;

      public:
        raylib::Vector2 GetPosition() const { return position; }
        raylib::Vector2 GetVelocity() const { return velocity; }
        float GetEnginePower() const { return velEngine / maxVelEngine; }

        bool IsAlive() const { return life > 0; }
        float GetSpeed() const { return maxVelEngine; }

        float GetGameTime() const { return gameTimer; }
        uint32_t GetMoney() const { return money; }

        uint32_t GetDestroyedAsteroids() const { return asteroidsCounter; }
        uint32_t GetDestroyedEnemies() const { return enemiesCounter; }

        float GetCollisionRadius() const { return collideRadius; }
        float GetCollisionRadiusSqr() const { return collideRadiusSq; }

        Vector2 GetCameraTarget() const { return camera.target; }
        float GetCameraZoom() const { return camera.zoom; }

        float CheckCollision(const raylib::Vector2& p, float r) const
        {
            float d = p.Distance(position) - (collideRadius + r);
            return d < 0 ? -d : 0;
        }

        bool PointOnScreen(raylib::Vector2 point, float r = 1) const
        {
            float scale = 1.0f/camera.zoom;
            raylib::Rectangle area(0, 0, BaseWidth * scale, BaseHeight * scale);
            area.SetPosition(raylib::Vector2(camera.target) - area.GetSize() * 0.5f);
            return area.CheckCollision(point, r);
        }

        // Indique si le point donné est dans la zone des entités actives
        bool PointOnActiveArea(const raylib::Vector2& point) const
        {
            return raylib::Rectangle(position.x - 1024, position.y - 1024, 2048, 2048).CheckCollision(point);
        }

        // Indique si le point donné est dans la zone de prise en compte des collisions
        bool PointOnCollideArea(const raylib::Vector2& point) const
        {
            return raylib::Rectangle(position.x - 512, position.y - 256, 1024, 512).CheckCollision(point);
        }

      public:
        void CameraBegin() { camera.BeginMode(); }
        void CameraEnd() { camera.EndMode(); }

        void Translate(const Vector2& v) { position += v; }
        void SetVelocity(const Vector2& v) { velocity = v; }

        void AsteroidDestroyed() { asteroidsCounter++; }
        void EnemyDestroyed() { enemiesCounter++; }

        void AddCoins(uint32_t value) { money += value; }

        void RemLife(uint8_t value);
        void BulletImpact(const Vector2& bulletPos, uint16_t damage);
        void ShakeImpact() { if (shakeCameraTime <= 0) shakeCameraTime = PI; }

        void Cheat();
    };

    constexpr PlayerConstants playerConstants[PlayerCount] = {

        {
            .source = { 0, 0, 64, 64 },
            .collideRadius = 16,
            .collideRadiusSq = 16*16,
            .blasterPos = { { 0, 0 } },
            .blasterNum = 1
        },

        {
            .source = { 64, 0, 64, 64 },
            .collideRadius = 16,
            .collideRadiusSq = 16*16,
            .blasterPos = { { 0, 0 } },
            .blasterNum = 1
        },

        {
            .source = { 128, 0, 64, 64 },
            .collideRadius = 24,
            .collideRadiusSq = 24*24,
            .blasterPos = { { 0, 0 } },
            .blasterNum = 1
        },

        {
            .source = { 0, 64, 64, 64 },
            .collideRadius = 16,
            .collideRadiusSq = 16*16,
            .blasterPos = { { 0, 0 } },
            .blasterNum = 1
        },

        {
            .source = { 64, 64, 64, 64 },
            .collideRadius = 24,
            .collideRadiusSq = 24*24,
            .blasterPos = { { -16, 8 }, { 16, 8 } },
            .blasterNum = 2
        },

        {
            .source = { 128, 64, 64, 64 },
            .collideRadius = 24,
            .collideRadiusSq = 24*24,
            .blasterPos = { { 0, 0 } },
            .blasterNum = 1
        },

        {
            .source = { 0, 128, 64, 64 },
            .collideRadius = 24,
            .collideRadiusSq = 24*24,
            .blasterPos = { { -14, 14 }, { 14, 14 } },
            .blasterNum = 2
        },

        {
            .source = { 64, 128, 64, 64 },
            .collideRadius = 24,
            .collideRadiusSq = 24*24,
            .blasterPos = { { -14, 18 }, { 14, 18 } },
            .blasterNum = 2
        },

        {
            .source = { 128, 128, 64, 64 },
            .collideRadius = 24,
            .collideRadiusSq = 24*24,
            .blasterPos = { { -20, -8 }, { 20, -8 } },
            .blasterNum = 2
        },

    };

}

#endif //ASTEROID_ENTITY_PLAYER_HPP
