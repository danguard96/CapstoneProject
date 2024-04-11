#ifndef SCENE1_H
#define SCENE1_H
#include <glew.h>
#include <iostream>
#include <algorithm>
#include "Scene.h"
#include "Renderer.h"
#include "Camera.h"
#include "SceneManager.h"
#include "VulkanRenderer.h"
#include "Debug.h"
#include "MMath.h"
#include "Debug.h"
#include "VMath.h"
#include "OpenGLRenderer.h"
#include "Camera.h"
#include "CameraPosition.h"
#include <irrKlang.h>
using namespace MATH;

/// Forward declarations 
union SDL_Event;

class Scene1 : public Scene {
private:
	const float deg2rad = M_PI / 180;
	const float rad2deg = 180 / M_PI;
	CameraPosition* cameraPosition;
	Renderer* renderer;
	Camera* camera;
	bool zoom = false;
	float fovy = 45.0f;
	float aspectRatio;
	bool distort;
	bool front = false;
	bool back = false;
	bool right = false;
	bool left = false;
	bool lookUp = false;
	bool lookDown = false;
	bool lookRight = false;
	bool lookLeft = false;
	VulkanRenderer* vRenderer;
	void setMatrix(Actor* a) const;
	SceneManager* sceneManager;
public:
	explicit Scene1(Renderer* renderer_, SceneManager* sceneManager_);
	virtual ~Scene1();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();;
	irrklang::ISound* stepSound;
};



#endif // SCENE1_H