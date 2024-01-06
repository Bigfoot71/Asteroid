#include "components/EndGamePanel.hpp"
#include "states/Game.hpp"

using namespace Asteroid;

void EndGamePanel::Enable()
{
    displayedTime = std::round(player.GetGameTime());
    displayedKills = player.GetDestroyedEnemies();
    earnedCoins = player.GetMoney();
    displayedMoney = 0;
    active = true;

    app->SetCursor(app->assetManager.Get<raylib::Texture>("icons"),
        {}, 0.25f, { 0, 0, 128, 128 },
        { 255, 255, 255, 191 });
}

void EndGamePanel::Init(rf::core::App* app)
{
    font = app->assetManager.Get<raylib::Font>("font");
    phaseAnim = 0.0f;
    this->app = app;
    active = false;

    btnRetry.SetBounds(app->GetResolution() * Vector2{ 0.5f, 0.65f }, { 128, 32 });
    btnTitle.SetBounds(app->GetResolution() * Vector2{ 0.5f, 0.75f }, { 128, 32 });
}

void EndGamePanel::Update(Game* game, float endGamePhase, float dt)
{
    if (!active) Enable();  // TODO: Make signal system (?)

    if (endGamePhase == 1)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            displayedMoney = earnedCoins + std::round(player.GetGameTime()) + 10 * player.GetDestroyedEnemies();
            displayedTime = displayedKills = 0;
        }

        if (displayedMoney < earnedCoins) displayedMoney++;
        else if (displayedTime > 0) displayedTime--, displayedMoney++;
        else if (displayedKills > 0) displayedKills--, displayedMoney += 10;
        else if (phaseAnim < 1.0f) phaseAnim = std::min(phaseAnim + 2.0f * dt, 1.0f);
        else
        {
            bool exitPanel = false;

            if (btnRetry.Update(app->GetMousePosition()))
            {
                app->assetManager.Get<raylib::Sound>("click")->Play();
                game->Exit(); game->Enter();
                exitPanel = true;
            }

            if (btnTitle.Update(app->GetMousePosition()))
            {
                app->assetManager.Get<raylib::Sound>("click")->Play();
                app->Transition("title", 0.5f, app->assetManager.Get<raylib::Shader>("shader transition menu"));
                exitPanel = true;
            }

            if (exitPanel)
            {
                auto save = app->saveManager->Get<Save>();
                save->hiscore = std::max(displayedMoney, save->hiscore);
                save->money += displayedMoney;
            }
        }
    }
}

void EndGamePanel::Draw(float endGamePhase)
{
    rlPushMatrix();
    rlTranslatef(BaseWidth*.5f, BaseHeight*.5f, 0);
    rlScalef(endGamePhase, endGamePhase, 1);
    rlTranslatef(-BaseWidth*.5f, -BaseHeight*.5f, 0);

        DrawRectangle(0, 0, BaseWidth, BaseHeight, { 0, 0, 0, static_cast<uint8_t>(31*endGamePhase) });
        DrawRectangleRounded(bounds, 0.1f, 18, { 0, 0, 0, static_cast<uint8_t>(127*endGamePhase) });
        DrawRectangleRoundedLines(bounds, 0.05f, 18, 3, WHITE);

        raylib::Text title("Game Over", 36, WHITE, *font, 3.6f);
        title.Draw(raylib::Vector2{ BaseWidth, BaseHeight } * Vector2{ 0.5f, 0.25f }, 0, title.MeasureEx() * 0.5f);

        raylib::Text time(TextFormat("Time: %s", utils::FormatTime(displayedTime)), 18, ColorAlpha(WHITE, 1.0f-phaseAnim), *font, 2.4f);
        time.Draw(raylib::Vector2{ BaseWidth, BaseHeight } * Vector2{ 0.5f, 0.425f }, 0, time.MeasureEx() * 0.5f);

        raylib::Text kill(TextFormat("Eliminations: %i", displayedKills), 18, ColorAlpha(WHITE, 1.0f-phaseAnim), *font, 2.4f);
        kill.Draw(raylib::Vector2{ BaseWidth, BaseHeight } * Vector2{ 0.5f, 0.475f }, 0, kill.MeasureEx() * 0.5f);

        raylib::Text money(TextFormat("Coins: %i", displayedMoney), Lerp(24, 32, phaseAnim), WHITE, *font, Lerp(2.4f, 3.2f, phaseAnim));
        money.Draw(raylib::Vector2{ BaseWidth, BaseHeight } * Vector2{ 0.5f, Lerp(0.55f, 0.45f, phaseAnim) }, 0, money.MeasureEx() * 0.5f);

        btnRetry.Draw("Retry", font, phaseAnim);
        btnTitle.Draw("Title", font, phaseAnim);

    rlPopMatrix();
}
