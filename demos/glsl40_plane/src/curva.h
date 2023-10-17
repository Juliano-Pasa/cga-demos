#ifndef __CURVA_H__
#define __CURVA_H__

#include <vector>
#include "glslprogram.h"


class Curva{
public:
    float steps;
    std::vector<vec3> controlPoints;
    std::vector<vec3> curvePoints;

    Curva(float steps);
    void addControlPoint(vec3 p);
};

#endif
