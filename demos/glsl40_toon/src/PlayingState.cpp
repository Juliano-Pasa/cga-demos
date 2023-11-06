#include "PlayingState.h"
#include <ctime>

char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY)             \
    (glfwGetKey(WINDOW, KEY) ?              \
     (keyOnce[KEY] ? false : (keyOnce[KEY] = true)) :   \
     (keyOnce[KEY] = false))

PlayingState::PlayingState(GLFWwindow* window) : GameState()
{
	loaded = false;
	camera = nullptr;
	wireframe = false;
	scenes = vector<Scene*>();
	this->window = window;
}

#pragma region StateTransitions

void PlayingState::OnStart()
{
	InitializeGL();
	srand((unsigned)time(NULL));

	scenes.push_back(new Sphere(window, 100, glm::vec3(-1, 0, -5)));
	scenes.back()->init();

	scenes.push_back(new Sphere(window, 50, glm::vec3(1, 0, -5)));
	scenes.back()->init();

	loaded = true;
	OnPlay();
}

void PlayingState::OnPlay()
{
	if (!loaded)
	{
		return;
	}

	double thisTime;
	double lastTime = glfwGetTime();
	do
	{
		if (glfwGetKeyOnce(window, 'Q')) {
			wireframe = !wireframe;
			if (wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set deltatime and call update
		thisTime = glfwGetTime();
		for (Scene* scene : scenes)
		{
			scene->update(thisTime - lastTime);
		}
		lastTime = thisTime;

		for (Scene* scene : scenes)
		{
			scene->render();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));
}

void PlayingState::OnPause()
{
	if (!loaded)
	{
		return;
	}
}

void PlayingState::OnResume()
{
	if (!loaded)
	{
		return;
	}
}

void PlayingState::OnExit()
{
	if (!loaded)
	{
		return;
	}
}

#pragma endregion


#pragma region InitializingFunctions

void PlayingState::InitializeGL()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

#pragma endregion
