#ifndef MAINMENUBAR_H
#define MAINMENUBAR_H

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"

#include "Utils/OpenGL/Window/Manager/WindowManager.h"
#include "Utils/OpenGL/Texture/Texture.h"
#include "Utils/File/File.h"
#include "Utils/Text/Encoding/Wstring/Wstring.h"




#include <Windows.h>
#include <string>

class MainMenuBar {
private:
	std::string Path2IconLogo = u8"resources\\LOGO_transparent.png";
	ImVec4 colorLogo = ImVec4(1.f, 1.f, 1.f, 1.f);
public:
	MainMenuBar(WindowManager* windowManager);
	~MainMenuBar();

	int GetHeightMenu();

	void Draw();
	bool IsPointOverTitleButton(const POINT& pt) const;
private:
	WindowManager* windowManager = nullptr;

	int heightMenu = 0;

	void DrawLogo();
	void DrawWindowButtons();

	void Draw_MainMenu();
	void Draw_SecondaryMenu();
	void Draw_DownMenu();

	void LoadIcon();
	Texture icon_logo;

	ImVec2 menuBarPos;
	ImVec2 menuBarSize;


	void PushSizeButtonIntoList();

public:
	GLFWwindow* window;
	bool isDragging = false;
	POINT  dragStartPoint;
	int    windowPosStartX;
	int    windowPosStartY;
	bool wasMaximizedOnDrag = false;

	ImVector<ImRect> titleButtonRects;
};


#endif 