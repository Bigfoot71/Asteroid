#include "entities/Bullet.hpp"

using namespace Asteroid;

/* PRIVATE */

bool Bullet::PlayerBehavior(Player& player, Enemies& enemies)
{
    for (auto& enemy : enemies)
    {
        if (enemy.IsAlive() && CheckCollisionSqr(enemy.GetPosition(), enemy.GetCollisionRadiusSqr()))
        {
            if (enemies.BulletImpact(enemy, position, damage))
            {
                player.EnemyDestroyed();
            }
            return true;
        }
    }
    return false;
}

bool Bullet::EnemyBehavior(Player& player)
{
    if (CheckCollisionSqr(player.GetPosition(), player.GetCollisionRadiusSqr()))
    {
        player.BulletImpact(position, damage);
        return true;
    }
    return false;
}

/* PUBLIC */

Bullet::Bullet(Owner owner, const Vector2& position, const raylib::Vector2& direction, const Vector2& baseVel, uint16_t damage)
: owner(owner), position(position), velocity(direction * 256 + baseVel), lifeTime(2.0f), damage(damage)
{ }

bool Bullet::Update(Player& player, Asteroids& asteroids, Enemies& enemies, float dt)
{
    position += velocity * dt;

    // Check the life time of the bullet

    if ((lifeTime -= dt * 0.5f) <= 0.0f)
    {
        return false;
    }

    // Checks if the bullet is in the collision detection zone

    if (!player.PointOnCollideArea(position))
    {
        return true;
    }

    // Check collision with asteroids

    for (Asteroid& asteroid : asteroids)
    {
        if (!asteroid.IsAlive()) continue;

        if (CheckCollisionSqr(asteroid.GetPosition(), asteroid.GetCollisionRadiusSqr()))
        {
            if (asteroids.BulletImpact(asteroid, position, damage))
            {
                player.AsteroidDestroyed(); // Asteroids destroyed by enemies count
            }
            return false;
        }
    }

    // Behavior depending on the owner of the bullet

    switch (owner)
    {
        case Owner::Player: {
            if (PlayerBehavior(player, enemies))
            {
                return false;
            }
        } break;

        case Owner::Enemy: {
            if (EnemyBehavior(player))
            {
                return false;
            }
        } break;
    }

    // Determines whether the bullet should be rendered on screen
    onScreen = player.PointOnScreen(position, collideRadius);

    return true;
}

void Bullet::Draw(const raylib::Texture* tilesheet, float rotation) const
{
    tilesheet->Draw(owner == Owner::Player ? srcPlayer : srcEnemy,
        { position.x, position.y, 32, 32 }, { 16, 16 }, rotation+velocity.x,
        { 255, 255, 255, static_cast<uint8_t>(255 * std::min(lifeTime, 1.0f)) });
}
