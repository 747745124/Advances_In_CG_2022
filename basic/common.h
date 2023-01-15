#ifndef COMMON_H
#define COMMON_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <string>

#include "Camera.h"

class Common
{
public:
	static GLFWwindow *gWindow;

	static void setWindow(GLFWwindow *window)
	{
		gWindow = window;
	};

	static void setWidthAndHeight()
	{
		glfwGetFramebufferSize(gWindow, (int *)&SCR_WIDTH, (int *)&(SCR_HEIGHT));
	};

	static unsigned int SCR_WIDTH, SCR_HEIGHT;
	static float perspective_clipping_near;
	static float perspective_clipping_far;
	static const std::string saveFileName;
	static glm::mat4 GetPerspectiveMat(Camera &cam);
	static void FilterCreation(double gauss[][21]);

	static double gauss[21][21];
	static float gauss_flat[441];
};

#endif