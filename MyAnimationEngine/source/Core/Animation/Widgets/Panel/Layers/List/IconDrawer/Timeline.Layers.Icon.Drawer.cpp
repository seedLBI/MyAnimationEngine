#include "Timeline.Layers.Icon.Drawer.h"


void DrawIcon_Solid(LayerObject_Solid* layer, float height) {
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 CursosPos = ImGui::GetCursorScreenPos();


	float icon_height = height * 0.6f;
	float icon_width = icon_height;


	float offsetY = (height - icon_height) * 0.5f;
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

	ImColor color;
	color.Value.x = color_object.kr.evaluate(0.f)[0];
	color.Value.y = color_object.kr.evaluate(0.f)[1];
	color.Value.z = color_object.kr.evaluate(0.f)[2];
	color.Value.w = color_object.kr.evaluate(0.f)[3];

	drawList->AddRectFilled(p_min, p_max, color);
	drawList->AddRect(p_min, p_max, ImColor(1.f, 1.f, 1.f, 1.f), 0.f, 0, 1.f);
}

void DrawIcon(LayerObject* layer, float height) {

	if (typeid(*layer) == typeid(LayerObject_Solid))
		DrawIcon_Solid(static_cast<LayerObject_Solid*>(layer), height);

}