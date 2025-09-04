#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include "Fonts/IconsFont.h"
#include "Fonts/MainFont.h"

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


struct FontInfo {
	std::string Name;
	std::string Path;
	std::vector<uint8_t> Data;
};

class FontManager {

public:
	FontManager();
	~FontManager();

	void SetFont(const std::string& NameFont);
	void SetSize(const int& sizeFont);

	void SetOneStepBigger();
	void SetOneStepSmaller();

	std::vector<FontInfo> GetList_FontInfo();
	std::vector<std::string> GetListSizes();

	/*
	void DrawSetting() override;
	std::string SaveSetting() override;
	void LoadSetting(const std::string& Data) override;
	*/

	void ReloadFont();


private:
	int Current_Scale = 21;
	static const int Min_Scale = 12;
	static const int Max_Scale = 34;

	std::vector<FontInfo> List_Fonts;


	bool NeedReloadFontSize = false;
	std::string Name_Font_Selected = "";

	uint8_t* ptr_MainFontData;
	uint8_t* ptr_IconsFontData;

	ImFont* MainFont = nullptr;
	ImFont* MainBoldFont = nullptr;
	ImFont* IconsFont = nullptr;

	std::vector<uint8_t> IconsFontData;
	std::vector<uint8_t> MainFontData;


	void LoadFontFromFile(std::string Path2File);

	void LoadDefaultMainFont_fromFile();
	void LoadIconsFont_fromFile();

	void Load_MainFont_FromMemory();
	void Load_IconsFont_FromMemory();

};





#endif // !FONT_MANAGER_H