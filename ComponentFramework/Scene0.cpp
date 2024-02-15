#include <glew.h>
#include <iostream>
#include "Debug.h"
#include "Scene0.h"
#include "MMath.h"
#include "Debug.h"
#include "VulkanRenderer.h"
#include "OpenGLRenderer.h"
#include "Camera.h"

Scene0::Scene0(Renderer *renderer_): 
	Scene(nullptr),renderer(renderer_), camera(nullptr) {
	camera = new Camera();
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0::~Scene0() {
	if(camera) delete camera;

}

bool Scene0::OnCreate() {
	int width = 0, height = 0;
	float aspectRatio;
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
		case SDL_SCANCODE_1:
			skin = 0;
			break;
		case SDL_SCANCODE_2:
			skin = 1;
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
void Scene0::Update(const float deltaTime) {
		
	static float elapsedTime = 0.0f;
	elapsedTime += deltaTime;
	float fb = (front ? 0.1 : 0 + back ? -0.1 : 0) * 0.01;
	float lr = (right ? 0.1 : 0 + left ? -0.1 : 0) * 0.01;
	cameraPosition.position += (Vec3(-sin(cameraPosition.gamma * deg2rad),0,cos(cameraPosition.gamma * deg2rad)) * rad2deg * fb)
		+ (Vec3(-sin((90 + cameraPosition.gamma) * deg2rad), 0, cos((90 + cameraPosition.gamma) * deg2rad)) * rad2deg * lr);
	cameraPosition.theta += lookUp ? -2 : 0 + lookDown ? 2 : 0;
	cameraPosition.gamma += lookLeft ? -2 : 0 + lookRight ? 2 : 0;
	mariosModelMatrix = MMath::rotate(elapsedTime * 90.0f, Vec3(0.0f, 1.0f, 0.0f));
	SphereRotationMatrix = MMath::rotate(elapsedTime * 90.0f, Vec3(0.0f, -1.0f, 0.0f));
}

void Scene0::Render() const {

	GlobalLighting gl = GlobalLighting{ { LightUBO{ Vec4(100.0f, 0.0f, 0.0f, 1.0f), Vec4(0.3, 0.0, 0.0, 0.2) },
										  LightUBO{ Vec4(-100.0f, 0.0f, 0.0f, 1.0f), Vec4(0.0, 0.3, 0.0, 0.2) }, 
										  LightUBO{ Vec4(0.0f, 100.0f, 0.0f, 1.0f), Vec4(0.0, 0.0, 0.3, 0.2) } }, 3 };
	
	switch (renderer->getRendererType()) {

	case RendererType::VULKAN:
		VulkanRenderer* vRenderer;
		vRenderer = dynamic_cast<VulkanRenderer*>(renderer);
		vRenderer->SetModelMatrixPush(mariosModelMatrix);
		vRenderer->SetSphereModelMatrixPush(SphereRotationMatrix * SphereModelMatrix * mariosModelMatrix);
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(), MMath::rotate(cameraPosition.theta,Vec3(1,0,0)) 
																	* MMath::rotate(cameraPosition.gamma, 0, 1, 0) 
																	* MMath::translate(cameraPosition.position));
		vRenderer->SetGLightsUBO(gl);
		vRenderer->SetTextureIndex(skin);
		vRenderer->Render();
		break;

	case RendererType::OPENGL:
		OpenGLRenderer* glRenderer;
		glRenderer = dynamic_cast<OpenGLRenderer*>(renderer);
		/// Clear the screen
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		/// Draw your scene here
		
		
		glUseProgram(0);
		
		break;
	}
}


void Scene0::OnDestroy() {
	
}
