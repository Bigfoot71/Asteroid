#include "entities/Asteroid.hpp"
#include "entities/Bullet.hpp"
#include "entities/Player.hpp"
#include "entities/Ore.hpp"

using namespace Asteroid;

void Asteroids::Spawner(const Player& player, float dt)
{
    spawnerTimer += dt;

    if (spawnerTimer >= maxSpawnerTimer && count < max)
    {
        float th = gen.Random(0.0f, 2.f*PI);
        float dist = gen.Random(864.f, 1064.f);
        raylib::Vector2 pos = { std::cos(th) * dist, std::sin(th) * dist };

        float speed = gen.Random(96.f, 256.f);
        Vector2 vel = (player.GetPosition() - pos).Normalize();

        asteroids[count++] = Asteroid(gen, player.GetPosition() + pos, vel, speed);
        maxSpawnerTimer = gen.Random(0.5f, 1.5f);
        spawnerTimer = 0;
    }
}

void Asteroids::Init()
{
    maxSpawnerTimer = spawnerTimer = 0;
    count = 0;
}

bool Asteroids::Impact(Asteroid& asteroid, uint16_t damage)
{
    switch (asteroid.RemLife(damage))
    {
        case Asteroid::RetLife::LevelChange:
            psys.SetPosition(asteroid.position);
            psys.Emit(24);
            return false;

        case Asteroid::RetLife::Destroyed:
            ores.Spawn(asteroid.position, gen.Random(16, 32));
            psys.SetPosition(asteroid.position);
            psys.Emit(32);
            return true;

        default: break;
    }
    return false;
}

bool Asteroids::BulletImpact(Asteroid& asteroid, const Vector2& bulletPos, uint16_t damage)
{
    psys.SetPosition(bulletPos);
    psys.Emit(8);

    return Impact(asteroid, damage);
}

void Asteroids::Update(Player& player, float dt)
{
    if (player.IsAlive()) Spawner(player, dt);

    for (int8_t i = count-1; i >= 0; i--)
    {
        Asteroid &asteroid = asteroids[i];

        if (!player.PointOnActiveArea(asteroid.position))
        {
            asteroid = asteroids[--count];
            continue;
        }

        if (!asteroid.Update(dt))
        {
            asteroid = asteroids[--count];
            continue;
        }

        if (!asteroid.alive)
        {
            continue;
        }

        if (!player.PointOnCollideArea(asteroid.position))
        {
            continue;
        }

        // Simple naive detection between asteroids, is not optimized
        for (int8_t j = i - 1; j >= 0; j--)
        {
            float penetration = asteroid.CheckCollision(asteroids[j]);

            if (penetration > 0)
            {
                // Calcul du rebond des deux asteroides (TODO: Improve bounce, the approach is not correct)
                raylib::Vector2 n = (asteroid.position - asteroids[j].position).Normalize();
                asteroids[j].velocity = asteroids[j].velocity.Reflect(-n);
                asteroid.velocity = asteroid.velocity.Reflect(n);

                // Réglage de la position pour annulé la pénétration
                asteroid.position += n * penetration;

                // Dégats d'impact entre les deux asteroids
                Impact(asteroid, 10);
                Impact(asteroids[j], 10);

                // Emission de particules suite à la collision
                psys.SetPosition((asteroid.position + asteroids[j].position) * 0.5f);
                psys.Emit(32);
            }
        }

        if (player.IsAlive())
        {
            // Collision detection with player
            float playerPen = asteroid.CheckCollision(
                player.GetPosition(), player.GetCollisionRadius());

            if (playerPen > 0)
            {
                player.Translate((player.GetPosition() - asteroid.position).Normalize() * playerPen);
                player.SetVelocity(player.GetVelocity().Negate());

                Impact(asteroid, 5);
                player.ShakeImpact();
                player.RemLife(10);

                psys.SetPosition((asteroid.position + player.GetPosition()) * 0.5f);
                psys.Emit(16);
            }
        }
    }
}

void Asteroids::Draw(const raylib::Texture* tilesheet)
{
    for (uint8_t i = 0; i < count; i++)
    {
        asteroids[i].Draw(tilesheet);
    }
}
