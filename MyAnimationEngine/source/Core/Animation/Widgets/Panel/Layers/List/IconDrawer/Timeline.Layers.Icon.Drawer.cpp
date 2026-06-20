#include "Timeline.Layers.Icon.Drawer.h"


void DrawIcon_Solid(LayerObject_Solid* layer, float height, float time_eval) {
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 CursosPos = ImGui::GetCursorScreenPos();


	float icon_height = height * 0.6f;
	float icon_width = icon_height;


	float offsetY = (height - icon_height) * 0.5f + 1.f;
	//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY + 1.f);


	float padding_x = ImGui::GetStyle().FramePadding.x;
	float padding_y = ImGui::GetStyle().FramePadding.y;

	ImVec2 p_min = CursosPos;
	p_min.y += offsetY;
	ImVec2 p_max = p_min;



	p_max.x += icon_width;
	p_max.y += icon_height;

	ImGui::ItemSize(ImRect(p_min.x, p_min.y, p_max.x, p_max.y));

	auto& color_object = layer->GetProperties().get<TPColor>("color");

	std::array<float, 4> color_array = color_object.kr.evaluate(time_eval);

	ImColor color;
	color.Value.x = color_array[0];
	color.Value.y = color_array[1];
	color.Value.z = color_array[2];
	color.Value.w = color_array[3];

	drawList->AddRectFilled(p_min, p_max, color);
	drawList->AddRect(p_min, p_max, ImColor(1.f, 1.f, 1.f, 1.f), 0.f, 0, 1.f);
}

void DrawIcon(LayerObject* layer, float height, float time_eval) {

	if (typeid(*layer) == typeid(LayerObject_Solid))
		DrawIcon_Solid(static_cast<LayerObject_Solid*>(layer), height, time_eval);

}