#include <vector>
#include <string>

#include "scene.h"
#include "GameController.h"

void Scene::InitScene(std::vector<std::string> ground_path)
{
	bool use_heightMap = false;
	string heightMapPath;
	if (groundPaths.size() == 8)
	{
		use_heightMap = true;
		heightMapPath = *ground_path.begin();
		ground_path.erase(ground_path.begin());
		assert(ground_path.size() == 7);
	}
	std::vector<std::string> types;
	for (int k = 0; k < ground_path.size(); k++)
	{
		types.push_back("texture_diffuse");
	}

	TextureManager texman(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	std::vector<Texture> textures = texman.LoadTexture(ground_path, types);
	std::vector<Texture> ground_textures;
	for (int l = 0; l < 5; l++)
	{
		ground_textures.push_back(textures[l]);
	}
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			if (!use_heightMap)
			{
				Terrain ground(i, j, ground_textures, chunk_size);
				all_terrain_chunks.push_back(ground);
				Water water_surface(i, j, water_height, chunk_size);
				all_water_chunks.push_back(water_surface);
			}
			else
			{
				Terrain ground(i, j, ground_textures, heightMapPath, chunk_size);
				all_terrain_chunks.push_back(ground);
#ifdef DEFERRED_SHADING
				Water water_surface(i, j, 4096, water_height, chunk_size);
#else // !
				Water water_surface(i, j, water_height, chunk_size);
#endif
				all_water_chunks.push_back(water_surface);
			}
		}

	WaterShader.use();

#ifndef DEFERRED_SHADING
	WaterShader.setVec3("material.diffuse", glm::vec3(0.2, 0.2, 0.2));
	WaterShader.setVec3("material.specular", glm::vec3(1.0, 1.0, 1.0));
	WaterShader.setFloat("material.shininess", 128.0f);
#endif // !DEFERRED_SHADING
	setDudvMap(textures[textures.size() - 2].id);
	setNormalMap(textures[textures.size() - 1].id);
}

void Scene::DrawSky(const glm::mat4 *projection, const glm::mat4 *jitteredProjection, const glm::mat4 *lastViewProjection)
{
	SkyShader.use();

	Camera &cam = GameController::mainCamera;
	if (jitteredProjection && lastViewProjection)
	{
		SkyShader.use();
		SkyShader.setMat4("jitteredProjection", *jitteredProjection);
		SkyShader.setMat4("lastViewProjection", *lastViewProjection);
	}
	skybox.Draw(SkyShader, glm::scale(glm::mat4(1.0f), glm::vec3(500.0f)), cam.GetViewMatrix(), *projection);
}

void Scene::DrawScene(float deltaTime, const glm::mat4 *projection, const glm::mat4 *jitteredProjection, const glm::mat4 *lastViewProjection, const glm::vec4 *clippling_plane, bool draw_water, bool draw_shadow)
{
	Camera &cam = GameController::mainCamera;
	glm::mat4 view = cam.GetViewMatrix();
	glm::vec3 viewPos = cam.Position;

	if (draw_shadow)
	{
		TerrainShader.use();
		TerrainShader.setMat4("projection", *projection);
		TerrainShader.setMat4("view", view);
		if (clippling_plane)
			TerrainShader.setVec4("plane", *clippling_plane);
		TerrainShader.setVec3("viewPos", viewPos);
		TerrainShader.setVec3("skyColor", glm::vec3(0.527f, 0.805f, 0.918f));
		TerrainShader.setInt("shadowMap", 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		for (int i = 0; i < all_terrain_chunks.size(); i++)
		{
			all_terrain_chunks[i].Draw(TerrainShader);
		}
	}
	else
	{
		TerrainShader.use();
		TerrainShader.setMat4("projection", *projection);
		if (jitteredProjection && lastViewProjection)
		{
			TerrainShader.setMat4("jitteredProjection", *jitteredProjection);
			TerrainShader.setMat4("lastViewProjection", *lastViewProjection);
		}
		TerrainShader.setMat4("view", view);
		TerrainShader.setMat4("model", glm::mat4(1.0f));
		if (clippling_plane)
			TerrainShader.setVec4("plane", *clippling_plane);
		for (int i = 0; i < all_terrain_chunks.size(); i++)
		{
			all_terrain_chunks[i].Draw(TerrainShader);
		}
	}

	if (draw_water)
	{
		waterMoveFactor += deltaTime * 0.1f;
		waterMoveFactor = waterMoveFactor - (int)waterMoveFactor;
		if (clippling_plane) // Forward rendering
		{
			WaterShader.use();
			WaterShader.setMat4("projection", *projection);
			WaterShader.setMat4("view", view);
			WaterShader.setVec3("viewPos", viewPos);
			WaterShader.setInt("reflection", 0);
			WaterShader.setInt("refraction", 1);
			WaterShader.setInt("dudvMap", 2);
			WaterShader.setInt("normalMap", 3);
			WaterShader.setInt("depthMap", 4);
			WaterShader.setFloat("chunk_size", chunk_size);
			WaterShader.setFloat("moveOffset", waterMoveFactor);
			WaterShader.setVec3("skyColor", glm::vec3(0.527f, 0.805f, 0.918f));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, reflect_text);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, refract_text);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, dudvMap);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, normalMap);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			for (int j = 0; j < all_water_chunks.size(); j++)
			{
				all_water_chunks[j].Draw(WaterShader);
			}
		}
		else // Deferred rendering
		{
			WaterShader.use();
			WaterShader.setMat4("view", view);
			WaterShader.setMat4("projection", *projection);
			WaterShader.setMat4("jitteredProjection", *jitteredProjection);
			WaterShader.setMat4("lastViewProjection", *lastViewProjection);
			WaterShader.setFloat("chunk_size", chunk_size);
			WaterShader.setFloat("moveOffset", waterMoveFactor);
			WaterShader.setInt("dudvMap", 0);
			WaterShader.setInt("normalMap", 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, dudvMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap);
			for (int j = 0; j < all_water_chunks.size(); j++)
			{
				all_water_chunks[j].Draw(WaterShader);
			}
		}
	}
}

void Scene::DrawCausticMap(Shader &causticShader, const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &lightDir)
{
	causticShader.use();
	causticShader.setMat4("projection", projection);
	causticShader.setMat4("view", view);
	causticShader.setVec3("lightDir", lightDir);
	causticShader.setFloat("chunk_size", chunk_size);
	causticShader.setFloat("moveOffset", waterMoveFactor);
	causticShader.setInt("dudvMap", 0);
	causticShader.setInt("normalMap", 1);
	causticShader.setInt("terrainDepth", 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dudvMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	for (int j = 0; j < all_water_chunks.size(); j++)
	{
		all_water_chunks[j].Draw(causticShader);
	}
}

void Scene::DrawSceneWithoutMVP(Shader &shadowPassShader)
{
	for (int i = 0; i < all_terrain_chunks.size(); i++)
	{
		all_terrain_chunks[i].Draw(shadowPassShader);
	}
}

float Scene::getTerrainHeight(float x, float z)
{
	float relativeX = x + chunk_size / 2;
	float relativeZ = z + chunk_size / 2;
	int gridX = (int)(relativeX / chunk_size);
	int gridZ = (int)(relativeZ / chunk_size);
	if (gridX != 0 || gridZ != 0)
		return 0;
	float height = all_terrain_chunks[gridX * width + gridZ].GetTerrainHeight(x, z);
	if (height < getWaterHeight())
		height = getWaterHeight();
	return height;
}
