#ifndef SKYBOX_H
#define SKYBOX_H

#include <stb_image.h>

#include <iostream>
#include <vector>
#include <string>

// GLM Math Library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD: A library that wraps OpenGL functions to make things easier
//       Note that GLAD MUST be included before GLFW
#include "glad/glad.h"

#include "Shader.h"

class Skybox
{
public:
	Skybox(std::vector<std::string> paths);

	void Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

	unsigned int getCubeMap() { return texture; }

	void bindSkyboxTexture(int i);

private:
	unsigned int VAO, VBO, texture;

	unsigned int GenCubeMap(std::vector<std::string> facePaths);
};

#endif // !SKYBOX_H
