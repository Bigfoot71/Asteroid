#include "components/SplashTitle.hpp"
#include "settings.hpp"

using namespace rf;
using namespace Asteroid;

constexpr char fragWave[] =
#ifdef PLATFORM_DESKTOP
    "#version 330\n"
    "in vec2 fragTexCoord;"
#else
    "#version 100\n"
    "precision mediump float;"
    "varying vec2 fragTexCoord;"
#endif
    "uniform sampler2D texture0;"
    "uniform float time;"
#ifdef PLATFORM_DESKTOP
    "out vec4 finalColor;"
#endif
    "void main()"
    "{"
        "vec2 uv = vec2(fragTexCoord.x,fragTexCoord.y+cos(2.*time+10.*fragTexCoord.x)*.015);"
    #ifdef PLATFORM_DESKTOP
        "finalColor = texture(texture0,uv);"
    #else
        "gl_FragColor = texture2D(texture0,uv);"
    #endif
    "}";

SplashTitle::SplashTitle()
{
    if ((count++) == 0)
    {
        shaderWave = LoadShaderFromMemory(nullptr, fragWave);
        locTime = GetShaderLocation(shaderWave, "time");
    }
}

SplashTitle::~SplashTitle()
{
    if ((--count) == 0)
    {
        UnloadShader(shaderWave);
    }
}

void SplashTitle::Init(const Vector2& start, const Vector2& end, float t)
{ 
    this->start = start;
    this->end = end;
    this->t = t;
}

bool SplashTitle::Update(float dt)
{
    return (t += 0.1f * dt) < 1.25f;
}

void SplashTitle::Draw(const raylib::Texture* texture, const raylib::Font* font) const
{
    // Smooth step interpolation
    float t0 = std::min(t, 1.0f);
    Vector2 pos = start + (end - start) * (t0 * t0 * (3.0f - 2.0f * t0));

    // Scaling after translation
    float t1 = t > 1.0f ? 4 * (t - static_cast<int>(t)) : 0;
    float s = 1.0f + 0.25f * (t1 * t1 * (3.0f - 2.0f * t1));

    // Title drawing
    texture->Draw(Rectangle{ 7, 427, 498, 42 }, raylib::Rectangle{ pos, { 498*s, 42*s } }, { 249*s, 21*s }, 0, ColorAlpha(WHITE, t));

    // Show skip tip
    constexpr char txt[] = "Double click to skip the intro";
    font->DrawText(txt, { (BaseWidth - font->MeasureText(txt, 12, 1).x) * 0.5f, BaseHeight * 0.75f - 12 }, 12, 1, ColorAlpha(WHITE, .5f*(1.f-std::cos(2.f*PI*t1))));
}

void SplashTitle::DrawWave(const raylib::Texture* texture, const Vector2& position, float scale)
{
    const float t = GetTime();
    SetShaderValue(shaderWave, locTime, &t, SHADER_UNIFORM_FLOAT);

    BeginShaderMode(shaderWave);
        texture->Draw(Rectangle{ 0, 384, 512, 128 }, raylib::Rectangle{ position, { 512*scale, 128*scale } }, { 256*scale, 64*scale }, 0, WHITE);
    EndShaderMode();
}
