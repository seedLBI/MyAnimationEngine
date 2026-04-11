#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "ThirdParty/ImGui/imgui_impl_glfw.h"
#include "ThirdParty/ImGui/imgui_impl_opengl3.h"

#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"

#include "Utils/File/File.h"
#include "Utils/Text/Text.h"
#include "Utils/ImGui/Utils.ImGui.h"

#include <string>
#include <vector>
#include <iostream>


class FontManager {
private:
	const std::string path_Icons = u8"resources\\fonts\\Font Awesome 6 Free-Solid-900.otf";
	const std::string path_Main  = u8"resources\\fonts\\JetBrainsMonoNL-Regular.ttf";
	const std::string path_Bold  = u8"resources\\fonts\\JetBrainsMonoNL-ExtraBold.ttf";
public:
	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;

	static FontManager& getInstance();

	void SetSize(const int& sizeFont);

	void SetOneStepBigger();
	void SetOneStepSmaller();

	ImFont* GetBoldFont();

	void Update();

private:
	FontManager();
	~FontManager();

	int Current_Scale = 21;
	static const int Min_Scale = 12;
	static const int Max_Scale = 34;

	bool NeedReloadFontSize = true;

	ImFont* MainFont = nullptr;
	ImFont* MainBoldFont = nullptr;
	ImFont* IconsFont = nullptr;

	std::vector<uint8_t> IconsFontData;
	std::vector<uint8_t> MainFontData;
	std::vector<uint8_t> BoldFontData;

	std::vector<uint8_t> LoadFontFromFile(const std::string& Path2File);
};

void Push_BoldFont();
void Pop_BoldFont();



#endif // !FONT_MANAGER_H