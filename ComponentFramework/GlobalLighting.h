#ifndef GLOBALLIGHTING_H
#define GLOBALLIGHTING_H
#include "Vector.h"

struct LightUBO {
    MATH::Vec4 position;
    MATH::Vec4 diffuse;
};

struct GlobalLighting {
    LightUBO lights[10];
    int numLights = 0;
    float frame = 0;
    bool distort = false;
};

#endif