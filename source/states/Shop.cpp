#include "states/Shop.hpp"

#include "entities/Player.hpp"
#include "settings.hpp"
#include "utils.hpp"
#include "save.hpp"

using namespace Asteroid;

// TODO: Rewrite intelligently and reduce complexity / macro abuse

namespace {

    inline constexpr int GetShipPrice(int playerSelected)
    {
        return shipsPrice[playerSelected];
    }

    inline constexpr int GetStatPrice(int playerSelected)
    {
        return 50 * (playerSelected + 2);
    }

    // Macro indicates if a ship stat is at its maximum level

    #define ButtonStat_IsMaxStat(stat) (                                                                                            \
        shop->save->playerStats[shop->playerSelected].stat >= saveOrigin.playerStats[shop->playerSelected].stat + 5 * statsInc.stat \
    )

    // Macro updating the labels of a ButtonStat

    #define ButtonStat_UpdateLabels(stat) ({                                                                                                    \
        if (ButtonStat_IsMaxStat(stat))                                                                                                         \
        {                                                                                                                                       \
            stat.labels[0] = raylib::Text("MAX", 14, WHITE, *shop->font, 1.4f);                                                                 \
            stat.labels[1] = raylib::Text(TextFormat("%i", shop->save->playerStats[shop->playerSelected].stat), 14, WHITE, *shop->font, 1.4f);  \
        }                                                                                                                                       \
        else                                                                                                                                    \
        {                                                                                                                                       \
            stat.labels[0] = raylib::Text(TextFormat("%i", shop->save->playerStats[shop->playerSelected].stat), 14, WHITE, *shop->font, 1.4f);  \
            stat.labels[1] = raylib::Text(TextFormat("%i$", GetStatPrice(shop->playerSelected)), 14, WHITE, *shop->font, 1.4f);                 \
        }                                                                                                                                       \
    })

    // Macro indicating if a ship stat can be purchased ( -> bool )

    #define ButtonStat_IsPurchasable(stat) (                                                        \
        shop->save->playerStats[shop->playerSelected].unlocked && !ButtonStat_IsMaxStat(stat) &&    \
        shop->save->money >= GetStatPrice(shop->playerSelected)                                     \
    )

    // Macro for purchasing a ship stat

    #define ButtonStat_Purchase(stat) ({                                        \
        shop->app->assetManager.Get<raylib::Sound>("purchase")->Play();         \
        shop->save->playerStats[shop->playerSelected].stat += statsInc.stat;    \
        shop->save->money -= GetStatPrice(shop->playerSelected);                \
        shop->UpdateLabels();                                                   \
    })

    // Macro for updating a ButtonStat when clicked

    #define ButtonStat_Update(stat) ({                                      \
        if (ButtonStat_IsPurchasable(stat)) ButtonStat_Purchase(stat);      \
        else shop->app->assetManager.Get<raylib::Sound>("wrong")->Play();   \
    })

}

/* STAT BUTTONS */

void Shop::SB::Update(Shop* shop)
{
    Vector2 mousePosition = shop->app->GetMousePosition();

    if (life.Update(mousePosition))                 ButtonStat_Update(life);
    else if (regen.Update(mousePosition))           ButtonStat_Update(regen);
    else if (maxVelEngine.Update(mousePosition))    ButtonStat_Update(maxVelEngine);
    else if (acceleration.Update(mousePosition))    ButtonStat_Update(acceleration);
    else if (bulletPerSec.Update(mousePosition))    ButtonStat_Update(bulletPerSec);
    else if (damage.Update(mousePosition))          ButtonStat_Update(damage);
}

void Shop::SB::Draw(const Shop* shop, float alpha)
{
    life.Draw(shop->texIcons, { 384, 256, 128, 128 }, alpha);
    regen.Draw(shop->texIcons, { 256, 384, 128, 128 }, alpha);
    maxVelEngine.Draw(shop->texIcons, { 382, 0, 128, 128 }, alpha);
    acceleration.Draw(shop->texIcons, { 382, 128, 128, 128 }, alpha);
    bulletPerSec.Draw(shop->texIcons, { 0, 384, 128, 128 }, alpha);
    damage.Draw(shop->texIcons, { 128, 384, 128, 128 }, alpha);
}

void Shop::SB::SetColor(const Color& color)
{
    for (auto& stat : *this)
    {
        stat.labels[0].color = color;
        stat.labels[1].color = color;
    }
}

void Shop::SB::UpdateLabels(const Shop* shop)
{
    if (shop->save->playerStats[shop->playerSelected].unlocked)
    {
        ButtonStat_UpdateLabels(life);
        ButtonStat_UpdateLabels(regen);
        ButtonStat_UpdateLabels(maxVelEngine);
        ButtonStat_UpdateLabels(acceleration);
        ButtonStat_UpdateLabels(bulletPerSec);
        ButtonStat_UpdateLabels(damage); 
    }
    else
    {
        for (auto& stat : *this)
        {
            stat.labels[0] = raylib::Text("???", 14, WHITE, *shop->font, 1.4f);
            stat.labels[1] = raylib::Text("???", 14, WHITE, *shop->font, 1.4f);
        }
    }
}

void Shop::SB::SetBounds()
{
    life.button.SetBounds({ BaseWidth / 2.0f - 224, playerPosition.y - 128 }, { 64, 64 });
    regen.button.SetBounds({ BaseWidth / 2.0f + 224, playerPosition.y - 128 }, { 64, 64 });
    maxVelEngine.button.SetBounds({ BaseWidth / 2.0f - 224, playerPosition.y }, { 64, 64 });
    acceleration.button.SetBounds({ BaseWidth / 2.0f + 224, playerPosition.y }, { 64, 64 });
    damage.button.SetBounds({ BaseWidth / 2.0f - 224, playerPosition.y + 128 }, { 64, 64 });
    bulletPerSec.button.SetBounds({ BaseWidth / 2.0f + 224, playerPosition.y + 128 }, { 64, 64 });
}

/* SHOP */

void Shop::UpdateLabels()
{
    txtMoney = raylib::Text(TextFormat("Money: %i", save->money), 26, WHITE, *font, 2.6f);
    txtShipPrice = raylib::Text(TextFormat("%i$", GetShipPrice(playerSelected)), 16, WHITE, *font, 1.6f);
    stats.UpdateLabels(this);
}

void Shop::SetAlphaLabels(float alpha)
{
    Color color = ColorAlpha(WHITE, alpha);
    txtShipPrice.color = color;
    stats.SetColor(color);
}

void Shop::Enter()
{
    tilesheet = app->assetManager.Get<raylib::Texture>("tilesheet");
    texIcons = app->assetManager.Get<raylib::Texture>("icons");
    font = app->assetManager.Get<raylib::Font>("font");
    save = app->saveManager->Get<Save>();

    btnSound.SetBounds({ 37, 37 }, { 42, 42 });
    btnFullscreen.SetBounds({ BaseWidth - 37, 37 }, { 42, 42 });

    btnAction.SetBounds({ BaseWidth * 0.5f, BaseHeight * 0.75f }, { 142, 42 });

    btnArrowLeft.SetBounds({ BaseWidth / 2.0f - 128, playerPosition.y }, { 96, 96 });
    btnArrowRight.SetBounds({ BaseWidth / 2.0f + 128, playerPosition.y }, { 96, 96 });

    stats.SetBounds();

    playerSelected = save->selectedPlayer;
    phaseSelectTransition = 1.0f;
    onSelectTransition = false;

    UpdateLabels();
}

void Shop::Update(float dt)
{
    background.Update();

    /* UPDATE GUI */

    if (btnFullscreen.Update(app->GetMousePosition()))
    {
        app->assetManager.Get<raylib::Sound>("click")->Play();
        app->ToggleFullscreen();
    }
    else if (btnSound.Update(app->GetMousePosition()))
    {
        app->assetManager.Get<raylib::Sound>("click")->Play();
        ToggleVolume(app);
    }
    else if (btnAction.Update(app->GetMousePosition()))
    {
        if (save->playerStats[playerSelected].unlocked && save->selectedPlayer == playerSelected)
        {
            app->assetManager.Get<raylib::Sound>("click")->Play();
            app->Transition("title", 0.5f, app->assetManager.Get<raylib::Shader>("shader transition menu"));
        }
        else if (!save->playerStats[playerSelected].unlocked && save->money >= GetShipPrice(playerSelected))
        {
            app->assetManager.Get<raylib::Sound>("purchase")->Play();
            save->playerStats[playerSelected].unlocked = true;
            save->money -= GetShipPrice(playerSelected);
            save->selectedPlayer = playerSelected;
            UpdateLabels();
        }
        else if (save->playerStats[playerSelected].unlocked && save->selectedPlayer != playerSelected)
        {
            app->assetManager.Get<raylib::Sound>("click")->Play();
            save->selectedPlayer = playerSelected;
        }
        else
        {
            app->assetManager.Get<raylib::Sound>("wrong")->Play();
        }
    }
    else if (btnArrowLeft.Update(app->GetMousePosition()))
    {
        app->assetManager.Get<raylib::Sound>("click")->Play();

        if (!onSelectTransition)
        {
            onSelectTransition = true;
            previousPlayerSelected = playerSelected;
            playerSelected = (playerSelected - 1 + PlayerCount) % PlayerCount;
        }
    }
    else if (btnArrowRight.Update(app->GetMousePosition()))
    {
        app->assetManager.Get<raylib::Sound>("click")->Play();

        if (!onSelectTransition)
        {
            onSelectTransition = true;
            previousPlayerSelected = playerSelected;
            playerSelected = (playerSelected + 1) % PlayerCount;
        }
    }
    else
    {
        stats.Update(this);
    }

    /* UPDATE PLAYER ROTATION */

    raylib::Vector2 dc = app->GetMousePosition() - playerPosition;
    float da = utils::GetDeltaRad(playerRotation, std::atan2(dc.y, dc.x) + PI/2);
    float prevRotation = playerRotation;
    playerRotation += da * 6 * dt;

    playerPitch = (GetMouseDelta().x || GetMouseDelta().y)
        ? std::clamp(playerPitch + (playerRotation - prevRotation), -PI/6, PI/6)
        : playerPitch = playerPitch * 0.975f;

    /* PLAYER SELECTION TRANSITION */

    if (onSelectTransition)
    {
        static bool labelsSet = false;
        phaseSelectTransition -= 4.0f * dt;

        if (!labelsSet && phaseSelectTransition <= 0.0f)
        {
            labelsSet = true;
            UpdateLabels();
        }

        if (phaseSelectTransition <= -1.0f)
        {
            phaseSelectTransition = 1.0f;
            onSelectTransition = false;
            labelsSet = false;
        }
    }
}

void Shop::Draw(const rf::core::Renderer& target)
{
    target.Clear(BLACK);

    background.Draw();

    txtMoney.Draw({ BaseWidth / 2.0f, playerPosition.y - 128 }, 0, txtMoney.MeasureEx() * 0.5f);

    btnSound.Draw(*texIcons, { 128, app->saveManager->Get<Save>()->volume ? 128.f : 256.f, 128, 128 });
    btnFullscreen.Draw(*texIcons, { 0, app->window.IsFullscreen() ? 256.f : 128.f, 128, 128 });

    if (onSelectTransition)
    {
        float alpha = (phaseSelectTransition + 1) * 0.5f;
        btnAction.Draw(!save->playerStats[previousPlayerSelected].unlocked ? "Buy" : save->selectedPlayer != previousPlayerSelected ? "Select" : "Title", font, alpha);
        btnAction.Draw(!save->playerStats[playerSelected].unlocked ? "Buy" : save->selectedPlayer != playerSelected ? "Select" : "Title", font, 1.0f-alpha);
    }
    if (save->playerStats[playerSelected].unlocked)
    {
        btnAction.Draw(save->selectedPlayer == playerSelected ? "Title" : "Select", font);
    }
    else
    {
        btnAction.Draw("Buy", font);
    }

    rlPushMatrix();

        rlTranslatef(BaseWidth/2.0f, BaseHeight/2.0f, 0);
        rlScalef(std::abs(phaseSelectTransition), std::abs(phaseSelectTransition), 1);
        rlTranslatef(-BaseWidth/2.0f, -BaseHeight/2.0f, 0);

        float alpha = std::abs(phaseSelectTransition);
        SetAlphaLabels(alpha);

        stats.Draw(this, alpha);

        if ((!onSelectTransition && !save->playerStats[playerSelected].unlocked)
         || (onSelectTransition && phaseSelectTransition < 0 && !save->playerStats[playerSelected].unlocked)
         || (onSelectTransition && phaseSelectTransition > 0 && !save->playerStats[previousPlayerSelected].unlocked))
        {
            BeginScissorMode((playerFrame.x + playerFrame.width * 0.5f) - 50, playerFrame.y + playerFrame.height, 100, 50);
            txtShipPrice.Draw({ playerFrame.x + playerFrame.width * 0.5f, playerFrame.y + playerFrame.height * 1.225f }, 0, txtShipPrice.MeasureEx() * 0.5f);
            EndScissorMode();
        }

    rlPopMatrix();

    btnArrowLeft.Draw(*texIcons, { 256, 256, 128, 128 }, 1.0f, false);
    btnArrowRight.Draw(*texIcons, { 256, 128, 128, 128 }, 1.0f, false);

    DrawRectangleRounded(playerFrame, 0.25f, 9, { 0, 0, 0, 63 });
    DrawRectangleRoundedLines(playerFrame, 0.25f, 9, 2, { 255, 255, 255, 127 });

    uint8_t playerIndex = onSelectTransition && phaseSelectTransition > 0.0f ? previousPlayerSelected : playerSelected;
    utils::DrawPerspective(tilesheet, playerConstants[playerIndex].source, playerPosition, playerRotation, playerPitch, std::abs(phaseSelectTransition)*1.5f);
}
