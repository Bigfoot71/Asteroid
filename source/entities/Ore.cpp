#include "entities/Ore.hpp"
#include "entities/Player.hpp"

using namespace Asteroid;

bool Ore::Update(Player& player, float dt)
{
    constexpr float attractionAreaSq = 128*128;
    float pDistSq = player.IsAlive() ? player.GetPosition().DistanceSqr(position) : attractionAreaSq;

    if (pDistSq < attractionAreaSq)
    {
        const float ts = 2*player.GetSpeed();
        if (speed < ts) speed += ts * dt;

        Vector2 dir = (player.GetPosition() - position).Normalize();
        velocity = velocity.Lerp(dir, pDistSq/(128*128));
        if (lifeTime < 2.0f) lifeTime = std::min(lifeTime + dt, 2.0f);

        if (pDistSq < player.GetCollisionRadiusSqr())
        {
            player.AddCoins(level);
            return false;
        }
    }
    else
    {
        speed = std::max(speed - 128 * dt, 0.0f);
        lifeTime -= 0.5f * dt;
    }

    position += velocity * speed * dt;
    onScreen = player.PointOnScreen(position, collideRadius);

    return lifeTime > 0.0f;
}

void Ores::Init()
{
    count = 0;
}

void Ores::Spawn(const Vector2& position, uint8_t num)
{
    for (uint8_t i = 0; i < num && count < max; i++)
    {
        ores[count++] = Ore(gen, position);
    }
}

void Ores::Update(Player& player, float dt)
{
    for (int16_t i = count-1; i >= 0; i--)
    {
        Ore &ore = ores[i];

        if (!ore.Update(player, dt))
        {
            ore = ores[--count];
        }
    }
}

void Ores::Draw(const raylib::Texture* gradient)
{
    for (uint16_t i = 0; i < count; i++)
    {
        const Ore &ore = ores[i];
        if (ore.onScreen) ore.Draw(gradient);
    }
}
