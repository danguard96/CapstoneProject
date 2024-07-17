#include <glew.h>
#include <iostream>
#include <algorithm>
#include "Scene1.h"

Scene1::Scene1(Renderer* renderer_, SceneManager* sceneManager_) :
	Scene(nullptr), renderer(renderer_), camera(nullptr), sceneManager(sceneManager_) {
	vRenderer = dynamic_cast<VulkanRenderer*>(renderer_);
	stepSound = engine->play2D("./audio/steps2.wav", true, true);
	camera = new Camera();
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
}

Scene1::~Scene1() {
	engine->stopAllSounds();
	engine->drop();
	if (camera) delete camera;
}

bool Scene1::OnCreate() {
	engine->play2D("./audio/ocean.wav", true);
	int width = 0, height = 0;
	switch (renderer->getRendererType()) {
	case RendererType::VULKAN:

		SDL_GetWindowSize(dynamic_cast<VulkanRenderer*>(renderer)->GetWindow(), &width, &height);
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		camera->Perspective(fovy, aspectRatio, 0.01f, 90.0f);
		camera->LookAt(Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
		cameraPosition = vRenderer->camPos;
		cameraPosition->setPosition(Vec3{-1.58,-1.2,-2});// = CameraPosition{ ,0,0, new Collider(Vec3{0,1.2,5}, 0.25), new Collider(Vec3{0,1.2,5}, 0.5) };
	break;

	case RendererType::OPENGL:
		break;
	}
	return true;
}

void Scene1::HandleEvents(const SDL_Event& sdlEvent) {
	if (sdlEvent.type == SDL_WINDOWEVENT) {
		switch (sdlEvent.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			printf("size changed %d %d\n", sdlEvent.window.data1, sdlEvent.window.data2);
			aspectRatio = static_cast<float>(sdlEvent.window.data1) / static_cast<float>(sdlEvent.window.data2);
			camera->Perspective(fovy, aspectRatio, 0.5f, 40.0f);
			break;
		}
	}
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			front = true;
			break;
		case SDL_SCANCODE_S:
			back = true;
			break;
		case SDL_SCANCODE_A:
			left = true;
			break;
		case SDL_SCANCODE_D:
			right = true;
			break;
		case SDL_SCANCODE_UP:
			lookUp = true;
			break;
		case SDL_SCANCODE_DOWN:
			lookDown = true;
			break;
		case SDL_SCANCODE_LEFT:
			lookLeft = true;
			break;
		case SDL_SCANCODE_RIGHT:
			lookRight = true;
			break;
		case SDL_SCANCODE_E:
			if(cameraPosition->actionCollider->isColliding(*vRenderer->actors2["door"].collider)) {
				sceneManager->ChangeScene(SceneManager::SCENE_NUMBER::SCENE0);
				return;
			}
			break;
		case SDL_SCANCODE_F:
			break;
		case SDL_SCANCODE_Z:
			zoom = true;
			break;
		case SDL_SCANCODE_G:
			distort = true;
			break;
		}
		break;
	case SDL_KEYUP:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			front = false;
			break;
		case SDL_SCANCODE_S:
			back = false;
			break;
		case SDL_SCANCODE_A:
			left = false;
			break;
		case SDL_SCANCODE_D:
			right = false;
			break;
		case SDL_SCANCODE_UP:
			lookUp = false;
			break;
		case SDL_SCANCODE_DOWN:
			lookDown = false;
			break;
		case SDL_SCANCODE_LEFT:
			lookLeft = false;
			break;
		case SDL_SCANCODE_RIGHT:
			lookRight = false;
			break;
		case SDL_SCANCODE_F:
			break;
		case SDL_SCANCODE_Z:
			zoom = false;
			break;
		case SDL_SCANCODE_G:
			distort = false;
			break;
		}
	case SDL_WINDOWEVENT:
		switch (sdlEvent.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			printf("size changed %d %d\n", sdlEvent.window.data1, sdlEvent.window.data2);
			aspectRatio = static_cast<float>(sdlEvent.window.data1) / static_cast<float>(sdlEvent.window.data2);
			camera->Perspective(fovy, aspectRatio, 0.5f, 40.0f);
			break;

		}
		break;
	}
}

void Scene1::Update(const float deltaTime) {

	if (!left && !right && !front && !back) {
		stepSound->setIsPaused(true);
	}
	else {
		stepSound->setIsPaused(false);
	}

	static float elapsedTime = 0.0f;
	elapsedTime += deltaTime;
	float fb = (front ? 0.125f : 0 + back ? -0.075f : 0) * 0.01;
	float lr = (right ? 0.1 : 0 + left ? -0.1 : 0) * 0.01;
	Vec3 movement = (
		Vec3(-sin(cameraPosition->gamma * deg2rad), 0, cos(cameraPosition->gamma * deg2rad)) * rad2deg * fb)
		+ (
			Vec3(-sin((90 + cameraPosition->gamma) * deg2rad), 0, cos((90 + cameraPosition->gamma) * deg2rad)) * rad2deg * lr);
	if (MATH::VMath::mag(movement) > 0.001f)
		movement = MATH::VMath::normalize(movement) * 0.1;
	Vec3 newPos = cameraPosition->position + movement;
	Collider temp = *cameraPosition->collider;
	temp.setPosition(Vec3{ -newPos.x, -newPos.y, -newPos.z });
	bool is_colliding = false;
	for (auto i : vRenderer->actors2)
	{
		if (!is_colliding && i.second.collider)
			is_colliding = is_colliding || temp.isColliding(*i.second.collider);
	}
	if (!is_colliding)
	{
		cameraPosition->setPosition(newPos);
	}

	cameraPosition->theta += lookUp ? -2 : 0 + lookDown ? 2 : 0;

	if (cameraPosition->theta < -90) {
		cameraPosition->theta = -90;
	}
	else if (cameraPosition->theta > 90) {
		cameraPosition->theta = 90;
	}

	if (zoom && fovy > 25) {
		fovy -= 0.9;
		camera->Perspective(fovy, aspectRatio, 0.5f, 40.0f);
	}
	if (!zoom && fovy < 45.0f) {
		fovy += 0.8;
		camera->Perspective(fovy, aspectRatio, 0.5f, 40.0f);
	}

	cameraPosition->gamma += lookLeft ? -2 : 0 + lookRight ? 2 : 0;
}

void Scene1::setMatrix(Actor* a) const {
	vRenderer->SetModelMatrixPush(a, MMath::translate(a->position)
		* MMath::rotate(a->thetaRadianRotation, 1, 0, 0)
		* MMath::rotate(a->gammaRadianRotation, 0, 1, 0)
		* MMath::scale(a->scale));
}

void Scene1::Render() const {

	GlobalLighting gl = GlobalLighting{ { LightUBO{ Vec4(0.0f, 50.0f, 0.0f, 1.0f), Vec4(0.9, 0.9, 0.8, 1)}}, 1, 0, distort};

	for (auto i : vRenderer->actors2)
	{
		setMatrix(&i.second);
		vRenderer->actors2.insert_or_assign(i.first, i.second);
	}

    vRenderer->SetCameraUBO(camera->GetProjectionMatrix(),    MMath::rotate(cameraPosition->theta,1,0,0)
                                                            * MMath::rotate(cameraPosition->gamma, 0, 1, 0) 
                                                            * MMath::translate(cameraPosition->position), -cameraPosition->position);
	vRenderer->commitFrame();

	vRenderer->SetGLightsUBO(gl);
	vRenderer->Render();
}


void Scene1::OnDestroy() {

}
