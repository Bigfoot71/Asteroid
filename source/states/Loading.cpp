#include "states/Loading.hpp"

#define RRES_IMPLEMENTATION
#include <rres.h>

#define RRES_SUPPORT_COMPRESSION_LZ4
#define RRES_RAYLIB_IMPLEMENTATION
#include <rres-raylib.h>

using namespace rf;
using namespace Asteroid;

/* STATE */

void Loading::Enter()
{
#   ifdef LINUX_APP_IMAGE
        assetPath = TextFormat("%s../share/asteroid/assets.rres", GetApplicationDirectory());
#   else
        assetPath = TextFormat("%sassets.rres", GetApplicationDirectory()); // WARNING: copy required if reusing TextFormat
#   endif

    dir = rresLoadCentralDirectory(assetPath);
    dotLen = 3;
}

void Loading::Exit()
{
    rresUnloadCentralDirectory(dir);
}

void Loading::Task()
{
    for (unsigned int i = 0; i < dir.count; i++)
    {
        //if (TextIsEqual(dir.entries[i].fileName, "font.tff")) continue; // Avoid resource alloc/free

        rresResourceMulti multi = rresLoadResourceMulti(assetPath, rresGetResourceId(dir, dir.entries[i].fileName));
        for (int i = 0; i < multi.count; i++) UnpackResourceChunk(&multi.chunks[i]);
        const char *assetName = GetFileNameWithoutExt(dir.entries[i].fileName);

        switch (rresGetDataType(multi.chunks[0].info.type))
        {
            case RRES_DATA_RAW:
            {
                // We assume here that the only raw files are .xm
                uint32_t dataSize = 0;
                uint8_t *data = reinterpret_cast<uint8_t*>(LoadDataFromResource(*multi.chunks, &dataSize));
                app->musicManager.Load(assetName, ".xm", data, dataSize);
                MemFree(data);
            }
            break;

            case RRES_DATA_TEXT:
            {
                app->assetManager.Add(assetName, std::string(LoadTextFromResource(*multi.chunks)));
            }
            break;

            case RRES_DATA_IMAGE:
            {
                app->assetManager.Add<raylib::Image>(assetName, LoadImageFromResource(*multi.chunks));
            }
            break;

            case RRES_DATA_WAVE:
            {
                auto snd = app->assetManager.Add<raylib::Sound>(assetName, raylib::Wave(LoadWaveFromResource(*multi.chunks)));
            }
            break;

            case RRES_DATA_FONT_GLYPHS:
            {
                // Loaded in PostTask (main thread)
            }
            break;

            default: break;
        }

        rresUnloadResourceMulti(multi);
    }

    app->assetManager.Add<raylib::Image>("gradient", raylib::Image::GradientRadial( // Return ::Image
        16, 16, 0, WHITE, { 255, 255, 255, 0 }));

    app->assetManager.Get<raylib::Sound>("player-shoot")->SetVolume(0.5f);

    app->musicManager.NewPlaylist("game", {
        "The Computer Adventures Of Drozerix",
        "Porn Industry",
        "Cabin Fever",
        "Bit Loader",
        "Rendezvous",
    });
}

void Loading::PostTask()
{
    // Upload image to GPU and free them
    for (auto& asset : app->assetManager)
    {
        if (asset.second.Type() == typeid(raylib::Image))
        {
            app->assetManager.AddOrReplace<raylib::Texture>(asset.first, *asset.second.Get<raylib::Image>());
        }
    }

    // Load font
    {
        rresResourceMulti multi = rresLoadResourceMulti(assetPath, rresGetResourceId(dir, "font.ttf"));
        for (int i = 0; i < multi.count; i++) UnpackResourceChunk(&multi.chunks[i]);
        app->assetManager.Add<raylib::Font>("font", LoadFontFromResource(multi));
        rresUnloadResourceMulti(multi);
    }

    // Set texture filter
    app->assetManager.Get<raylib::Texture>("tilesheet")->SetFilter(TEXTURE_FILTER_BILINEAR);
    app->assetManager.Get<raylib::Texture>("icons")->SetFilter(TEXTURE_FILTER_BILINEAR);

    /* Load transition shaders */

    app->assetManager.Add<raylib::Shader>("shader transition menu", LoadShaderFromMemory(nullptr,
    #ifdef PLATFORM_DESKTOP
        "#version 330\n"
        "in vec2 fragTexCoord;"
    #else
        "#version 100\n"
        "precision mediump float;"
        "varying vec2 fragTexCoord;"
    #endif
        "uniform sampler2D texture0;"
        "uniform sampler2D textureN;"
        "uniform float progress;"
    #ifdef PLATFORM_DESKTOP
        "out vec4 finalColor;"
    #endif
        "void main()"
        "{"
        #ifdef PLATFORM_DESKTOP
            "vec4 prevTexel = texture(texture0,fragTexCoord);"
            "vec4 nextTexel = texture(textureN,fragTexCoord);"
        #else
            "vec4 prevTexel = texture2D(texture0,fragTexCoord);"
            "vec4 nextTexel = texture2D(textureN,fragTexCoord);"
        #endif
        #ifdef PLATFORM_DESKTOP
            "finalColor = mix(prevTexel,nextTexel,progress);"
        #else
            "gl_FragColor = mix(prevTexel,nextTexel,progress);"
        #endif
        "}"
    ));

    app->assetManager.Add<raylib::Shader>("shader transition game", LoadShaderFromMemory(nullptr,
    #ifdef PLATFORM_DESKTOP
        "#version 330\n"
        "in vec2 fragTexCoord;"
    #else
        "#version 100\n"
        "precision mediump float;"
        "varying vec2 fragTexCoord;"
    #endif
        "uniform sampler2D texture0;"
        "uniform sampler2D textureN;"
        "uniform float progress;"
    #ifdef PLATFORM_DESKTOP
        "out vec4 finalColor;"
    #endif
        "vec2 zoom(vec2 uv, float amount)"
        "{"
            "return .5+((uv-.5)*(1.-amount));"
        "}"
        "void main()"
        "{"
            "float p = smoothstep(0.,1.,progress);"
        #ifdef PLATFORM_DESKTOP
            "vec4 prevTexel = texture(texture0,zoom(fragTexCoord,p));"
            "vec4 nextTexel = texture(textureN,fragTexCoord);"
        #else
            "vec4 prevTexel = texture2D(texture0,zoom(fragTexCoord,p));"
            "vec4 nextTexel = texture2D(textureN,fragTexCoord);"
        #endif
        #ifdef PLATFORM_DESKTOP
            "finalColor = mix(prevTexel,nextTexel,p);"
        #else
            "gl_FragColor = mix(prevTexel,nextTexel,p);"
        #endif
        "}"
    ));
}

void Loading::Update(float dt)
{
    if ((++frameCounter) == 3)
    {
        dotLen = (dotLen + 1) % 4;
        frameCounter = 0;
    }
}

void Loading::Draw(const rf::core::Renderer& target)
{
    target.Clear(BLACK);
    constexpr char text[] = "Loading...";
    DrawText(TextSubtext(text, 0, 7+dotLen), (BaseWidth - MeasureText(text, 96)) * 0.5f, (BaseHeight - 96) * 0.5f, 96, WHITE);
}
