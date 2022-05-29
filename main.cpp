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
Shader *DeferredShading::ssrShader = nullptr;
Shader *DeferredShading::reflectDrawShader = nullptr;
Shader *DeferredShading::ssaoShader = nullptr;
Shader *DeferredShading::simpleBlurShader = nullptr;
Shader *DeferredShading::kuwaharaBlurShader = nullptr;
Shader *DeferredShading::combineColorShader = nullptr;
Shader *DeferredShading::csmShader = nullptr;
Shader *DeferredShading::taaShader = nullptr;
const float Render::cascade_Z[NUM_CASCADES + 1] = {0.1f, 30.0f, 100.0f, 1000.0f};
unsigned Render::cascadeUpdateCounter[NUM_CASCADES] = {1, 1, 1};

const glm::vec2 Render::haltonSequence[NUM_TAA_SAMPLES] = {
	glm::vec2(0.500000, 0.333333),
	glm::vec2(0.250000, 0.666667),
	glm::vec2(0.750000, 0.111111),
	glm::vec2(0.125000, 0.444444),
	glm::vec2(0.625000, 0.777778),
	glm::vec2(0.375000, 0.222222),
	glm::vec2(0.875000, 0.555556),
	glm::vec2(0.062500, 0.888889),
	glm::vec2(0.562500, 0.037037),
	glm::vec2(0.312500, 0.370370),
	glm::vec2(0.812500, 0.703704),
	glm::vec2(0.187500, 0.148148),
	glm::vec2(0.687500, 0.481481),
	glm::vec2(0.437500, 0.814815),
	glm::vec2(0.937500, 0.259259),
	glm::vec2(0.031250, 0.592593)};

/*
const glm::vec2 Render::haltonSequence[NUM_TAA_SAMPLES] = {
	glm::vec2(3 / 8.0,1 / 8.0),
	glm::vec2(7 / 8.0,3 / 8.0),
	glm::vec2(1 / 8.0,5 / 8.0),
	glm::vec2(5 / 8.0,7 / 8.0)
};
*/
unsigned Render::haltonIndex = 0;
glm::mat4 Render::lastViewProjection;
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
#ifdef __APPLE__
	GLFWwindow* window = glfwCreateWindow(Common::SCR_WIDTH / 2, Common::SCR_HEIGHT / 2, "Koonan", NULL, NULL);
#else
	GLFWwindow* window = glfwCreateWindow(Common::SCR_WIDTH, Common::SCR_HEIGHT, "Koonan", NULL, NULL);
#endif // __APPLE__

	

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
	// There is an error but I don't exactly know why
	glGetError(); //!!!!!!!!!!!!!!!!!!!!!!!Clear it

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
	Shader ssrShader(FileSystem::getPath("shaders/deferred/ssr.vs").c_str(), FileSystem::getPath("shaders/deferred/ssr.fs").c_str());
	Shader reflectDrawShader(FileSystem::getPath("shaders/deferred/reflectdraw.vs").c_str(), FileSystem::getPath("shaders/deferred/reflectdraw.fs").c_str());
	Shader ssaoShader(FileSystem::getPath("shaders/deferred/ssao.vs").c_str(), FileSystem::getPath("shaders/deferred/ssao.fs").c_str());
	Shader simpleblurShader(FileSystem::getPath("shaders/deferred/simpleblur.vs").c_str(), FileSystem::getPath("shaders/deferred/simpleblur.fs").c_str());
	Shader kuwaharaBlurShader(FileSystem::getPath("shaders/deferred/kuwaharablur.vs").c_str(), FileSystem::getPath("shaders/deferred/kuwaharablur.fs").c_str());
	Shader combineColorShader(FileSystem::getPath("shaders/deferred/combinecolor.vs").c_str(), FileSystem::getPath("shaders/deferred/combinecolor.fs").c_str());
	Shader csmShader(FileSystem::getPath("shaders/deferred/csm.vs").c_str(), FileSystem::getPath("shaders/deferred/csm.fs").c_str());
	Shader taaShader(FileSystem::getPath("shaders/deferred/taa.vs").c_str(), FileSystem::getPath("shaders/deferred/taa.fs").c_str());
	DeferredShading::lightingShader = &lightingShader;
	DeferredShading::ssrShader = &ssrShader;
	DeferredShading::reflectDrawShader = &reflectDrawShader;
	DeferredShading::ssaoShader = &ssaoShader;
	DeferredShading::simpleBlurShader = &simpleblurShader;
	DeferredShading::kuwaharaBlurShader = &kuwaharaBlurShader;
	DeferredShading::combineColorShader = &combineColorShader;
	DeferredShading::csmShader = &csmShader;
	DeferredShading::taaShader = &taaShader;
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
		glm::vec3(0.3f, 0.3f, 0.3f),
		glm::vec3(0.35f, 0.35f, 0.35f),
		glm::vec3(0.4f, 0.4f, 0.4f)};
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

#ifndef DEFERRED_SHADING
	Water_Frame_Buffer waterfb;
	Shadow_Frame_Buffer shadowfb;
	Render main_renderer(main_scene, *GameController::mainLight, waterfb, mouse_picking, shadowfb);
#endif

#ifdef DEFERRED_SHADING
	Render main_renderer(main_scene, *GameController::mainLight, mouse_picking);
#endif

	// a frame counter
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{

#ifdef FRAME_COUNT
		// frame count
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0)
		{ // If last prinf() was more than 1 sec ago
			// printf and reset timer
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}
#endif

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

#ifndef DEFERRED_SHADING
	waterfb.cleanUp();
#endif

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
			glm::vec3(0.15f, 0.15f, 0.15f),
		};
		light.AddPointLight(l);
	}
}
