#ifndef APPLICATION_H
#define APPLICATION_H

#include <Windows.h>
#include <stb_image.h>
#include <dwmapi.h>
#include <windowsx.h>
#include <string>
#include <unordered_map>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dwmapi.lib")

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_glfw.h"
#include "ThirdParty/ImGui/imgui_impl_opengl3.h"


#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "WindowManager/WindowManager.h"
#include "CursorManager/CursorManger.h"

#include "Utils/Timer/Timer.Framerate.h"
#include "Utils/OpenGL/Enums/OpenGL.Enums.h"
#include "Utils/File/File.h"

#include "MainMenuBar/MainMenuBar.h"
#include "FontManager/FontManager.h"


#include "Core/Animation/Widgets/Panel/Timeline.Panel.h"
#include "Core/Animation/Widgets/Player/Timeline.Player.h"
#include "Core/Animation/Timeline/Controller/Timeline.Controller.h"

#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Layer/Factory/Layer.Factory.h"

#include "Core/DebugOverlay/DebugOverlay.h"


static WNDPROC orig_wndproc = nullptr;
static std::unordered_map<HWND, GLFWwindow*> g_hwnd_to_window;




class Application {
private:
	const std::string Title = u8"My Animation Engine";
	const std::string Path2IconApplication = u8"resources\\logo\\small.png";
public:
	Application();
	~Application();

	void Run();
	MainMenuBar* GetPTR_MainMenuBar();
private:

	FPS_Timer* fps_limiter = nullptr;
	MainMenuBar* mainMenuBar = nullptr;

	LayersDataProvider* layersData = nullptr;
	LayerFactory* layersFactory = nullptr;
	TimelineProviderData* timelineData = nullptr;
	TimelineController* timelineController = nullptr;
	TimelinePanel* timelinePanel = nullptr;

	Timeline_Player* playerComposition = nullptr;

	void Draw();

	void BeginDraw();
	void EndDraw();

	void Update();

	void Init_OpenGL();
	void Init_BorderWindow();
	void Init_IconApplication();
	void Init_ImGui();

	void Init_DockLayout(ImGuiViewport* viewport, const ImGuiID& dockspace_id);
};



#endif