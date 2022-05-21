#include "common.h"
#include "light.h"
#include "GameController.h"

namespace KooNan 
{
	void Light::SetLight(Shader& SomeEntities)
	{
		SomeEntities.use();
		SomeEntities.setVec3("dirLight.direction", parallel_light.direction);
		SomeEntities.setVec3("dirLight.ambient", parallel_light.ambient);
		SomeEntities.setVec3("dirLight.diffuse", parallel_light.diffuse);
		SomeEntities.setVec3("dirLight.specular", parallel_light.specular);
		SomeEntities.setMat4("view", GameController::mainCamera.GetViewMatrix());

		std::string s_lights = "pointLights[";
		std::string s_position = "].position";
		std::string s_constant = "].constant";
		std::string s_linear = "].linear";
		std::string s_quadratic = "].quadratic";
		std::string s_ambient = "].ambient";
		std::string s_diffuse = "].diffuse";
		std::string s_specular = "].specular";
		for (unsigned int i = 0; i < point_lights.size(); i++)
		{
			SomeEntities.setVec3(s_lights + std::to_string(i) + s_position, point_lights[i].position);
			SomeEntities.setFloat(s_lights + std::to_string(i) + s_constant, point_lights[i].constant);
			SomeEntities.setFloat(s_lights + std::to_string(i) + s_linear, point_lights[i].linear);
			SomeEntities.setFloat(s_lights + std::to_string(i) + s_quadratic, point_lights[i].quadratic);
			SomeEntities.setVec3(s_lights + std::to_string(i) + s_ambient, point_lights[i].ambient);
			SomeEntities.setVec3(s_lights + std::to_string(i) + s_diffuse, point_lights[i].diffuse);
			SomeEntities.setVec3(s_lights + std::to_string(i) + s_specular, point_lights[i].specular);
		}
	}

	void Light::Draw(const glm::vec4* clipping_plane)
	{
		std::vector<Texture> textures;//Load a null texture
		Cube lightcube(textures, LightboxShader);
		Camera& cam = GameController::mainCamera;
		if (clipping_plane)
		{
			LightboxShader.use();
			LightboxShader.setVec4("plane", *clipping_plane);
		}
		for (int i = 0; i < point_lights.size(); i++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, point_lights[i].position);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			lightcube.Draw(cam, model, false);
		}
	};
}