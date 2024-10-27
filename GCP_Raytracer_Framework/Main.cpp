#include "GCP_GFX_Framework.h"

#include "RayTracer.h"
#include "Camera.h"
#include "RayObject.h"
#include "Sphere.h"
#include "Plane.h"
#include "Light.h"
#include "Timer.h"

#include <vector>

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
	Light light1(glm::vec3(-40, 0, -50), glm::vec3(1, 1, 1));
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

	// Pushes the framebuffer to OpenGL and renders to screen
	// Also contains an event loop that keeps the window going until it's closed
	_myFramework.ShowAndHold();

	return 0;
}
