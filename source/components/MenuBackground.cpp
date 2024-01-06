#include "components/MenuBackground.hpp"
#include "settings.hpp"

constexpr char fragBackground[] =
#ifdef PLATFORM_DESKTOP
    "#version 330\n"
#else
    "#version 100\n"
    "precision mediump float;\n"
#endif
    "#define NUM_LAYERS 3.\n"
    "#define QUALITY 2\n"
    "uniform sampler2D texture0;"
    "uniform vec4 colDiffuse;"
    "uniform vec2 resolution;"
    "uniform float time;"
#ifdef PLATFORM_DESKTOP
    "out vec4 finalColor;"
#endif
    "mat2 rotate(float theta)"
    "{"
        "float s = sin(theta);"
        "float c = cos(theta);"
        "return mat2(c,-s,s,c);"
    "}"
    "float rand(vec2 r)"
    "{"
        "r = fract(r*vec2(123.34, 456.21));"
        "r += dot(r,r+45.32);"
        "return fract(r.x*r.y);"
    "}"
    "float star(vec2 uv, float flares)"
    "{"
        "float d = length(uv);"
        "float m = .05/d;"
        "float rays = max(0., 1.-abs(uv.x*uv.y*1000.));"
        "m += rays*flares;"
        "uv *= rotate(3.1415/4.);"
        "float rotated_rays = max(0.,1.-abs(uv.x*uv.y*1000.));"
        "m += rotated_rays*.3*flares;"
        "m *= smoothstep(1.,.2,d);"
        "return m;"
    "}"
    "vec3 starLayer(vec2 uv)"
    "{"
        "vec2 luv = fract(uv)-.5;"
        "vec2 lid = floor (uv);"
        "vec3 col = vec3(0.);"
        "for(int y = -1*QUALITY; y <= 1*QUALITY; y++)"
        "{"
            "for(int x = -1*QUALITY; x <= 1*QUALITY; x++)"
            "{"
                "vec2 offsets = vec2(x,y);"
                "float n1 = rand(lid + offsets);"
                "float n2 = fract(n1*34.);"
                "float size = fract(n1*345.32);"
                "float star = star(luv-offsets-vec2(n1, n2)+.5,smoothstep(.9,1.,size)*.05);"
                "float n3 = fract(n1 * 412.124);"
                "star *= (sin(time*.3+n1*6.283)+1.)*.25+.5;"
                "col += star * size * vec3(1.,1.,.5+size) * .4545;"
            "}"
        "}"
        "return col;"
    "}"
    "void main()"
    "{"
        "vec2 uv = (gl_FragCoord.xy-.5*resolution.xy)/resolution.y;"
        "float t = time*.125;"
        "uv *= rotate(t);"
        "vec3 col = vec3(0.);"
        "for(float i = 0.; i < 1.; i += 1./NUM_LAYERS)"
        "{"
            "float depth = fract(i+t);"
            "float scale = mix(10.,.5,depth);"
            "float fade = depth * smoothstep(1.,.9,depth);"
            "col += starLayer(uv*scale+i*453.2)*fade;"
        "}"
#   ifdef PLATFORM_DESKTOP
        "finalColor = vec4(col,1.);"
#   else
        "gl_FragColor = vec4(col,1.);"
#   endif
    "}";

using namespace Asteroid;

MenuBackground::MenuBackground()
{
    if (counter == 0)
    {
        shader = LoadShaderFromMemory(nullptr, fragBackground);
        locTime = GetShaderLocation(shader, "time");

        constexpr float resolution[2] = { BaseWidth, BaseHeight };
        SetShaderValue(shader, GetShaderLocation(shader, "resolution"), resolution, SHADER_UNIFORM_VEC2);
    }
    counter++;
}

MenuBackground::~MenuBackground()
{
    if (counter == 1)
    {
        UnloadShader(shader);
    }
    counter--;
}

void MenuBackground::Update()
{
    const float t = GetTime();
    SetShaderValue(shader, locTime, &t, SHADER_UNIFORM_FLOAT);
}

void MenuBackground::Draw()
{
    BeginShaderMode(shader);
        DrawRectangle(0, 0, BaseWidth, BaseHeight, BLACK);
    EndShaderMode();
}
