#include "GCP_GFX_Framework.h"

#include "RayTracer.h"
#include "Camera.h"
#include "RayObject.h"
#include "Sphere.h"
#include "Plane.h"
#include "Light.h"
#include "Timer.h"

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

	std::vector<Light> lights;
	Light light1(glm::vec3(-40, 0, -40), glm::vec3(1, 1, 1));
	lights.push_back(light1);

	Light light2(glm::vec3(20, 0, -50), glm::vec3(1, 1, 1));
	lights.push_back(light2);

	RayTracer rayTracer;
	rayTracer.SetLights(&lights);

	Sphere sphere1(glm::vec3(-10, 0, -50), 10, glm::vec3(0, 1, 0), 1);
	RayObject* raySphere1 = (RayObject*)&sphere1;
	rayTracer.rayObjects.push_back(raySphere1);

	Sphere sphere2(glm::vec3(5, -5, -62), 10, glm::vec3(1, 0, 0), 1);
	RayObject* raySphere2 = (RayObject*)&sphere2;
	rayTracer.rayObjects.push_back(raySphere2);

	Plane plane1(glm::vec3(0, -14, -50), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), 0);
	RayObject* rayPlane1 = (RayObject*)&plane1;
	rayTracer.rayObjects.push_back(rayPlane1);

	Plane plane2(glm::vec3(0, 0, -70), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), 0);
	RayObject* rayPlane2 = (RayObject*)&plane2;
	rayTracer.rayObjects.push_back(rayPlane2);


	bool running = true;
	while (running)
	{
		Timer timer;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
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
				}
			}
		}

		_myFramework.UpdateWindow(winSize.x, winSize.y);

		RayTraceParallel(16, winSize, &camera, &rayTracer, &_myFramework);

		_myFramework.Show();

		//std::cout << "Frame time: " << timer.GetElapsedMilliseconds() << std::endl;
		std::cout << "FPS: " << 1000 / timer.GetElapsedMilliseconds() << std::endl;
		timer.Stop();
	}
	

	return 0;
}