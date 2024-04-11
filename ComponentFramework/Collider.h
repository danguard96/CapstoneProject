#pragma once
#include "VMath.h"
#include <algorithm>
#include <math.h>

enum ColliderType{
    sphere, box
};

class Collider{
private:
    ColliderType type;
    float colliderRadius;
    MATH::Vec3 sPosition;
public:
    MATH::Vec3 maxCorner;
    MATH::Vec3 minCorner;
    bool active = true;
    Collider(MATH::Vec3 nsPosition, float nColliderRadius) : sPosition(nsPosition), colliderRadius(nColliderRadius) {
        type = ColliderType::sphere;
    }

    Collider(MATH::Vec3 nMaxCorner, MATH::Vec3 nMinCorner) : maxCorner(nMaxCorner), minCorner(nMinCorner) {
        type = ColliderType::box;
    }

    void setActive(bool active_){
        active = active_;
    }

    void setPosition(MATH::Vec3 nPosition){
        sPosition = nPosition;
    }

    bool boxBox( Collider c1, Collider c2 ) {
        return (
            c1.minCorner.x <= c2.maxCorner.x &&
            c1.maxCorner.x >= c2.minCorner.x &&
            c1.minCorner.y <= c2.maxCorner.y &&
            c1.maxCorner.y >= c2.minCorner.y &&
            c1.minCorner.z <= c2.maxCorner.z &&
            c1.maxCorner.z >= c2.minCorner.z
        );
    }

    bool sphereSphere ( Collider c1, Collider c2 ) {
        return MATH::VMath::distance(c1.sPosition, c2.sPosition) < c1.colliderRadius + c2.colliderRadius;
    }

    bool sphereBox ( Collider c1, Collider c2 ) {

        float x = std::fmax(c2.minCorner.x, std::fmin(c1.sPosition.x, c2.maxCorner.x));
        float y = std::fmax(c2.minCorner.y, std::fmin(c1.sPosition.y, c2.maxCorner.y));
        float z = std::fmax(c2.minCorner.z, std::fmin(c1.sPosition.z, c2.maxCorner.z));

        float distance = sqrtf(
            (x - c1.sPosition.x) * (x - c1.sPosition.x) +
            (y - c1.sPosition.y) * (y - c1.sPosition.y) +
            (z - c1.sPosition.z) * (z - c1.sPosition.z)
        );

        return distance < c1.colliderRadius;
    }

    bool isColliding(Collider c1) {
        if (!active || !c1.active)
            return false;
        if (type == ColliderType::box) {
            if(c1.type == ColliderType::sphere) {
                return sphereBox(c1, *this);
            }
            else {
                return boxBox(c1, *this);
            }
        }
        else{
            if(c1.type == ColliderType::sphere){
                return sphereSphere(*this, c1);
            }
            else {
                return sphereBox(*this, c1);
            }
        }
    }
};