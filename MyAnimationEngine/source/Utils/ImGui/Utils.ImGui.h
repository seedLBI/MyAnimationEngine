#ifndef UTILS_IMGUI_H
#define UTILS_IMGUI_H


#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"

#include "Utils/Text/Text.h"

#include <string>
#include <vector>

void RenderTextWrapped(const char* text, bool bIndentToHere);

void TextCenteredOnLine(const char* label, int col, int row, float alignment, bool AndY);

void TextCenteredOnLine(const char* label, int col, int row);


void RenderTextAlignedClipped(const char* text, const char* text_end = NULL, const ImVec2* p_text_size_if_avail = NULL, float align_x = 1.0f);

void HelpMarker(const char* desc);

void DrawTextWithBackground(const char* text, ImVec4 bgColor);

void RenderCharacterInRect(ImFont* font, ImDrawList* draw_list, const ImVec2& pos, const ImVec2& size, const char* text, ImU32 color);

ImVec2 GetCharacterSize(ImFont* font, char character);

bool ButtonCenteredOnLine(const char* label, float alignment);

void ButtonCheckBox(const char* label, const char* help, bool& state);

bool MyButtonBehavior(const ImVec2& bb_min,const ImVec2& bb_max, const char* id, bool& hovered, bool& held);

bool ButtonDelete(ImGuiWindow* imgui_window, ImDrawList* draw_list, const char* nameIndex, const ImU32& ColorText);

void AddDashedLine(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness, float dash_length, float gap_length);

std::string TruncateTextMiddle(const std::string& text, float maxWidth);

#endif