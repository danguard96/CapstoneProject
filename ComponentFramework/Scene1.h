#ifndef SCENE1_H
#define SCENE1_H
#include "Scene.h"
#include "Renderer.h"
#include "Camera.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;


class Scene1 : public Scene {
private:
	Renderer* renderer;
	Camera* camera;
	Matrix4 mariosModelMatrix;
public:
	explicit Scene1(Renderer* renderer_);
	virtual ~Scene1();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // SCENE1_H