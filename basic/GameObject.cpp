#include "GameObject.h"

namespace KooNan
{

    std::list<GameObject *> GameObject::gameObjList;
    
    void GameObject::Update()
    {
        modelMat = glm::translate(glm::mat4(1.0f), pos); // 位移
        modelMat = glm::rotate(modelMat, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
        modelMat = glm::scale(modelMat, sca); // 缩放
    }

    void GameObject::Draw(Shader &shader,
				  const glm::vec3 viewPos,
				  const glm::mat4 &projectionMat,
				  const glm::mat4 &viewMat,
				  bool isHit)
    {
        shader.use();
        if (isHit)
            shader.setVec3("selected_color", glm::vec3(0.5f, 0.5f, 0.5f));
        else
            shader.setVec3("selected_color", glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setMat4("projection", projectionMat);
        shader.setMat4("view", viewMat);
        shader.setVec3("viewPos", viewPos);
        shader.setMat4("model", modelMat);
        model->Draw(&shader);
    }

    void GameObject::Draw(Mesh &mesh, Shader &shader,
						 const glm::vec3 viewPos,
						 const glm::mat4 &projectionMat,
						 const glm::mat4 &viewMat,
						 const glm::mat4 &modelMat,
						 const glm::vec4 &clippling_plane,
						 bool isHit)
    {
        shader.use();
        if (isHit)
            shader.setVec3("selected_color", glm::vec3(0.5f, 0.5f, 0.5f));
        else
            shader.setVec3("selected_color", glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setMat4("projection", projectionMat);
        shader.setMat4("view", viewMat);
        shader.setVec4("plane", clippling_plane);
        shader.setVec3("viewPos", viewPos);
        shader.setMat4("model", modelMat);
        mesh.Draw(&shader);
    }

    void  GameObject::Pick(Shader &shader, unsigned int objIndex, unsigned int drawIndex,
				  const glm::mat4 &projectionMat,
				  const glm::mat4 &viewMat)
    {
        shader.use();
        shader.setMat4("projection", projectionMat);
        shader.setMat4("view", viewMat);
        shader.setMat4("model", modelMat);
        shader.setUint("drawIndex", drawIndex);
        shader.setUint("objIndex", objIndex);
        model->Draw(&shader);
    }

    void GameObject::Pick(Mesh &mesh, Shader &shader, unsigned int objIndex, unsigned int drawIndex,
						 const glm::mat4 &projectionMat,
						 const glm::mat4 &viewMat,
						 const glm::mat4 &modelMat)
    {
        shader.use();
        shader.setMat4("projection", projectionMat);
        shader.setMat4("view", viewMat);
        shader.setMat4("model", modelMat);
        shader.setUint("drawIndex", drawIndex);
        shader.setUint("objIndex", objIndex);
        mesh.Draw(&shader);
    }
    
}