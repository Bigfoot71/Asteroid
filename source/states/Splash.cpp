#include "states/Splash.hpp"

#include "components/SplashTitle.hpp"
#include "settings.hpp"

using namespace rf;
using namespace Asteroid;

/* SplashEntity Scripts */

namespace {

    /**
        TODO: The use of static variables is a bad idea, especially in terms of reusability. It would be cleaner
            to create a more elaborate object system for these behavioral functions.
     */

    void Behavior_Alien_Phase3(SplashEntity& self, const SplashEntity::BulletAdder& addBullet)
    {
        static uint8_t shoted[5]{};
        static uint8_t count = 0;

        if (count < 5 && self.pos.y > 112.5f)
        {
            for (uint8_t i = 0; i < count; i++) if (shoted[i] == self.id) return;
            addBullet(self.pos, { 0, 900 }, SplashEntity::Type::Human);
            shoted[count++] = self.id;
        }
    }

    void Behavior_Human_Phase14(SplashEntity& self, const SplashEntity::BulletAdder& addBullet)
    {
        static uint8_t shoted[42]{};
        static uint8_t count = 0;

        if (count < 42 && self.pos.y < BaseHeight-64.0f)
        {
            for (uint8_t i = 0; i < count; i++) if (shoted[i] == self.id) return;
            addBullet(self.pos, { 0, -640 }, SplashEntity::Type::Alien);
            shoted[count++] = self.id;
        }
    }

    void Behavior_Alien_Phase14(SplashEntity& self, const SplashEntity::BulletAdder& addBullet)
    {
        static uint8_t shoted[42]{};
        static uint8_t count = 0;

        if (count < 42 && self.pos.y > 64.0f)
        {
            for (uint8_t i = 0; i < count; i++) if (shoted[i] == self.id) return;
            addBullet(self.pos, { 0, 640 }, SplashEntity::Type::Human);
            shoted[count++] = self.id;
        }
    }

}

/* STATE */

void Splash::Enter()
{
    bgPos = Vector2{ 0, 0 };
    bgSpeed = 64;

    font = app->assetManager.Get<raylib::Font>("font");
    tilesheet = app->assetManager.Get<raylib::Texture>("tilesheet");

    entities.Init();
    psys.SetVelocity({ -256, -256 }, { 256, 256 });
    title.Init(Vector2{ BaseWidth/2.f, -21 }, { BaseWidth/2.f, BaseHeight/2.f }, 0);

    state = 0;
    phaseFlash = 1.0f;
    phaseTitle17 = 0.0f;
    phaseTitle18 = 0.0f;
    phaseSubTitle18 = 0.0f;

    app->musicManager.SetLooping(true);
    app->musicManager.Play("Crush");

    exit = false;
}

void Splash::Update(float dt)
{
    if (exit) app->musicManager.SetVolume(utils::SmoothToZero(app->musicManager.GetVolume(), 0.175f, dt));

    else if (!app->OnTransition() && ((state == 18 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsGestureDetected(GESTURE_DOUBLETAP)))
    {
        app->Transition("game-state-loading", 0.75f, app->assetManager.Get<raylib::Shader>("shader transition game"));
        exit = true;
    }

    bgPos += { 0, -1 * bgSpeed * dt };
    background.Update(bgPos);
    psys.Update(dt);

    switch (state)
    {
        case 0: {
            if (!title.Update(dt)) state++;
        } break;

        case 1: {

            if (phaseFlash > -0.9f)
            {
                const float inc = 6 * std::abs(phaseFlash+1);
                phaseFlash -= inc * dt, bgSpeed += inc * 128 * dt;

                if (phaseFlash <= -0.9f)
                {
                    entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f, BaseHeight+64 }, { BaseWidth/2.f, -64 }, { 0, 64, 64, 64 });
                    entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f-128, BaseHeight+128 }, { BaseWidth/2.f-64, -64 }, { 0, 0, 64, 64 });
                    entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f+128, BaseHeight+128 }, { BaseWidth/2.f+64, -64 }, { 0, 0, 64, 64 });
                    entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f-256, BaseHeight+256 }, { BaseWidth/2.f-128, -64 }, { 0, 0, 64, 64 });
                    entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f+256, BaseHeight+256 }, { BaseWidth/2.f+128, -64 }, { 0, 0, 64, 64 });
                }

                return;
            }

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f, BaseHeight+64 }, { BaseWidth/2.f, -64 }, { 0, 64, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f-64, BaseHeight+128 }, { BaseWidth/2.f-128, -64 }, { 0, 0, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f+64, BaseHeight+128 }, { BaseWidth/2.f+128, -64 }, { 0, 0, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f-128, BaseHeight+256 }, { BaseWidth/2.f-256, -64 }, { 0, 0, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f+128, BaseHeight+256 }, { BaseWidth/2.f+256, -64 }, { 0, 0, 64, 64 });

                state++;
            }

        } break;

        case 2: {

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                entities.Add(1.25f, SplashEntity::Type::Human, { BaseWidth/2.f, BaseHeight+64 }, { BaseWidth/2.f, -64 }, { 0, 64, 64, 64 });
                entities.Add(1.25f, SplashEntity::Type::Human, { BaseWidth/2.f-64, BaseHeight+128 }, { BaseWidth/2.f-64, -64 }, { 0, 0, 64, 64 });
                entities.Add(1.25f, SplashEntity::Type::Human, { BaseWidth/2.f+64, BaseHeight+128 }, { BaseWidth/2.f+64, -64 }, { 0, 0, 64, 64 });
                entities.Add(1.25f, SplashEntity::Type::Human, { BaseWidth/2.f-128, BaseHeight+256 }, { BaseWidth/2.f-128, -64 }, { 0, 0, 64, 64 });
                entities.Add(1.25f, SplashEntity::Type::Human, { BaseWidth/2.f+128, BaseHeight+256 }, { BaseWidth/2.f+128, -64 }, { 0, 0, 64, 64 });

                entities.Add(2.0f, SplashEntity::Type::Alien, { BaseWidth/2.f, -64 }, { BaseWidth/2.f, BaseHeight+64 }, { 320, 0, 64, 64 }, Behavior_Alien_Phase3);
                entities.Add(2.0f, SplashEntity::Type::Alien, { BaseWidth/2.f-64, -64 }, { BaseWidth/2.f-64, BaseHeight+128 }, { 256, 0, 64, 64 }, Behavior_Alien_Phase3);
                entities.Add(2.0f, SplashEntity::Type::Alien, { BaseWidth/2.f+64, -64 }, { BaseWidth/2.f+64, BaseHeight+128 }, { 256, 0, 64, 64 }, Behavior_Alien_Phase3);
                entities.Add(2.0f, SplashEntity::Type::Alien, { BaseWidth/2.f-128, -64 }, { BaseWidth/2.f-128, BaseHeight+256 }, { 256, 0, 64, 64 }, Behavior_Alien_Phase3);
                entities.Add(2.0f, SplashEntity::Type::Alien, { BaseWidth/2.f+128, -64 }, { BaseWidth/2.f+128, BaseHeight+256 }, { 256, 0, 64, 64 }, Behavior_Alien_Phase3);

                state++;
            }

        } break;

        case 3: {

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                entities.Add(2.0f, SplashEntity::Type::Alien, { BaseWidth/2.f, BaseHeight+64 }, { BaseWidth/2.f, -128 }, { 320, 0, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Alien, { BaseWidth/2.f-64, BaseHeight+128 }, { BaseWidth/2.f-64, -128 }, { 256, 0, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Alien, { BaseWidth/2.f+64, BaseHeight+128 }, { BaseWidth/2.f+64, -128 }, { 256, 0, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Alien, { BaseWidth/2.f-128, BaseHeight+256 }, { BaseWidth/2.f-128, -128 }, { 256, 0, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Alien, { BaseWidth/2.f+128, BaseHeight+256 }, { BaseWidth/2.f+128, -128 }, { 256, 0, 64, 64 });

                entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f, BaseHeight+576 }, { BaseWidth/2.f, -64 }, { 64, 64, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f-64, BaseHeight+640 }, { BaseWidth/2.f-64, -64 }, { 128, 64, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f+64, BaseHeight+640 }, { BaseWidth/2.f+64, -64 }, { 128, 64, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f-128, BaseHeight+768 }, { BaseWidth/2.f-128, -64 }, { 128, 64, 64, 64 });
                entities.Add(2.0f, SplashEntity::Type::Human, { BaseWidth/2.f+128, BaseHeight+768 }, { BaseWidth/2.f+128, -64 }, { 128, 64, 64, 64 });

                state++;
            }

        } break;

        case 4: {

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                psys.SetVelocity({ -256, 8 }, { 256, 256 });
                for (float x = BaseWidth/2.f-128; x < BaseWidth/2.f+128; x += 16.0f)
                {
                    psys.SetPosition({ x, 0 });
                    psys.Emit(8);
                }
                state++;
            }

        } break;

        case 5: {

            if (psys.Count() == 0)
            {
                entities.Add(1.33f, SplashEntity::Type::Human, { BaseWidth/2.f, -64 }, { BaseWidth/2.f, BaseHeight+128 }, { 64, 64, 64, 64 });
                entities.Add(1.33f, SplashEntity::Type::Human, { BaseWidth/2.f-64, -128 }, { BaseWidth/2.f-64, BaseHeight+128 }, { 128, 64, 64, 64 });
                entities.Add(1.33f, SplashEntity::Type::Human, { BaseWidth/2.f+64, -128 }, { BaseWidth/2.f+64, BaseHeight+128 }, { 128, 64, 64, 64 });
                entities.Add(1.33f, SplashEntity::Type::Human, { BaseWidth/2.f-128, -256 }, { BaseWidth/2.f-128, BaseHeight+128 }, { 128, 64, 64, 64 });
                entities.Add(1.33f, SplashEntity::Type::Human, { BaseWidth/2.f+128, -256 }, { BaseWidth/2.f+128, BaseHeight+128 }, { 128, 64, 64, 64 });

                entities.Add(1.33f, SplashEntity::Type::Alien, { BaseWidth/2.f, -576 }, { BaseWidth/2.f, BaseHeight+64 }, { 384, 0, 64, 64 });
                entities.Add(1.33f, SplashEntity::Type::Alien, { BaseWidth/2.f-64, -640 }, { BaseWidth/2.f-64, BaseHeight+64 }, { 448, 0, 64, 64 });
                entities.Add(1.33f, SplashEntity::Type::Alien, { BaseWidth/2.f+64, -640 }, { BaseWidth/2.f+64, BaseHeight+64 }, { 448, 0, 64, 64 });
                entities.Add(1.33f, SplashEntity::Type::Alien, { BaseWidth/2.f-128, -768 }, { BaseWidth/2.f-128, BaseHeight+64 }, { 448, 0, 64, 64 });
                entities.Add(1.33f, SplashEntity::Type::Alien, { BaseWidth/2.f+128, -768 }, { BaseWidth/2.f+128, BaseHeight+64 }, { 448, 0, 64, 64 });

                state++;
            }

        } break;

        case 6: {

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                psys.SetLifeTime(0.75f);
                psys.SetVelocity({ -256, -256 }, { 256, -8 });

                for (float x = BaseWidth/2.f-128; x < BaseWidth/2.f+128; x += 16.0f)
                {
                    psys.SetPosition({ x, BaseHeight });
                    psys.Emit(8);
                }
                state++;
            }

        } break;

        case 7: {

            if (psys.Count() == 0)
            {
                for (uint8_t i = 0; i < 3; i++)
                {
                    const float s = i*512, t = i*.6180f;

                    entities.Add(1.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f, BaseHeight+64+s }, { BaseWidth/2.f, -64 }, { 64, 64, 64, 64 });
                    entities.Add(1.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f-64, BaseHeight+128+s }, { BaseWidth/2.f-128, -64 }, { 128, 64, 64, 64 });
                    entities.Add(1.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f+64, BaseHeight+128+s }, { BaseWidth/2.f+128, -64 }, { 128, 64, 64, 64 });
                    entities.Add(1.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f-128, BaseHeight+192+s }, { BaseWidth/2.f-192, -64 }, { 128, 64, 64, 64 });
                    entities.Add(1.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f+128, BaseHeight+192+s }, { BaseWidth/2.f+192, -64  }, { 128, 64, 64, 64 });
                    entities.Add(1.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f-192, BaseHeight+256+s }, { BaseWidth/2.f-256, -64  }, { 128, 64, 64, 64 });
                    entities.Add(1.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f+192, BaseHeight+256+s }, { BaseWidth/2.f+256, -64  }, { 128, 64, 64, 64 });
                }

                state++;
            }

        } break;

        case 8: {

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                psys.SetLifeTime(0.8f);
                psys.SetVelocity({ -256, 8 }, { 256, 256 });

                for (float x = BaseWidth/2.f-256; x < BaseWidth/2.f+256; x += 8.0f)
                {
                    psys.SetPosition({ x, 0 });
                    psys.Emit(3);
                }
                state++;
            }

        } break;

        case 9: {

            static uint8_t count = 0;

            if (psys.Count() == 0)
            {
                psys.SetLifeTime(0.75f);
                psys.SetVelocity({ -256, 8 }, { 256, 256 });

                for (float x = BaseWidth/2.f-256; x < BaseWidth/2.f+256; x += 8.0f)
                {
                    psys.SetPosition({ x, 0 });
                    psys.Emit(3);
                }

                if ((++count) == 2)
                {
                    for (uint8_t i = 0; i < 6; i++)
                    {
                        const float s = i*512, t = i*.6180f;

                        entities.Add(1.25f+t, SplashEntity::Type::Alien, { BaseWidth/2.f, -64-s }, { BaseWidth/2.f, BaseHeight+64 }, { 384, 64, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Alien, { BaseWidth/2.f-64, -128-s }, { BaseWidth/2.f-128, BaseHeight+64 }, { 192, 64, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Alien, { BaseWidth/2.f+64, -128-s }, { BaseWidth/2.f+128, BaseHeight+64 }, { 192, 64, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Alien, { BaseWidth/2.f-128, -192-s }, { BaseWidth/2.f-192, BaseHeight+64 }, { 192, 64, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Alien, { BaseWidth/2.f+128, -192-s }, { BaseWidth/2.f+192, BaseHeight+64  }, { 192, 64, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Alien, { BaseWidth/2.f-192, -256-s }, { BaseWidth/2.f-256, BaseHeight+64  }, { 192, 64, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Alien, { BaseWidth/2.f+192, -256-s }, { BaseWidth/2.f+256, BaseHeight+64  }, { 192, 64, 64, 64 });
                    }

                    state++;
                }
            }

        } break;

        case 10: {

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                psys.SetLifeTime(0.3f);
                psys.SetVelocity({ -384, -384 }, { 384, -16 });

                for (float x = BaseWidth/2.f-256; x < BaseWidth/2.f+256; x += 8.0f)
                {
                    psys.SetPosition({ x, BaseHeight });
                    psys.Emit(3);
                }
                state++;
            }

        } break;

        case 11: {

            static uint8_t count = 0;

            if (psys.Count() == 0)
            {
                psys.SetLifeTime(0.6f);
                psys.SetVelocity({ -384, -384 }, { 384, -16 });

                for (float x = BaseWidth/2.f-256; x < BaseWidth/2.f+256; x += 8.0f)
                {
                    psys.SetPosition({ x, BaseHeight });
                    psys.Emit(3);
                }

                if ((++count) == 3)
                {
                    for (uint8_t i = 0; i < 6; i++)
                    {
                        const float s = i*512, t = i*.6180f;

                        entities.Add(1.25f+t, SplashEntity::Type::Human, { BaseWidth/2.f, BaseHeight+64+s }, { BaseWidth/2.f, -64 }, { 128, 128, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Human, { BaseWidth/2.f-64, BaseHeight+128+s }, { BaseWidth/2.f-128, -64 }, { 64, 128, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Human, { BaseWidth/2.f+64, BaseHeight+128+s }, { BaseWidth/2.f+128, -64 }, { 64, 128, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Human, { BaseWidth/2.f-128, BaseHeight+192+s }, { BaseWidth/2.f-192, -64 }, { 64, 128, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Human, { BaseWidth/2.f+128, BaseHeight+192+s }, { BaseWidth/2.f+192, -64  }, { 64, 128, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Human, { BaseWidth/2.f-192, BaseHeight+256+s }, { BaseWidth/2.f-256, -64  }, { 64, 128, 64, 64 });
                        entities.Add(1.25f+t, SplashEntity::Type::Human, { BaseWidth/2.f+192, BaseHeight+256+s }, { BaseWidth/2.f+256, -64  }, { 64, 128, 64, 64 });
                    }

                    state++;
                }
            }

        } break;

        case 12: {

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                psys.SetLifeTime(0.3f);
                psys.SetVelocity({ -384, 16 }, { 384, 384 });

                for (float x = BaseWidth/2.f-256; x < BaseWidth/2.f+256; x += 8.0f)
                {
                    psys.SetPosition({ x, 0 });
                    psys.Emit(3);
                }
                state++;
            }

        } break;

        case 13: {

            static uint8_t count = 0;

            if (psys.Count() == 0)
            {
                if ((++count) <= 2)
                {
                    psys.SetLifeTime(0.6f);
                    psys.SetVelocity({ -384, 16 }, { 384, 384 });

                    for (float x = BaseWidth/2.f-256; x < BaseWidth/2.f+256; x += 8.0f)
                    {
                        psys.SetPosition({ x, 0 });
                        psys.Emit(3);
                    }
                }
                else
                {
                    for (uint8_t i = 0; i < 6; i++)
                    {
                        const float s = i*128, t = i*.6180f;

                        entities.Add(3.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f, BaseHeight+64+s }, { BaseWidth/2.f, -64 }, { 128, 128, 64, 64 }, Behavior_Human_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f-64, BaseHeight+128+s }, { BaseWidth/2.f-64, -64 }, { 64, 128, 64, 64 }, Behavior_Human_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f+64, BaseHeight+128+s }, { BaseWidth/2.f+64, -64 }, { 64, 128, 64, 64 }, Behavior_Human_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f-128, BaseHeight+256+s }, { BaseWidth/2.f-128, -64 }, { 64, 128, 64, 64 }, Behavior_Human_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f+128, BaseHeight+256+s }, { BaseWidth/2.f+128, -64 }, { 64, 128, 64, 64 }, Behavior_Human_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f-192, BaseHeight+512+s }, { BaseWidth/2.f-192, -64 }, { 64, 128, 64, 64 }, Behavior_Human_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Human, { BaseWidth/2.f+192, BaseHeight+512+s }, { BaseWidth/2.f+192, -64 }, { 64, 128, 64, 64 }, Behavior_Human_Phase14);

                        entities.Add(3.0f+t, SplashEntity::Type::Alien, { BaseWidth/2.f, -64-s }, { BaseWidth/2.f, BaseHeight+64 }, { 384, 64, 64, 64 }, Behavior_Alien_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Alien, { BaseWidth/2.f-64, -128-s }, { BaseWidth/2.f-64, BaseHeight+64 }, { 192, 64, 64, 64 }, Behavior_Alien_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Alien, { BaseWidth/2.f+64, -128-s }, { BaseWidth/2.f+64, BaseHeight+64 }, { 192, 64, 64, 64 }, Behavior_Alien_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Alien, { BaseWidth/2.f-128, -256-s }, { BaseWidth/2.f-128, BaseHeight+64 }, { 192, 64, 64, 64 }, Behavior_Alien_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Alien, { BaseWidth/2.f+128, -256-s }, { BaseWidth/2.f+128, BaseHeight+64 }, { 192, 64, 64, 64 }, Behavior_Alien_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Alien, { BaseWidth/2.f-192, -512-s }, { BaseWidth/2.f-192, BaseHeight+64 }, { 192, 64, 64, 64 }, Behavior_Alien_Phase14);
                        entities.Add(3.0f+t, SplashEntity::Type::Alien, { BaseWidth/2.f+192, -512-s }, { BaseWidth/2.f+192, BaseHeight+64 }, { 192, 64, 64, 64 }, Behavior_Alien_Phase14);
                    }

                    state++;
                }
            } break;

        } break;

        case 14: {

            static uint8_t count = 0;

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f, BaseHeight+64 }, { -BaseWidth/4.f, -64 }, { 128, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f-64, BaseHeight+128 }, { -BaseWidth/4.f-64, -64 }, { 64, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f+64, BaseHeight+128 }, { -BaseWidth/4.f+64, -64 }, { 64, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f-128, BaseHeight+256 }, { -BaseWidth/4.f-128, -64 }, { 64, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f+128, BaseHeight+256 }, { -BaseWidth/4.f+128, -64 }, { 64, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f-192, BaseHeight+512 }, { -BaseWidth/4.f-192, -64 }, { 64, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f+192, BaseHeight+512 }, { -BaseWidth/4.f+192, -64 }, { 64, 128, 64, 64 });

                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f, -64 }, { BaseWidth+BaseWidth/4.f, BaseHeight+64 }, { 384, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f-64, -128 }, { BaseWidth+BaseWidth/4.f-64, BaseHeight+64 }, { 192, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f+64, -128 }, { BaseWidth+BaseWidth/4.f+64, BaseHeight+64 }, { 192, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f-128, -256 }, { BaseWidth+BaseWidth/4.f-128, BaseHeight+64 }, { 192, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f+128, -256 }, { BaseWidth+BaseWidth/4.f+128, BaseHeight+64 }, { 192, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f-192, -512 }, { BaseWidth+BaseWidth/4.f-192, BaseHeight+64 }, { 192, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f+192, -512 }, { BaseWidth+BaseWidth/4.f+192, BaseHeight+64 }, { 192, 64, 64, 64 });

                if ((++count) == 2) state++;
            }

        } break;

        case 15: {

            static uint8_t count = 0;

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f, BaseHeight+64 }, { BaseWidth+BaseWidth/4.f, -64 }, { 128, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f-64, BaseHeight+128 }, { BaseWidth+BaseWidth/4.f-64, -64 }, { 64, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f+64, BaseHeight+128 }, { BaseWidth+BaseWidth/4.f+64, -64 }, { 64, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f-128, BaseHeight+256 }, { BaseWidth+BaseWidth/4.f-128, -64 }, { 64, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f+128, BaseHeight+256 }, { BaseWidth+BaseWidth/4.f+128, -64 }, { 64, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f-192, BaseHeight+512 }, { BaseWidth+BaseWidth/4.f-192, -64 }, { 64, 128, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Human, { BaseWidth/2.f+192, BaseHeight+512 }, { BaseWidth+BaseWidth/4.f+192, -64 }, { 64, 128, 64, 64 });

                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f, -64 }, { -BaseWidth/4.f, BaseHeight+64 }, { 384, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f-64, -128 }, { -BaseWidth/4.f-64, BaseHeight+64 }, { 192, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f+64, -128 }, { -BaseWidth/4.f+64, BaseHeight+64 }, { 192, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f-128, -256 }, { -BaseWidth/4.f-128, BaseHeight+64 }, { 192, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f+128, -256 }, { -BaseWidth/4.f+128, BaseHeight+64 }, { 192, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f-192, -512 }, { -BaseWidth/4.f-192, BaseHeight+64 }, { 192, 64, 64, 64 });
                entities.Add(1.5f, SplashEntity::Type::Alien, { BaseWidth/2.f+192, -512 }, { -BaseWidth/4.f+192, BaseHeight+64 }, { 192, 64, 64, 64 });

                if ((++count) == 2) state++;
            }

        } break;

        case 16: {

            entities.Update(psys, dt);

            if (entities.Count() == 0)
            {
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f, BaseHeight+64 }, { BaseWidth/2.f, -128 }, { 128, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f-32, BaseHeight+96 }, { BaseWidth/2.f-128, -64 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f+32, BaseHeight+96 }, { BaseWidth/2.f+128, -64 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f-64, BaseHeight+128 }, { BaseWidth/2.f-256, -128 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f+64, BaseHeight+128 }, { BaseWidth/2.f+256, -128 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f-96, BaseHeight+192 }, { BaseWidth/2.f-384, -64 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f+96, BaseHeight+192 }, { BaseWidth/2.f+384, -64 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f-128, BaseHeight+256 }, { BaseWidth/2.f-512, -128 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f+128, BaseHeight+256 }, { BaseWidth/2.f+512, -128 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f-160, BaseHeight+384 }, { BaseWidth/2.f-640, -64 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f+160, BaseHeight+384 }, { BaseWidth/2.f+640, -64 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f-192, BaseHeight+512 }, { BaseWidth/2.f-768, -128 }, { 64, 128, 64, 64 });
                entities.Add(10.5f, SplashEntity::Type::Human, { BaseWidth/2.f+192, BaseHeight+512 }, { BaseWidth/2.f+768, -128 }, { 64, 128, 64, 64 });

                state++;
            }

        } break;

        case 17: {
            entities.Update(psys, dt);
            if (entities.Count() == 0) state++;
        } break;

        default:
            break;
    }
}

void Splash::Draw(const rf::core::Renderer& target)
{
    background.Draw();
    psys.Draw();

    switch (state)
    {
        case 0: {
            title.Draw(tilesheet, font);
        } break;

        case 1: {
            if (phaseFlash > 0.0f)
            {
                title.Draw(tilesheet, font);
            }
            if (phaseFlash > -0.9f)
            {
                DrawRectangle(0, 0, BaseWidth, BaseHeight, ColorAlpha(WHITE, 1.0f-std::abs(phaseFlash)));
                return;
            }
        }

        case 2:
        case 3:
        case 4:
        case 6:
        case 8:
        case 10:
        case 12:
        case 14:
        case 15:
        case 16:
        case 17: {
            entities.Draw(tilesheet);
        } break;

        default:
            break;
    }

    if (state == 17)
    {
        constexpr Vector2 flagStart{ BaseWidth/2.f, BaseHeight+256 };
        constexpr Vector2 flagEnd{ BaseWidth/2.f, BaseHeight/2.f };
        constexpr float invDuration = 1.0f / 10.5f;

        phaseTitle17 = std::min(phaseTitle17 + GetFrameTime() * invDuration, 1.0f);
        float v = phaseTitle17 * phaseTitle17 * (3.0f - 2.0f * phaseTitle17);

        Vector2 pos = {
            flagStart.x + v * (flagEnd.x - flagStart.x),
            flagStart.y + v * (flagEnd.y - flagStart.y)
        };

        SplashTitle::DrawWave(tilesheet, pos);
    }
    else if (state == 18)
    {
        constexpr Vector2 flagStart{ BaseWidth/2.f, BaseHeight/2.f };
        constexpr Vector2 flagEnd{ BaseWidth/2.f, BaseHeight/5.f };

        phaseTitle18 = std::min(phaseTitle18 + GetFrameTime(), 1.0f);
        float v = phaseTitle18 * phaseTitle18 * (3.0f - 2.0f * phaseTitle18);

        Vector2 pos = {
            flagStart.x + v * (flagEnd.x - flagStart.x),
            flagStart.y + v * (flagEnd.y - flagStart.y)
        };

        SplashTitle::DrawWave(tilesheet, pos, 1.0f + 0.125f * v);

        phaseSubTitle18 += (phaseTitle18 == 1.0f ? 3.0f * GetFrameTime() : 0.0f);
        float s = 1.0f + (1.0f + std::sin(phaseSubTitle18)) * 0.0625f;
        float fontSize = 24 * s, spacing = 2.4f * s;

        constexpr char txt[] = "Click to play!";
        font->DrawText(txt, { (BaseWidth - font->MeasureText(txt, fontSize, spacing).x) * 0.5f, BaseHeight * 0.75f - 12 }, fontSize, spacing, ColorAlpha(WHITE, v));
    }
}
