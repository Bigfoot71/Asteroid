#include "utils.hpp"
#include <cstdint>

bool utils::CheckCollisionLineCircle(const Vector2& a, const Vector2& b, const Vector2& c, float r)
{
    float dot = ((c.x - a.x) * (b.x - a.x) + (c.y - a.y) * (b.y - a.y)) / std::pow(Vector2Distance(a, b), 2);
    Vector2 closestPoint = {a.x + dot * (b.x - a.x), a.y + dot * (b.y - a.y)};

    if (dot < 0 || dot > 1)
    {
        closestPoint = (Vector2Distance(c, a) < Vector2Distance(c, b)) ? a : b;
    }

    return Vector2Distance(closestPoint, c) <= r;
}

void utils::DrawPerspective(const raylib::Texture* texture, const Rectangle& source, const Vector2& position, float rotation, float sidePitch, float scale, const Color& tint)
{
    //constexpr Vector2 destSize{ 64, 64 };

    const Vector2 destSize{ 64*scale, 64*scale };
    const float dx = destSize.x*.5f, dy = destSize.y*.5f;

    Vector3 topLeft3D = Vector3RotateByAxisAngle({ -dx, -dy, 0 }, { 0, 1, 0 }, -sidePitch);
    Vector3 topRight3D = Vector3RotateByAxisAngle({ dx, -dy, 0 }, { 0, 1, 0 }, -sidePitch);
    Vector3 bottomLeft3D = Vector3RotateByAxisAngle({ -dx, dy, 0 }, { 0, 1, 0 }, -sidePitch);
    Vector3 bottomRight3D = Vector3RotateByAxisAngle({ dx, dy, 0 }, { 0, 1, 0 }, -sidePitch);

    topLeft3D = Vector3RotateByAxisAngle(topLeft3D, { 0, 0, 1 }, rotation);
    topRight3D = Vector3RotateByAxisAngle(topRight3D, { 0, 0, 1 }, rotation);
    bottomLeft3D = Vector3RotateByAxisAngle(bottomLeft3D, { 0, 0, 1 }, rotation);
    bottomRight3D = Vector3RotateByAxisAngle(bottomRight3D, { 0, 0, 1 }, rotation);

    Vector2 topLeft = Project3D(topLeft3D, position);
    Vector2 topRight = Project3D(topRight3D, position);
    Vector2 bottomLeft = Project3D(bottomLeft3D, position);
    Vector2 bottomRight = Project3D(bottomRight3D, position);

    Vector2 center = utils::Project3D({
        (topLeft3D.x + topRight3D.x + bottomLeft3D.x + bottomRight3D.x) / 4.0f,
        (topLeft3D.y + topRight3D.y + bottomLeft3D.y + bottomRight3D.y) / 4.0f,
        (topLeft3D.z + topRight3D.z + bottomLeft3D.z + bottomRight3D.z) / 4.0f
    }, position);

    Vector2 uvLeft{ source.x / texture->width, source.y / texture->height };
    Vector2 uvRight{ (source.x + source.width) / texture->width, (source.y + source.height) / texture->height };
    Vector2 uvCenter{ (uvLeft.x + uvRight.x) * 0.5f, (uvLeft.y + uvRight.y) * 0.5f };

    float nPitch = sidePitch / (PI/6);
    float nTintLeft = (nPitch < 0 ? 1.0 + nPitch : 1.0f);
    float nTintRight = (nPitch > 0 ? 1.0 - nPitch : 1.0f);

    Color tintLeft = {
        static_cast<uint8_t>(tint.r*nTintLeft),
        static_cast<uint8_t>(tint.g*nTintLeft),
        static_cast<uint8_t>(tint.b*nTintLeft),
        tint.a
    };

    Color tintRight = {
        static_cast<uint8_t>(tint.r*nTintRight),
        static_cast<uint8_t>(tint.g*nTintRight),
        static_cast<uint8_t>(tint.b*nTintRight),
        tint.a
    };

    // texcoords doesn't work with RL_TRIANGLE, and it's 'impossible' to use textureProj in a precise and simple way in shaders
    // The only method (which only alleviates the problems of incorrect perspective mapping) that I could find is this one

    rlSetTexture(texture->id);
    rlBegin(RL_QUADS);

        // Triangle 1
        rlColor4ub(tintLeft.r, tintLeft.g, tintLeft.b, tintLeft.a);
        rlTexCoord2f(uvLeft.x, uvLeft.y);
        rlVertex2f(topLeft.x, topLeft.y);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);
        rlTexCoord2f(uvCenter.x, uvCenter.y);
        rlVertex2f(center.x, center.y);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);
        rlTexCoord2f(uvCenter.x, uvCenter.y);
        rlVertex2f(center.x, center.y);

        rlColor4ub(tintRight.r, tintRight.g, tintRight.b, tintRight.a);
        rlTexCoord2f(uvRight.x, uvLeft.y);
        rlVertex2f(topRight.x, topRight.y);

        // Triangle 2
        rlColor4ub(tintRight.r, tintRight.g, tintRight.b, tintRight.a);
        rlTexCoord2f(uvRight.x, uvLeft.y);
        rlVertex2f(topRight.x, topRight.y);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);
        rlTexCoord2f(uvCenter.x, uvCenter.y);
        rlVertex2f(center.x, center.y);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);
        rlTexCoord2f(uvCenter.x, uvCenter.y);
        rlVertex2f(center.x, center.y);

        rlColor4ub(tintRight.r, tintRight.g, tintRight.b, tintRight.a);
        rlTexCoord2f(uvRight.x, uvRight.y);
        rlVertex2f(bottomRight.x, bottomRight.y);

        // Triangle 3
        rlColor4ub(tintRight.r, tintRight.g, tintRight.b, tintRight.a);
        rlTexCoord2f(uvRight.x, uvRight.y);
        rlVertex2f(bottomRight.x, bottomRight.y);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);
        rlTexCoord2f(uvCenter.x, uvCenter.y);
        rlVertex2f(center.x, center.y);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);
        rlTexCoord2f(uvCenter.x, uvCenter.y);
        rlVertex2f(center.x, center.y);

        rlColor4ub(tintLeft.r, tintLeft.g, tintLeft.b, tintLeft.a);
        rlTexCoord2f(uvLeft.x, uvRight.y);
        rlVertex2f(bottomLeft.x, bottomLeft.y);

        // Triangle 4
        rlColor4ub(tintLeft.r, tintLeft.g, tintLeft.b, tintLeft.a);
        rlTexCoord2f(uvLeft.x, uvRight.y);
        rlVertex2f(bottomLeft.x, bottomLeft.y);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);
        rlTexCoord2f(uvCenter.x, uvCenter.y);
        rlVertex2f(center.x, center.y);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);
        rlTexCoord2f(uvCenter.x, uvCenter.y);
        rlVertex2f(center.x, center.y);

        rlColor4ub(tintLeft.r, tintLeft.g, tintLeft.b, tintLeft.a);
        rlTexCoord2f(uvLeft.x, uvLeft.y);
        rlVertex2f(topLeft.x, topLeft.y);

    rlEnd();
    rlSetTexture(0);
}

