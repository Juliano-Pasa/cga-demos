#include "GamerManager.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

GameManager::GameManager()
{
	currentState = nullptr;
	window = nullptr;
}

void GameManager::Launch()
{
	InitializeGLFW();
	InitializeCallbacks();
	InitializeGLEW();

	currentState = new PlayingState(window);
	currentState->OnStart();
}

#pragma region InitializingFunctions

void GameManager::InitializeGLFW()
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	//window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "glsl40_toon", glfwGetPrimaryMonitor(), NULL);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "glsl40_toon", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		system("pause");
		exit(EXIT_FAILURE);
	}
}

void GameManager::InitializeCallbacks()
{
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyboardCallback);
	glfwSetErrorCallback(ErrorCallback);
}

void GameManager::InitializeGLEW()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		system("pause");
		exit(EXIT_FAILURE);
	}
}

#pragma endregion


#pragma region CallbackFunctions

void GameManager::ErrorCallback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

void GameManager::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

#pragma endregion


