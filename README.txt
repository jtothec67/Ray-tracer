

When you run the program 3 windows should open - the console, the actual ray tracing output, and an Imgui window with menus.
You can move around the scene using wsad for horizontal camera movement, q to go up, e to go down, and the left and right arrow keys to look left/right.
At the top of the Imgui window you can see the exact FPS number. The console outputs the amount of frames that have passed in the last second since it last displayed, so this isn't as accurate but is easier to tell a rough average fps at a glance.
The top 4 buttons will initialise scenes.
The empty scene is there for you to make your own scene by creating objects at the cameras position with the key bindings (a bit fidly)
	- m adds a sphere
	- p adds a plane
	- l adds a light
	- c adds a cylinder
	- b adds a box
The Imgui menu should update automatically with the new items in the scene. You will be able to change material properties as well as physical properties such as position and the rotation.
Also note that all lights are point lights. They are initialised with a small white sphere around them to make visualisation easier. These spheres do not move with the light if you move the lights at run time, they also don't contribute to shadows or ambient occlusion calculations.
The output window can also be resized and the new frames will render at this resolution. The view will be stretched, all you need to do is move the camera once to recalculare the viewing matrix.
The test buttons will automatically run the 3 tests for you with the correst settings. In tests 1 and 2 nothing visual changes, but in test 3 you should see the ambient occlusion changing with each test. All tests automatically write the results to a csv file.