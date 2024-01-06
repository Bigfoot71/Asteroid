#include "ui/Button.hpp"

void ui::Button::SetBounds(const raylib::Vector2& center, const raylib::Vector2& size)
{
    bounds = raylib::Rectangle(center - size * 0.5f, size);
    focusEffect.position = center;
    focusEffect.radius = std::max(size.x, size.y) * 0.75f;
    focusEffect.speed = 2.0f*PI;
    focusEffect.drag = PI/2.0f;
    focusEffect.strength = 6.0f;
}

bool ui::Button::Update(const Vector2& mousePosition)
{
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        focus = bounds.CheckCollision(mousePosition);
        bool ret = isPressed && focus; isPressed = false;
        return ret;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        isPressed = focus;
        return false;
    }

    if (isPressed)
    {
        focus = bounds.CheckCollision(mousePosition);
    }

    return false;
}

void ui::Button::Draw(float alpha)
{
    const raylib::Rectangle rect = utils::ScaleRectangleFromCenter(bounds, (isPressed && focus) ? 0.95f : 1.0f);

    if (focus) focusEffect.BeginMode(GetTime());
    rect.DrawRoundedLines(0.6f, 9, 3, { 255, 255, 255, static_cast<uint8_t>(63*alpha) });
    if (focus) focusEffect.EndMode();

    if (focus) focusEffect.BeginMode(GetTime()+31.4f);
    rect.DrawRoundedLines(0.6f, 9, 3, { 255, 255, 255, static_cast<uint8_t>(63*alpha) });
    if (focus) focusEffect.EndMode();

    rect.DrawRounded(0.6f, 9, { 0, 0, 0, static_cast<uint8_t>(63*alpha) });
}

void ui::Button::Draw(const std::string& text, const Font* font, float alpha)
{
    float h = bounds.height * 0.5f * (isPressed && focus ? 0.95f : 1.0f);
    raylib::Text t(text, h, { 255, 255, 255, static_cast<uint8_t>((focus ? 249 : 191) * alpha) }, font ? *font : GetFontDefault(), h * 0.05f);
    Draw(alpha); t.Draw(raylib::Vector2(focusEffect.position) - t.MeasureEx() * 0.5f);
}

void ui::Button::Draw(const raylib::Texture& texture, const raylib::Rectangle& source, float alpha, bool drawFrame)
{
    raylib::Rectangle dest = { focusEffect.position, {} };

    if (texture.width == texture.height)
    {
        float size = std::min(bounds.width, bounds.height) * 0.75f;
        dest.width = size, dest.height = size;
    }
    else
    {
        dest.width = bounds.width * 0.75f, dest.height = bounds.height * 0.75f;
    }

    if (isPressed && focus)
    {
        dest.width *= 0.95f, dest.height *= 0.95f;
    }

    if (drawFrame) Draw(alpha);
    
    texture.Draw(source, dest,
        { dest.width * 0.5f, dest.height * 0.5f }, 0,
        { 255, 255, 255, static_cast<uint8_t>((focus ? 249 : 191) * alpha)});
}
