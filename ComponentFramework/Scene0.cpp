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

Scene0::Scene0(Renderer *renderer_): 
	Scene(nullptr),renderer(renderer_), camera(nullptr) {
	vRenderer = dynamic_cast<VulkanRenderer*>(renderer_);
	camera = new Camera();
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0::~Scene0() {
	if(camera) delete camera;

}

bool Scene0::OnCreate() {
	int width = 0, height = 0;
	float aspectRatio;
//	engine = irrklang::createIrrKlangDevice();
	switch (renderer->getRendererType()){
	case RendererType::VULKAN:
		
		SDL_GetWindowSize(dynamic_cast<VulkanRenderer*>(renderer)->GetWindow(), &width, &height);
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		camera->Perspective(45.0f, aspectRatio, 0.5f, 20.0f);
		camera->LookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
		SphereModelMatrix = MMath::translate(Vec3{ 2,0,0 }) * MMath::scale(Vec3{0.5f,0.5f,0.5f});
		break;

	case RendererType::OPENGL:
		break;
	}
	cameraPosition = CameraPosition{Vec3{0,0,-5},1,0};

	return true;
}

void Scene0::HandleEvents(const SDL_Event& sdlEvent) {	
	if (sdlEvent.type == SDL_WINDOWEVENT) {
		switch (sdlEvent.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			printf("size changed %d %d\n", sdlEvent.window.data1, sdlEvent.window.data2);
			float aspectRatio = static_cast<float>(sdlEvent.window.data1) / static_cast<float>(sdlEvent.window.data2);
			camera->Perspective(45.0f, aspectRatio, 0.5f, 20.0f);
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
			if(MATH::VMath::distance(vRenderer->lamp.position, cameraPosition.position) < 3){
				light = !light;
			}
			if(MATH::VMath::distance(Vec3{-vRenderer->chair.position.x, vRenderer->chair.position.y, -vRenderer->chair.position.z }, cameraPosition.position) < 3){
				sit = !sit;
			}
			if(MATH::VMath::distance(Vec3{-vRenderer->musicBox.position.x, vRenderer->musicBox.position.y, -vRenderer->musicBox.position.z }, cameraPosition.position) < 3){
				/*music = !music;
				if(music){
					sound = engine->play3D("./audio/song.mp3", irrklang::vec3df(vRenderer->musicBox.position.x,
																				vRenderer->musicBox.position.y,
																				vRenderer->musicBox.position.z), false, false, true);
					sound->setMinDistance(5.0f);
				}
				else{
					sound->stop();
					sound->drop();
				}*/
			}
			break;
		case SDL_SCANCODE_F:
			doorOpen = true;
			break; 
		case SDL_SCANCODE_3:
			skin = 2;
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
			doorOpen = false;
			break; 
		}
	case SDL_WINDOWEVENT:
		switch (sdlEvent.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			printf("size changed %d %d\n", sdlEvent.window.data1, sdlEvent.window.data2);
			float aspectRatio = static_cast<float>(sdlEvent.window.data1) / static_cast<float>(sdlEvent.window.data2);
			camera->Perspective(45.0f, aspectRatio, 0.5f, 20.0f);
			break;

		}
		break;
	}
}

//.25 .05 .005

void Scene0::Update(const float deltaTime) {
		
	static float elapsedTime = 0.0f;
	elapsedTime += deltaTime;
	if(!sit){
		float fb = (front ? 0.1 : 0 + back ? -0.1 : 0) * 0.01;
		float lr = (right ? 0.1 : 0 + left ? -0.1 : 0) * 0.01;
		cameraPosition.position += (Vec3(-sin(cameraPosition.gamma * deg2rad),0,cos(cameraPosition.gamma * deg2rad)) * rad2deg * fb)
			+ (Vec3(-sin((90 + cameraPosition.gamma) * deg2rad), 0, cos((90 + cameraPosition.gamma) * deg2rad)) * rad2deg * lr);
	}

	cameraPosition.theta += lookUp ? -2 : 0 + lookDown ? 2 : 0;

	if (cameraPosition.theta < -90) {
		cameraPosition.theta = -90;
	}
	else if (cameraPosition.theta > 90) {
		cameraPosition.theta = 90;
	}

	if (doorOpen && (0.25f - vRenderer->door.position.z) > 0) {
		vRenderer->door.position += Vec3(0.005f, 0, 0.005f);
		vRenderer->door.gammaRadianRotation += 1.8f;
	}
	if (!doorOpen && (vRenderer->door.position.z) > 0) {
		vRenderer->door.position -= Vec3(0.005f, 0, 0.005f);
		vRenderer->door.gammaRadianRotation -= 1.8f;
	}

	cameraPosition.gamma += lookLeft ? -2 : 0 + lookRight ? 2 : 0;
}

void Scene0::Render() const {

	GlobalLighting gl = GlobalLighting{ { LightUBO{ Vec4(0.0f, 0.0f, 10.0f, 1.0f), Vec4(0.03, 0.03, 0.03, 1) } }, 1 };
	vRenderer->SetModelMatrixPush(&vRenderer->chair,  MMath::translate(vRenderer->chair.position)
													* MMath::rotate(vRenderer->chair.thetaRadianRotation, 1, 0, 0) 
													* MMath::rotate(vRenderer->chair.gammaRadianRotation, 0, 1, 0)  
													* MMath::scale(vRenderer->chair.scale));

	vRenderer->SetModelMatrixPush(&vRenderer->lamp,  MMath::translate(vRenderer->lamp.position)
													* MMath::rotate(vRenderer->lamp.thetaRadianRotation, 1, 0, 0) 
													* MMath::rotate(vRenderer->lamp.gammaRadianRotation, 0, 1, 0)  
													* MMath::scale(vRenderer->lamp.scale));

	vRenderer->SetModelMatrixPush(&vRenderer->musicBox,   MMath::translate(vRenderer->musicBox.position)
														* MMath::rotate(vRenderer->musicBox.thetaRadianRotation, 1, 0, 0) 
														* MMath::rotate(vRenderer->musicBox.gammaRadianRotation, 0, 1, 0)  
														* MMath::scale(vRenderer->musicBox.scale));

	vRenderer->SetModelMatrixPush(&vRenderer->door,   MMath::translate(vRenderer->door.position)
													* MMath::rotate(vRenderer->door.thetaRadianRotation, 1, 0, 0) 
													* MMath::rotate(vRenderer->door.gammaRadianRotation, 0, 1, 0)
													* MMath::scale(vRenderer->door.scale));

	if(!sit) {
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(),    MMath::rotate(cameraPosition.theta,Vec3(1,0,0)) 
																* MMath::rotate(cameraPosition.gamma, 0, 1, 0) 
																* MMath::translate(cameraPosition.position));
	}
	else {
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(),    MMath::rotate(cameraPosition.theta,Vec3(1,0,0)) 
																* MMath::rotate(cameraPosition.gamma, 0, 1, 0) 
																* MMath::translate(Vec3{-vRenderer->chair.position.x, vRenderer->chair.position.y, -vRenderer->chair.position.z } - Vec3(0,0.6f,0)));
	}

	vRenderer->commitFrame();

	if(light){
		gl = GlobalLighting{ { LightUBO{ Vec4(0.0f, 0.0f, 10.0f, 1.0f), Vec4(0.03, 0.03, 0.03, 1) },
							LightUBO{ Vec4(vRenderer->lamp.position, 1), Vec4(0.03, 0.03, 0.03, 1)} }, 2 };
	}
	if(music){
	//	engine->setListenerPosition(irrklang::vec3df(-cameraPosition.position.x, cameraPosition.position.y, -cameraPosition.position.z),
    //                              irrklang::vec3df(cameraPosition.theta,cameraPosition.gamma,0));
	}

	vRenderer->SetGLightsUBO(gl);
	vRenderer->SetTextureIndex(skin);
	vRenderer->Render();
}


void Scene0::OnDestroy() {
	
}
