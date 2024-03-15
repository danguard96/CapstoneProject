#ifndef SCENE0_H
#define SCENE0_H
#include "Scene.h"
#include "Renderer.h"
#include "Camera.h"
#include "VulkanRenderer.h"
//#include <irrKlang.h>
using namespace MATH;

/// Forward declarations 
union SDL_Event;

struct CameraPosition {
	Vec3 position;
	float theta;
	float gamma;
};

class Scene0 : public Scene {
private:
	const float deg2rad = M_PI / 180;
	const float rad2deg = 180 / M_PI;
	CameraPosition cameraPosition;
	Renderer *renderer;
	Camera *camera;
	Matrix4 mariosModelMatrix;
	Matrix4 SphereModelMatrix;
	Matrix4 SphereRotationMatrix;
	int skin = 0;
	bool light = false;
	bool sit = false;
	bool music = false;
//	irrklang::ISound* sound;
	bool doorOpen = false;
	bool front = false;
	bool back = false;
	bool right = false;
	bool left = false;
	bool lookUp = false;
	bool lookDown = false;
	bool lookRight = false;
	bool lookLeft = false;
//	irrklang::ISoundEngine* engine;
	VulkanRenderer* vRenderer;
public:
	explicit Scene0(Renderer* renderer_);
	virtual ~Scene0();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};



#endif // SCENE0_H