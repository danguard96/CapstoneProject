#include <glew.h>
#include <iostream>
#include <algorithm>
#include "Debug.h"
#include "Scene0.h"
#include "MMath.h"
#include "Debug.h"
#include "VMath.h"
#include "OpenGLRenderer.h"
#include "Camera.h"

Scene0::Scene0(Renderer *renderer_, SceneManager *sceneManager_): 
	Scene(nullptr),renderer(renderer_), camera(nullptr), sceneManager(sceneManager_) {
	vRenderer = dynamic_cast<VulkanRenderer*>(renderer_);
	sound = engine->play2D("./audio/menu.wav", true, true);
	stepSound = engine->play2D("./audio/steps1.wav", true, true);
	sound->setVolume(0.3);
	camera = new Camera();
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0::~Scene0() {
	engine->stopAllSounds();
	engine->drop();
	if(camera) delete camera;
}

bool Scene0::OnCreate() {
	int width = 0, height = 0;
	switch (renderer->getRendererType()){
	case RendererType::VULKAN:
		
		SDL_GetWindowSize(dynamic_cast<VulkanRenderer*>(renderer)->GetWindow(), &width, &height);
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		camera->Perspective(fovy, aspectRatio, 0.01f, 90.0f);
		camera->LookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
		SphereModelMatrix = MMath::translate(Vec3{ 2,0,0 }) * MMath::scale(Vec3{0.5f,0.5f,0.5f});
		break;

	case RendererType::OPENGL:
		break;
	}
	cameraPosition = CameraPosition{Vec3{0,-1.2,-5},0,0, new Collider(Vec3{0,1.2,5}, 0.25), new Collider(Vec3{0,1.2,5}, 0.5)};

	return true;
}

void Scene0::HandleEvents(const SDL_Event& sdlEvent) {	
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
			if(cameraPosition.actionCollider->isColliding(*vRenderer->actors[1].collider)){
				light = !light;
				engine->play2D("./audio/lamp.wav");
			}
			if(cameraPosition.actionCollider->isColliding(*vRenderer->actors[0].collider)){
				sit = !sit;
			}
			if(cameraPosition.actionCollider->isColliding(*vRenderer->actors[4].collider)){
				sceneManager->ChangeScene(SceneManager::SCENE_NUMBER::SCENE1);
				return;
			}
			if(cameraPosition.actionCollider->isColliding(*vRenderer->actors[2].collider)){

				music = !music;
				if(music){
					sound->setIsPaused(false);
				}
				else{
					sound->setIsPaused(true);
				}
			}
			break;
		case SDL_SCANCODE_F:
			doorOpen = !doorOpen;
			if(doorIsClosed)
				engine->play2D("./audio/doorOpen.wav");
			if (!doorIsClosed)
				engine->play2D("./audio/doorClose.wav");
			vRenderer->actors[3].collider->setActive(false);
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

void Scene0::Update(const float deltaTime) {

	if(!left && !right && !front && !back){
		stepSound->setIsPaused(true);
	}
	else {
		stepSound->setIsPaused(false);
	}

	static float elapsedTime = 0.0f;
	elapsedTime += deltaTime;
	if(!sit){
		float fb = (front ? 0.1 : 0 + back ? -0.1 : 0) * 0.01;
		float lr = (right ? 0.1 : 0 + left ? -0.1 : 0) * 0.01;
		Vec3 newPos = cameraPosition.position + (Vec3(-sin(cameraPosition.gamma * deg2rad),0,cos(cameraPosition.gamma * deg2rad)) * rad2deg * fb)
			+ (Vec3(-sin((90 + cameraPosition.gamma) * deg2rad), 0, cos((90 + cameraPosition.gamma) * deg2rad)) * rad2deg * lr);
		Collider temp = *cameraPosition.collider;
		temp.setPosition(Vec3{-newPos.x, -newPos.y, -newPos.z});
		bool is_colliding = false;
		for(int actorI = 0; actorI < vRenderer->actors.size(); actorI++)
		{
			if(!is_colliding && vRenderer->actors[actorI].collider)
				is_colliding = is_colliding || temp.isColliding(*vRenderer->actors[actorI].collider);
		}
		if(!is_colliding)
		{
			cameraPosition.setPosition(newPos);
		}
	}

	cameraPosition.theta += lookUp ? -2 : 0 + lookDown ? 2 : 0;

	if (cameraPosition.theta < -90) {
		cameraPosition.theta = -90;
	}
	else if (cameraPosition.theta > 90) {
		cameraPosition.theta = 90;
	}

	if (doorOpen && vRenderer->actors[3].gammaRadianRotation >= 0 && vRenderer->actors[3].gammaRadianRotation < 90) {
		vRenderer->actors[3].gammaRadianRotation += 1.8f;
		doorIsClosed = false;
	}

	if (!doorOpen && vRenderer->actors[3].gammaRadianRotation >= 1.8) {
		vRenderer->actors[3].gammaRadianRotation -= 1.8f;
		doorIsClosed = true;
	}

	if (!doorOpen && vRenderer->actors[3].gammaRadianRotation <= 1.8) {
		vRenderer->actors[3].collider->setActive(true);
	}

	if (zoom && fovy > 25) {
		fovy-= 0.9;
		camera->Perspective(fovy, aspectRatio, 0.5f, 40.0f);
	}
	if (!zoom && fovy < 45.0f) {
		fovy+= 0.8;
		camera->Perspective(fovy, aspectRatio, 0.5f, 40.0f);
	}

	cameraPosition.gamma += lookLeft ? -2 : 0 + lookRight ? 2 : 0;
}

void Scene0::setMatrix(Actor* a) const{
	vRenderer->SetModelMatrixPush(a,  MMath::translate(a->position)
									* MMath::rotate(a->thetaRadianRotation, 1, 0, 0) 
									* MMath::rotate(a->gammaRadianRotation, 0, 1, 0)  
									* MMath::scale(a->scale));
}

void Scene0::Render() const {

	GlobalLighting gl = GlobalLighting{ { LightUBO{ Vec4(0.0f, 5.0f, 0.0f, 1.0f), Vec4(1, 1, 1, 1) } }, 1, 0, distort };

	for (int i = 0; i < vRenderer->actors.size(); i++)
	{
		setMatrix(&vRenderer->actors[i]);
	}

	if(!sit) {
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(),    MMath::rotate(cameraPosition.theta,Vec3(1,0,0)) 
																* MMath::rotate(cameraPosition.gamma, 0, 1, 0) 
																* MMath::translate(cameraPosition.position), -cameraPosition.position);
	}
	else {
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(),    MMath::rotate(cameraPosition.theta,Vec3(1,0,0)) 
																* MMath::rotate(cameraPosition.gamma, 0, 1, 0) 
																* MMath::translate(Vec3{-vRenderer->actors[0].position.x, cameraPosition.position.y - 0.0f, -vRenderer->actors[0].position.z}), cameraPosition.position);
	}

	vRenderer->commitFrame();

	if(light){
		gl = GlobalLighting{ { LightUBO{ Vec4(0.0f, 5.0f, 0.0f, 1.0f), Vec4(1, 1, 1, 1) },
							   LightUBO{ Vec4(vRenderer->actors[1].position, 1), Vec4(0.3, 0.3, 0.3, 1)} }, 2, 0, distort };
	}

	vRenderer->SetGLightsUBO(gl);
	vRenderer->SetTextureIndex(skin);
	vRenderer->Render();
}


void Scene0::OnDestroy() {
	
}
