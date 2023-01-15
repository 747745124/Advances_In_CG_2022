#pragma once

#include <model.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GameController.h>

namespace GUI_Param
{
	int menuFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground;
	int selectPageFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	bool ssrOn = true;
	bool ssRefractOn = true;
	bool ssaoOn = true;
	bool csmOn = false;
	bool ReflectOn = true;
	bool showFPS = true;
	bool taaOn = true;
	bool toneMapping = false;
	int effectType = -1; // sobel=0
	float ssrThickness = 1.0f;
	float exposure = 1.0f;
	int softShadowType = 2; // 2 for PCSS, 1 for PCF

	class GUI
	{
	public:
		static void initEnv(GLFWwindow *window)
		{
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO &io = ImGui::GetIO();

			// Setup Platform/Renderer bindings
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init();

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
		}

		// Use preview.png instead.
		static void updateModelTextures(Shader &shader)
		{
			modelTextures.clear();
			int ntextures = Model::modelList.size();
			GLuint *textures = new GLuint[ntextures];
			GLuint *framebuffers = new GLuint[ntextures];
			glGenTextures(ntextures, textures);
			glGenFramebuffers(ntextures, framebuffers);
			modelTextures.resize(ntextures);
			frameBuffers.resize(ntextures);

			int i = 0;
			for (pair<const string, Model *> p : Model::modelList)
			{
				GLuint curTexture = textures[i], curFrameBuffer = framebuffers[i];
				modelTextures[i] = curTexture;
				frameBuffers[i] = curFrameBuffer;
				glBindTexture(GL_TEXTURE_2D, curTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 480, 360, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				glBindFramebuffer(GL_FRAMEBUFFER, curFrameBuffer);
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, curTexture, 0);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);

				glViewport(0, 0, 480, 360);
				i++;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT);
			delete[] textures, framebuffers;
		}

		static void newFrame()
		{
			// feed inputs to dear imgui, start new frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		static void drawWidgets(GLFWwindow *window)
		{
			static ImVec2 menuButtonSize(buttonWidth1, buttonHeight1);
			static ImVec2 shotcutButtonSize(buttonWidth2, buttonHeight2);
			ImVec2 maxpos;
			GameController::isCursorOnGui = false;
			ImGui::Begin("Menu", 0, menuFlags);
			switch (GameController::gameMode)
			{
			case GameMode::Title:
				ImGui::SetWindowPos(ImVec2((Common::SCR_WIDTH - menuWidth) / 2, Common::SCR_HEIGHT * 2 / 3));
				if (ImGui::Button("Start a New Game", menuButtonSize))
				{
					GameController::changeGameModeTo(GameMode::Wandering);
				}
				if (ImGui::Button("Load a Game", menuButtonSize))
				{
				}
				if (ImGui::Button("Exit", menuButtonSize))
				{
					exit(0);
				}
				break;
			case GameMode::Wandering:
				ImGui::SetWindowPos(ImVec2(0, 0));
				maxpos.x = ImGui::GetWindowSize().x + ImGui::GetWindowPos().x;
				maxpos.y = ImGui::GetWindowSize().y + ImGui::GetWindowPos().y;
				if (ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), maxpos))
					GameController::isCursorOnGui = true;
				if (!hideGui && ImGui::Button("Pause", shotcutButtonSize))
				{
					GameController::changeGameModeTo(GameMode::Pause);
				}
				if ((!hideGui && !GameController::isRecording && ImGui::Button("Begin Record", shotcutButtonSize)) ||
					(!hideGui && GameController::isRecording && ImGui::Button("End Record", shotcutButtonSize)))
				{
					GameController::isRecording = !GameController::isRecording;
				}
				if ((!hideGui && ImGui::Button("Hide UI", shotcutButtonSize)) ||
					(hideGui && ImGui::Button("Show UI", shotcutButtonSize)))
				{
					hideGui = !hideGui;
				}
				if (!hideGui && ImGui::Button("Edit Scene", shotcutButtonSize))
				{
					GameController::changeGameModeTo(GameMode::Creating);
				}
				if (ImGui::Button("Normal View", shotcutButtonSize))
				{
					GameController::changeGameModeTo(GameMode::Creating);
					GameController::updateCursorMode(window);
				}
				else
					;

				ImGui::Checkbox("SSReflect On/Off", &ssrOn);
				ImGui::SliderFloat("SSR Thickness", &ssrThickness, 0.f, 10.f);
				ImGui::Checkbox("SSRefract On/Off", &ssRefractOn);
				ImGui::Checkbox("SSAO On/Off", &ssaoOn);
				ImGui::Checkbox("CSM Color On/Off", &csmOn);
				ImGui::Checkbox("Reflect On/Off", &ReflectOn);
				ImGui::Checkbox("TAA On/Off", &taaOn);

				ImGui::Text("Soft Shadow Type");
				ImGui::RadioButton("PCF", &softShadowType, 1);
				ImGui::SameLine();
				ImGui::RadioButton("PCSS", &softShadowType, 2);
				ImGui::Checkbox("Tone Mapping", &toneMapping);
				ImGui::SliderFloat("Exposure", &exposure, 0.f, 10.f);
				ImGui::Checkbox("Show FPS", &showFPS);

				ImGui::Text("Postprocessing");
				ImGui::RadioButton("None", &effectType, -1);
				ImGui::SameLine();
				ImGui::RadioButton("Sobel", &effectType, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Depth of Field", &effectType, 1);

				if (showFPS)
				{
					ImGui::SameLine();
					ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
				}

				break;
				/*
			case KooNan::GUIState::Recording:
				ImGui::SetWindowPos(ImVec2(0, 0));
				if (!hideGui && ImGui::Button("End Record", shotcutButtonSize)) {
					changeStateTo(GUIState::Wandering);
					// todo: emit record end
				}
				break;*/
			case GameMode::Creating:
				ImGui::SetWindowPos(ImVec2(0, 0));
				if (ImGui::Button("Pause", shotcutButtonSize))
				{
					GameController::changeGameModeTo(GameMode::Pause);
				}
				if (ImGui::Button("Save", shotcutButtonSize))
				{
					GameController::SaveGameToFile();
				}
				if (GameController::creatingMode == CreatingMode::Placing)
					if (GameController::modelType == Model::ModelType::ComplexModel &&
						ImGui::Button("Basic Voxel", shotcutButtonSize))
						GameController::modelType = Model::ModelType::BasicVoxel;
					else if (GameController::modelType == Model::ModelType::BasicVoxel &&
							 ImGui::Button("Complex Model", shotcutButtonSize))
						GameController::modelType = Model::ModelType::ComplexModel;
					else
						;
				if (GameController::creatingMode == CreatingMode::Selecting && ImGui::Button("Place", shotcutButtonSize))
				{
					GameController::creatingMode = CreatingMode::Placing;
				}
				else if (GameController::creatingMode == CreatingMode::Placing && ImGui::Button("Select", shotcutButtonSize))
				{
					GameController::creatingMode = CreatingMode::Selecting;
				}
				if (GameController::creatingMode == CreatingMode::Selecting && ImGui::Button("Edit Light", shotcutButtonSize))
				{
					GameController::creatingMode = CreatingMode::EditingLight;
				}
				else if (GameController::creatingMode == CreatingMode::EditingLight && ImGui::Button("Select", shotcutButtonSize))
				{
					GameController::creatingMode = CreatingMode::Selecting;
				}
				if (ImGui::Button("Wander", shotcutButtonSize))
				{
					if (1)
					{
						GameController::changeGameModeTo(GameMode::Wandering);
					}
				}
				if (ImGui::Button("Back Home", shotcutButtonSize))
				{
					GameController::mainCamera = GameController::oriCreatingCamera;
				}
				ImGui::Checkbox("SSReflect On/Off", &ssrOn);
				ImGui::SliderFloat("SSR Thickness", &ssrThickness, 0.f, 10.f);
				ImGui::Checkbox("SSRefract On/Off", &ssRefractOn);

				ImGui::Checkbox("SSAO On/Off", &ssaoOn);
				ImGui::Checkbox("CSM Color On/Off", &csmOn);
				ImGui::Checkbox("Cubemap Reflect On/Off", &ReflectOn);
				ImGui::Checkbox("TAA On/Off", &taaOn);
				ImGui::Text("Soft Shadow Type");
				ImGui::RadioButton("PCF", &softShadowType, 1);
				ImGui::SameLine();
				ImGui::RadioButton("PCSS", &softShadowType, 2);
				ImGui::Checkbox("Tone Mapping", &toneMapping);
				ImGui::SliderFloat("Exposure", &exposure, 0.f, 10.f);
				ImGui::Checkbox("Show FPS", &showFPS);

				ImGui::Text("Postprocessing");
				ImGui::RadioButton("None", &effectType, -1);
				ImGui::SameLine();
				ImGui::RadioButton("Sobel", &effectType, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Depth of Field", &effectType, 1);

				if (showFPS)
				{
					ImGui::SameLine();
					ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
				}
				break;
			case GameMode::Pause:
				ImGui::SetWindowPos(ImVec2((Common::SCR_WIDTH - menuWidth) / 2, Common::SCR_HEIGHT * 2 / 3));
				if (ImGui::Button("Continue", menuButtonSize))
				{
					GameController::revertGameMode();
				}
				if (ImGui::Button("Save and Quit to Title", menuButtonSize))
				{
					// todo����鵱ǰ�Ƿ�ѡ���˽�����λ�ò��Ϸ�??
					GameController::SaveGameToFile();
					if (1)
					{
						GameController::changeGameModeTo(GameMode::Title);
					}
				}
				break;
			default:
				break;
			}
			checkMouseOnGui();
			ImGui::End();

			if (GameController::gameMode == GameMode::Creating)
			{
				if (GameController::creatingMode == CreatingMode::Placing)
				{
					int pageHeight = Common::SCR_HEIGHT / 8;
					pageHeight = pageHeight < 150 ? 150 : pageHeight;
					ImVec2 selectButtonSize((pageHeight - 30) / 3 * 4, pageHeight - 30);
					ImGui::Begin("Select Page", 0, selectPageFlags);
					ImGui::SetWindowPos(ImVec2(10, Common::SCR_HEIGHT / 2 - 10 - pageHeight));
					ImGui::SetWindowSize(ImVec2((Common::SCR_WIDTH - 20) / 2, pageHeight));

					/*
					int i = 0;
					for (pair<const string, Model*> p : Model::modelList) {
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffers[i]);
						glViewport(0, 0, 480, 360);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						glGenerateMipmap(GL_TEXTURE_2D);
						p.second->Draw(NULL);
						i++;
					}
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
					glViewport(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT);
					*/

					int i = 0;
					for (pair<const string, Model *> p :
						 (GameController::modelType == Model::ModelType::ComplexModel ? Model::modelList : Model::basicVoxelList))
					{
						if (i)
							ImGui::SameLine();

						if (p.second->previewImage)
						{
							if (ImGui::ImageButton((void *)(p.second->previewImage->id), selectButtonSize))
							{
								GameController::selectedModel = p.first;
								GameController::creatingMode = CreatingMode::Placing;
							}
						}
						else
						{
							if (ImGui::Button("Preview Image not Found", selectButtonSize))
							{
								GameController::selectedModel = p.first;
								GameController::creatingMode = CreatingMode::Placing;
							}
						}
						i++;
					}

					checkMouseOnGui(0.f, -1.f, Common::SCR_WIDTH, Common::SCR_HEIGHT - 2);
					/*
					cout << "pos: " << ImGui::GetWindowPos().x << " " << ImGui::GetWindowPos().y << endl;
					cout << "size: " << ImGui::GetWindowSize().x << " " << ImGui::GetWindowSize().y << endl;
					cout << "windowsize: " << Common::SCR_WIDTH << " " << Common::SCR_HEIGHT << endl;
					cout << "area: " << ImGui::GetWindowSize().x << " " << Common::SCR_HEIGHT - ImGui::GetWindowPos().y << endl;
					 */
					ImGui::End();
				}
				else if (GameController::creatingMode == CreatingMode::EditingLight)
				{
					int pageHeight = Common::SCR_HEIGHT / 4;
					pageHeight = pageHeight < 250 ? 250 : pageHeight;
					ImVec2 selectButtonSize((pageHeight - 30) * 2, pageHeight);
					ImGui::Begin("Select Page", 0, selectPageFlags);
					ImGui::SetWindowPos(ImVec2(10, Common::SCR_HEIGHT - 10 - pageHeight));
					ImGui::SetWindowSize(ImVec2(Common::SCR_WIDTH - 20, pageHeight));

					static vector<PointLight *> pls;
					static DirLight *dl = NULL;
					if (GameController::mainLight)
					{
						unsigned int num = GameController::mainLight->numOfPointLight();
						if (pls.size() != num)
						{
							pls.clear();
							for (unsigned int i = 0; i < num; i++)
								pls.push_back(GameController::mainLight->getPointLightAt(i));
						}
						if (dl == NULL)
							dl = GameController::mainLight->getDirectionLight();

						unsigned int i = 0;
						for (i = 0; i < num; i++)
						{
							if (i)
								ImGui::SameLine();
							ImGui::BeginChild(i + 1, ImVec2(selectButtonSize.x, selectButtonSize.y));
							ImGui::SliderFloat("X", &pls[i]->position.x, -100.f, 100.f);
							ImGui::SliderFloat("Y", &pls[i]->position.y, -100.f, 100.f);
							ImGui::SliderFloat("Z", &pls[i]->position.z, -100.f, 100.f);
							ImGui::SliderFloat("R Ambient", &pls[i]->ambient.r, 0.f, 1.f);
							ImGui::SliderFloat("G Ambient", &pls[i]->ambient.g, 0.f, 1.f);
							ImGui::SliderFloat("B Ambient", &pls[i]->ambient.b, 0.f, 1.f);
							ImGui::SliderFloat("R Diffuse", &pls[i]->diffuse.r, 0.f, 1.f);
							ImGui::SliderFloat("G Diffuse", &pls[i]->diffuse.g, 0.f, 1.f);
							ImGui::SliderFloat("B Diffuse", &pls[i]->diffuse.b, 0.f, 1.f);
							ImGui::SliderFloat("R Specular", &pls[i]->specular.r, 0.f, 1.f);
							ImGui::SliderFloat("G Specular", &pls[i]->specular.g, 0.f, 1.f);
							ImGui::SliderFloat("B Specular", &pls[i]->specular.b, 0.f, 1.f);
							ImGui::EndChild();
						}
						if (i)
							ImGui::SameLine();
						ImGui::BeginChild(i + 1, ImVec2(selectButtonSize.x, selectButtonSize.y));
						ImGui::SliderFloat("X", &dl->direction.x, -100.f, 100.f);
						ImGui::SliderFloat("Y", &dl->direction.y, -100.f, 100.f);
						ImGui::SliderFloat("Z", &dl->direction.z, -100.f, 100.f);
						ImGui::SliderFloat("R Ambient", &dl->ambient.r, 0.f, 1.f);
						ImGui::SliderFloat("G Ambient", &dl->ambient.g, 0.f, 1.f);
						ImGui::SliderFloat("B Ambient", &dl->ambient.b, 0.f, 1.f);
						ImGui::SliderFloat("R Diffuse", &dl->diffuse.r, 0.f, 1.f);
						ImGui::SliderFloat("G Diffuse", &dl->diffuse.g, 0.f, 1.f);
						ImGui::SliderFloat("B Diffuse", &dl->diffuse.b, 0.f, 1.f);
						ImGui::SliderFloat("R Specular", &dl->specular.r, 0.f, 1.f);
						ImGui::SliderFloat("G Specular", &dl->specular.g, 0.f, 1.f);
						ImGui::SliderFloat("B Specular", &dl->specular.b, 0.f, 1.f);
						ImGui::EndChild();
					}

					checkMouseOnGui();
					ImGui::End();
				}
				else if (GameController::creatingMode == CreatingMode::Editing)
				{
					ImGui::Begin("Edit Menu", 0, menuFlags & ~ImGuiWindowFlags_NoBackground);
					// ImGui::SetWindowPos(ImVec2(Common::SCR_WIDTH - shotcutButtonSize.x - 10, 0));

					ImGui::SetWindowPos(ImVec2(GameController::lastCursorX, GameController::lastCursorY));
					if (ImGui::Button("Transform", shotcutButtonSize))
					{
						GameController::helperGameObj = GameController::selectedGameObj;
						GameController::selectedGameObj = NULL;
						GameController::creatingMode = CreatingMode::Placing;
					}
					if (ImGui::Button("Delete", shotcutButtonSize))
					{
						auto itr = GameObject::gameObjList.begin();
						for (; itr != GameObject::gameObjList.end(); ++itr)
							if (*itr == GameController::selectedGameObj)
								break;
						GameObject::gameObjList.erase(itr);
						delete GameController::selectedGameObj;
						GameController::selectedGameObj = NULL;
						GameController::creatingMode = CreatingMode::Selecting;
					}
					if (ImGui::Button("OK", shotcutButtonSize))
					{
						GameController::selectedGameObj = NULL;
						GameController::creatingMode = CreatingMode::Selecting;
					}
					ImGui::End();
				}
				/*else if (GameController::creatingMode == CreatingMode::Placing) {
					ImGui::Begin("Edit Menu", 0, menuFlags);
					ImGui::SetWindowPos(ImVec2(Common::SCR_WIDTH - shotcutButtonSize.x - 10, 0));
					if (ImGui::Button("Cancel", shotcutButtonSize)) {
						GameController::creatingMode = CreatingMode::Placing;
						GameController::selectedModel = "";
					}
					ImGui::End();
				}*/
			}

			// Render dear imgui into screen
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

	private:
		static bool hideGui;
		static vector<GLuint> modelTextures, frameBuffers;
		static const int buttonWidth1 = 200, buttonHeight1 = 30;
		static const int buttonWidth2 = 100, buttonHeight2 = 17;
		static const int menuWidth = 200;

		static void checkMouseOnGui(float minx = -1.f, float miny = -1.f, float maxx = -1.f, float maxy = -1.f)
		{
			float mix, miy, max, may;
			mix = minx >= 0.f ? minx : ImGui::GetWindowPos().x;
			miy = miny >= 0.f ? miny : ImGui::GetWindowPos().y;
			max = maxx >= 0.f ? maxx : ImGui::GetWindowPos().x + ImGui::GetWindowSize().x;
			may = maxy >= 0.f ? maxy : ImGui::GetWindowPos().y + ImGui::GetWindowSize().y;
			// cout << mix << "," << miy << endl << max << "," << may << endl;
			if (ImGui::IsMouseHoveringRect(ImVec2(mix, miy), ImVec2(max, may), false))
				GameController::isCursorOnGui = true;
			// if (ImGui::IsMouseHoveringRect(ImVec2(0, 0), ImGui::GetWindowSize())) GameController::isCursorOnGui = true;
		}
	};
	bool GUI::hideGui = false;
	vector<GLuint> GUI::modelTextures, GUI::frameBuffers;
}