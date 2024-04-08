#ifndef CAMPOS
#define CAMPOS
#include "VulkanRenderer.h"
class CameraPosition {
public:
	Vec3 position;
	float theta;
	float gamma;
	Collider* collider;
	Collider* actionCollider;
	void setPosition(Vec3 nPosition){
		position = nPosition;
		collider->setPosition(Vec3{-position.x, -position.y, -position.z});
		actionCollider->setPosition(Vec3{-position.x, -position.y, -position.z});
	}
};
#endif