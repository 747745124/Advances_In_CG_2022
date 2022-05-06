#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "GameController.h"
#include "terrain.h"
#include "gui.h"
#include "GameObject.h"
#include "common.h"
#include "waterframebuffer.h"
#include "pickingtexture.h"
#include "scene.h"
#include "cube.h"
#include "gui.h"
#include "light.h"
#include "Texture.h"
#include "Render.h"


using namespace KooNan;
using namespace glm;

// Define four point lights position
glm::vec3 pointLightPositions[] = {
	glm::vec3(2.0f, 3.0f, 2.0f),
	glm::vec3(2.3f, 2.0f, -5.0f),
	glm::vec3(-4.0f, 2.5f, -8.0f),
	glm::vec3(0.0f, 1.5f, -3.0f)}; // This should match up with the macro NR_POINT_LIGHTS in fragment shader

// Define positions of cubes
glm::vec3 cubePositions[] = {
	glm::vec3(-9.0f, 0.0f, 0.0f),
	glm::vec3(-10.0f, 0.0f, 0.0f),
	glm::vec3(-9.0f, 0.0f, -3.0f),
	glm::vec3(-10.5f, 0.0f, 0.0f),
	glm::vec3(-9.0f, 0.0f, -3.7f),
	glm::vec3(-10.5f, 0.0f, 0.0f),
	glm::vec3(-9.0f, 0.0f, -4.7f),
	glm::vec3(-11.0f, 0.0f, 0.0f),
	glm::vec3(-15.0f, 0.0f, -6.7f),
	glm::vec3(-16.0f, 0.0f, 0.0f),
};

// Define paths of sky box textures
std::vector<std::string> skyboxPaths = {
	FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_px.jpg"),
	FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_nx.jpg"),
	FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_py.jpg"),
	FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_ny.jpg"),
	FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_pz.jpg"),
	FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_nz.jpg"),
};

// Define paths of ground and water textures
// Format:(1).groundheightmap (2~size-2).groundtexture (size-1).waterdudvmap (size).waternormalmap
std::vector<std::string> groundPaths = {
	FileSystem::getPath("landscape/resources/textures/heightmap.png"),
	FileSystem::getPath("landscape/resources/textures/blendermap.jpg"),
	FileSystem::getPath("landscape/resources/textures/background1.jpg"),
	FileSystem::getPath("landscape/resources/textures/groundR.jpg"),
	FileSystem::getPath("landscape/resources/textures/groundG.png"),
	FileSystem::getPath("landscape/resources/textures/groundB.jpg"),
	FileSystem::getPath("landscape/resources/textures/waterDUDV.png"),
	FileSystem::getPath("landscape/resources/textures/normal.png")};

void addlights(Light &light);

Shader *DeferredShading::lightingShader = nullptr;
GLFWwindow *Common::gWindow = nullptr;
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef DEFERRED_SHADING
	glfwWindowHint(GLFW_SAMPLES, 4);
#endif // !DEFERRED_SHADING

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------

	GLFWwindow *window = glfwCreateWindow(Common::SCR_WIDTH, Common::SCR_HEIGHT, "Koonan", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	GameController::initGameController(window);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	// glEnable(GL_MULTISAMPLE);

	// build and compile our shader program
	// ------------------------------------
#ifdef DEFERRED_SHADING
	Shader terrainShader(FileSystem::getPath("shaders/deferred/gbuffer.vs").c_str(), FileSystem::getPath("shaders/deferred/terrain.fs").c_str());
	Shader waterShader(FileSystem::getPath("shaders/deferred/water.vs").c_str(), FileSystem::getPath("shaders/deferred/water.fs").c_str());
	Shader skyShader(FileSystem::getPath("shaders/deferred/skybox.vs").c_str(), FileSystem::getPath("shaders/deferred/skybox.fs").c_str());
	Shader lightShader(FileSystem::getPath("shaders/deferred/lightcube.vs").c_str(), FileSystem::getPath("shaders/deferred/lightcube.fs").c_str());
	Shader pickingShader(FileSystem::getPath("shaders/deferred/picking.vs").c_str(), FileSystem::getPath("shaders/deferred/picking.fs").c_str());
	Shader modelShader(FileSystem::getPath("shaders/deferred/gbuffer.vs").c_str(), FileSystem::getPath("shaders/deferred/gbuffer.fs").c_str());
	Shader shadowShader(FileSystem::getPath("shaders/deferred/shadow.vs").c_str(), FileSystem::getPath("shaders/deferred/shadow.fs").c_str());
	Shader lightingShader(FileSystem::getPath("shaders/deferred/light.vs").c_str(), FileSystem::getPath("shaders/deferred/light.fs").c_str());
	DeferredShading::lightingShader = &lightingShader;
#else
	Shader terrainShader(FileSystem::getPath("shaders/forward/terrain.vs").c_str(), FileSystem::getPath("shaders/forward/terrain.fs").c_str());
	Shader waterShader(FileSystem::getPath("shaders/forward/water.vs").c_str(), FileSystem::getPath("shaders/forward/water.fs").c_str());
	Shader skyShader(FileSystem::getPath("shaders/forward/skybox.vs").c_str(), FileSystem::getPath("shaders/forward/skybox.fs").c_str());
	Shader lightShader(FileSystem::getPath("shaders/forward/lightcube.vs").c_str(), FileSystem::getPath("shaders/forward/lightcube.fs").c_str());
	Shader pickingShader(FileSystem::getPath("shaders/forward/picking.vs").c_str(), FileSystem::getPath("shaders/forward/picking.fs").c_str());
	Shader modelShader(FileSystem::getPath("shaders/forward/model.vs").c_str(), FileSystem::getPath("shaders/forward/model.fs").c_str());
	Shader shadowShader(FileSystem::getPath("shaders/forward/shadow.vs").c_str(), FileSystem::getPath("shaders/forward/shadow.fs").c_str());
#endif

	// Instantiate the main_scene
	Scene main_scene(256.0f, 1, 1, -0.7f, terrainShader, waterShader, skyShader, groundPaths, skyboxPaths);
	GameController::mainScene = &main_scene; // 这个设计实在是不行

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	// Model
	// ------------------------------------

	// Model::loadModelsFromPath("model/rsc/", Model::ModelType::ComplexModel);
	// Model::loadModelsFromPath("model/basic voxel/", Model::ModelType::BasicVoxel);

	// Instantiate the light(with only "parallel" light component)
	// ------------------------------------
	DirLight parallel{
		glm::vec3(0.3f, -0.7f, 1.0f),
		glm::vec3(0.45f, 0.45f, 0.45f),
		glm::vec3(0.35f, 0.35f, 0.35f),
		glm::vec3(0.2f, 0.2f, 0.2f)};
	Light main_light(parallel, lightShader);
	GameController::mainLight = &main_light; // 这个设计实在不行

	if (!GameController::LoadGameFromFile())
	{
		addlights(main_light); // Add four point lights

		GameObject *p3 = new GameObject("model/rsc/Temple1/Temple1.obj",
										scale(translate(mat4(1.0f), vec3(-7.0f, main_scene.getTerrainHeight(-7.0f, -7.0f), -7.0f)), vec3(0.2f, 0.2f, 0.2f)), true);
	}

	// GUI
	// ------------------------------------
	GUI::initEnv(window);
	// GUI::updateModelTextures(modelShader);

	PickingTexture mouse_picking;
	Water_Frame_Buffer waterfb;
	Shadow_Frame_Buffer shadowfb;
	Render main_renderer(main_scene, *GameController::mainLight, waterfb, mouse_picking, shadowfb);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		GameController::updateGameController(window);

		//需要渲染三次 前两次不渲染水面 最后一次渲染水面

#ifndef DEFERRED_SHADING

		main_renderer.DrawReflection(modelShader);

		main_renderer.DrawRefraction(modelShader);

#endif // !DEFERRED_SHADING

		main_renderer.DrawAll(pickingShader, modelShader, shadowShader);

		/*
		Render the else you need to render here!! Remember to set the clipping plane!!!
		*/

		glDisable(GL_DEPTH_TEST);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		// for (GameObject* obj : GameObject::gameObjList)
		//	obj->Draw(ourShader, projection, view);

		GUI::newFrame();
		GUI::drawWidgets();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// GameObject clear
	for (GameObject *obj : GameObject::gameObjList)
		delete obj;
	GameObject::gameObjList.clear();

	for (auto &itr : Model::modelList)
		delete itr.second;

	Model::modelList.clear();
	waterfb.cleanUp();
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void addlights(Light &light)
{
	for (int i = 0; i < 4; i++)
	{
		PointLight l{
			glm::vec3(pointLightPositions[i]),
			1.0f,
			0.09f,
			0.032f,
			glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			glm::vec3(1.0f, 1.0f, 1.0f),
		};
		light.AddPointLight(l);
	}
}
