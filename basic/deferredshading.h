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
        static Shader* reflectDrawShader;
        static Shader* ssaoShader;
        static Shader* simpleBlurShader;
        static Shader* kuwaharaBlurShader;
        static Shader* combineColorShader;
        static Shader* csmShader;
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
        static void setLightingPassShader(const glm::mat4* lightMVP,const float* endViewSpace)
        {
            lightingShader->use();
            lightingShader->setInt("gPosition", 0);
            lightingShader->setInt("gNormal", 1);
            lightingShader->setInt("gAlbedoSpec", 2);
            lightingShader->setInt("gMask", 3);
            lightingShader->setInt("SSAOMask", 4);
            glm::mat4 view = GameController::mainCamera.GetViewMatrix();
            lightingShader->setMat4("view", view);
            lightingShader->setMat4("inv_view", glm::inverse(view));
            for (int i = 0; i < 3; i++)
            {
                lightingShader->setInt("ShadowMap[" + std::to_string(i) + "]", 5 + i);
                lightingShader->setMat4("lightMVP[" + std::to_string(i) + "]", lightMVP[i]);
                lightingShader->setFloat("EndViewSpace[" + std::to_string(i) + "]", endViewSpace[i + 1]);
            }
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
        static void setReflectDrawShader()
        {
            reflectDrawShader->use();
            reflectDrawShader->setInt("rColor", 0);
            reflectDrawShader->setInt("rTexcoord", 1);
        }
        static void setSSAOShader()
        {
            
            static std::vector<glm::vec3> ssaoKernel;
            if (ssaoKernel.empty())
            {
                //Kernel data initialization
                SSAOKernalInit(ssaoKernel);
            }
            static GLuint noiseTexture = 0;
            if (!noiseTexture)
            {
                //Generate noise map
                SSAONoiseTextureInit(noiseTexture);
            }
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);

            ssaoShader->use();
            Camera& cam = GameController::mainCamera;
            ssaoShader->setInt("gPosition", 0);
            ssaoShader->setInt("gNormal", 1);
            ssaoShader->setInt("gMask", 3);
            ssaoShader->setInt("texNoise", 4);
            ssaoShader->setMat4("projection", Common::GetPerspectiveMat(cam));
            ssaoShader->setMat4("view", cam.GetViewMatrix());
            for (int i = 0; i < ssaoKernel.size(); i++)
            {
                ssaoShader->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
            }
        }
        static void setSimpleBlurShader()
        {
            simpleBlurShader->use();
            simpleBlurShader->setInt("Input", 4);
        }
        static void setKuwaharaBlurShader()
        {
            kuwaharaBlurShader->use();
            kuwaharaBlurShader->setInt("Input", 1);
            kuwaharaBlurShader->setInt("Mask", 3);
            kuwaharaBlurShader->setVec2("parameters", glm::vec2(1.0f));
        }
        static void setCombineColorShader()
        {
            combineColorShader->use();
            combineColorShader->setInt("Tcolor", 0);
            combineColorShader->setInt("Treflection", 1);
            combineColorShader->setInt("gMask", 3);
        }
        static void setCSMShader(const glm::mat4& view, const glm::mat4& projection)
        {
            csmShader->use();
            csmShader->setMat4("view", view);
            csmShader->setMat4("model", glm::mat4(1.0f));
            csmShader->setMat4("projection", projection);
        }
    private:
        static void SSAOKernalInit(std::vector<glm::vec3>& ssaoKernel)
        {
            std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
            std::default_random_engine generator;
            for (GLuint i = 0; i < 64; ++i)
            {
                glm::vec3 sample(
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator)
                );
                sample = glm::normalize(sample);
                sample *= randomFloats(generator);
                GLfloat scale = GLfloat(i) / 64.0;
                scale = 0.1f + scale * scale * 0.9;
                sample *= scale;
                ssaoKernel.push_back(sample);
            }
        }
        static void SSAONoiseTextureInit(GLuint& noiseTexture)
        {
            std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
            std::default_random_engine generator;
            std::vector<glm::vec3> ssaoNoise;
            for (GLuint i = 0; i < 16; i++)
            {
                glm::vec3 noise(
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0,
                    0.0f);
                ssaoNoise.push_back(noise);
            }

            glGenTextures(1, &noiseTexture);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

	};
    
}

#endif // !DEFERREDSHADING
