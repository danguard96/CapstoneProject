#include "Scene0.h"

Scene0::Scene0(Renderer *renderer_, SceneManager *sceneManager_): 
	Scene(nullptr),renderer(renderer_), camera(nullptr), sceneManager(sceneManager_) {
	vRenderer = dynamic_cast<VulkanRenderer*>(renderer_);
	sound = engine->play2D("./audio/song.wav", true, true);
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
	std::vector<Vec3> path = {Vec3{-10,1.2,-10}, Vec3{-10,1.2,10}, Vec3{10,1.2,10}, Vec3{10,1.2,-10}};
	switch (renderer->getRendererType()) {
	case RendererType::VULKAN:
		
		SDL_GetWindowSize(dynamic_cast<VulkanRenderer*>(renderer)->GetWindow(), &width, &height);
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		camera->Perspective(fovy, aspectRatio, 0.01f, 90.0f);
		camera->LookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
		SphereModelMatrix = MMath::translate(Vec3{ 2,0,0 }) * MMath::scale(Vec3{0.5f,0.5f,0.5f});
		cameraPosition = vRenderer->camPos;
		cameraPosition->setPosition(Vec3{0,-1.2,-10});
		vRenderer->actors[5].move(Vec3(-6, -5.75, 2));
		ai = IA(&vRenderer->actors[5], 0.05, cameraPosition, path);
		break;

	case RendererType::OPENGL:
		break;
	}

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
			if(cameraPosition->actionCollider->isColliding(*vRenderer->actors[1].collider)){
				light = !light;
				engine->play2D("./audio/lamp.wav");
			}
			for (int i = 9; i <= 13; i++) {
				if (cameraPosition->actionCollider->isColliding(*vRenderer->actors[i].collider)) {
					sit = !sit;
					if (sit)
						chair = i;
				}
			}
			
			if (cameraPosition->actionCollider->isColliding(*vRenderer->actors[0].collider)) {
				sit = !sit;
				if (sit)
					chair = 0;
			}

			if (cameraPosition->actionCollider->isColliding(*vRenderer->actors[14].collider)) {
				sleep = !sleep;
			}

			if(cameraPosition->actionCollider->isColliding(*vRenderer->actors[4].collider)){
				sceneManager->ChangeScene(SceneManager::SCENE_NUMBER::SCENE1);
				return;
			}
			if(cameraPosition->actionCollider->isColliding(*vRenderer->actors[2].collider)){

				music = !music;
				if(music){
					sound->setIsPaused(false);
				}
				else{
					sound->setIsPaused(true);
				}
			}
			if(cameraPosition->actionCollider->isColliding(*vRenderer->actors[7].collider)){
				danny = !danny;
			}
			if (cameraPosition->actionCollider->isColliding(*vRenderer->actors[3].collider)) {
				doorOpen = !doorOpen;
				if (doorIsClosed)
					engine->play2D("./audio/doorOpen.wav");
				if (!doorIsClosed)
					engine->play2D("./audio/doorClose.wav");
				vRenderer->actors[3].collider->setActive(false);
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
			camera->Perspective(fovy, aspectRatio, 0.5f, 90.0f);
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
	if(!sit && !sleep){
		float fb = (front ? 0.125f : 0 + back ? -0.075f : 0) * 0.01;
		float lr = (right ? 0.1 : 0 + left ? -0.1 : 0) * 0.01;
		Vec3 movement = (
			Vec3(-sin(cameraPosition->gamma * deg2rad), 0, cos(cameraPosition->gamma * deg2rad)) * rad2deg * fb)
			+ (
				Vec3(-sin((90 + cameraPosition->gamma) * deg2rad), 0, cos((90 + cameraPosition->gamma) * deg2rad)) * rad2deg * lr);
		if (MATH::VMath::mag(movement) > 0.001f)
			movement = MATH::VMath::normalize(movement) * speed; 
		Vec3 newPos = cameraPosition->position + movement;
		Collider temp = *cameraPosition->collider;
		temp.setPosition(Vec3{-newPos.x, -newPos.y, -newPos.z});
		bool is_colliding = false;
		for(int actorI = 0; actorI < vRenderer->actors.size(); actorI++)
		{
			if(!is_colliding && vRenderer->actors[actorI].collider)
				is_colliding = is_colliding || temp.isColliding(*vRenderer->actors[actorI].collider);
		}
		if(!is_colliding)
		{
			cameraPosition->setPosition(newPos);
		}
	}

	cameraPosition->setTheta(cameraPosition->theta += lookUp ? -2 : 0 + lookDown ? 2 : 0);

	if (cameraPosition->theta < -90) {
		cameraPosition->setTheta(-90);
	}
	else if (cameraPosition->theta > 90) {
		cameraPosition->setTheta(90);
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
		camera->Perspective(fovy, aspectRatio, 0.5f, 90.0f);
	}
	if (!zoom && fovy < 45.0f) {
		fovy+= 0.9;
		camera->Perspective(fovy, aspectRatio, 0.5f, 90.0f);
	}

	cameraPosition->gamma += lookLeft ? -2 : 0 + lookRight ? 2 : 0;
	if(danny){
		ai.update();
		if (vRenderer->actors[5].collider && cameraPosition->collider->isColliding(*vRenderer->actors[5].collider))
			sceneManager->ChangeScene(SceneManager::SCENE_NUMBER::SCENE1);
	}
}

void Scene0::setMatrix(Actor* a) const{
	vRenderer->SetModelMatrixPush(a,  MMath::translate(a->position)
									* MMath::rotate(a->thetaRadianRotation, 1, 0, 0) 
									* MMath::rotate(a->gammaRadianRotation, 0, 1, 0)  
									* MMath::scale(a->scale));
}

void Scene0::Render() const {

	GlobalLighting gl = GlobalLighting{ { LightUBO{ Vec4{0.0f, 5.0f, 0.0f, 1.0f}, Vec4{0.75, 0.70, 0.70, 1}}}, 1, 0, distort };

	for (int i = 0; i < vRenderer->actors.size(); i++)
	{
		setMatrix(&vRenderer->actors[i]);
	}

	if(sit) {
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(), MMath::rotate(cameraPosition->theta, Vec3(1, 0, 0))
			* MMath::rotate(cameraPosition->gamma, 0, 1, 0)
			* MMath::translate(Vec3{ -vRenderer->actors[chair].position.x, cameraPosition->position.y - 0.0f, -vRenderer->actors[chair].position.z }), cameraPosition->position);
	}
	else if (sleep) {
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(), MMath::rotate(-30, Vec3(1, 0, 0))
			* MMath::rotate(-90, 0, 1, 0)
			* MMath::translate(Vec3{ -vRenderer->actors[14].position.x, cameraPosition->position.y + 0.5f, -vRenderer->actors[14].position.z }), cameraPosition->position);
	}
	else {
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(), MMath::rotate(cameraPosition->theta, Vec3(1, 0, 0))
			* MMath::rotate(cameraPosition->gamma, 0, 1, 0)
			* MMath::translate(cameraPosition->position), -cameraPosition->position);
	}

	vRenderer->SetModelMatrixPush(&vRenderer->actors[6], MMath::translate(-vRenderer->pi.pos)
													  * MMath::rotate(vRenderer->pi.theta,Vec3(1,0,0)) 
													  * MMath::rotate(vRenderer->pi.gamma, 0, 1, 0)
													  * MMath::scale(vRenderer->actors[6].scale));

	vRenderer->commitFrame();

	if(light){
		gl = GlobalLighting{ { LightUBO{ Vec4(0.0f, 5.0f, 0.0f, 1.0f),Vec4(1, 1, 1, 1) },
							   LightUBO{ Vec4(vRenderer->actors[1].position, 1), Vec4(0.3, 0.3, 0.3, 1)} }, 2, 0, distort };
	}

	vRenderer->SetGLightsUBO(gl);
	vRenderer->SetTextureIndex(skin);
	vRenderer->Render();
}


void Scene0::OnDestroy() {
	
}
