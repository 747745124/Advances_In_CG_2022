#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

#include "Shader.h"
#include "Camera.h"
#include "cube.h"
#include "common.h"

struct PointLight
{
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};
struct DirLight
{
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};
class Light
{
private:
	DirLight parallel_light;
	std::vector<PointLight> point_lights;
	Shader &LightboxShader;

public:
	Light(DirLight parallel_light, Shader &lightbox) : parallel_light(parallel_light), LightboxShader(lightbox) {}
	void AddPointLight(PointLight light)
	{
		point_lights.push_back(light);
	}
	void Draw(const glm::mat4 *projection, const glm::mat4 *jitteredProjection, const glm::mat4 *lastViewProjection, const glm::vec4 *clipping_plane);
	// Set the light for shader
	void SetLight(Shader &SomeEntities);

	glm::vec3 GetDirLightDirection()
	{
		return parallel_light.direction;
	}

	unsigned int numOfPointLight()
	{
		return point_lights.size();
	}

	PointLight *getPointLightAt(unsigned int idx)
	{
		return &(point_lights[idx]);
	}

	DirLight *getDirectionLight()
	{
		return &parallel_light;
	}
};

#endif // !LIGHT_H
