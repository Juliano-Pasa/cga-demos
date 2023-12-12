#include "InputManager.h"
#include <algorithm>

std::vector<InputManager*> InputManager::_instances;

InputManager::InputManager(vector<int> keysToMonitor, bool mouseCoords) : _isEnabled(true) 
{
    for (int key : keysToMonitor) {
        _keys[key] = false;
    }

    InputManager::_instances.push_back(this);
}

InputManager::~InputManager() 
{
    _instances.erase(remove(_instances.begin(), _instances.end(), this), _instances.end());
}

bool InputManager::GetIsKeyDown(int key) 
{
    bool result = false;
    if (_isEnabled) {
        map<int, bool>::iterator it = _keys.find(key);
        if (it != _keys.end()) {
            result = _keys[key];
        }
    }
    return result;
}

dvec2 InputManager::GetMouseCoords()
{
    return lastMouseCoords;
}

void InputManager::SetIsKeyDown(int key, bool isDown) 
{
    map<int, bool>::iterator it = _keys.find(key);
    if (it != _keys.end()) {
        _keys[key] = isDown;
    }
}

void InputManager::SetMouseCoords(double xCoord, double yCoord) 
{
    lastMouseCoords.x = xCoord;
    lastMouseCoords.y = yCoord;
}

void InputManager::SetupInputs(GLFWwindow* window) 
{
    glfwSetKeyCallback(window, InputManager::KeyboardCallback);
    glfwSetCursorPosCallback(window, InputManager::MouseCallback);
}

void InputManager::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
    for (InputManager* inputManager : _instances) {
        inputManager->SetIsKeyDown(key, action != GLFW_RELEASE);
    }
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos) 
{
    for (InputManager* inputManager : _instances) {
        inputManager->SetMouseCoords(xpos, ypos);
    }
}