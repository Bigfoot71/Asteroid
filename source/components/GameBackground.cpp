#include "components/GameBackground.hpp"
#include "settings.hpp"

constexpr char fragBackground[] =
#ifdef PLATFORM_DESKTOP
    "#version 330\n"
#else
    "#version 100\n"
    "precision mediump float;\n"
#endif
    "#define LAYER_COUNT 10\n"
    "#define DIST_FACTOR 5.\n"
    "uniform vec2 resolution;"
    "uniform vec2 position;"
    "uniform float time;"
    "uniform float zoom;"
#ifdef PLATFORM_DESKTOP
    "out vec4 finalColor;"
#endif
    "vec2 hash(in vec2 p)"
    "{"
        "return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);"
    "}"
    "vec2 hash2d(vec2 p)"
    "{"
        "vec3 p3 = fract(vec3(p.xyx)*vec3(.1031,.1030,.0973));"
        "p3 += dot(p3,p3.yzx+19.19);"
        "return fract((p3.xx+p3.yz)*p3.zy);"
    "}"
    "float noise(in vec2 p)"
    "{"
        "vec2 n = floor(p);"
        "vec2 f = fract(p);"
        "float md = 1.;"
        "vec2 o = hash2d(n)*.96+.02;"   // Scale slightly to move away from the cell edges
        "vec2 r = o-f;"
        "float d = dot(r,r);"
        "md = min(d,md);"
        "return md;"
    "}"
    "vec3 starfield(vec2 uv, float threshold)"
    "{"
        "float star = noise(uv);"
        "float pow = max(1.-(star/threshold),0.);"
        "return vec3(pow*pow*pow);"
    "}"
    "void main()"
    "{"
        "vec2 uv = (gl_FragCoord.xy/max(resolution.x, resolution.y))*(DIST_FACTOR+zoom);"   // TODO: review zoom (distance)
        "vec3 col;"
        "for (int i = 1; i <= LAYER_COUNT; i++)"
        "{"
            "float fi = float(i);"
            "float inv = sqrt(1./fi);"
            "col += starfield((uv+vec2(fi*100.,fi*50.))*(1.+fi*.2)+position,.0005)*inv;"
        "}"
#   ifdef PLATFORM_DESKTOP
        "finalColor = vec4(col,1.);"
#   else
        "gl_FragColor = vec4(col,1.);"
#   endif
    "}";

using namespace Asteroid;

GameBackground::GameBackground()
{
    if (counter == 0)
    {
        shader = LoadShaderFromMemory(nullptr, fragBackground);
        locPosition = GetShaderLocation(shader, "position");
        locTime = GetShaderLocation(shader, "time");
        locZoom = GetShaderLocation(shader, "zoom");

        constexpr float resolution[2] = { BaseWidth, BaseHeight };
        SetShaderValue(shader, GetShaderLocation(shader, "resolution"), resolution, SHADER_UNIFORM_VEC2);
    }
    counter++;
}

GameBackground::~GameBackground()
{
    if (counter == 1)
    {
        UnloadShader(shader);
    }
    counter--;
}

void GameBackground::Update(const Vector2& position, float zoom)
{
    const float t = GetTime();
    const float p[2] = { position.x * 0.006f, position.y * -0.006f };  // .0075 for dist 5

    SetShaderValue(shader, locTime, &t, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, locPosition, p, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, locZoom, &zoom, SHADER_UNIFORM_FLOAT);
}

void GameBackground::Draw()
{
    BeginShaderMode(shader);
        DrawRectangle(0, 0, BaseWidth, BaseHeight, BLACK);
    EndShaderMode();
}
