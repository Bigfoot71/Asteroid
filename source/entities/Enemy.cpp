#include "entities/Enemy.hpp"

#include "entities/Asteroid.hpp"
#include "entities/Player.hpp"
#include "entities/Bullet.hpp"
#include "entities/Ore.hpp"
#include "raylib.h"
#include "utils.hpp"

using namespace rf;
using namespace Asteroid;

/* ENEMY - PRIVATE */

void Enemy::UpdateRotation(Player& player, float dt)
{
    raylib::Vector2 targetDir = player.GetPosition() - position;
    if (state == State::Repositioning) targetDir *= -1;

    // Calculation of the progressive rotation of the spaceship
    float da = utils::GetDeltaRad(rotation, std::atan2(targetDir.y, targetDir.x) + PI/2);
    float prevRotation = rotation;
    rotation += da * 6 * dt;

    // Calculation of the depth sidePitch
    sidePitch = (GetMouseDelta().x || GetMouseDelta().y)
        ? std::clamp(sidePitch + (rotation - prevRotation), -PI/6, PI/6)
        : sidePitch = sidePitch * 0.975f;
}

void Enemy::UpdateMovement(Player& player, float dt)
{
    // Update engine velocity applied to spaceship
    if (state == State::Pursuit || state == State::Repositioning)
    {
        // Increase engine velocity
        velEngine = std::min(velEngine + acceleration * dt, maxVelEngine);

        // Calculate velocity applied to the spaceship from its rotation and engine velocity
        float th = rotation-PI/2;
        raylib::Vector2 dir(std::cos(th), std::sin(th));
        raylib::Vector2 velDir = dir * velEngine;
        Vector2 velDirMax = dir * maxVelEngine;
        
        // Apply limited velocity to the spaceship
        velocity += velDir * 0.1f;
        if ( (velDirMax.x > 0 && velocity.x > velDirMax.x) || (velDirMax.x < 0 && velocity.x < velDirMax.x) ) velocity.x = velDirMax.x;
        if ( (velDirMax.y > 0 && velocity.y > velDirMax.y) || (velDirMax.y < 0 && velocity.y < velDirMax.y) ) velocity.y = velDirMax.y;
    }
    else // Reset engine velocity when idle
    {
        velEngine *= 0.95f;
    }

    // Update position
    position += velocity * dt;
}

void Enemy::UpdateCollision(const Enemy* enemies, uint8_t currentIndex, Player& player, Asteroids& asteroids, gfx2d::ParticleSystem& psys, float dt)
{
    // Check collision between enemies to correct position
    for (int8_t i = currentIndex - 1; i >= 0; i--)
    {
        float penetration = CheckCollision(enemies[i]);
        if (penetration > 0) position += (position - enemies[i].position).Normalize() * penetration;
    }

    // Will report if there is a need for repositioning
    // Unless the player is no longer alive, then the enemies will leave
    bool needRepositioning = !player.IsAlive();

    // Gestion des collisions avec les asteroids
    for (auto& asteroid : asteroids)
    {
        if (!asteroid.IsAlive()) continue;

        if (!needRepositioning)
        {
            // Detection if the asteroid is facing the enemy
            needRepositioning = utils::CheckCollisionLineCircle(
                position, player.GetPosition(), asteroid.GetPosition(),
                asteroid.GetCollisionRadius());
        }

        // Fixed penetration with asteroids
        float penetration = asteroid.CheckCollision(position, collideRadius);

        if (penetration > 0)
        {
            position += (position - asteroid.GetPosition()).Normalize() * penetration;
            velocity = velocity.Negate();

            asteroids.Impact(asteroid, 5);
            this->RemLife(10);

            psys.SetPosition((asteroid.GetPosition() + position) * 0.5f);
            psys.Emit(16);
        }
    }

    // Define movement behavior after collision detection
    state = needRepositioning ? State::Repositioning : State::Pursuit;

    // Collide with player
    if (player.IsAlive())
    {
        float penetration = player.CheckCollision(position, collideRadius);

        if (penetration > 0)
        {
            player.Translate((player.GetPosition() - position).Normalize() * penetration);
            this->RemLife(1); player.RemLife(1);

            psys.SetPosition((player.GetPosition() + position) * 0.5f);
            psys.Emit(4);
        }
    }
}

void Enemy::UpdateBlaster(Bullets<MaxBullets>& bullets, Asteroids& asteroids, Player& player, float dt)
{
    state = (player.IsAlive() && player.GetPosition().DistanceSqr(position) < 300*300) ? State::Shoot : state;

    if (state == State::Shoot && (fireTimer += dt) >= fireRate)
    {
        fireTimer = 0;
        const float th = rotation-PI/2;
        const raylib::Vector2 dir(std::cos(th), std::sin(th));

        if (blasterNum == 1) // NOTE: It is assumed that ships with one blaster all have it in the center of the ship
        {
            bullets.Add(Bullet::Owner::Enemy, position, dir, velocity, damage);
        }
        else
        {
            for (uint8_t i = 0; i < blasterNum; i++)
            {
                bullets.Add(Bullet::Owner::Enemy, position + raylib::Vector2(blasterPos[i]).Rotate(rotation), dir, velocity, damage);
            }
        }
    }
}

/* ENEMY - PUBLIC */

Enemy::Enemy(uint8_t level, const Vector2& position, float rotation)
: EnemyStats(enemyStats[level]), position(position), velocity()
, rotation(rotation), sidePitch(0), velEngine(0)
, fireTimer(0), fireRate(1.0f/bulletPerSec)
, killTimer(1.0f), level(level)
, state(State::Pursuit)
, alive(true)
{
    trail.Init();
}

bool Enemy::Update(Bullets<MaxBullets>& bullets, const Enemy* enemies, uint8_t currentIndex, Player& player, Asteroids& asteroids, rf::gfx2d::ParticleSystem& psys, float dt)
{
    if (alive)
    {
        UpdateRotation(player, dt);
        UpdateMovement(player, dt);

        if (player.PointOnCollideArea(position))
        {
            UpdateCollision(enemies, currentIndex, player, asteroids, psys, dt);
        }

        trail.Update(position, velEngine / maxVelEngine);
        UpdateBlaster(bullets, asteroids, player, dt);
    }
    else
    {
        if (killTimer > 0.0f)
        {
            killTimer = std::max(killTimer - 4.0f * dt, 0.0f);
            trail.Update(position, velEngine / maxVelEngine);
        }
    }

    return killTimer > 0.0f;
}

void Enemy::Draw(const raylib::Texture* tilesheet)
{
    trail.Draw(16, { 255, 0, 0, 191 });
    utils::DrawPerspective(tilesheet, source, position, rotation, sidePitch, killTimer, ColorAlpha(WHITE, killTimer));
}

/* ENEMIES - PRIVATE */

uint8_t Enemies::GetEnemyIndex(const Player& player) const
{
    uint8_t l1 = std::min(player.GetDestroyedEnemies()/3, 14u);

    uint8_t l2 = gen.Random(0, 1) // spawn by time ?
        ? std::min(static_cast<int>(player.GetGameTime()), 14*14)/14
        : std::clamp(player.GetDestroyedAsteroids()/2, 1u, 14u);

    return gen.Random(std::min(l1, l2), std::max(l1, l2));
}

void Enemies::Spawner(const Player& player, float dt)
{
    spawnerTimer += dt;

    if (spawnerTimer >= maxSpawnerTimer && count < max)
    {
        float th = gen.Random(0.0f, 2.f*PI);
        float dist = gen.Random(768.f, 1024.f);
        raylib::Vector2 pos = player.GetPosition() + Vector2{
            std::cos(th) * dist, std::sin(th) * dist
        };

        enemies[count++] = Enemy(GetEnemyIndex(player), pos, player.GetPosition().Angle(pos));
        maxSpawnerTimer = gen.Random(0.5f, 1.5f);
        spawnerTimer = 0;
    }
}

/* ENEMIES - PUBLIC */

void Enemies::Init()
{
    maxSpawnerTimer = gen.Random(3.0f, 4.0f);
    spawnerTimer = 0;
    count = 0;
}

bool Enemies::Impact(Enemy& enemy, uint16_t damage)
{
    if (!enemy.RemLife(damage)) ///< false if enemy is destroyed
    {
        ores.Spawn(enemy.GetPosition(), (enemy.level+1) * gen.Random(0, 4));
        psys.SetPosition(enemy.GetPosition());
        psys.Emit(24);
        return true;
    }
    return false;
}

bool Enemies::BulletImpact(Enemy& enemy, const Vector2& bulletPos, uint16_t damage)
{
    psys.SetPosition(bulletPos);
    psys.Emit(8);

    return Impact(enemy, damage);
}

void Enemies::Update(Bullets<MaxBullets>& bullets, Player& player, Asteroids& asteroids, float dt)
{
    if (player.IsAlive()) Spawner(player, dt);

    for (int8_t i = count-1; i >= 0; i--)
    {
        Enemy &enemy = enemies[i];

        if (!player.PointOnActiveArea(enemy.position))
        {
            enemy = enemies[--count];
            continue;
        }

        if (!enemy.Update(bullets, enemies, i, player, asteroids, psys, dt))
        {
            enemy = enemies[--count];
        }
    }
}

void Enemies::Draw(const raylib::Texture* tilesheet)
{
    for (uint8_t i = 0; i < count; i++)
    {
        enemies[i].Draw(tilesheet);
    }
}
