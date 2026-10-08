#ifndef TIMELINE_PREVIEW_H
#define TIMELINE_PREVIEW_H

#include "Core/Animation/Renderer/Renderer.h"
#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"
#include "Utils/ImGui/Utils.ImGui.h"

#include "Utils/Timer/Timer.Framerate.h"
#include "Utils/TransformScaled/Transform.Scaled.h"

#include <Utils/OpenGL/Texture/Texture.h>

#include "Core/DebugOverlay/DebugOverlay.h"
#include <CursorManager/CursorManger.h>



class Timeline_Preview
{
public:
	Timeline_Preview(FPS_Timer* deltaTime, RendererAnimationScene* renderer);
	~Timeline_Preview();

	void Draw();
	void Update();

private:
	FPS_Timer* deltaTime;
	RendererAnimationScene* renderer;

	TransformScaled horizontal, vertical;

	bool firstCentered = false;
	bool mouseIsDraging = false;

	Texture* temp;

	float height_bottom_menu;

	const float padding = 20.f;

	struct PreviewContext {
		ImVec2 mouse_global_relative;
		ImVec2 mouse_screen_relative;

		bool isHoveredTextureFramebuffer;
		bool isHoveredWidgetRegion;
	} pc;

	void DrawBottomMenu(const ImVec2& p_min, const ImVec2& p_max);

	void DrawBottomMenu_Scale(float& pos_cursor_draw, const ImVec2& p_min, const ImVec2& p_max);
	void DrawBottomMenu_TextureSetting(float& pos_cursor_draw, const ImVec2& p_min, const ImVec2& p_max);
	void DrawBottomMenu_Resolution(float& pos_cursor_draw, const ImVec2& p_min, const ImVec2& p_max);
	void DrawBottomMenu_ColorPickerInfo(float& pos_cursor_draw, const ImVec2& p_min, const ImVec2& p_max);

};




#endif