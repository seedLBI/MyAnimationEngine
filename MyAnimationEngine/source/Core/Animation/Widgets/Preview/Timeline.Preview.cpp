#include "Timeline.Preview.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"

#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"

#include <ThirdParty/LivePT/LivePT.h>

#include "WindowManager/WindowManager.h"

Timeline_Preview::Timeline_Preview(FPS_Timer* deltaTime, RendererAnimationScene* renderer) : deltaTime(deltaTime), renderer(renderer) {

	horizontal.setBoundsScale(0.3f, 100.f);
	vertical.setBoundsScale(0.3f, 100.f);

	TextureSetting ts;
	ts.Min = TextureFilter::NEAREST;
	ts.Max = TextureFilter::NEAREST;
	ts.WrapX = TextureWrap::CLAMP_TO_BORDER;
	ts.WrapY = TextureWrap::CLAMP_TO_BORDER;

	temp = new Texture(ts, "resources\\images\\temp.jpg");

}
Timeline_Preview::~Timeline_Preview() {

}

void Timeline_Preview::Draw() {

	horizontal.update(deltaTime->GetDeltaTime() * 20.f);
	vertical.update(deltaTime->GetDeltaTime() * 20.f);

	if (firstCentered == false)
	{
		horizontal.centerizeForce(0.f);
		vertical.centerizeForce(0.f);

		if (horizontal.getAvailableScreenValue() > 100.f)
			firstCentered = true;
	}


	ImGui::PushStyleColor(ImGuiCol_WindowBg, COLOR_BG_LIST.Value);
	ImGui::PushStyleColor(ImGuiCol_Separator, COLOR_SEPARATOR.Value);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGui::Begin(u8"Предпросмотр", nullptr, ImGuiWindowFlags_NoScrollWithMouse);

	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();


	float height_bottom_menu = eval(20.f);

	region_max.y -= height_bottom_menu;

	horizontal.updateAvailableScreenValue(region_max.x);
	vertical.updateAvailableScreenValue(region_max.y);

	ImVec2 mouse_Screen = ImGui::GetMousePos();
	ImVec2 relative_Mouse_Screen = { mouse_Screen.x - cursor_pos.x, mouse_Screen.y - cursor_pos.y };
	ImVec2 mouse_Global = { horizontal.getPosGlobal_real(relative_Mouse_Screen.x), vertical.getPosGlobal_real(relative_Mouse_Screen.y) };



	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	bool isHovered = ImGui::IsMouseHoveringRect(p_min, p_max);



	float aspect_ratio = 16.f / 9.f;


	ImVec2 LU = {
		cursor_pos.x + horizontal.getPosScreen_lerped(-1.f * aspect_ratio),
		cursor_pos.y + vertical.getPosScreen_lerped(1.f)
	};

	ImVec2 RU = {
		cursor_pos.x + horizontal.getPosScreen_lerped(1.f * aspect_ratio),
		cursor_pos.y + vertical.getPosScreen_lerped(1.f)
	};

	ImVec2 LD = {
		cursor_pos.x + horizontal.getPosScreen_lerped(-1.f * aspect_ratio),
		cursor_pos.y + vertical.getPosScreen_lerped(-1.f)
	};

	ImVec2 RD = {
		cursor_pos.x + horizontal.getPosScreen_lerped(1.f * aspect_ratio),
		cursor_pos.y + vertical.getPosScreen_lerped(-1.f)
	};

	drawlist->AddCircle(LU, 2.f, ImColor(1.f, 1.f, 1.f, 1.f));
	drawlist->AddCircle(RU, 2.f, ImColor(1.f, 1.f, 1.f, 1.f));
	drawlist->AddCircle(LD, 2.f, ImColor(1.f, 1.f, 1.f, 1.f));
	drawlist->AddCircle(RD, 2.f, ImColor(1.f, 1.f, 1.f, 1.f));


	static bool camera_mode = false;

	if (ImGui::IsKeyPressed(ImGuiKey_RightShift)) {
		camera_mode = !camera_mode;

		GLFWwindow* window = WindowManager::getInstance().GetHandle();

		if (camera_mode) {
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}

	}
	
	static glm::vec2 prev_m = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };

	if (camera_mode) {
		float scrol_zoom = ImGui::GetIO().MouseWheel;


		glm::vec2 curr_m = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };

		glm::vec2 delta = prev_m - curr_m;
		renderer->getCamera().Update(deltaTime->GetDeltaTime(), { delta.x, delta.y }, scrol_zoom);
		prev_m = curr_m;
	}
	else {
		renderer->getCamera().Update(deltaTime->GetDeltaTime(), { 0.f,0.f }, 0.f);
		prev_m = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
	}

	if (isHovered && !camera_mode) {
		float scrol_zoom = ImGui::GetIO().MouseWheel;

		if (scrol_zoom > 0.f) {
			horizontal.zoomToPointByValue(mouse_Global.x, 1.2f, false);
			vertical.zoomToPointByValue(mouse_Global.y, 1.2f, false);
		}
		else if (scrol_zoom < 0.f) {
			horizontal.zoomToPointByValue(mouse_Global.x, 0.8f, false);
			vertical.zoomToPointByValue(mouse_Global.y, 0.8f, false);
		}
	}
	


	if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		mouseIsDraging = true;
	}




	if (mouseIsDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
		horizontal.setOffsetDelta(delta.x);
		vertical.setOffsetDelta(delta.y);
	}
	else {
		mouseIsDraging = false;
		horizontal.applyOffsetDelta();
		vertical.applyOffsetDelta();
	}

	std::string str_mouse_global_x = std::to_string(mouse_Global.x);
	std::string str_mouse_global_y= std::to_string(mouse_Global.y);

	std::string str_left_range = std::to_string(horizontal.getMinBoundView_lerped());
	std::string str_right_range = std::to_string(horizontal.getMaxBoundView_lerped());
	DebugOverlay::AddText(
		{
			{u8"Видимые границы X", ImColor(1.f, 1.f, 1.f, 1.f)},
			{str_left_range, ImColor(0.f, 1.f, 0.f, 1.f)},
			{str_right_range, ImColor(1.f, 0.f, 0.f, 1.f)}
		}
	);

	DebugOverlay::AddText(
		{
			{u8"Mouse global", ImColor(1.f, 1.f, 1.f, 1.f)},
			{str_mouse_global_x, ImColor(0.f, 1.f, 0.f, 1.f)},
			{str_mouse_global_y, ImColor(1.f, 0.f, 0.f, 1.f)}
		}
	);

	drawlist->AddImage(renderer->getTexture().GetID(), LU, RD);


	p_min.y = p_max.y;
	p_max.y += height_bottom_menu;

	ImVec2 line_begin = p_min;
	ImVec2 line_end = { p_max.x, p_min.y };


	drawlist->AddRectFilled(p_min, p_max, COLOR_BG_LIST);
	drawlist->AddLine(line_begin, line_end, ImColor(0.f, 0.f, 0.f, 1.f), 1.f);


	/*
	Texture& sceneTex = renderer->getTexture();

	if (isHovered)
	{
		const float qw = RD.x - LU.x;
		const float qh = RD.y - LU.y;

		if (qw > 1e-6f && qh > 1e-6f)
		{
			const float u = (mouse_Screen.x - LU.x) / qw;
			const float v = (mouse_Screen.y - LU.y) / qh;

			if (u >= 0.f && u < 1.f && v >= 0.f && v < 1.f)
			{
				const int tx = int(u * float(sceneTex.GetWidth()));
				const int ty = int(v * float(sceneTex.GetHeight()));

				pixelPicker.Pick(renderer->getFramebuffer(), COLOR_ATTACHMENT0, tx, ty);
			}
		}
	}

	pixelPicker.Fetch();

	char buf[96];
	snprintf(buf, sizeof(buf), "Pixel: R %3d  G %3d  B %3d  A %3d",
		pixelPicker.Data()[0], pixelPicker.Data()[1],
		pixelPicker.Data()[2], pixelPicker.Data()[3]);
	DebugOverlay::AddText({ {buf, ImColor(1.f, 1.f, 0.f, 1.f)} });
	

	float size_rect_color_picker = height_bottom_menu / 2.2f;

	float padding = 20.f;

	ImVec2 min_color_rect = { 
		p_min.x + padding,
		p_min.y + height_bottom_menu / 2.f - size_rect_color_picker / 2.f 
	};

	ImVec2 max_color_rect = { 
		p_min.x + padding + size_rect_color_picker / 2.f,
		p_min.y + height_bottom_menu / 2.f + size_rect_color_picker / 2.f 
	};

	
	drawlist->AddRectFilled(min_color_rect, max_color_rect, ImColor(pixelPicker.Data()[0], pixelPicker.Data()[1], pixelPicker.Data()[2]));
	*/


	DrawBorderForFocusedWindow(COLOR_WINDOW_BORDER_FOCUS);

	ImGui::End();

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);


}
void Timeline_Preview::Update() {

}