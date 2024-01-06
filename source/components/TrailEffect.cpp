#include "components/TrailEffect.hpp"

void Asteroid::TrailEffect::Update(const Vector2& position, float strength)
{
    uint8_t max = points.size()*strength;
    if (max == 0) { count = 0; return; }

    if (count < max)
    {
        points[count++] = position;
    }
    else
    {
        uint8_t offset = (count-max)+1;
        for (int8_t i = 0; i < max-1; i++)
        {
            points[i] = points[i+offset];
        }
        if (offset > 1) count = max;
        points[count-1] = position;
    }
}

void Asteroid::TrailEffect::Draw(float thick, const Color& color) const
{
    if (count < 4) return;

    float a[4]{}, b[4]{};
    float dy = 0, dx = 0, size = 0;

    Vector2 currentPoint{}, nextPoint{};
    Vector2 vertices[2*SplineSegDiv + 2]{};

    rlBegin(RL_TRIANGLES);

    for (uint8_t i = 0; i < (count - 3); i++)
    {
        float t = 0.0f;

        const Vector2 &p1 = points[i];
        const Vector2 &p2 = points[i + 1];
        const Vector2 &p3 = points[i + 2];
        const Vector2 &p4 = points[i + 3];

        a[0] = (-p1.x + 3.0f*p2.x - 3.0f*p3.x + p4.x)/6.0f;
        a[1] = (3.0f*p1.x - 6.0f*p2.x + 3.0f*p3.x)/6.0f;
        a[2] = (-3.0f*p1.x + 3.0f*p3.x)/6.0f;
        a[3] = (p1.x + 4.0f*p2.x + p3.x)/6.0f;

        b[0] = (-p1.y + 3.0f*p2.y - 3.0f*p3.y + p4.y)/6.0f;
        b[1] = (3.0f*p1.y - 6.0f*p2.y + 3.0f*p3.y)/6.0f;
        b[2] = (-3.0f*p1.y + 3.0f*p3.y)/6.0f;
        b[3] = (p1.y + 4.0f*p2.y + p3.y)/6.0f;

        currentPoint.x = a[3];
        currentPoint.y = b[3];

        if (i > 0)
        {
            vertices[0].x = currentPoint.x + dy*size;
            vertices[0].y = currentPoint.y - dx*size;
            vertices[1].x = currentPoint.x - dy*size;
            vertices[1].y = currentPoint.y + dx*size;
        }

        for (uint8_t j = 1; j <= SplineSegDiv; j++)
        {
            t = static_cast<float>(j)/SplineSegDiv;

            nextPoint.x = a[3] + t*(a[2] + t*(a[1] + t*a[0]));
            nextPoint.y = b[3] + t*(b[2] + t*(b[1] + t*b[0]));

            dy = nextPoint.y - currentPoint.y;
            dx = nextPoint.x - currentPoint.x;
            size = 0.5f*thick/sqrtf(dx*dx+dy*dy);

            if ((i == 0) && (j == 1))
            {
                vertices[0].x = currentPoint.x + dy*size;
                vertices[0].y = currentPoint.y - dx*size;
                vertices[1].x = currentPoint.x - dy*size;
                vertices[1].y = currentPoint.y + dx*size;
            }

            vertices[2*j + 1].x = nextPoint.x - dy*size;
            vertices[2*j + 1].y = nextPoint.y + dx*size;
            vertices[2*j].x = nextPoint.x + dy*size;
            vertices[2*j].y = nextPoint.y - dx*size;

            currentPoint = nextPoint;
        }

        for (uint8_t k = 2; k < 2*SplineSegDiv + 2; k++)
        {
            float t = static_cast<float>(i * 2 * SplineSegDiv + k - 2) / (count * 2 * SplineSegDiv);
            rlColor4ub(color.r, color.g, color.b, Lerp(0, color.a, t));

            if (k % 2 == 0)
            {
                rlVertex2f(vertices[k].x, vertices[k].y);
                rlVertex2f(vertices[k - 2].x, vertices[k - 2].y);
                rlVertex2f(vertices[k - 1].x, vertices[k - 1].y);
            }
            else
            {
                rlVertex2f(vertices[k].x, vertices[k].y);
                rlVertex2f(vertices[k - 1].x, vertices[k - 1].y);
                rlVertex2f(vertices[k - 2].x, vertices[k - 2].y);
            }
        }
    }

    rlEnd();
}
