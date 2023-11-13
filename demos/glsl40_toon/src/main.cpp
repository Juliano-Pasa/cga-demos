/*
GLSL 4.x demo
Mostra o uso de:
-GLM - Math library
-VBO & VAO
-GL error detection
-glfwGetKeyOnce
-toon shading 

May 2016 - Tiago Augusto Engel - tengel@inf.ufsm.br
*/


//Include GLEW - always first 
#include "GL/glew.h"
#include <GLFW/glfw3.h>

//Include the standard C++ headers 
#include "Scene.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include "GameState.h"
#include <ctime>

#include "GamerManager.h"


int main(void)
{
	GameManager gameManager = GameManager();
	gameManager.Launch();
}