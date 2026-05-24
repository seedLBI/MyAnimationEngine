#include "Layer.Object.Solid.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

LayerObject_Solid::~LayerObject_Solid()
{

}

void LayerObject_Solid::Draw()
{
	quad.Render();
}

void LayerObject_Solid::Update()
{
	quad.Update();
}

void LayerObject_Solid::DrawHitBoxViewport()
{
	
}

bool LayerObject_Solid::isHit() {
	return false;
}

void LayerObject_Solid::LayerList_DrawIconType() {
	auto drawList = ImGui::GetWindowDrawList();

	ImVec2 CursosPos = ImGui::GetCursorScreenPos();

	float icon_height = ImGui::GetTextLineHeight();
	float icon_width = icon_height;

	float padding_x = ImGui::GetStyle().FramePadding.x;
	float padding_y = ImGui::GetStyle().FramePadding.y;

	ImVec2 p_min = CursosPos;
	ImVec2 p_max = p_min;
	p_max.x += icon_width;
	p_max.y += icon_height;




	ImGui::ItemSize(ImRect(p_min.x, p_min.y, p_max.x, p_max.y));

	ImColor color;
	color.Value.x = this->color.color.x;
	color.Value.y = this->color.color.y;
	color.Value.z = this->color.color.z;
	color.Value.w = this->color.color.w;

	drawList->AddRectFilled(p_min, p_max, color);
	drawList->AddRect(p_min, p_max, ImColor(1.f,1.f,1.f,1.f),0.f,0,2.f);


}


LayerObject_Solid::LayerObject_Solid(const LayerInfo& info) : LayerObject(info)
{

}
