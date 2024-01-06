#include "entities/Player.hpp"
#include "entities/Bullet.hpp"
#include "settings.hpp"
#include "utils.hpp"
#include <cstdint>

using namespace rf;
using namespace Asteroid;

constexpr char fragLifeBar[] =
#ifdef PLATFORM_DESKTOP
    "#version 330\n"
    "in vec2 fragTexCoord;"
#else
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 fragTexCoord;"
#endif
    "uniform sampler2D texture0;"
    "uniform float normLife;"
    "uniform float time;"
#ifdef PLATFORM_DESKTOP
    "out vec4 finalColor;"
#endif
    "void main()"
    "{"
        "if (fragTexCoord.x > normLife) discard;"
        "vec3 col = vec3(1.-normLife,normLife,0.);"
        "float offset = fract(fragTexCoord.x-time);"
        "col.rg *= (sin(offset*6.2831)+3.)*.5;"
#   ifdef PLATFORM_DESKTOP
        "finalColor = texture(texture0,fragTexCoord)*vec4(col,1.);"
#   else
        "gl_FragColor = texture2D(texture0,fragTexCoord)*vec4(col,1.);"
#   endif
    "}";

/* PRIVATE */

void Player::UpdateMovement(float dt)
{
    // Update engine velocity applied to spaceship
    // NOTE: The engine is unusable following a big impact
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && shakeCameraTime <= 0)
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
    trail.Update(position, velEngine / maxVelEngine);
}

void Player::UpdateRotation(float dt)
{
    // Calculation of the progressive rotation of the spaceship
    raylib::Vector2 dc = camera.GetScreenToWorld(app->GetMousePosition()) - GetPosition();
    float da = utils::GetDeltaRad(rotation, std::atan2(dc.y, dc.x) + PI/2);
    float prevRotation = rotation;
    rotation += da * 6 * dt;

    // Calculation of the depth sidePitch
    sidePitch = (GetMouseDelta().x || GetMouseDelta().y)
        ? std::clamp(sidePitch + (rotation - prevRotation), -PI/6, PI/6)
        : sidePitch = sidePitch * 0.975f;
}

void Player::UpdateCamera(float dt)
{
    camera.zoom = 1 - GetEnginePower() * 0.125f;
    camera.target = position;

    if (shakeCameraTime > 0)
    {
        camera.target.x += 16 * std::sin(shakeCameraTime);
        shakeCameraTime -= 4*PI * dt; // 0.25 secondes
    }
}

void Player::UpdateBlaster(Bullets<MaxBullets>& bullets, Asteroids& asteroids, Enemies& enemies, float dt)
{
    // Bullets update
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        if ((fireTimer += dt) >= fireRate)
        {
            app->assetManager.Get<raylib::Sound>("player-shoot")->Play();

            fireTimer = 0;
            const float th = rotation-PI/2;
            const raylib::Vector2 dir(std::cos(th), std::sin(th));

            if (blasterNum == 1) // NOTE: It is assumed that ships with one blaster all have it in the center of the ship
            {
                bullets.Add(Bullet::Owner::Player, position, dir, velocity, bulletDamage);
            }
            else
            {
                for (uint8_t i = 0; i < blasterNum; i++)
                {
                    bullets.Add(Bullet::Owner::Player, position + blasterPos[i].Rotate(rotation), dir, velocity, bulletDamage);
                }
            }
        }
    }
}

/* PUBLIC */

Player::Player(rf::gfx2d::ParticleSystem& psys) : psys(psys)
{
    shaderLifeBar = LoadShaderFromMemory(nullptr, fragLifeBar);
    locNormLife = GetShaderLocation(shaderLifeBar, "normLife");
    locTime = GetShaderLocation(shaderLifeBar, "time");
}

Player::~Player()
{
    UnloadShader(shaderLifeBar);
}

void Player::Init(rf::core::App* app)
{
    const Save *save = app->saveManager->Get<Save>();
    const PlayerStats &stats = save->playerStats[save->selectedPlayer];
    const PlayerConstants &playerConsts = playerConstants[save->selectedPlayer];

    this->app = app;

    camera.offset = { 400, 225 };
    camera.target = { 0, 0 };
    camera.rotation = 0;
    camera.zoom = 1;

    shakeCameraTime = 0;

    source = playerConsts.source;
    gameTimer = 0;

    position = Vector2{};
    velocity = Vector2{};
    rotation = 0;
    sidePitch = 0;

    velEngine = 0.0f;

    collideRadius = playerConsts.collideRadius;
    collideRadiusSq = playerConsts.collideRadiusSq;

    acceleration = stats.acceleration;
    maxVelEngine = stats.maxVelEngine;
    fireRate = 1.0f/stats.bulletPerSec;
    fireTimer = 0;

    bulletDamage = stats.damage;
    blasterNum = playerConsts.blasterNum;
    std::copy(playerConsts.blasterPos, playerConsts.blasterPos+blasterNum, blasterPos);

    regenLife = stats.regen;
    regenLifeTime = 1.0f;

    maxLife = life = stats.life;
    money = displayedMoney = 0;

    asteroidsCounter = 0;
    enemiesCounter = 0;

    cheated = false;

    trail.Init();
}

void Player::Update(Bullets<MaxBullets>& bullets, Asteroids& asteroids, Enemies& enemies, float dt)
{
    if (IsAlive())
    {
        UpdateMovement(dt);
        UpdateRotation(dt);
        UpdateCamera(dt);

        UpdateBlaster(bullets, asteroids, enemies, dt);

        // Update life regen
        if (life < maxLife)
        {
            regenLifeTime = std::max(regenLifeTime - dt, 0.0f);

            if (regenLifeTime == 0.0f)
            {
                life = std::min(life + regenLife * dt, maxLife);
            }
        }

        // Displayed HUD values update
        if (displayedMoney < money) displayedMoney++;
        gameTimer += dt;
    }
}

void Player::Draw(const raylib::Texture* tilesheet) const
{
    if (IsAlive())
    {
        trail.Draw(16, { 0, 0, 255, 191 });
        utils::DrawPerspective(tilesheet, source, position, rotation, sidePitch, 1.0f, cheated ? PINK : WHITE);
    }
}

void Player::DrawHUD(const raylib::Texture* tilesheet, float nAlpha) const
{
    uint8_t alpha = 200 * nAlpha;

    static raylib::Text text("", 16, WHITE, *app->assetManager.Get<raylib::Font>("font"), 1.6f);
    text.color = { 255, 255, 255, alpha };

    // DRAW HUD LEFT
    tilesheet->Draw(Rectangle{ 0, 256, 384, 64 }, { BaseWidth * 0.175f, 24, 256, 48 }, { 128, 24 }, 180, { 255, 255, 255, alpha });
    text.text = TextFormat("TIME: %s", utils::FormatTime(gameTimer));
    text.Draw(Vector2{ BaseWidth * 0.175f, 24 }, 0, text.MeasureEx() * 0.5f);

    // DRAW HUD RIGHT
    tilesheet->Draw(Rectangle{ 0, 256, 384, 64 }, { BaseWidth * 0.825f, 24, 256, 48 }, { 128, 24 }, 180, { 255, 255, 255, alpha });
    text.text = TextFormat("MONEY: %i", displayedMoney);
    text.Draw(Vector2{ BaseWidth * 0.825f, 24 }, 0, text.MeasureEx() * 0.5f);

    // DRAW LIFE BAR
    tilesheet->Draw(Rectangle{ 0, 256, 384, 64 }, { BaseWidth * 0.5f, 24, 256, 48 }, { 128, 24 }, 0, { 255, 255, 255, alpha });

    const float time = GetTime(), normLife = static_cast<float>(life)/maxLife;
    SetShaderValue(shaderLifeBar, locTime, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shaderLifeBar, locNormLife, &normLife, SHADER_UNIFORM_FLOAT);

    BeginShaderMode(shaderLifeBar);
        tilesheet->Draw(Rectangle{ 0, 320, 384, 64 }, { BaseWidth * 0.5f, 24, 256, 48 }, { 128, 24 }, 0, ColorAlpha(WHITE, nAlpha));
    EndShaderMode();
}

void Player::RemLife(uint8_t value)
{
    auto snd = app->assetManager.Get<raylib::Sound>("player-hit");
    if (!snd->IsPlaying()) snd->Play();

    regenLifeTime = 1.0f;

    life -= value;
    if (life > 0) return;

    psys.SetPosition(position);
    psys.Emit(32);
}

void Player::BulletImpact(const Vector2& bulletPos, uint16_t damage)
{
    psys.SetPosition(bulletPos);
    psys.Emit(8);

    RemLife(damage);
}

void Player::Cheat()
{
    const auto& consts = playerConstants[8];

    source          = consts.source;
    collideRadius   = consts.collideRadius;
    collideRadiusSq = consts.collideRadiusSq;
    blasterNum      = consts.blasterNum;

    std::memcpy(blasterPos, consts.blasterPos, sizeof(consts.blasterPos));

    maxVelEngine    = 410;
    acceleration    = 205;
    fireRate        = 1.f/15;
    bulletDamage    = 45;
    maxLife         = 250;
    life            = 250;
    regenLife       = 55;

    cheated         = true;
}
