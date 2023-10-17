#include "curva.h"
constexpr auto SIXTH = 1.0 / 6.0;

Curva::Curva(float steps)
{
    this->steps = steps;
}

void Curva::addControlPoint(vec3 p)
{
    controlPoints.push_back(p);

    if (controlPoints.size() >= 4)
    {
        int pn = controlPoints.size() - 1;
        float increment = 1.0f / steps;

        for (float t = 0.0; t < 1.0; t += increment)
        {
            float xt = (controlPoints[pn - 3].x * (SIXTH * ((1 - t) * (1 - t) * (1 - t))) + (controlPoints[pn - 2].x * (SIXTH * ((3 * (t * t * t)) - (6 * (t * t)) + 4))) + (controlPoints[pn - 1].x * (SIXTH * ((-3 * (t * t * t)) + (3 * (t * t)) + (3 * t) + 1))) + controlPoints[pn].x * (SIXTH * (t * t * t)));
            float yt = (controlPoints[pn - 3].y * (SIXTH * ((1 - t) * (1 - t) * (1 - t))) + (controlPoints[pn - 2].y * (SIXTH * ((3 * (t * t * t)) - (6 * (t * t)) + 4))) + (controlPoints[pn - 1].y * (SIXTH * ((-3 * (t * t * t)) + (3 * (t * t)) + (3 * t) + 1))) + controlPoints[pn].y * (SIXTH * (t * t * t)));

            vec3 ponto;
            ponto.x = xt;
            ponto.y = yt;

            curvePoints.push_back(ponto);
        }
    }
}