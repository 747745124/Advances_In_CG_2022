#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "Camera.h"
#include "model.h"
#include "scene.h"
#include "light.h"
#include "VideoRecord.h"

#include <glm/glm.hpp>
#define GLM_SWIZZLE
#include <GLFW/glfw3.h>

#include "common.h"
#include "GameObject.h"
#include "mousepicker.h"

#include <unordered_map>
#include <fstream>
#include "FileSystem.h"


namespace KooNan
{
	enum class GameMode
	{			   // ��һ��״̬
		Title,	   // ����ҳ
		Pause,	   // ��ͣҳ
		Wandering, // ��������ģʽ
		Creating   // ����ģʽ
	};
	enum class CreatingMode
	{
		Placing,	  // ����ģʽ
		Selecting,	  // ѡ��ģʽ
		Editing,	  // �༭ģ��ģʽ
		EditingLight, // �༭����
	};
	enum class MouseMode
	{
		GUIMode,
		CameraMode
	};
	class GameController
	{
		// ȫ��״̬
	public:
		static MouseMode mouseMode;
		static float deltaTime;
		static float lastFrame;

		static double cursorX, cursorY;
		static double lastCursorX, lastCursorY;

		static Camera mainCamera;
		static Camera oriCreatingCamera;

		static Scene *mainScene;
		static Light *mainLight;

		static string selectedModel;		// ��ǰѡ���ģ��?
		static GameObject *helperGameObj;	// ������Ϸ����
		static GameObject *selectedGameObj; // ����ƶ�������ѡ�е���Ϸ����?

		static MousePicker mousePicker;
		static int RECURSION_COUNT;

		// ȫ���źţ���GUIģ����������д�룬������ģ����?
	public:
		static GameMode gameMode; // ��Ϸģʽ
		static GameMode lastGameMode;
		static CreatingMode creatingMode;  // ����ģʽ��ģʽ
		static bool isRecordingLast;	   // ��һ��ѭ���Ƿ�����¼��
		static bool isRecording;		   // �Ƿ�����¼��
		static Model::ModelType modelType; // ģ���б�������
		static bool isCursorOnGui;
		// ����
	public:
		const static unsigned int EDGE_WIDTH = 50;
		// ״̬
	private:
		static bool firstMouse;		   // �Ƿ��ǵ�һ�ε������������ƶ��¼���
		static bool ctrlPressedLast;   // ��һ��ѭ���Ƿ���ctrl��
		static bool altPressedLast;	   // ��һ��ѭ���Ƿ���alt��
		static bool midBtnPressedLast; // ��һ��ѭ���Ƿ�������м�?
	public:
		static void initGameController(GLFWwindow *window);

		static void updateGameController(GLFWwindow *window);
		
		static void changeGameModeTo(GameMode newmode);
		

		static void revertGameMode()
		{
			changeGameModeTo(lastGameMode);
		}

		static void addLightToMainLight(const PointLight &pl)
		{
			if (mainLight == NULL)
				return;
			mainLight->AddPointLight(pl);
		}

		static bool LoadGameFromFile();
		
		static void SaveGameToFile();

	private:
		static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
		static void cursor_callback(GLFWwindow *window, double xpos, double ypos);
		static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
		static void processInput(GLFWwindow *window);
		static void updateCursorMode(GLFWwindow *window);

		// �ҵ������������εĽ���
		static glm::vec3 findFocusInScene();
		static glm::vec3 binarySearch(int count, float start, float finish, glm::vec3 ray);
		static bool intersectionInRange(float start, float finish, glm::vec3 ray);
	};

	

	
}

#endif