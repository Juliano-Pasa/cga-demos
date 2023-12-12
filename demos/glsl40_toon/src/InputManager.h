#pragma once

#include "glslprogram.h"
#include "GLFW\glfw3.h"
#include <map>
#include <vector>

using namespace std;

class InputManager {
public:
    InputManager(vector<int> keysToMonitor, bool mouseCoords);
    ~InputManager();

    bool GetIsKeyDown(int key);
    dvec2 GetMouseCoords();

    bool getIsEnabled() { return _isEnabled; }
    void setIsEnabled(bool value) { _isEnabled = value; }

private:
    void SetIsKeyDown(int key, bool isDown);
    void SetMouseCoords(double xCoord, double yCoord);

    map<int, bool> _keys;
    dvec2 lastMouseCoords;
    bool _isEnabled;

public:
    static void SetupInputs(GLFWwindow* window);

private:
    static void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static vector<InputManager*> _instances;
};