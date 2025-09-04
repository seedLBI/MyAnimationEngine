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

#include "Utils/OpenGL/Window/Manager/WindowManager.h"
#include "Utils/Timer/Timer.Framerate.h"

#include "Utils/File/File.h"

#include "MainMenuBar/MainMenuBar.h"
#include "FontManager/FontManager.h"


static WNDPROC orig_wndproc = nullptr;
static std::unordered_map<HWND, GLFWwindow*> g_hwnd_to_window;



enum BufferBit {
	COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT,
	DEPTH_BUFFER_BIT = GL_DEPTH_BUFFER_BIT,
	STENCIL_BUFFER_BIT = GL_STENCIL_BUFFER_BIT,
};

class Application {
private:
	const std::string Title = u8"Template";
	const std::string Path2IconApplication = u8"resources\\LOGO_small.png";
public:
	Application();
	~Application();

	void Run();
	MainMenuBar* GetPTR_MainMenuBar();
private:

	FPS_Timer* fps_limiter = nullptr;
	WindowManager* windowManager = nullptr;
	MainMenuBar* mainMenuBar = nullptr;
	FontManager* fontManager = nullptr;


	void Draw();

	void BeginDraw();
	void EndDraw();

	void Update();

	void Init_OpenGL();
	void Init_BorderWindow();
	void Init_IconApplication();
	void Init_ImGui();
};



#endif