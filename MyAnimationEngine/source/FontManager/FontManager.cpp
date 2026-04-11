#include "FontManager.h"


FontManager::FontManager() {
	IconsFontData = LoadFontFromFile(path_Icons);
	MainFontData  = LoadFontFromFile(path_Main);
	BoldFontData  = LoadFontFromFile(path_Bold);
}

FontManager::~FontManager() {

}

FontManager& FontManager::getInstance() {
	static FontManager instance;
	return instance;
}

void FontManager::SetSize(const int& sizeFont) {
	if (sizeFont > Max_Scale) return;
	if (sizeFont < Min_Scale) return;

	NeedReloadFontSize = Current_Scale != sizeFont;
	Current_Scale = sizeFont;
}

void FontManager::SetOneStepBigger() {
	SetSize(Current_Scale + 1);
}
void FontManager::SetOneStepSmaller() {
	SetSize(Current_Scale - 1);
}
ImFont* FontManager::GetBoldFont() {
	return MainBoldFont;
}

void FontManager::Update() {
	if (NeedReloadFontSize) {

		NeedReloadFontSize = false;

		ImGuiIO& io = ImGui::GetIO();
		if (io.Fonts->Locked) {
			std::cout << "[FontManager]: cannot resize font(((" << std::endl;
			return;
		}

		ImFontConfig font_cfg;
		font_cfg.FontDataOwnedByAtlas = false;
		font_cfg.MergeMode = false;


		io.Fonts->Clear();
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

		MainFont = io.Fonts->AddFontFromMemoryTTF(
			MainFontData.data(),
			static_cast<int>(MainFontData.size()),
			static_cast<float>(Current_Scale),
			&font_cfg,
			ranges
		);

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

		font_cfg.MergeMode = false;

		MainBoldFont = io.Fonts->AddFontFromMemoryTTF(BoldFontData.data(), static_cast<int>(BoldFontData.size()), static_cast<float>(Current_Scale), &font_cfg, ranges);

		io.Fonts->Build();


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
	}
}

std::vector<uint8_t> FontManager::LoadFontFromFile(const std::string& Path2File) {
	return ReadFileToByteArray(Path2File);
}

void Push_BoldFont() {
	ImGui::PushFont(FontManager::getInstance().GetBoldFont());
}
void Pop_BoldFont() {
	ImGui::PopFont();
}

