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
			glm::vec3 colour = _rayTracer->TraceRay(ray);
			_myFramework->DrawPixel(glm::ivec2(x, y), colour);
		}
	}
	/*Ray ray = _camera->GetRay(glm::ivec2(_fromx, _fromy), _winSize);
	glm::vec3 colour = _rayTracer->TraceRay(ray);
	_myFramework->DrawPixel(glm::ivec2(_fromx, _fromy), colour);*/
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

	Sphere sphere1(glm::vec3(-10, 0, -50), 10, glm::vec3(0, 1, 0));
	RayObject* raySphere1 = (RayObject*)&sphere1;
	raySphere1->SetLights(&lights);
	rayTracer.rayObjects.push_back(raySphere1);

	Sphere sphere2(glm::vec3(5, -5, -62), 10, glm::vec3(1, 0, 0));
	RayObject* raySphere2 = (RayObject*)&sphere2;
	raySphere2->SetLights(&lights);
	rayTracer.rayObjects.push_back(raySphere2);

	Plane plane1(glm::vec3(0, -14, -50), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
	RayObject* rayPlane1 = (RayObject*)&plane1;
	rayPlane1->SetLights(&lights);
	rayTracer.rayObjects.push_back(rayPlane1);

	Plane plane2(glm::vec3(0, 0, -70), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1));
	RayObject* rayPlane2 = (RayObject*)&plane2;
	rayPlane2->SetLights(&lights);
	rayTracer.rayObjects.push_back(rayPlane2);

	/*std::cout << "Sequentially - " << std::endl;
	{
		Timer timer;

		for (int y = 0; y < winSize.y; ++y)
		{
			for (int x = 0; x < winSize.x; ++x)
			{
				Ray ray = camera.GetRay(glm::ivec2(x, y), winSize);
				glm::vec3 colour = rayTracer.TraceRay(ray);
				_myFramework.DrawPixel(glm::ivec2(x, y), colour);
			}
		}
	}

	std::cout << "4 Threads - " << std::endl;
	{
		Timer timer;

		std::thread bottomLeft(TracePixel, 0, 0, winSize.x / 2, winSize.y / 2, winSize, &camera, &rayTracer, &_myFramework);
		std::thread bottomRight(TracePixel, winSize.x / 2, 0, winSize.x, winSize.y / 2, winSize, &camera, &rayTracer, &_myFramework);
		std::thread topRight(TracePixel, winSize.x / 2, winSize.y / 2, winSize.x, winSize.y, winSize, &camera, &rayTracer, &_myFramework);
		std::thread topLeft(TracePixel, 0, winSize.y / 2, winSize.x / 2, winSize.y, winSize, &camera, &rayTracer, &_myFramework);

		bottomLeft.join();
		bottomRight.join();
		topRight.join();
		topLeft.join();
	}*/

	int numberOfThreads = 16;
	std::cout << numberOfThreads << " Threads - " << std::endl;
	{
		Timer timer;

		std::vector<std::thread> threads;
		int rowsPerThread = std::ceil(winSize.y / static_cast<float>(numberOfThreads));

		for (int i = 0; i < numberOfThreads; ++i) {
			int startY = i * rowsPerThread;
			int endY = std::min(startY + rowsPerThread, winSize.y);

			threads.emplace_back(TracePixel, 0, startY, winSize.x - 1, endY - 1, winSize, &camera, &rayTracer, &_myFramework);
		}

		for (auto& thread : threads) {
			thread.join();
		}
	}

	int numberOfThreads2 = 24;
	std::cout << numberOfThreads2 << " Threads - " << std::endl;
	{
		Timer timer;

		std::vector<std::thread> threads;
		int rowsPerThread = std::ceil(winSize.y / static_cast<float>(numberOfThreads2));

		for (int i = 0; i < numberOfThreads2; ++i) {
			int startY = i * rowsPerThread;
			int endY = std::min(startY + rowsPerThread, winSize.y);

			threads.emplace_back(TracePixel, 0, startY, winSize.x - 1, endY - 1, winSize, &camera, &rayTracer, &_myFramework);
		}

		for (auto& thread : threads) {
			thread.join();
		}
	}

	int numberOfThreads3 = 32;
	std::cout << numberOfThreads3 << " Threads - " << std::endl;
	{
		Timer timer;

		std::vector<std::thread> threads;
		int rowsPerThread = std::ceil(winSize.y / static_cast<float>(numberOfThreads3));

		for (int i = 0; i < numberOfThreads3; ++i) {
			int startY = i * rowsPerThread;
			int endY = std::min(startY + rowsPerThread, winSize.y);

			threads.emplace_back(TracePixel, 0, startY, winSize.x - 1, endY - 1, winSize, &camera, &rayTracer, &_myFramework);
		}

		for (auto& thread : threads) {
			thread.join();
		}
	}

	// Pushes the framebuffer to OpenGL and renders to screen
	// Also contains an event loop that keeps the window going until it's closed
	_myFramework.ShowAndHold();

	return 0;
}
