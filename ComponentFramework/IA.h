#pragma once
#include <vector>
#include "Actor.h"
#include "CameraPosition.h"
#include "VMath.h"
#include "Vector.h"
#include <math.h>

#define PI 3.14159265

class IA {
public:
    void update() {
        behaviour();
    }
    IA(Actor* nA, float nSpeed, CameraPosition* ca, std::vector<MATH::Vec3, std::allocator<MATH::Vec3>> nPath):a(nA), speed(nSpeed), cPos(ca), path(nPath) {};
    IA(){};
private:
    Actor* a;
    CameraPosition* cPos;
    int nextPath = 0;
    float speed;
    std::vector<MATH::Vec3> path;

    void behaviour() {
        if(MATH::VMath::distance(a->position, -(cPos->position)) < 4) {
            follow();
        }
        else {
            patrol();
        }
    }

    void follow(){
        move(-(cPos->position));
    }

    void patrol() {
        if (nextPath < path.size() && move(path[nextPath])){
            nextPath = (nextPath + 1) % path.size();
        }
    }

    bool move(MATH::Vec3 finalPos) {
        if(MATH::VMath::mag(finalPos - a->position)< 0.001){
            return true;
        }
        MATH::Vec3 movement = MATH::VMath::normalize(finalPos - a->position) * speed;
        if(MATH::VMath::distance(a->position, finalPos) < MATH::VMath::mag(movement)) {
            a->move( finalPos );
            float angle = atan2(a->position.z,a->position.x)*180/PI;
            a->setTheta(angle);
            return true;
        }
        else {
            a->move( a->position + movement );
            float angle = atan2(movement.x,movement.z)*180/PI;
            a->setTheta(angle);
            return false;
        }
    }
};