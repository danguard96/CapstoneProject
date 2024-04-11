#pragma once
#include <unordered_map>
#include "Vector.h"
class PlayerInfo {
public:
    int scene;
    MATH::Vec3 pos;
    float theta;
    float gamma;
    PlayerInfo(int nScene, MATH::Vec3 nPos, float nTheta, float nGamma) : scene(nScene), pos(nPos), theta(nTheta), gamma(nGamma) {};
    PlayerInfo(){};
};