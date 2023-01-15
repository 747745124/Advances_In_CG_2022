#ifndef RENDER_H
#define RENDER_H
#define GLM_SWIZZLE
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <limits>

#include "scene.h"
#include "light.h"
#include "vector"
#include "shadow.h"
#include "GameController.h"
#include "gbuffer.h"
#include "deferredshading.h"
#include "ssreflectuvbuffer.h"
#include "ssrefractuvbuffer.h"
#include "ssaobuffer.h"
#include "ssaoblurbuffer.h"
#include "reflectionblurbuffer.h"
#include "reflectiondrawbuffer.h"
#include "csmbuffer.h"
#include "taabuffer.h"
#include "causticDepthBuffer.h"
#include "causticMapBuffer.h"
#include "refractionPositionBuffer.h"
#include "refractiondrawbuffer.h"
#include "originColorBuffer.h"
#include "postprocess.h"
using namespace GUI_Param;
class Render
{

private:
	Scene &main_scene;
	Light &main_light;

#ifndef DEFERRED_SHADING
	Water_Frame_Buffer &waterfb;
	Shadow_Frame_Buffer &shadowfb;

#endif
	PickingTexture &mouse_picking;
	TAABuffer taabuf;
	GBuffer gbuf;
	SSReflectUVBuffer ssreflectbuf;
	SSRefractUVBuffer ssrefractbuf;
	SSAOBuffer aobuf;
	SSAOBlurBuffer aoblurbuf;
	ReflectionDrawBuffer reflectdrawbuf;
	ReflectionBlurBuffer reflectblurbuf;
	RefractionDrawBuffer refractdrawbuf;
	CSMBuffer csmbuf;
	CausticDepthBuffer causticdepthbuf;
	CausticMapBuffer causticmapbuf;
	RefractionPositionBuffer refractionposbuf;
	OriginColorBuffer lightingcolorbuf;
	Postprocessor postprocessor;

	static const int NUM_CASCADES = 3;
	static const float cascade_Z[NUM_CASCADES + 1];
	struct
	{
		float xmin, xmax, ymin, ymax, zmin, zmax;
	} CSMOrthoProjAABB[NUM_CASCADES];
	static unsigned cascadeUpdateCounter[NUM_CASCADES];
	static const int NUM_TAA_SAMPLES = 16;
	static const glm::vec2 haltonSequence[NUM_TAA_SAMPLES];
	static unsigned haltonIndex;
	static glm::mat4 lastViewProjection;

public:
#ifdef DEFERRED_SHADING
	Render(Scene &main_scene, Light &main_light, PickingTexture &mouse_picking) : main_scene(main_scene), main_light(main_light), mouse_picking(mouse_picking)
	{
		if (glGetError() == GL_INVALID_OPERATION)
		{
			// throw std::runtime_error("Oopps!");
		}
	}
#endif

#ifndef DEFERRED_SHADING
	Render(Scene &main_scene, Light &main_light, Water_Frame_Buffer &waterfb, PickingTexture &mouse_picking, Shadow_Frame_Buffer &shadowfb) : main_scene(main_scene), main_light(main_light), waterfb(waterfb), mouse_picking(mouse_picking), shadowfb(shadowfb)
	{
		if (glGetError() == GL_INVALID_OPERATION)
		{
			// throw std::runtime_error("Oopps!");
		}

		InitLighting(main_scene.TerrainShader);
		InitLighting(main_scene.WaterShader);
	}
#endif

	void DrawReflection(Shader &modelShader)
	{
#ifndef DEFERRED_SHADING

		InitLighting(main_scene.TerrainShader);

		glm::vec4 clipping_plane = glm::vec4(0.0, 1.0, 0.0, -main_scene.getWaterHeight());
		glEnable(GL_CLIP_DISTANCE0);
		waterfb.bindReflectionFrameBuffer();
		glEnable(GL_DEPTH_TEST);
		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set the main camera to the position and direction of reflection
		float distance = 2 * (GameController::mainCamera.Position.y - main_scene.getWaterHeight());
		GameController::mainCamera.Position.y -= distance;
		GameController::mainCamera.Pitch = -GameController::mainCamera.Pitch;
		glm::mat4 projection = Common::GetPerspectiveMat(GameController::mainCamera);
		DrawObjects(modelShader, &projection, nullptr, nullptr, &clipping_plane, false, true);

		// we now draw as many light bulbs as we have point lights.
		main_light.Draw(&projection, nullptr, nullptr, &clipping_plane);
		// render the main scene
		main_scene.DrawScene(GameController::deltaTime, &projection, nullptr, nullptr, &clipping_plane, false);
		main_scene.DrawSky(&projection, nullptr, nullptr);

		// Restore the main camera
		GameController::mainCamera.Position.y += distance;
		GameController::mainCamera.Pitch = -GameController::mainCamera.Pitch;
		GameController::mainCamera.GetViewMatrix();

		waterfb.unbindCurrentFrameBuffer();
#endif
	}

	void DrawRefraction(Shader &modelShader)
	{
#ifndef DEFERRED_SHADING

		InitLighting(main_scene.TerrainShader);

		glm::vec4 clipping_plane = glm::vec4(0.0, -1.0, 0.0, main_scene.getWaterHeight());
		glEnable(GL_CLIP_DISTANCE0);
		waterfb.bindRefractionFrameBuffer();
		glEnable(GL_DEPTH_TEST);
		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = Common::GetPerspectiveMat(GameController::mainCamera);
		DrawObjects(modelShader, &projection, nullptr, nullptr, &clipping_plane, false, true);

		// render the main scene
		main_scene.DrawScene(GameController::deltaTime, &projection, nullptr, nullptr, &clipping_plane, false);
		main_scene.DrawSky(&projection, nullptr, nullptr);

		waterfb.unbindCurrentFrameBuffer();
#endif
	}

	void DrawAll(Shader &pickingShader, Shader &modelShader, Shader &shadowShader)
	{

#ifdef DEFERRED_SHADING

		// Shadow pass
		glm::vec3 DivPos = GameController::mainCamera.Position;
		glm::mat4 lightView = glm::lookAt(DivPos - main_light.GetDirLightDirection() * 10.0f, DivPos, glm::vec3(0.0f, 1.0f, 0.0f));

		CSMUpdateOrthoProj();
		glEnable(GL_DEPTH_TEST);

		for (int i = 0; i < NUM_CASCADES; i++)
		{
			cascadeUpdateCounter[i] = 1;
			csmbuf.bindToWrite(i);
			glClear(GL_DEPTH_BUFFER_BIT);
			DeferredShading::setCSMShader(lightView, GetCSMProjection(i));

			// Common::setWidthAndHeight();
			glViewport(0, 0, csmbuf.viewPortSize[i * 2 + 0], csmbuf.viewPortSize[i * 2 + 1]);

			main_scene.DrawSceneWithoutMVP(*DeferredShading::csmShader);
			DrawObjectsWithoutVP(*DeferredShading::csmShader);
		}

		// Draw depth for caustics calculation
		glm::mat4 caustics_proj = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 30.0f);
		glm::mat4 caustics_view = glm::lookAt(GameController::mainCamera.Position + glm::vec3(0.0, 10.0, 0.0), GameController::mainCamera.Position, glm::vec3(0.0, 0.0, -1.0));
		causticdepthbuf.bindToWrite();
		glClear(GL_DEPTH_BUFFER_BIT);
		DeferredShading::setCSMShader(caustics_view, caustics_proj);
		glViewport(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT);
		main_scene.DrawSceneWithoutMVP(*DeferredShading::csmShader);

		// Draw caustics map
		causticmapbuf.bindToWrite();
		glClear(GL_COLOR_BUFFER_BIT);
		causticdepthbuf.bindTexture();
		glViewport(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT);
		main_scene.DrawCausticMap(*DeferredShading::causticShader, caustics_proj, caustics_view, main_light.GetDirLightDirection());

		glViewport(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT);

		// Set up jitter in projection(TAA)
		glm::mat4 projection = Common::GetPerspectiveMat(GameController::mainCamera);
		glm::mat4 jittered_projection = projection;
		glm::vec2 offset = haltonSequence[haltonIndex];
		offset = (offset - 0.5f) * 2.0f / glm::vec2(float(Common::SCR_WIDTH), float(Common::SCR_HEIGHT));
		haltonIndex = (haltonIndex + 1) % NUM_TAA_SAMPLES;

		// Draw terrain position for refraction
		refractionposbuf.bindToWrite();
		glClear(GL_COLOR_BUFFER_BIT);
		DeferredShading::setRefractionPositionShader(GameController::mainCamera.GetViewMatrix(), projection);
		main_scene.DrawSceneWithoutMVP(*DeferredShading::refractionPositionShader);

		// causticmapbuf.bindToRead();
		// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		// glBlitFramebuffer(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		if (taaOn)
		{
			jittered_projection[2][0] += offset.x;
			jittered_projection[2][1] += offset.y;
		}

		// Geometry pass
		gbuf.bindToWrite();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		main_scene.DrawScene(GameController::deltaTime, &projection, &jittered_projection, &lastViewProjection, nullptr, true);
		DrawObjects(modelShader, &projection, &jittered_projection, &lastViewProjection, nullptr, false, false);
		// Draw things that do not need to be lit
		main_scene.DrawSky(&projection, &jittered_projection, &lastViewProjection);
		main_light.Draw(&projection, &jittered_projection, &lastViewProjection, nullptr);

		// SSDO pass
		gbuf.bindTexture();
		aobuf.bindToWrite();
		glClear(GL_COLOR_BUFFER_BIT);
		DeferredShading::setSSDOShader(jittered_projection);
		DeferredShading::DrawQuad();

		// aobuf.bindToRead();
		// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		// glReadBuffer(GL_COLOR_ATTACHMENT1);
		// glBlitFramebuffer(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// SSDO blur pass
		aobuf.bindTexture();
		aoblurbuf.bindToWrite();
		DeferredShading::setSimpleBlurShader();
		DeferredShading::DrawQuad();

		glm::mat4 lightMVP[NUM_CASCADES];
		for (int i = 0; i < NUM_CASCADES; i++)
		{
			lightMVP[i] = GetCSMProjection(i) * lightView;
		}

		// Drawing quads from now on
		glDisable(GL_DEPTH_TEST);

		// Lighting pass
		gbuf.bindTexture();
		aoblurbuf.bindTexture();
		csmbuf.bindTexture();
		refractionposbuf.bindTexture();
		causticmapbuf.bindTexture();
		lightingcolorbuf.bindToWrite();
		glClear(GL_COLOR_BUFFER_BIT);
		InitLighting(*DeferredShading::lightingShader);
		glDisable(GL_BLEND);
		glm::mat4 causticsVP = caustics_proj * caustics_view;
		DeferredShading::setLightingPassShader(lightMVP, &causticsVP, cascade_Z);
		DeferredShading::DrawQuad();

		// SSReflection pass
		gbuf.bindTexture();
		ssreflectbuf.bindToWrite();
		DeferredShading::setSSReflectionShader(jittered_projection);
		DeferredShading::DrawQuad();

		// Draw reflection
		gbuf.bindTexture();
		lightingcolorbuf.bindTexture();
		ssreflectbuf.bindTexture();
		reflectdrawbuf.bindToWrite();
		DeferredShading::setReflectDrawShader();
		main_scene.bindSkyboxTexture(6);
		DeferredShading::DrawQuad();

		// SSRefraction pass
		gbuf.bindTexture();
		refractionposbuf.bindTexture();
		ssrefractbuf.bindToWrite();
		DeferredShading::setSSRefractionShader(jittered_projection);
		DeferredShading::DrawQuad();

		// Draw refraction
		gbuf.bindTexture();
		lightingcolorbuf.bindTexture();
		ssrefractbuf.bindTexture();
		refractdrawbuf.bindToWrite();
		DeferredShading::setRefractDrawShader();
		DeferredShading::DrawQuad();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		ssrefractbuf.bindToRead();
		glBlitFramebuffer(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// Combine reflect/refract and origin color
		taabuf.bindToWrite();
		glClear(GL_COLOR_BUFFER_BIT);
		gbuf.bindTexture();
		lightingcolorbuf.bindTexture();
		reflectdrawbuf.bindTexture();
		refractdrawbuf.bindTexture();
		DeferredShading::setCombineColorShader();
		DeferredShading::DrawQuad();

		// blit depth from gbuffer to taabuffer
		taabuf.bindToWrite();
		gbuf.bindToRead();
		glClear(GL_DEPTH_BUFFER_BIT);
		glBlitFramebuffer(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		gbuf.bindTexture();
		taabuf.bindTexture();
		postprocessor.bindToWrite();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DeferredShading::setTAAShader();
		DeferredShading::DrawQuad();

		// postprocess effect
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		postprocessor.bindTexture();
		DeferredShading::setPostprocessShader();
		DeferredShading::DrawQuad();

		taabuf.copyToLast();
		lastViewProjection = projection * GameController::mainCamera.GetViewMatrix();
#else
		InitLighting(main_scene.WaterShader);
		InitLighting(main_scene.TerrainShader);

		glm::vec4 clipping_plane = glm::vec4(0.0, -1.0, 0.0, 99999.0f);

		// ����ʰȡ
		if (GameController::gameMode == GameMode::Creating)
			if (GameController::creatingMode == CreatingMode::Selecting)

			{
				mouse_picking.bindFrameBuffer();
				glEnable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				PickObjects(pickingShader);
				mouse_picking.unbindFrameBuffer();
			}
			else
				;
		else
			GameController::selectedGameObj = NULL;

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawShadowMap(shadowShader);
		glm::mat4 projection = Common::GetPerspectiveMat(GameController::mainCamera);
		DrawObjects(modelShader, &projection, nullptr, nullptr, &clipping_plane, true, true);
		main_light.Draw(&projection, nullptr, nullptr, &clipping_plane);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		main_scene.setReflectText(waterfb.getReflectionTexture());
		main_scene.setRefractText(waterfb.getRefractionTexture());
		main_scene.setDepthMap(waterfb.getRefractionDepthTexture());
		main_scene.setShadowMap(shadowfb.getShadowTexture());
		main_scene.TerrainShader.use();
		main_scene.TerrainShader.setMat4("lightProjection", shadowfb.lightProjection); // Bad implementation
		main_scene.TerrainShader.setMat4("lightView", shadowfb.lightView);			   // Bad implementation
		main_scene.DrawScene(GameController::deltaTime, &projection, nullptr, nullptr, &clipping_plane, true, true);
		main_scene.DrawSky(&projection, nullptr, nullptr);

		glDisable(GL_BLEND);
#endif
	}

	glm::mat4 GetCSMProjection(int index) const
	{
		assert(index < NUM_CASCADES);
		return glm::ortho(CSMOrthoProjAABB[index].xmin, CSMOrthoProjAABB[index].xmax, CSMOrthoProjAABB[index].ymin, CSMOrthoProjAABB[index].ymax, CSMOrthoProjAABB[index].zmin, CSMOrthoProjAABB[index].zmax);
	}

private:
	void InitLighting(Shader &shader)
	{
		main_light.SetLight(shader);
	}

	void DrawObjects(Shader &modelShader, const glm::mat4 *projection, const glm::mat4 *jitteredProjection, const glm::mat4 *lastViewProjection, const glm::vec4 *clippling_plane, bool IsAfterPicking, bool IsWithShading)
	{
		if (IsWithShading)
			InitLighting(modelShader);
		glEnable(GL_CULL_FACE);
		bool enablePicking = GameController::gameMode == GameMode::Creating &&
							 GameController::creatingMode == CreatingMode::Selecting &&
							 IsAfterPicking && !GameController::isCursorOnGui;
		if (enablePicking)
			GameController::selectedGameObj = NULL;
		if (jitteredProjection && lastViewProjection)
		{
			modelShader.use();
			modelShader.setMat4("jitteredProjection", *jitteredProjection);
			modelShader.setMat4("lastViewProjection", *lastViewProjection);
		}

		auto itr = GameObject::gameObjList.begin();
		for (int i = 0; i < GameObject::gameObjList.size(); i++, ++itr)
		{
			bool intersected = false;
			// ����ģʽ�ķ���ģʽ����û������ѡ�У��Ҹ�������Ա�ѡ�У�����ʰȡ
			if (enablePicking)
			{
				float hitObjID = mouse_picking.ReadPixel(GameController::cursorX,
														 Common::SCR_HEIGHT - GameController::cursorY)
									 .ObjID; // deviation of y under resolution 1920*1080 maybe 22
				if ((unsigned int)hitObjID == i + 1)
				{
					GameController::selectedGameObj = *itr;
					intersected = true;
				}
			}
			if (clippling_plane)
			{
				modelShader.use();
				modelShader.setVec4("plane", *clippling_plane);
			}
			modelShader.use();

			(*itr)->Draw(modelShader, GameController::mainCamera.Position,
						 *projection, GameController::mainCamera.GetViewMatrix(),
						 intersected);
		}
		glDisable(GL_CULL_FACE);
	}

	void DrawObjectsWithoutVP(Shader &shadowPassShader)
	{
		glEnable(GL_CULL_FACE);
		auto itr = GameObject::gameObjList.begin();
		for (int i = 0; i < GameObject::gameObjList.size(); i++, itr++)
		{
			(*itr)->DrawShadowPass(shadowPassShader);
		}
		glDisable(GL_CULL_FACE);
	}

	void PickObjects(Shader &modelShader)
	{
		glEnable(GL_CULL_FACE);
		unsigned int object_counter = 0;
		auto itr = GameObject::gameObjList.begin();
		for (int i = 0; i < GameObject::gameObjList.size(); i++, ++itr)
		{
			if ((*itr)->IsPickable)
				(*itr)->Pick(modelShader, ++object_counter, 0,
							 Common::GetPerspectiveMat(GameController::mainCamera), GameController::mainCamera.GetViewMatrix());
		}
		glDisable(GL_CULL_FACE);
	}

#ifndef DEFERRED_SHADING
	void DrawShadowMap(Shader &shadowShader)
	{
		Camera &cam = GameController::mainCamera;
		glm::vec3 LightDir = main_light.GetDirLightDirection() * 10.0f;
		glm::vec3 DivPos = cam.Position;
		DivPos.z -= 20.0f;
		GLfloat near_plane = 1.0f, far_plane = 1000.0f;
		shadowfb.lightProjection = glm::ortho(-50.0f, 50.0f, -80.0f, 20.0f, near_plane, far_plane);
		// glm::lookat(eye, center, up)
		shadowfb.lightView = glm::lookAt(DivPos - LightDir, DivPos, glm::vec3(0.0f, 1.0f, 0.0f));

		Common::setWidthAndHeight();
		glViewport(0, 0, shadowfb.SHADOW_WIDTH, shadowfb.SHADOW_HEIGHT);
		shadowfb.bindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT);
		auto itr = GameObject::gameObjList.begin();
		for (int i = 0; i < GameObject::gameObjList.size(); i++, ++itr)
		{
			(*itr)->Draw(shadowShader, cam.Position, shadowfb.lightProjection, shadowfb.lightView);
		}
		shadowfb.unbindFrameBuffer();

		Common::setWidthAndHeight();
		glViewport(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
#endif

	void CSMUpdateOrthoProj()
	{
		glm::vec3 DivPos = GameController::mainCamera.Position;
		glm::mat4 lightView = glm::lookAt(DivPos - main_light.GetDirLightDirection() * 10.0f, DivPos, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 camView = GameController::mainCamera.GetViewMatrix();
		glm::mat4 inv_camView = glm::inverse(camView);
		float tanHalfFOVY = glm::tan(glm::radians(GameController::mainCamera.Zoom) / 2.0f);
		float aspectRatio = (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT;
		for (int i = 0; i < NUM_CASCADES; i++)
		{
			float yNear = cascade_Z[i] * tanHalfFOVY;
			float yFar = cascade_Z[i + 1] * tanHalfFOVY;
			float xNear = yNear * aspectRatio;
			float xFar = yFar * aspectRatio;
			glm::vec4 frustumVertices[8] = {
				glm::vec4(xNear, yNear, -cascade_Z[i], 1.0f),
				glm::vec4(-xNear, yNear, -cascade_Z[i], 1.0f),
				glm::vec4(xNear, -yNear, -cascade_Z[i], 1.0f),
				glm::vec4(-xNear, -yNear, -cascade_Z[i], 1.0f),
				glm::vec4(xFar, yFar, -cascade_Z[i + 1], 1.0f),
				glm::vec4(-xFar, yFar, -cascade_Z[i + 1], 1.0f),
				glm::vec4(xFar, -yFar, -cascade_Z[i + 1], 1.0f),
				glm::vec4(-xFar, -yFar, -cascade_Z[i + 1], 1.0f)};

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();
			for (int j = 0; j < 8; j++)
			{
				glm::vec4 worldPos = inv_camView * frustumVertices[j];
				frustumVertices[j] = lightView * worldPos;
				minX = minX < frustumVertices[j].x ? minX : frustumVertices[j].x;
				maxX = maxX > frustumVertices[j].x ? maxX : frustumVertices[j].x;
				minY = minY < frustumVertices[j].y ? minY : frustumVertices[j].y;
				maxY = maxY > frustumVertices[j].y ? maxY : frustumVertices[j].y;
				minZ = minZ < -frustumVertices[j].z ? minZ : -frustumVertices[j].z;
				maxZ = maxZ > -frustumVertices[j].z ? maxZ : -frustumVertices[j].z;
			}
			CSMOrthoProjAABB[i].xmin = minX;
			CSMOrthoProjAABB[i].xmax = maxX;
			CSMOrthoProjAABB[i].ymin = minY;
			CSMOrthoProjAABB[i].ymax = maxY;
			CSMOrthoProjAABB[i].zmin = minZ;
			CSMOrthoProjAABB[i].zmax = maxZ;
		}
	}
};
#endif
