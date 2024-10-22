#include "GCP_GFX_Framework.h"

#include "RayTracer.h"
#include "Camera.h"
#include "RayObject.h"
#include "Sphere.h"
#include "Plane.h"

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

	Camera camera(glm::vec3(0.f, 0.f, 0.f), winSize);

	RayTracer rayTracer;

	Sphere sphere1(glm::vec3(-5, 0, -50), 10, glm::vec3(0, 1, 0));
	rayTracer.rayObjects.push_back((RayObject*)&sphere1);

	Sphere sphere2(glm::vec3(5, 0, -60), 10, glm::vec3(1, 0, 0));
	rayTracer.rayObjects.push_back((RayObject*)&sphere2);

	Plane plane(glm::vec3(0, -9, -50), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
	rayTracer.rayObjects.push_back((RayObject*)&plane);

	for (int y = 0; y < winSize.y; ++y)
	{
		for (int x = 0; x < winSize.x; ++x)
		{
			Ray ray = camera.GetRay(glm::ivec2(x, y), winSize);
			glm::vec3 colour = rayTracer.TraceRay(ray);
			_myFramework.DrawPixel(glm::ivec2(x, y), colour);
		}
	}

	// Pushes the framebuffer to OpenGL and renders to screen
	// Also contains an event loop that keeps the window going until it's closed
	_myFramework.ShowAndHold();

	return 0;
}
