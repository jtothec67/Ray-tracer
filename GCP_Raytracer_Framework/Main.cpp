#include "GCP_GFX_Framework.h"

#include "RayTracer.h"
#include "Camera.h"
#include "Sphere.h"

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

	Camera camera;

	RayTracer rayTracer;

	Sphere sphere1(glm::vec3((winSize.x / 2) - 100, winSize.y / 2, -100), 100, glm::vec3(0, 1, 0));
	rayTracer.mSpheres.push_back(&sphere1);

	Sphere sphere2(glm::vec3((winSize.x / 2) + 100, winSize.y / 2, -100), 100, glm::vec3(1, 0, 0));
	rayTracer.mSpheres.push_back(&sphere2);

	for (int y = 0; y < winSize.y; ++y)
	{
		for (int x = 0; x < winSize.x; ++x)
		{
			Ray ray = camera.GetRay(glm::ivec2(x, y));
			glm::vec3 colour = rayTracer.TraceRay(ray);
			_myFramework.DrawPixel(glm::ivec2(x, y), colour);
		}
	}

	// Pushes the framebuffer to OpenGL and renders to screen
	// Also contains an event loop that keeps the window going until it's closed
	_myFramework.ShowAndHold();

	return 0;
}
