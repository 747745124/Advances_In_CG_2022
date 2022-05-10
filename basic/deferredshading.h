#ifndef DEFERREDSHADING
#define DEFERREDSHADING

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

namespace KooNan {
	class DeferredShading {
    public:
        DeferredShading() = delete;
        static Shader* lightingShader;
        static Shader* ssrShader;
        static Shader* finalShader;
        static void DrawQuad()
		{
			static GLuint quadVAO = 0;
            static GLuint quadVBO;
            const Camera& cam = GameController::mainCamera;
            
            if (quadVAO == 0)
            {
                GLfloat quadVertices[] = {
                    // Positions       
                    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                    1.0f, -1.0f, 0.0f, 1.0f, 0.0f
                };
                // Setup plane VAO
                glGenVertexArrays(1, &quadVAO);
                glGenBuffers(1, &quadVBO);
                glBindVertexArray(quadVAO);
                glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
            }
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
		}
        static void setLightingPassShader()
        {
            lightingShader->use();
            lightingShader->setVec3("viewPos", GameController::mainCamera.Position);
            lightingShader->setInt("gPosition", 0);
            lightingShader->setInt("gNormal", 1);
            lightingShader->setInt("gAlbedoSpec", 2);
            lightingShader->setInt("gReflect_mask", 3);
        }
        static void setSSRShader()
        {
            ssrShader->use();
            Camera& cam = GameController::mainCamera;
            ssrShader->setMat4("projection", Common::GetPerspectiveMat(cam));
            ssrShader->setMat4("view", cam.GetViewMatrix());
            ssrShader->setInt("gPosition", 0);
            ssrShader->setInt("gNormal", 1);
            ssrShader->setInt("gReflect_mask", 3);
            ssrShader->setVec3("viewPos", GameController::mainCamera.Position);

        }
        static void setFinalShader()
        {
            finalShader->use();
            finalShader->setInt("rColor", 0);
            finalShader->setInt("rTexcoord", 1);
            
        }
	};
    
}

#endif // !DEFERREDSHADING
