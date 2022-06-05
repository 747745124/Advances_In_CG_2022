#ifndef DEFERREDSHADING
#define DEFERREDSHADING

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

namespace KooNan
{
    class DeferredShading
    {
    public:
        DeferredShading() = delete;
        static Shader *lightingShader;
        static Shader *ssreflectionShader;
        static Shader *ssrefractionShader;
        static Shader *reflectDrawShader;
        static Shader *refractDrawShader;
        static Shader *ssaoShader;
        static Shader *simpleBlurShader;
        static Shader *ssdoShader;
        static Shader *kuwaharaBlurShader;
        static Shader *combineColorShader;
        static Shader *csmShader;
        static Shader *taaShader;
        static Shader *causticShader;
        static Shader *bufferDebugShader;
        static Shader *refractionPositionShader;
        static void DrawQuad()
        {
            static GLuint quadVAO = 0;
            static GLuint quadVBO;
            const Camera &cam = GameController::mainCamera;

            if (quadVAO == 0)
            {
                GLfloat quadVertices[] = {
                    // Positions
                    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                    1.0f, -1.0f, 0.0f, 1.0f, 0.0f};
                // Setup plane VAO
                glGenVertexArrays(1, &quadVAO);
                glGenBuffers(1, &quadVBO);
                glBindVertexArray(quadVAO);
                glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
            }
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }

        static void setLightingPassShader(const glm::mat4 *lightMVP, const glm::mat4 *causticVP, const float *endViewSpace)
        {
            lightingShader->use();
            if (toneMapping)
                lightingShader->setInt("toneMapping", 1);
            else
                lightingShader->setInt("toneMapping", 0);
            lightingShader->setFloat("exposure", exposure);
            lightingShader->setInt("softShadowType", softShadowType);
            if (csmOn)
                lightingShader->setInt("csmShow", 1);
            else
                lightingShader->setInt("csmShow", 0);
            lightingShader->setInt("gPosition", 0);
            lightingShader->setInt("gNormal", 1);
            lightingShader->setInt("gAlbedoSpec", 2);
            lightingShader->setInt("gMask", 3);
            lightingShader->setInt("SSAOMask", 5);
            lightingShader->setInt("refractionPos", 11);
            lightingShader->setInt("causticMap", 10);
            lightingShader->setMat4("causticVP", *causticVP);
            
            lightingShader->setInt("SSDOMask", 13);
            glm::mat4 view = GameController::mainCamera.GetViewMatrix();
            lightingShader->setMat4("view", view);
            lightingShader->setMat4("inv_view", glm::inverse(view));
            for (int i = 0; i < 3; i++)
            {
                lightingShader->setInt("ShadowMap[" + std::to_string(i) + "]", 6 + i);
                lightingShader->setMat4("lightMVP[" + std::to_string(i) + "]", lightMVP[i]);
                lightingShader->setFloat("EndViewSpace[" + std::to_string(i) + "]", endViewSpace[i + 1]);
            }
            static glm::vec2 poissonDisk[] = {
                glm::vec2(-0.94201624, -0.39906216),
                glm::vec2(0.94558609, -0.76890725),
                glm::vec2(-0.094184101, -0.92938870),
                glm::vec2(0.34495938, 0.29387760),
                glm::vec2(-0.91588581, 0.45771432),
                glm::vec2(-0.81544232, -0.87912464),
                glm::vec2(-0.38277543, 0.27676845),
                glm::vec2(0.97484398, 0.75648379),
                glm::vec2(0.44323325, -0.97511554),
                glm::vec2(0.53742981, -0.47373420),
                glm::vec2(-0.26496911, -0.41893023),
                glm::vec2(0.79197514, 0.19090188),
                glm::vec2(-0.24188840, 0.99706507),
                glm::vec2(-0.81409955, 0.91437590),
                glm::vec2(0.19984126, 0.78641367),
                glm::vec2(0.14383161, -0.14100790)};
            static GLuint rotationNoise = 0;
            if (!rotationNoise)
            {
                PCSSNoiseTextureInit(rotationNoise);
            }
            for (int i = 0; i < sizeof(poissonDisk) / sizeof(poissonDisk[0]); i++)
            {
                lightingShader->setVec2("poissonDisk[" + std::to_string(i) + "]", poissonDisk[i]);
            }
            glActiveTexture(GL_TEXTURE9);
            glBindTexture(GL_TEXTURE_2D, rotationNoise);
            lightingShader->setInt("rotationNoise", 9);
        }
        static void setSSReflectionShader(const glm::mat4 &projection)
        {
            ssreflectionShader->use();
            Camera &cam = GameController::mainCamera;
            if (ssrOn)
            {
                ssreflectionShader->setInt("enable", 1);
            }
            else
            {
                ssreflectionShader->setInt("enable", 0);
            }
            ssreflectionShader->setFloat("thickness", ssrThickness);
            ssreflectionShader->setMat4("projection", projection);
            ssreflectionShader->setMat4("view", cam.GetViewMatrix());
            ssreflectionShader->setInt("gPosition", 0);
            ssreflectionShader->setInt("gNormal", 1);
            ssreflectionShader->setInt("gMask", 3);
            ssreflectionShader->setVec3("viewPos", GameController::mainCamera.Position);
        }

        static void setReflectDrawShader()
        {

            reflectDrawShader->use();
            if (ReflectOn)
            {
                reflectDrawShader->setInt("enable", 1);
            }
            else
            {
                reflectDrawShader->setInt("enable", 0);
            }
            reflectDrawShader->setInt("gPosition", 0);
            reflectDrawShader->setInt("gNormal", 1);
            reflectDrawShader->setInt("gMask", 3);
            reflectDrawShader->setInt("rColor", 4);
            reflectDrawShader->setInt("rTexcoord", 5);
            reflectDrawShader->setInt("skybox", 6);
            reflectDrawShader->setMat4("inv_view", glm::inverse(GameController::mainCamera.GetViewMatrix()));
        }

        static void setSSRefractionShader(const glm::mat4 &projection)
        {
            ssrefractionShader->use();
            Camera &cam = GameController::mainCamera;
            ssrefractionShader->setMat4("projection", projection);
            ssrefractionShader->setMat4("view", cam.GetViewMatrix());
            ssrefractionShader->setInt("gPosition", 0);
            ssrefractionShader->setInt("gNormal", 1);
            ssrefractionShader->setInt("gMask", 3);
            ssrefractionShader->setInt("gTerrain", 11);
        }

        static void setRefractDrawShader()
        {
            refractDrawShader->use();
            refractDrawShader->setInt("gMask", 3);
            refractDrawShader->setInt("rColor", 4);
            refractDrawShader->setInt("rTexcoord", 5);
        }

        static void setSSAOShader(const glm::mat4 &projection)
        {

            static std::vector<glm::vec3> ssaoKernel;
            if (ssaoKernel.empty())
            {
                // Kernel data initialization
                SSAOKernalInit(ssaoKernel);
            }
            static GLuint noiseTexture = 0;
            if (!noiseTexture)
            {
                // Generate noise map
                SSAONoiseTextureInit(noiseTexture);
            }
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);

            ssaoShader->use();
            Camera &cam = GameController::mainCamera;
            if (ssaoOn)
            {
                ssaoShader->setInt("enable", 1);
            }
            else
            {
                ssaoShader->setInt("enable", 0);
            }
            ssaoShader->setInt("gPosition", 0);
            ssaoShader->setInt("gNormal", 1);
            ssaoShader->setInt("gMask", 3);
            ssaoShader->setInt("texNoise", 4);
            ssaoShader->setMat4("projection", projection);
            ssaoShader->setMat4("view", cam.GetViewMatrix());
            for (int i = 0; i < ssaoKernel.size(); i++)
            {
                ssaoShader->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
            }
        }
         static void setSSDOShader(const glm::mat4& projection)
        {
            static std::vector<glm::vec3> ssdoKernel;
            if (ssdoKernel.empty())
            {
                //Kernel data initialization
                SSDOKernalInit(ssdoKernel);
            }
          /*  static GLuint noiseTexture = 0;
            if (!noiseTexture)
            {
                //Generate noise map
                SSDONoiseTextureInit(noiseTexture);
            }
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            */
            ssdoShader->use();
            Camera& cam = GameController::mainCamera;
            if (ssdoOn) {
                ssdoShader->setInt("enable", 1);
            }
            else {
                ssdoShader->setInt("enable", 0);
            }
            ssdoShader->setInt("gPosition", 0);
            ssdoShader->setInt("gNormal", 1);
            ssdoShader->setInt("gAlbedoSpec", 2);
            ssdoShader->setInt("gMask", 3);
            ssdoShader->setInt("texNoise", 4);
            ssdoShader->setMat4("projection", projection);
            ssdoShader->setMat4("view", cam.GetViewMatrix());
            for (int i = 0; i < ssdoKernel.size(); i++)
            {
                ssdoShader->setVec3("samples[" + std::to_string(i) + "]", ssdoKernel[i]);
            }
        }
        static void setSimpleBlurShader()
        {
            simpleBlurShader->use();
            simpleBlurShader->setInt("Input", 5);
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
            combineColorShader->setInt("Tcolor", 4);
            combineColorShader->setInt("Treflection", 5);
            combineColorShader->setInt("Trefraction", 6);
            combineColorShader->setInt("gPosition", 0);
            combineColorShader->setInt("gNormal", 1);
            combineColorShader->setInt("gMask", 3);
        }
        static void setCSMShader(const glm::mat4 &view, const glm::mat4 &projection)
        {
            csmShader->use();
            csmShader->setMat4("view", view);
            csmShader->setMat4("model", glm::mat4(1.0f));
            csmShader->setMat4("projection", projection);
        }
        static void setTAAShader()
        {
            taaShader->use();
            if (taaOn == 1)
                taaShader->setInt("taaOn", 1);
            else
                taaShader->setInt("taaOn", 0);
            taaShader->setInt("gPosition", 0);
            taaShader->setInt("currFrame", 1);
            taaShader->setInt("lastFrame", 2);
            taaShader->setInt("gDepth", 3);
            taaShader->setInt("gVelocity", 4);
        }
        static void setRefractionPositionShader(const glm::mat4 &view, const glm::mat4 &projection)
        {
            refractionPositionShader->use();
            refractionPositionShader->setMat4("view", view);
            refractionPositionShader->setMat4("model", glm::mat4(1.0f));
            refractionPositionShader->setMat4("projection", projection);
        }

    private:
        static void SSAOKernalInit(std::vector<glm::vec3> &ssaoKernel)
        {
            std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
            std::default_random_engine generator;
            for (GLuint i = 0; i < 64; ++i)
            {
                glm::vec3 sample(
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator));
                sample = glm::normalize(sample);
                sample *= randomFloats(generator);
                GLfloat scale = GLfloat(i) / 64.0;
                scale = 0.1f + scale * scale * 0.9;
                sample *= scale;
                ssaoKernel.push_back(sample);
            }
        }
         static void SSDOKernalInit(std::vector<glm::vec3>& ssdoKernel)
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
                ssdoKernel.push_back(sample);
            }
        }
        static void SSAONoiseTextureInit(GLuint &noiseTexture)
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
        static void PCSSNoiseTextureInit(GLuint &noiseTexture)
        {
            std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
            std::default_random_engine generator;
            std::vector<glm::vec2> pcssNoise;
            const int size = 16;
            for (int i = 0; i < size; i++)
            {
                float angle = randomFloats(generator) * glm::pi<float>() * 2.0f;
                glm::vec2 noise(
                    glm::cos(angle),
                    glm::sin(angle));
                pcssNoise.push_back(noise);
            }
            glGenTextures(1, &noiseTexture);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 4, 4, 0, GL_RG, GL_FLOAT, &pcssNoise[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
    };

}

#endif // !DEFERREDSHADING
