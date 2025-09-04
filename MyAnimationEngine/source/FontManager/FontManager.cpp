#include "FontManager.h"



FontManager::FontManager() {
	Load_MainFont_FromMemory();
	Load_IconsFont_FromMemory();

	//LoadIconsFont_fromFile();
	//LoadDefaultMainFont_fromFile();

	SetFont("JetBrainsMonoNL");

}

FontManager::~FontManager() {

#ifdef _DEBUG
	std::cout << "ENTER -> FontManager::~FontManager()\n";
#endif


	//MainFont->~ImFont();
	//IconsFont->~ImFont();

#ifdef _DEBUG
	std::cout << "LEAVE -> FontManager::~FontManager()\n";
#endif
}

void FontManager::SetFont(const std::string& NameFont) {

	int Index_Font = -1;
	for (int i = 0; i < List_Fonts.size(); i++)
	{
		if (List_Fonts[i].Name == NameFont) {
			Index_Font = i;
		}
	}
	
	if (Index_Font == -1){

		Name_Font_Selected = "JetBrainsMonoNL";
		//MainFontData = ReadFileToByteArray("fonts/JetBrainsMonoNL-Regular.ttf");

	}
	else{
		Name_Font_Selected = NameFont;

		MainFontData = List_Fonts[Index_Font].Data;

#ifdef _DEBUG
		std::cout << "[FontManager]: set data: " << MainFontData.size() << std::endl;
#endif
	}

	
	NeedReloadFontSize = true;
}

void FontManager::SetSize(const int& sizeFont) {
	if (sizeFont < Min_Scale)
		Current_Scale = Min_Scale;
	else if(sizeFont > Max_Scale)
		Current_Scale = Max_Scale;
	else
		Current_Scale = sizeFont;

	NeedReloadFontSize = true;
}


void FontManager::SetOneStepBigger() {
	SetSize(Current_Scale + 1);
}
void FontManager::SetOneStepSmaller() {
	SetSize(Current_Scale - 1);
}



void FontManager::LoadFontFromFile(std::string Path2File) {
	std::string name = Path2File.substr(Path2File.find_last_of('\\') + 1);
	name.erase(name.begin() + name.find_last_of("."), name.end());

	FontInfo new_font;
	new_font.Path = Path2File;
	new_font.Name = name;
	new_font.Data = ReadFileToByteArray(Path2File);

	if (new_font.Data.empty())
		return;

#ifdef _DEBUG
	std::cout << "[FontManager]: Loading new font:\n";
	std::cout << "Path: [" << new_font.Path << "]" << std::endl;
	std::cout << "Name: [" << new_font.Name << "]" << std::endl;
	std::cout << "size: [" << new_font.Data.size() << "]" << std::endl;
#endif

	List_Fonts.push_back(new_font);
}




void FontManager::ReloadFont() {
	if (NeedReloadFontSize) {
		NeedReloadFontSize = false;

		ImGuiIO& io = ImGui::GetIO();
		if (io.Fonts->Locked)
		{
			std::cout << "[FontManager]: cannot resize font(((" << std::endl;
			return;
		}

		ImFontConfig font_cfg;
		font_cfg.FontDataOwnedByAtlas = false;
		font_cfg.MergeMode = false;


		io.Fonts->Clear();  // Очистить текущие шрифты
		io.Fonts->Fonts.clear();





		if (MainFont != nullptr) {
			MainFont->ClearOutputData();
		}


		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
			0x2000, 0x206f,
			0x20ac, 0x20ac,
			0x2112, 0x2122,
			0x2DE0, 0x2DFF, // Cyrillic Extended-A
			0xA640, 0xA69F, // Cyrillic Extended-B
			0,
		};




		MainFont = io.Fonts->AddFontFromMemoryTTF(MainFontData.data(), static_cast<int>(MainFontData.size()), static_cast<float>(Current_Scale), &font_cfg, ranges);

		//MainFont = io.Fonts->AddFontFromFileTTF("fonts/JetBrainsMonoNL-Regular.ttf", Current_Scale, &font_cfg, ranges);
		
		float iconFontSize = Current_Scale * 2.0f / 3.0f;
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphMinAdvanceX = iconFontSize;
		icons_config.FontDataOwnedByAtlas = false;


		if (IconsFont != nullptr) {
			IconsFont->ClearOutputData();
		}

		IconsFont = io.Fonts->AddFontFromMemoryTTF(IconsFontData.data(), static_cast<int>(IconsFontData.size()), iconFontSize, &icons_config, icons_ranges);
		//IconsFont = io.Fonts->AddFontFromFileTTF("fonts/Font Awesome 6 Free-Solid-900.otf", iconFontSize, &icons_config, icons_ranges);


		font_cfg.MergeMode = false;

		// new font
		//MainBoldFont = io.Fonts->AddFontFromMemoryTTF(MainBoldFontData.data(), static_cast<int>(MainBoldFontData.size()), static_cast<float>(Current_Scale), &font_cfg, ranges);




		io.Fonts->Build();  // Перестроить атлас шрифтов


		ImGui_ImplOpenGL3_DestroyDeviceObjects();
		if (ImGui_ImplOpenGL3_CreateDeviceObjects() == false)
		{
			std::cout << "[FontManager]: Error CreateDeviceObjects" << std::endl;
		}
		ImGui_ImplOpenGL3_DestroyFontsTexture();
		if (ImGui_ImplOpenGL3_CreateFontsTexture() == false) {
			std::cout << "[FontManager]: Error CreateFontsTexture" << std::endl;
		}



		ImGui::SetCurrentFont(MainFont);

		//cout << "font seted" << endl;


		//ImGui::LoadIniSettingsFromMemory(INIdata, size);
	}
}

std::vector<std::string> FontManager::GetListSizes() {
	static std::vector<std::string> list;

	if (list.empty()) {
		for (int i = Min_Scale; i <= Max_Scale; i++){
			list.push_back(std::to_string(i));
		}
	}

	return list;
}

std::vector<FontInfo> FontManager::GetList_FontInfo() {
	return List_Fonts;
}


/*
void FontManager::DrawSetting() {

	ISettingObject::DrawBegin();

	if (ImGui::BeginCombo(u8"Шрифт", Name_Font_Selected.c_str(), ImGuiComboFlags_WidthFitPreview)) {

		for (int i = 0; i < List_Fonts.size(); i++)
		{
			bool selected = List_Fonts[i].Name == Name_Font_Selected;

			if (ImGui::Selectable(List_Fonts[i].Name.c_str(), &selected)) {
				SetFont(List_Fonts[i].Name);
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}


		if (ButtonCenteredOnLine(u8"Добавить свой шрифт", 0.5f)) {
			nfdchar_t* outPath = NULL;
			nfdresult_t result = NFD_OpenDialog("ttf,otf", NULL, &outPath);
			if (result == NFD_OKAY) {
				LoadFontFromFile(outPath);
			}
			else if (result == NFD_CANCEL) {
#ifdef WITH_DEBUG_OUTPUT
				std::cout << "User pressed cancel.\n";
#endif
			}
			else {
				printf("Error: %s\n", NFD_GetError());
			}
		}

		ImGui::EndCombo();
	}
	ImGui::SameLine();
	HelpMarker(u8"Выбор типа шрифта");



	if (ImGui::BeginCombo(u8"Размер", std::to_string(Current_Scale).c_str(), ImGuiComboFlags_WidthFitPreview)) {

		for (int i = 0; i < GetListSizes().size(); i++) {

			bool selected = stoi(GetListSizes()[i]) == Current_Scale;

			if (ImGui::Selectable(GetListSizes()[i].c_str(), &selected)) {
				Current_Scale = stoi(GetListSizes()[i]);
				NeedReloadFontSize = true;
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
	ImGui::SameLine();
	HelpMarker(u8"Единица измерения - пиксель.");



}

std::string FontManager::SaveSetting() {
	std::string result = save_MakeBegin(2 + List_Fonts.size() - 1);

	for (int i = 0; i < List_Fonts.size(); i++) {
		if (!List_Fonts[i].Path.empty())
			result += save_MakeItem("Path", List_Fonts[i].Path);
	}

	result += save_MakeItem("Scale", std::to_string(Current_Scale));
	result += save_MakeItem("Font selected", Name_Font_Selected);


	return result;
}

void FontManager::LoadSetting(const std::string& Data) {
	auto info = load_TokenizeData(Data);

	for (SettingLoadData data : info) {
		if (data.NameVar == "Path")
			LoadFontFromFile(data.ValueVar);
		else if (data.NameVar == "Font selected")
			SetFont(data.ValueVar);
		else if (data.NameVar == "Scale")
			SetSize(stoi(data.ValueVar));
		else
			std::cout << "FontManager::LoadSetting -> Unknown name argument" << data.NameVar << std::endl;
	}
}
*/


void FontManager::LoadDefaultMainFont_fromFile() {


	if (isFileExist(L"resources/fonts/JetBrainsMonoNL-Regular.ttf") == false)
	{
		MessageBoxW(0, L"А где шрифты? (JetBrainsMonoNL-Regular.ttf)", L"Ошибка загрузки шрифтов", MB_ICONERROR | MB_OK);
		exit(0);
	}
	
	FontInfo new_font;
	new_font.Path = "resources/fonts/JetBrainsMonoNL-Regular.ttf";
	new_font.Name = u8"JetBrainsMonoNL";
	new_font.Data = ReadFileToByteArray("resources/fonts/JetBrainsMonoNL-Regular.ttf");

	MainFontData = new_font.Data;

	List_Fonts.push_back(new_font);


}

void FontManager::LoadIconsFont_fromFile() {
	if (isFileExist(L"resources/fonts/Font Awesome 6 Free-Solid-900.otf") == false)
	{
		MessageBoxW(0, L"А где шрифты? (Font Awesome 6 Free-Solid-900.otf)", L"Ошибка загрузки шрифтов", MB_ICONERROR | MB_OK);
		exit(0);
	}
	IconsFontData = ReadFileToByteArray("resources/fonts/Font Awesome 6 Free-Solid-900.otf");
}


void FontManager::Load_MainFont_FromMemory() {
	FontInfo new_font;
	new_font.Path = "resources/fonts/JetBrainsMonoNL-Regular.ttf";
	new_font.Name = u8"JetBrainsMonoNL";
	new_font.Data = Default_MainFont;

	MainFontData = new_font.Data;

	List_Fonts.push_back(new_font);
}
void FontManager::Load_IconsFont_FromMemory() {
	IconsFontData = fa_solid_900;
}