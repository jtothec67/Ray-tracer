#include "GCP_GFX_Framework.h"

#include "RayTracer.h"
#include "Camera.h"
#include "RayObject.h"
#include "Sphere.h"
#include "Plane.h"
#include "Light.h"
#include "Timer.h"
#include "ThreadPool.h"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <vector>
#include <thread>

void TracePixels(int _fromy, int _toy, glm::ivec2 _winSize, Camera* _camera, RayTracer* _rayTracer, GCP_Framework* _myFramework)
{
	for (int y = _fromy; y <= _toy; ++y)
	{
		if (y >= _winSize.y) break;
		for (int x = 0; x <= _winSize.x; ++x)
		{
			if (x >= _winSize.x) break;
			Ray ray = _camera->GetRay(glm::ivec2(x, y), _winSize);
			glm::vec3 colour = _rayTracer->TraceRay(ray, _camera->GetPosition(), 0);
			_myFramework->DrawPixel(glm::ivec2(x, y), colour);
		}
	}
}

void RayTraceParallel(ThreadPool& threadPool, int _numTasks, glm::ivec2 _winSize, Camera* _camera, RayTracer* _rayTracer, GCP_Framework* _myFramework)
{
	//std::vector<std::thread> threads;
	int rowsPerThread = std::ceil(_winSize.y / static_cast<float>(_numTasks));

	for (int i = 0; i < _numTasks; ++i)
	{
		int startY = i * rowsPerThread;
		int endY = std::min(startY + rowsPerThread, _winSize.y);

		threadPool.EnqueueTask([=] {
			TracePixels(startY, endY - 1, _winSize, _camera, _rayTracer, _myFramework);
			});

		//threads.emplace_back(TracePixels, startY, endY - 1, _winSize, _camera, _rayTracer, _myFramework);
	}

	/*for (auto& thread : threads)
	{
		thread.join();
	}*/

	threadPool.WaitForCompletion();
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
	Sphere lightSphere1(lightPos1, 2, glm::vec3(1, 1, 1));
	RayObject* rayLightSphere1 = (RayObject*)&lightSphere1;
	rayLightSphere1->mIsLight = true;
	rayTracer.rayObjects.push_back(rayLightSphere1);

	glm::vec3 lightPos2 = glm::vec3(45, 10, -50);
	Light light2(lightPos2, glm::vec3(1, 1, 1));
	lights.push_back(light2);
	Sphere lightSphere2(lightPos2, 2, glm::vec3(1, 1, 1));
	RayObject* rayLightSphere2 = (RayObject*)&lightSphere2;
	rayLightSphere2->mIsLight = true;
	rayTracer.rayObjects.push_back(rayLightSphere2);

	rayTracer.SetLights(&lights);

	Sphere* sphere1 = new Sphere(glm::vec3(-10, 0, -50), 10, glm::vec3(1.f, 0.898, 0.477));
	sphere1->mMetallic = 0.0f;
	sphere1->mShininess = 0.0f;
	sphere1->mRoughness = 1;
	RayObject* raySphere1 = (RayObject*)sphere1;
	rayTracer.rayObjects.push_back(raySphere1);

	Sphere* sphere2 = new Sphere(glm::vec3(5, -5, -62), 10, glm::vec3(1, 0, 0));
	sphere2->mMetallic = 0;
	RayObject* raySphere2 = (RayObject*)sphere2;
	rayTracer.rayObjects.push_back(raySphere2);

	Plane plane1(glm::vec3(0, -14, -50), glm::vec3(0, 1, 0), glm::vec3(1, 1, 1));
	plane1.mShininess = 0;
	plane1.mMetallic = 0;
	plane1.mRoughness = 1;
	RayObject* rayPlane1 = (RayObject*)&plane1;
	rayTracer.rayObjects.push_back(rayPlane1);

	Plane plane2(glm::vec3(0, 0, -70), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));
	plane2.mShininess = 0;
	plane2.mMetallic = 0;
	plane2.mRoughness = 1;
	RayObject* rayPlane2 = (RayObject*)&plane2;
	rayTracer.rayObjects.push_back(rayPlane2);


	// Setting up the GUI system
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	const char* glslVersion = "#version 430";
	ImGui_ImplSDL2_InitForOpenGL(_myFramework.GetWindow(), _myFramework.GetGLContext());
	ImGui_ImplOpenGL3_Init(glslVersion);

	SDL_Event e;

	bool restartFrameTimer = true;
	Timer averageFrameTimer;
	int frames = 0;

	float fps = 0.f;

	int numTasks = 16;
	ThreadPool threadPool(16);

	bool running = true;
	while (running)
	{
		Timer timer;

		if (restartFrameTimer)
		{
			averageFrameTimer.Start();
			restartFrameTimer = false;
		}

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
					camera.SetPosition(camera.GetPosition() + camera.GetForward());
					break;
				case SDLK_s:
					camera.SetPosition(camera.GetPosition() - camera.GetForward());
					break;
				case SDLK_a:
					camera.SetPosition(camera.GetPosition() + camera.GetRight());
					break;
				case SDLK_d:
					camera.SetPosition(camera.GetPosition() - camera.GetRight());
					break;
				case SDLK_q:
					camera.SetPosition(camera.GetPosition() - camera.GetUp());
					break;
				case SDLK_e:
					camera.SetPosition(camera.GetPosition() + camera.GetUp());
					break;
				case SDLK_UP:
					camera.SetRotation(glm::vec3(camera.GetRotation().x - 1, camera.GetRotation().y, camera.GetRotation().z));
					break;
				case SDLK_DOWN:
					camera.SetRotation(glm::vec3(camera.GetRotation().x + 1, camera.GetRotation().y, camera.GetRotation().z));
					break;
				case SDLK_LEFT:
					camera.SetRotation(glm::vec3(camera.GetRotation().x, camera.GetRotation().y - 1, camera.GetRotation().z));
					break;
				case SDLK_RIGHT:
					camera.SetRotation(glm::vec3(camera.GetRotation().x, camera.GetRotation().y + 1, camera.GetRotation().z));
					break;
				}
			}
		}

		_myFramework.UpdateWindow(winSize.x, winSize.y);

		_myFramework.ClearWindow();

		RayTraceParallel(threadPool, numTasks, winSize, &camera, &rayTracer, &_myFramework);

		{

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Scene Controls");

			ImGui::Text("FPS: %s", std::to_string(fps).c_str());

			int tasks = numTasks;
			ImGui::SliderInt("Number of tasks", &tasks, 0, 128);
			numTasks = tasks;

			bool pbr = rayTracer.mPBR;
			ImGui::Checkbox("PBR", &pbr);
			rayTracer.mPBR = pbr;

			bool shadows = rayTracer.mShadows;
			ImGui::Checkbox("Shadows", &shadows);
			rayTracer.mShadows = shadows;

			bool ambientOcclusion = rayTracer.mAmbientOcclusion;
			ImGui::Checkbox("Ambient Occlusion", &ambientOcclusion);
			rayTracer.mAmbientOcclusion = ambientOcclusion;

			glm::vec3 ambientColour = rayTracer.mAmbientColour;
			ImGui::ColorEdit3("Ambient Colour", &ambientColour[0]);
			rayTracer.mAmbientColour = ambientColour;

			float aoStrength = rayTracer.mAOStrength;
			ImGui::SliderFloat("AO Strength", &aoStrength, 0.0f, 1.0f);
			rayTracer.mAOStrength = aoStrength;

			float aoRadius = rayTracer.mAORadius;
			ImGui::SliderFloat("AO Radius", &aoRadius, 0.0f, 10.0f);
			rayTracer.mAORadius = aoRadius;

			int numSamples = rayTracer.mNumAOSamples;
			ImGui::SliderInt("Num AO Samples", &numSamples, 0, 128);
			rayTracer.SetNumSamples(numSamples);

			int depth = rayTracer.mMaxDepth;
			ImGui::SliderInt("Max reflectivity depth", &depth, 0, 10);
			rayTracer.mMaxDepth = depth;


			ImGui::Text("Sphere 1");

			float shiney1 = sphere1->mShininess;
			ImGui::SliderFloat("Shininess(pbr)", &shiney1, 0.0f, 1.0f);
			sphere1->mShininess = shiney1;

			glm::vec3 albedo1 = sphere1->mAlbedo;
			ImGui::ColorEdit3("Albedo", &albedo1[0]);
			sphere1->mAlbedo = albedo1;

			float metallic1 = sphere1->mMetallic;
			ImGui::SliderFloat("Metallic", &metallic1, 0.0f, 1.0f);
			sphere1->mMetallic = metallic1;

			float roughness1 = sphere1->mRoughness;
			ImGui::SliderFloat("Roughness", &roughness1, 0.0f, 1.0f);
			sphere1->mRoughness = roughness1;

			float reflectivity1 = sphere1->mReflectivity;
			ImGui::SliderFloat("Reflectivity", &reflectivity1, 0.0f, 1.0f);
			sphere1->mReflectivity = reflectivity1;

			float refractiveIndex1 = sphere1->mRefractiveIndex;
			ImGui::SliderFloat("Refractive Index", &refractiveIndex1, 1.0f, 2.0f);
			sphere1->mRefractiveIndex = refractiveIndex1;

			float transparency1 = sphere1->mTransparency;
			ImGui::SliderFloat("Transparency", &transparency1, 0.0f, 1.0f);
			sphere1->mTransparency = transparency1;


			ImGui::Text("Sphere 2");

			float shiney2 = sphere2->mShininess;
			ImGui::SliderFloat("Shininess2(pbr)", &shiney2, 0.0f, 1.0f);
			sphere2->mShininess = shiney2;

			glm::vec3 albedo2 = sphere2->mAlbedo;
			ImGui::ColorEdit3("Albedo2", &albedo2[0]);
			sphere2->mAlbedo = albedo2;

			float metallic2 = sphere2->mMetallic;
			ImGui::SliderFloat("Metallic2", &metallic2, 0.0f, 1.0f);
			sphere2->mMetallic = metallic2;

			float roughness2 = sphere2->mRoughness;
			ImGui::SliderFloat("Roughness2", &roughness2, 0.0f, 1.0f);
			sphere2->mRoughness = roughness2;

			float reflectivity2 = sphere2->mReflectivity;
			ImGui::SliderFloat("Reflectivity2", &reflectivity2, 0.0f, 1.0f);
			sphere2->mReflectivity = reflectivity2;

			float refractiveIndex2 = sphere2->mRefractiveIndex;
			ImGui::SliderFloat("Refractive Index2", &refractiveIndex2, 1.0f, 2.0f);
			sphere2->mRefractiveIndex = refractiveIndex2;

			float transparency2 = sphere2->mTransparency;
			ImGui::SliderFloat("Transparency2", &transparency2, 0.0f, 1.0f);
			sphere2->mTransparency = transparency2;

			ImGui::End();

		}

		_myFramework.DrawScreenTexture();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		_myFramework.SwapWindow();

		fps = 1000 / timer.GetElapsedMilliseconds();
		timer.Stop();

		if (averageFrameTimer.GetElapsedMilliseconds() > 1000)
		{
			std::cout << "Average FPS: " << frames << std::endl;
			averageFrameTimer.Stop();
			restartFrameTimer = true;
			frames = 0;
		}
		else
		{
			frames++;
		}
	}

	// Shut down GUI system
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// Shutdown thread pool
	threadPool.Shutdown();

	return 0;
}