#include "components/RadarEffect.hpp"
#include "raylib.h"

constexpr char fragRadar[] =
#ifdef PLATFORM_DESKTOP
"#version 330\n"
#else
"#version 100\n"
"precision mediump float;\n"
#endif
"#define PI 3.1415926535897932384626433832795\n"
"#define TAU 2.*PI\n"
#ifdef PLATFORM_DESKTOP
"in vec4 fragColor;"
#else
"varying vec4 fragColor;"
#endif
"uniform vec2 center;"              ///< Center of the radar effect
"uniform float radius;"             ///< Radius of the radar effect
"uniform float speed;"              ///< Speed in radian per second
"uniform float drag;"               ///< Drag size in radian
"uniform float strength;"           ///< Effect multiplier
"uniform float time;"
"uniform float sh;"
#ifdef PLATFORM_DESKTOP
"out vec4 finalColor;"
#endif
"void main()"
"{"
    // Current angle of the line in radian
    "float th0 = speed * time;"

    // Calculate the distance between the center
    // of the effect and the current pixel
    "vec2 fragCoord = vec2(gl_FragCoord.x,sh-gl_FragCoord.y);"
    "vec2 delta = fragCoord-center;"
    "float dist = length(delta);"

    "float val = .0;"
    "if (dist < radius)"
    "{"
        // Compute the distance to the line
        "vec2 p = radius*vec2(cos(th0),-sin(th0));"
        "float l = length(delta-p*clamp(dot(delta,p)/dot(p,p),.0,1.));"

        // Compute gradient based on angle difference to th0
   	 	"float th = mod(atan(delta.y,delta.x)+th0,TAU);"
        "float gradient = clamp(1.-th/drag,.0,1.);"
        "val = (1.-smoothstep(.0,.002,l))+.5*gradient;"
    "}"

#   ifdef PLATFORM_DESKTOP
    "finalColor = fragColor + val*fragColor*strength;"
#   else
    "gl_FragColor = fragColor + val*fragColor*strength;"
#   endif
"}";

uint16_t Asteroid::RadarEffect::Counter   = 0;
::Shader Asteroid::RadarEffect::Shader{};
int Asteroid::RadarEffect::LocStrength    = 0;
int Asteroid::RadarEffect::LocCenter      = 0;
int Asteroid::RadarEffect::LocRadius      = 0;
int Asteroid::RadarEffect::LocSpeed       = 0;
int Asteroid::RadarEffect::LocDrag        = 0;
int Asteroid::RadarEffect::LocTime        = 0;

Asteroid::RadarEffect::RadarEffect()
{
    if (Counter == 0)
    {
        Shader = LoadShaderFromMemory(nullptr, fragRadar);
        LocStrength = GetShaderLocation(Shader, "strength");
        LocCenter = GetShaderLocation(Shader, "center");
        LocRadius = GetShaderLocation(Shader, "radius");
        LocSpeed = GetShaderLocation(Shader, "speed");
        LocDrag = GetShaderLocation(Shader, "drag");
        LocTime = GetShaderLocation(Shader, "time");

        const float targetHeight = 450;
        SetShaderValue(Shader, GetShaderLocation(Shader, "sh"),
            &targetHeight, SHADER_UNIFORM_FLOAT);
    }
    Counter++;
}

Asteroid::RadarEffect::~RadarEffect()
{
    if (Counter == 1) UnloadShader(Shader);
    Counter--;
}

void Asteroid::RadarEffect::SendToShader()
{
    SetShaderValue(Shader, LocCenter, reinterpret_cast<const float*>(&position), SHADER_UNIFORM_VEC2);
    SetShaderValue(Shader, LocStrength, &strength, SHADER_UNIFORM_FLOAT);
    SetShaderValue(Shader, LocRadius, &radius, SHADER_UNIFORM_FLOAT);
    SetShaderValue(Shader, LocSpeed, &speed, SHADER_UNIFORM_FLOAT);
    SetShaderValue(Shader, LocDrag, &drag, SHADER_UNIFORM_FLOAT);
}

void Asteroid::RadarEffect::BeginMode(float time, bool sendToShader)
{
    SetShaderValue(Shader, LocTime, &time, SHADER_UNIFORM_FLOAT);
    if (sendToShader) SendToShader();
    BeginShaderMode(Shader);
}

void Asteroid::RadarEffect::EndMode()
{
    EndShaderMode();
}
