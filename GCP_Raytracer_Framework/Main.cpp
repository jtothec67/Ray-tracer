#include "GCP_GFX_Framework.h"

#include "RayTracer.h"
#include "Camera.h"
#include "RayObject.h"
#include "Sphere.h"
#include "Plane.h"
#include "Light.h"
#include "Timer.h"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <vector>
#include <thread>

void TracePixel(int _fromx, int _fromy, int _tox, int _toy, glm::ivec2 _winSize, Camera* _camera, RayTracer* _rayTracer, GCP_Framework* _myFramework)
{
	for (int y = _fromy; y <= _toy; ++y)
	{
		if (y == _winSize.y) break;
		for (int x = _fromx; x <= _tox; ++x)
		{
			if (x == _winSize.x) break;
			Ray ray = _camera->GetRay(glm::ivec2(x, y), _winSize);
			glm::vec3 colour = _rayTracer->TraceRay(ray, _camera->GetPosition());
			_myFramework->DrawPixel(glm::ivec2(x, y), colour);
		}
	}
}

void RayTraceParallel(int _numOfThreads, glm::ivec2 _winSize, Camera* _camera, RayTracer* _rayTracer, GCP_Framework* _myFramework)
{
	//std::cout << _numOfThreads << " Rendering time: ";
	//Timer timer;
	
	std::vector<std::thread> threads;
	int rowsPerThread = std::ceil(_winSize.y / static_cast<float>(_numOfThreads));

	for (int i = 0; i < _numOfThreads; ++i) {
		int startY = i * rowsPerThread;
		int endY = std::min(startY + rowsPerThread, _winSize.y);

		threads.emplace_back(TracePixel, 0, startY, _winSize.x - 1, endY - 1, _winSize, _camera, _rayTracer, _myFramework);
	}

	for (auto& thread : threads) {
		thread.join();
	}
}

void PlaceSpheresRandomly(RayTracer* _rayTracer, int _numOfSpheres, glm::vec3 _minPos, glm::vec3 _maxPos, float _minRadius, float _maxRadius, glm::vec3 _minColour, glm::vec3 _maxColour, float _minReflectivity, float _maxReflectivity)
{
	for (int i = 0; i < _numOfSpheres; ++i)
	{
		glm::vec3 pos = glm::vec3(
			_minPos.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxPos.x - _minPos.x))),
			_minPos.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxPos.y - _minPos.y))),
			_minPos.z + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxPos.z - _minPos.z))
				));

		float radius = _minRadius + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxRadius - _minRadius)));
		glm::vec3 colour = glm::vec3(
			_minColour.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxColour.x - _minColour.x))),
			_minColour.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxColour.y - _minColour.y))),
			_minColour.z + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxColour.z - _minColour.z))
				));

		float reflectivity = _minReflectivity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxReflectivity - _minReflectivity)));

		Sphere sphere(pos, radius, colour, reflectivity);
		RayObject* raySphere = (RayObject*)&sphere;
		_rayTracer->rayObjects.push_back(raySphere);
	}
}

int main(int argc, char* argv[])
{
	// Set window size
	glm::ivec2 winSize(640, 480);

	// This will handle rendering to screen
	GCP_Framework _myFramework;

	// Initialises SDL and OpenGL and sets up a framebuffer
	if (!_myFramework.Init(winSize))
	{
		return -1;
	}

	glm::vec3 camPos = glm::vec3(0, 0, 0);
	glm::vec3 camRot = glm::vec3(0, 0, 0);

	Camera camera(camPos, camRot, winSize);

	RayTracer rayTracer;

	std::vector<Light> lights;

	glm::vec3 lightPos1 = glm::vec3(-40, 0, -40);
	Light light1(lightPos1, glm::vec3(1, 1, 1));
	lights.push_back(light1);
	Sphere lightSphere1(lightPos1, 2, glm::vec3(1, 1, 1), 0);
	RayObject* rayLightSphere1 = (RayObject*)&lightSphere1;
	rayLightSphere1->mIsLight = true;
	rayTracer.rayObjects.push_back(rayLightSphere1);

	glm::vec3 lightPos2 = glm::vec3(45, 10, -50);
	Light light2(lightPos2, glm::vec3(1, 1, 1));
	lights.push_back(light2);
	Sphere lightSphere2(lightPos2, 2, glm::vec3(1, 1, 1), 0);
	RayObject* rayLightSphere2 = (RayObject*)&lightSphere2;
	rayLightSphere2->mIsLight = true;
	rayTracer.rayObjects.push_back(rayLightSphere2);

	rayTracer.SetLights(&lights);

	Sphere* sphere1 = new Sphere(glm::vec3(-10, 0, -50), 10, glm::vec3(0, 1, 0), 1);
	sphere1->mMetallic = 0.6f;
	RayObject* raySphere1 = (RayObject*)sphere1;
	rayTracer.rayObjects.push_back(raySphere1);

	Sphere* sphere2 = new Sphere(glm::vec3(5, -5, -62), 10, glm::vec3(1, 0, 0), 1);
	sphere2->mMetallic = 0;
	RayObject* raySphere2 = (RayObject*)sphere2;
	rayTracer.rayObjects.push_back(raySphere2);

	Plane plane1(glm::vec3(0, -14, -50), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), 0);
	plane1.mMetallic = 0;
	plane1.mRoughness = 1;
	RayObject* rayPlane1 = (RayObject*)&plane1;
	rayTracer.rayObjects.push_back(rayPlane1);

	Plane plane2(glm::vec3(0, 0, -70), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), 0);
	plane2.mMetallic = 0;
	plane2.mRoughness = 1;
	RayObject* rayPlane2 = (RayObject*)&plane2;
	rayTracer.rayObjects.push_back(rayPlane2);

	glm::vec3 minPos = glm::vec3(-40, -14, -50);
	glm::vec3 maxPos = glm::vec3(40, 30, -50);

	float minRadius = 1;
	float maxRadius = 5;

	glm::vec3 _minColour = glm::vec3(0, 0, 0);
	glm::vec3 _maxColour = glm::vec3(1, 1, 1);

	float _minReflectivity = 0;
	float _maxReflectivity = 1;

	srand(time(NULL));
	for (int i = 0; i < 10; ++i)
	{
		glm::vec3 pos = glm::vec3(
			minPos.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxPos.x - minPos.x))),
			minPos.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxPos.y - minPos.y))),
			minPos.z + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxPos.z - minPos.z))
				));

		float radius = minRadius + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxRadius - minRadius)));
		glm::vec3 colour = glm::vec3(
			_minColour.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxColour.x - _minColour.x))),
			_minColour.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxColour.y - _minColour.y))),
			_minColour.z + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxColour.z - _minColour.z))
				));

		float reflectivity = _minReflectivity + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_maxReflectivity - _minReflectivity)));

		Sphere* sphere = new Sphere(pos, radius, colour, reflectivity);
		RayObject* raySphere = (RayObject*)sphere;
		//rayTracer.rayObjects.push_back(raySphere);
	}

	/*for (int i = 0; i < 2; i++)
	{

		Sphere sphere(glm::vec3((-10 + rand() % 21), 0, -50), 4, glm::vec3(1, 0, 0), 0);
		RayObject* raySphere = (RayObject*)&sphere;
		rayTracer.rayObjects.push_back(raySphere);

		Sphere sphere2(glm::vec3((-10 + rand() % 21), 0, -50), 4, glm::vec3(1, 0, 0), 0);
		RayObject* raySphere2 = (RayObject*)&sphere2;
		rayTracer.rayObjects.push_back(raySphere2);

		Sphere sphere3(glm::vec3((-10 + rand() % 21), 0, -50), 4, glm::vec3(1, 0, 0), 0);
		RayObject* raySphere3 = (RayObject*)&sphere3;
		rayTracer.rayObjects.push_back(raySphere3);
	}*/

	// Setting up the GUI system
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	const char* glslVersion = "#version 430";
	ImGui_ImplSDL2_InitForOpenGL(_myFramework.GetWindow(), _myFramework.GetGLContext());
	ImGui_ImplOpenGL3_Init(glslVersion);

	SDL_Event e;

	bool running = true;
	while (running)
	{
		Timer timer;

		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);

			if (e.type == SDL_QUIT)
			{
				running = false;
			}
			if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_w:
					camPos.z += 1;
					camera.SetPosition(camPos);
					break;
				case SDLK_s:
					camPos.z -= 1;
					camera.SetPosition(camPos);
					break;
				case SDLK_a:
					camPos.x += 1;
					camera.SetPosition(camPos);
					break;
				case SDLK_d:
					camPos.x -= 1;
					camera.SetPosition(camPos);
					break;
				case SDLK_q:
					camPos.y -= 1;
					camera.SetPosition(camPos);
					break;
				case SDLK_e:
					camPos.y += 1;
					camera.SetPosition(camPos);
					break;
				case SDLK_UP:
					camRot.x -= 1;
					camera.SetRotation(camRot);
					break;
				case SDLK_DOWN:
					camRot.x += 1;
					camera.SetRotation(camRot);
					break;
				case SDLK_LEFT:
					camRot.y -= 1;
					camera.SetRotation(camRot);
					break;
				case SDLK_RIGHT:
					camRot.y += 1;
					camera.SetRotation(camRot);
					break;
				case SDLK_p:
					for (auto rayObject : rayTracer.rayObjects)
					{
						rayObject->mPBR = !rayObject->mPBR;
					}
				}
			}
		}

		_myFramework.UpdateWindow(winSize.x, winSize.y);

		_myFramework.ClearWindow();

		RayTraceParallel(16, winSize, &camera, &rayTracer, &_myFramework);

		{

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Material Controls");
			ImGui::Text("Sphere 1");

			glm::vec3 albedo1 = sphere1->mAlbedo;
			ImGui::ColorEdit3("Albedo", &albedo1[0]);
			sphere1->mAlbedo = albedo1;

			float metallic1 = sphere1->mMetallic;
			ImGui::SliderFloat("Metallic", &metallic1, 0.0f, 1.0f);
			sphere1->mMetallic = metallic1;

			float roughness1 = sphere1->mRoughness;
			ImGui::SliderFloat("Roughness", &roughness1, 0.0f, 1.0f);
			sphere1->mRoughness = roughness1;

			float ambientOcclusion1 = sphere1->mAmbientOcclusion;
			ImGui::SliderFloat("Ambient Occlusion", &ambientOcclusion1, 0.0f, 1.0f);
			sphere1->mAmbientOcclusion = ambientOcclusion1;

			ImGui::Text("Sphere 2");

			glm::vec3 albedo2 = sphere2->mAlbedo;
			ImGui::ColorEdit3("Albedo2", &albedo2[0]);
			sphere2->mAlbedo = albedo2;

			float metallic2 = sphere2->mMetallic;
			ImGui::SliderFloat("Metallic2", &metallic2, 0.0f, 1.0f);
			sphere2->mMetallic = metallic2;

			float roughness2 = sphere2->mRoughness;
			ImGui::SliderFloat("Roughness2", &roughness2, 0.0f, 1.0f);
			sphere2->mRoughness = roughness2;

			float ambientOcclusion2 = sphere2->mAmbientOcclusion;
			ImGui::SliderFloat("Ambient Occlusion2", &ambientOcclusion2, 0.0f, 1.0f);
			sphere2->mAmbientOcclusion = ambientOcclusion2;

			ImGui::End();

		}

		_myFramework.DrawScreenTexture();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		_myFramework.SwapWindow();

		//std::cout << "Frame time: " << timer.GetElapsedMilliseconds() << std::endl;
		std::cout << "FPS: " << 1000 / timer.GetElapsedMilliseconds() << std::endl;
		timer.Stop();
	}

	// Shut down GUI system
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return 0;
}