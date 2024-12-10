#include "GCP_GFX_Framework.h"

#include "RayTracer.h"
#include "Camera.h"
#include "RayObject.h"
#include "Sphere.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Box.h"
#include "Light.h"
#include "Timer.h"
#include "ThreadPool.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include <fstream>
#include <vector>
#include <thread>

void Test1(glm::ivec2& _winSize, Camera& _camera, RayTracer& _rayTracer, GCP_Framework& _myFramework, ThreadPool& _threadPool);
void Test2(glm::ivec2& _winSize, Camera& _camera, RayTracer& _rayTracer, GCP_Framework& _myFramework, ThreadPool& _threadPool);

void InitialiseScene1(RayTracer& _rayTracer);
void InitialiseScene2(RayTracer& _rayTracer);

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
	int rowsPerThread = std::ceil(_winSize.y / static_cast<float>(_numTasks));

	for (int i = 0; i < _numTasks; ++i)
	{
		int startY = i * rowsPerThread;
		int endY = std::min(startY + rowsPerThread, _winSize.y);

		threadPool.EnqueueTask([=] { TracePixels(startY, endY - 1, _winSize, _camera, _rayTracer, _myFramework); });
	}

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

	InitialiseScene1(rayTracer);

	// Setting up the GUI system
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	const char* glslVersion = "#version 430";
	ImGui_ImplSDL2_InitForOpenGL(_myFramework.GetWindow(), _myFramework.GetGLContext());
	ImGui_ImplOpenGL3_Init(glslVersion);

	SDL_Event e;

	bool restartFrameTimer = true;
	Timer averageFrameTimer;
	int frames = 0;

	float fps = 0.f;

	int numTasks = 16;
	int numThreads = 16;
	ThreadPool threadPool(numThreads);

	int newBallCount = 0;

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
					camera.SetPosition(camera.GetPosition() - glm::vec3(0, 1, 0));
					break;
				case SDLK_e:
					camera.SetPosition(camera.GetPosition() + glm::vec3(0, 1, 0));
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
				case SDLK_m:
				{
					Sphere* sphere = new Sphere("New Sphere " + std::to_string(rayTracer.GetSizeOfRayObjects()), -camera.GetPosition(), 10, glm::vec3(1, 1, 1));
					RayObject* raySphere = (RayObject*)sphere;
					rayTracer.AddRayObject(raySphere);
					break;
				}
				case SDLK_p:
				{
					Plane* plane = new Plane("New Plane " + std::to_string(rayTracer.GetSizeOfRayObjects()), -camera.GetPosition(), glm::vec3(0, 1, 0), glm::vec3(1, 1, 1));
					RayObject* rayPlane = (RayObject*)plane;
					rayTracer.AddRayObject(rayPlane);
					break;
				}
				case SDLK_l:
				{
					Light* light = new Light("Light " + std::to_string(rayTracer.GetSizeOfLights()), - camera.GetPosition(), glm::vec3(1, 1, 1));
					rayTracer.AddLight(light);

					Sphere* lightSphere = new Sphere("Light " + std::to_string(rayTracer.GetSizeOfLights()), -camera.GetPosition(), 2, glm::vec3(1, 1, 1));
					RayObject* rayLightSphere = (RayObject*)lightSphere;
					rayLightSphere->IsLight(true);
					rayTracer.AddRayObject(rayLightSphere);
					break;
				}
				case SDLK_c:
				{
					Cylinder* cylinder = new Cylinder("New Cylinder " + std::to_string(rayTracer.GetSizeOfRayObjects()), -camera.GetPosition(), 10, 20, glm::vec3(1, 1, 1));
					RayObject* rayCylinder = (RayObject*)cylinder;
					rayTracer.AddRayObject(rayCylinder);
					break;
				}
				case SDLK_b:
				{
					Box* box = new Box("New Box " + std::to_string(rayTracer.GetSizeOfRayObjects()), -camera.GetPosition(), glm::vec3(20, 20, 20), glm::vec3(1, 1, 1));
					RayObject* rayBox = (RayObject*)box;
					rayTracer.AddRayObject(rayBox);
					break;
				}
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

			if (ImGui::Button("Scene 1"))
			{
				rayTracer.ClearScene();
				InitialiseScene1(rayTracer);
			}

			if (ImGui::Button("Scene 2"))
			{
				rayTracer.ClearScene();
				InitialiseScene2(rayTracer);
			}

			if (ImGui::Button("Test 1"))
			{
				Test1(winSize, camera, rayTracer, _myFramework, threadPool);
			}

			if (ImGui::Button("Test 2"))
			{
				Test2(winSize, camera, rayTracer, _myFramework, threadPool);
			}

			int tasks = numTasks;
			ImGui::SliderInt("Number of tasks", &tasks, 0, 128);
			numTasks = tasks;

			int threads = numThreads;
			ImGui::SliderInt("Number of threads", &threads, 0, 128);
			if (threads != numThreads)
			{
				threadPool.Shutdown();
				threadPool.InitialiseThreads(threads);
			}
			numThreads = threads;

			bool pbr = rayTracer.GetPBR();
			ImGui::Checkbox("PBR", &pbr);
			rayTracer.SetPBR(pbr);

			bool shadows = rayTracer.GetShadows();
			ImGui::Checkbox("Shadows", &shadows);
			rayTracer.SetShadows(shadows);

			bool ambientOcclusion = rayTracer.GetAmbientOcclusion();
			ImGui::Checkbox("Ambient Occlusion", &ambientOcclusion);
			rayTracer.SetAmbientOcclusion(ambientOcclusion);

			glm::vec3 ambientColour = rayTracer.GetAmbientColour();
			ImGui::ColorEdit3("Ambient Colour", &ambientColour[0]);
			rayTracer.SetAmbientColour(ambientColour);

			float aoStrength = rayTracer.GetAOStrength();
			ImGui::SliderFloat("AO Strength", &aoStrength, 0.0f, 2.0f);
			rayTracer.SetAOStrength(aoStrength);

			float aoRadius = rayTracer.GetAORadius();
			ImGui::SliderFloat("AO Radius", &aoRadius, 0.0f, 10.0f);
			rayTracer.SetAORadius(aoRadius);

			int numSamples = rayTracer.GetNumAOSamples();
			ImGui::SliderInt("Num AO Samples", &numSamples, 0, 128);
			rayTracer.SetNumAOSamples(numSamples);

			int depth = rayTracer.GetMaxDepth();
			ImGui::SliderInt("Max reflectivity depth", &depth, 0, 10);
			rayTracer.SetMaxDepth(depth);

			for (auto& light : rayTracer.GetLights())
			{
				light->UpdateUI();
			}

			for (auto& rayObject : rayTracer.GetRayObjects())
			{
				rayObject->UpdateUI();
			}

			ImGui::End();

		}

		_myFramework.DrawScreenTexture();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
			SDL_GLContext baclup_current_context = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backup_current_window, baclup_current_context);
		}

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

	return 0;
}

void Test1(glm::ivec2& _winSize, Camera& _camera, RayTracer& _rayTracer, GCP_Framework& _myFramework, ThreadPool& _threadPool)
{
	std::ofstream csvFile("test_1_results.csv");
	csvFile << "Threads,AverageFrameTime\n";

	_camera.SetPosition(glm::vec3(0, 0, 0));
	_camera.SetRotation(glm::vec3(0, 0, 0));

	InitialiseScene1(_rayTracer);

	for (int numThreads = 1; numThreads <= 250; ++numThreads)
	{
		std::cout << "Testing " << numThreads << " threads" << std::endl;
		int numTasks = numThreads;
		_threadPool.Shutdown();
		_threadPool.InitialiseThreads(numThreads);

		float totalFrameTime = 0.0f;
		for (int frame = 0; frame < 10; ++frame)
		{
			_myFramework.ClearWindow();
			Timer timer;
			RayTraceParallel(_threadPool, numTasks, _winSize, &_camera, &_rayTracer, &_myFramework);
			totalFrameTime += timer.GetElapsedMilliseconds();
			timer.Stop();
			_myFramework.DrawScreenTexture();
		}

		float averageFrameTime = totalFrameTime / 10.0f;
		csvFile << numThreads << "," << averageFrameTime << "\n";
	}

	csvFile.close();
}

void Test2(glm::ivec2& _winSize, Camera& _camera, RayTracer& _rayTracer, GCP_Framework& _myFramework, ThreadPool& _threadPool)
{
	std::ofstream csvFile("test_2_results.csv");
	csvFile << "Tasks,16 thread,32 threads,64 threads,128 threads\n";

	_camera.SetPosition(glm::vec3(0, 0, 0));
	_camera.SetRotation(glm::vec3(0, 0, 0));

	InitialiseScene1(_rayTracer);

	std::vector<int> threadCounts = { 16, 32, 64, 128 };

	for (int numTasks = 1; numTasks <= 128; ++numTasks)
	{
		csvFile << numTasks;

		for (int numThreads : threadCounts)
		{
			std::cout << "Testing " << numTasks << " tasks with " << numThreads << " threads" << std::endl;
			_threadPool.Shutdown();
			_threadPool.InitialiseThreads(numThreads);

			float totalFrameTime = 0.0f;
			for (int frame = 0; frame < 10; ++frame)
			{
				_myFramework.ClearWindow();
				Timer timer;
				RayTraceParallel(_threadPool, numTasks, _winSize, &_camera, &_rayTracer, &_myFramework);
				totalFrameTime += timer.GetElapsedMilliseconds();
				timer.Stop();
				_myFramework.DrawScreenTexture();
			}

			float averageFrameTime = totalFrameTime / 10.0f;
			csvFile << "," << averageFrameTime;
		}

		csvFile << "\n";
	}

	csvFile.close();
}

void InitialiseScene1(RayTracer& _rayTracer)
{
	_rayTracer.SetAORadius(5.f);

	glm::vec3 lightPos1 = glm::vec3(-40, 0, -50);
	Light* light1 = new Light("Light 1", lightPos1, glm::vec3(1, 1, 1));
	_rayTracer.AddLight(light1);
	Sphere* lightSphere1 = new Sphere("Light1", lightPos1, 2, glm::vec3(1, 1, 1));
	RayObject* rayLightSphere1 = (RayObject*)lightSphere1;
	rayLightSphere1->IsLight(true);
	_rayTracer.AddRayObject(rayLightSphere1);

	glm::vec3 lightPos2 = glm::vec3(45, 10, -50);
	Light* light2 = new Light("Light 2", lightPos2, glm::vec3(1, 1, 1));
	_rayTracer.AddLight(light2);
	Sphere* lightSphere2 = new Sphere("Light2", lightPos2, 2, glm::vec3(1, 1, 1));
	RayObject* rayLightSphere2 = (RayObject*)lightSphere2;
	rayLightSphere2->IsLight(true);
	_rayTracer.AddRayObject(rayLightSphere2);

	Box* box1 = new Box("Box1", glm::vec3(-46.3, 33, -67), glm::vec3(20, 20, 20), glm::vec3(0, 0, 1));
	box1->SetAxis(glm::vec3(1, -0.36, 0.58));
	box1->SetShininess(0.0f);
	box1->SetReflectivity(0.7f);
	RayObject* rayBox1 = (RayObject*)box1;
	_rayTracer.AddRayObject(rayBox1);

	Cylinder* cylinder1 = new Cylinder("Cylinder1", glm::vec3(25.5f, -14.6f, -64), 8.5, 20, glm::vec3(0, 1, 0));
	cylinder1->SetMetallic(0.34f);
	cylinder1->SetShininess(0.0f);
	cylinder1->SetRoughness(0.13f);
	RayObject* rayCylinder1 = (RayObject*)cylinder1;
	_rayTracer.AddRayObject(rayCylinder1);

	Sphere* sphere1 = new Sphere("Sphere1", glm::vec3(-10, 0, -50), 10, glm::vec3(1.f, 0.898, 0.477));
	sphere1->SetShininess(0.0f);
	sphere1->SetTransparency(0.56f);
	sphere1->SetRefractiveIndex(1.15f);
	RayObject* raySphere1 = (RayObject*)sphere1;
	_rayTracer.AddRayObject(raySphere1);

	Sphere* sphere2 = new Sphere("Sphere2", glm::vec3(5, -5, -62), 10, glm::vec3(1, 0, 0));
	RayObject* raySphere2 = (RayObject*)sphere2;
	_rayTracer.AddRayObject(raySphere2);

	Plane* plane1 = new Plane("Plane1", glm::vec3(0, -14, -50), glm::vec3(0, 1, 0), glm::vec3(1, 1, 1));
	plane1->SetShininess(0);
	RayObject* rayPlane1 = (RayObject*)plane1;
	_rayTracer.AddRayObject(rayPlane1);

	Plane* plane2 = new Plane("Plane2", glm::vec3(0, 0, -70), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));
	plane2->SetShininess(0);
	plane2->SetRoughness(1);
	RayObject* rayPlane2 = (RayObject*)plane2;
	_rayTracer.AddRayObject(rayPlane2);
}

void InitialiseScene2(RayTracer& _rayTracer)
{
	_rayTracer.SetAORadius(1.1f);

	glm::vec3 sphereColor = glm::vec3(1, 1, 1);
	float sphereRadius = 1.0f;
	float sphereSpacingX = 2.01f; // Slightly more than diameter to avoid overlap
	float sphereSpacingY = 1.6f;
	float sphereShininess = 0.0f;
	int gridSize = 7; // 7x7 grid of spheres
	float offSet = -3.5f;

	for (int x = 0 + offSet; x < gridSize + offSet; ++x)
	{
		for (int y = 0 + offSet; y < gridSize + offSet; ++y)
		{
			float xOffset = (y % 2 == 0) ? 0.0f : sphereRadius; // Offset every other row
			glm::vec3 position = glm::vec3(x * sphereSpacingX + xOffset, y * sphereSpacingY, -10);
			Sphere* sphere = new Sphere("Sphere " + std::to_string(_rayTracer.GetSizeOfRayObjects()), position, sphereRadius, sphereColor);
			sphere->SetMetallic(0.0f);
			sphere->SetRoughness(1.0f);
			sphere->SetShininess(sphereShininess);
			RayObject* raySphere = (RayObject*)sphere;
			_rayTracer.AddRayObject(raySphere);
		}
	}

	glm::vec3 planePosition = glm::vec3(0, 0, -10 - sphereSpacingX);
	Plane* plane = new Plane("Plane", planePosition, glm::vec3(0, 0, 1), sphereColor);
	plane->SetMetallic(0.0f);
	plane->SetRoughness(1.0f);
	RayObject* rayPlane = (RayObject*)plane;
	_rayTracer.AddRayObject(rayPlane);

	glm::vec3 lightPosition = glm::vec3(0, 0, 0);
	Light* light = new Light("Light", lightPosition, glm::vec3(1, 1, 1));
	_rayTracer.AddLight(light);
	Sphere* lightSphere = new Sphere("LightSphere", lightPosition, 2, glm::vec3(1, 1, 1));
	lightSphere->IsLight(true);
	RayObject* rayLightSphere = (RayObject*)lightSphere;
}