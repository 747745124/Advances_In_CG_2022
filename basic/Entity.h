#ifndef ENTITY_H
#define ENTITY_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "common.h"
#include "mesh.h"

class Entity
{
private:
	Shader &EntityShader;

public:
	Entity(Shader &SomeShader) : EntityShader(SomeShader) {}
	void Draw(Mesh &mesh, Camera &cam, const glm::mat4 projection, glm::mat4 model, bool if_hit = false)
	{
		EntityShader.use();
		if (if_hit)
			EntityShader.setVec3("selected_color", glm::vec3(0.5f, 0.5f, 0.5f));
#ifndef DEFERRED_SHADING
		else
			EntityShader.setVec3("selected_color", glm::vec3(0.0f, 0.0f, 0.0f));
#endif
		EntityShader.setMat4("projection", projection);
		EntityShader.setMat4("view", cam.GetViewMatrix());
		EntityShader.setVec3("viewPos", cam.Position);
		EntityShader.setMat4("model", model);
		mesh.Draw(&EntityShader);
	}
	void Pick(Mesh &mesh, Shader &pickingShader, Camera &cam, glm::mat4 model, unsigned int objIndex, unsigned int drawIndex)
	{
		glm::mat4 projection = Common::GetPerspectiveMat(cam);
		pickingShader.use();
		pickingShader.setMat4("projection", projection);
		pickingShader.setMat4("view", cam.GetViewMatrix());
		pickingShader.setMat4("model", model);
		pickingShader.setUint("drawIndex", drawIndex);
		pickingShader.setUint("objIndex", objIndex);
		mesh.Draw(&pickingShader);
	}
};

#endif // !ENTITY_H
