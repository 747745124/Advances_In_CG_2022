#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "common.h"
#include "Shader.h"
#include "Camera.h"
#include "terrain.h"
#include "water.h"
#include "skybox.h"
#include "Texture.h"

class Scene
{
private:
	float chunk_size;

public:
	vector<Terrain> all_terrain_chunks;
	vector<Water> all_water_chunks;
	Skybox skybox;
	vector<string> groundPaths;
	int width;
	int height;
	float water_height;
	Shader &TerrainShader;
	Shader &WaterShader;
	Shader &SkyShader;
	float waterMoveFactor;
	unsigned int reflect_text, refract_text, dudvMap, normalMap, depthMap, shadowMap;

public:
	/*
	float chunk_size: define size of each chunk
	int scene_width: should be integer > 0, define the chunk grid width
	int scene_height: should be integer > 0, define the chunk grid height
	Shader& TerrainShader: Pass the terrain shader to it
	Shader& WaterShader: Pass the water shader to it
	Shader& SkyShader: Pass the skybox shader to it
	string ground_textures: Pass a string of ground texture
	vector<string> skyboxPaths: Pass a string vector contains skybox textures
	*/
	Scene(float chunk_size, int scene_width, int scene_height, float water_height, Shader &TerrainShader, Shader &WaterShader, Shader &SkyShader, vector<string> groundPaths, vector<string> skyboxPaths) : chunk_size(chunk_size), skybox(skyboxPaths), groundPaths(groundPaths), width(scene_width), height(scene_height), water_height(water_height), TerrainShader(TerrainShader), WaterShader(WaterShader), SkyShader(SkyShader)
	{
		waterMoveFactor = 0.0f;
		InitScene(groundPaths);
	}

	float getWaterHeight()
	{
		return this->water_height;
	}
	void setReflectText(unsigned int textID)
	{
		reflect_text = textID;
	}
	void setRefractText(unsigned int textID)
	{
		refract_text = textID;
	}
	void setDudvMap(unsigned int textID)
	{
		dudvMap = textID;
	}
	void setNormalMap(unsigned int textID)
	{
		normalMap = textID;
	}
	void setDepthMap(unsigned int textID)
	{
		depthMap = textID;
	}
	void setShadowMap(unsigned int textID)
	{
		shadowMap = textID;
	}

	void bindSkyboxTexture(int i)
	{
		skybox.bindSkyboxTexture(i);
	}
	// Draw sky(Skybox doesn't need to be lit)
	void DrawSky(const glm::mat4 *projection, const glm::mat4 *jitteredProjection, const glm::mat4 *lastViewProjection);

	// Draw all scene with shading (ground and water) (Used in forward shading)
	void DrawScene(float deltaTime, const glm::mat4 *projection, const glm::mat4 *jitteredProjection, const glm::mat4 *lastViewProjection, const glm::vec4 *clippling_plane, bool draw_water, bool draw_shadow = false);

	void DrawSceneWithoutMVP(Shader &shadowPassShader);

	float getTerrainHeight(float x, float z);

	void DrawCausticMap(Shader &causticShader, const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &lightDir);

private:
	void InitScene(vector<string> ground_path);
};

#endif