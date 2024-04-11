#pragma once
#include "Vector.h"
#include "Collider.h"

class CameraPosition {
public:
	MATH::Vec3& position;
	float theta;
	float gamma;
	Collider* collider;
	Collider* actionCollider;
    CameraPosition(MATH::Vec3& nposition, float ntheta, float ngamma, Collider* ncollider, Collider* nactionCollider): position(nposition), theta(ntheta), gamma(ngamma), collider(ncollider),actionCollider(nactionCollider){};
	void setPosition(MATH::Vec3 nPosition){
		position = nPosition;
		collider->setPosition(MATH::Vec3{-position.x, -position.y, -position.z});
		actionCollider->setPosition(MATH::Vec3{-position.x, -position.y, -position.z});
	}
    void setTheta(float nTheta){
        theta = nTheta;
    }
    void setGamma(float nGamma){
        gamma = nGamma;
    }
};