#include "components/SplashEntity.hpp"
#include "utils.hpp"

using namespace rf;
using namespace Asteroid;

/* ENTITY */

SplashEntity::SplashEntity(float timeStep, Type type, uint8_t id, const Vector2& start, const Vector2& end, const Rectangle& source, const Script& script)
: source(source), trailColor(type == Type::Human ? humanColor : alienColor), start(start), end(end)
, invTimeStep(1.0f/timeStep), t(0), pos(start)
, sidePitch(0), rotation(0)
, killTime(1), alive(true)
, type(type), id(id)
, script(script)
{
    trail.Init();
}

bool SplashEntity::Update(const BulletAdder& addBullet, float dt)
{
    if (!alive)
    {
        return (killTime -= 6.0f * dt) > 0.0f;
    }

    t += invTimeStep * dt;

    const Vector2 prevPos = pos;
    pos = start.Lerp(end, std::min(t, 1.0f));
    raylib::Vector2 dir = (pos - prevPos).Normalize();

    float prevRotation = rotation;
    rotation = std::atan2(dir.y, dir.x) + PI/2;
    float diffRotation = rotation - prevRotation;

    sidePitch = (std::abs(diffRotation) > 0.01f)
        ? std::clamp(sidePitch + (rotation - prevRotation), -PI/6, PI/6)
        : sidePitch = sidePitch * 0.975f;

    trail.Update(pos, 0.5f);

    if (script != nullptr)
    {
        script(*this, addBullet);
    }

    return pos != end;
}

void SplashEntity::Draw(const raylib::Texture *texture) const
{
    if (alive) trail.Draw(16, trailColor);
    utils::DrawPerspective(texture, source, pos, rotation, sidePitch, killTime, ColorAlpha(WHITE, killTime));
}

/* ENTITIES */

void SplashEntities::Update(rf::gfx2d::ParticleSystem& psys, float dt)
{
    for (int8_t i = entCount-1; i >= 0; i--)
    {
        auto &entity = entities[i];

        if (!entity.Update([this](const Vector2& x, const Vector2& y, SplashEntity::Type z) {
            AddBullet(x, y, z);
        }, dt))
        {
            entity = entities[--entCount];
        }
    }

    for (int8_t i = bulCount-1; i >= 0; i--)
    {
        auto &bullet = bullets[i];

        if (!bullet.Update(dt))
        {
            bullet = bullets[--bulCount];
            continue;
        }

        for (int8_t j = entCount-1; j >= 0; j--)
        {
            auto &entity = entities[j];

            if (!entity.alive || bullet.target != entity.type) continue;

            if (bullet.pos.CheckCollision(raylib::Rectangle{
                entity.pos - Vector2{ 32, 32 },
                { 64, 64 }
            }))
            {
                psys.SetPosition((bullet.pos + entity.pos) * 0.5f);
                psys.Emit(16);

                bullet = bullets[--bulCount];
                entity.alive = false;
                break;
            }
        }
    }
}

void SplashEntities::Draw(const raylib::Texture* texture) const
{
    for (uint8_t i = 0; i < bulCount; i++)
    {
        bullets[i].Draw(texture);
    }
    for (uint8_t i = 0; i < entCount; i++)
    {
        entities[i].Draw(texture);
    }
}
