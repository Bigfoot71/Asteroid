#ifndef ASTEROID_COMPONENT_SPLASH_ENTITY_HPP
#define ASTEROID_COMPONENT_SPLASH_ENTITY_HPP

#include <rayflex.hpp>

#include "../components/TrailEffect.hpp"
#include "settings.hpp"

namespace Asteroid {

    struct SplashEntity
    {
        static constexpr Color humanColor = { 0, 0, 255, 191 };
        static constexpr Color alienColor = { 255, 0, 0, 191 };

        enum class Type : uint8_t { Human, Alien };

        TrailEffect trail;
        Rectangle source;
        Color trailColor;

        raylib::Vector2 start;
        raylib::Vector2 end;
        float invTimeStep;
        float t;

        raylib::Vector2 pos;
        float sidePitch;
        float rotation;
        float killTime;
        bool alive;
        uint8_t id;
        Type type;

        using BulletAdder = std::function<void(const Vector2&, const Vector2&, SplashEntity::Type)>;
        using Script = std::function<void(SplashEntity&, const BulletAdder&)>;
        Script script;

        SplashEntity() = default;
        SplashEntity(float timeStep, Type type, uint8_t id, const Vector2& start, const Vector2& end, const Rectangle& source, const Script& script);

        bool Update(const BulletAdder& addBullet, float dt);
        void Draw(const raylib::Texture *texture) const;
    };

    struct SplashBullet
    {
        raylib::Vector2 pos, vel;
        SplashEntity::Type target;

        bool Update(float dt)
        {
            pos += vel * dt;

            return pos.CheckCollision(Rectangle{
                0, 0, BaseWidth, BaseHeight
            });
        }

        void Draw(const raylib::Texture* texture) const
        {
            texture->Draw(target == SplashEntity::Type::Alien ? Rectangle{ 384, 256, 64, 64 } : Rectangle{ 448, 256, 64, 64 },
                { pos.x, pos.y, 32, 32 }, { 16, 16 }, 0, WHITE);
        }
    };

    class SplashEntities
    {
      private:
        SplashEntity entities[84]{};
        SplashBullet bullets[42]{};
        uint8_t entCount, bulCount;

      private:
        void AddBullet(const Vector2& pos, const Vector2& vel, SplashEntity::Type target)
        {
            if (bulCount < 42) bullets[bulCount++] = SplashBullet{ pos, vel, target };
        }

      public:
        void Init() { entCount = bulCount = 0; }

        uint8_t Count() const { return entCount; }

        void Add(float timeStep, SplashEntity::Type type, const Vector2& start, const Vector2& end, const Rectangle& source, const SplashEntity::Script script = nullptr)
        {
            // The ID corresponds to the index; it won't pose a problem even if we reorganize the array because the animation
            // works in waves of enemies, and we only add them again when there are none left, resetting the counter back to 0.
            entities[entCount++] = SplashEntity(timeStep, type, entCount, start, end, source, script);
        }

        void Update(rf::gfx2d::ParticleSystem& psys, float dt);
        void Draw(const raylib::Texture* texture) const;
    };

}

#endif //ASTEROID_COMPONENT_SPLASH_ENTITY_HPP