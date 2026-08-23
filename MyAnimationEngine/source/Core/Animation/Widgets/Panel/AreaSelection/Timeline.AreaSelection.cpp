#include "Timeline.AreaSelection.h"

#include <algorithm>


const ImRect& Timeline_AreaSelection::GetAreaSelection() {
	return AreaSelection;
}

void Timeline_AreaSelection::UpdateAreaSelection() {
	if (!status) return;

	ImVec2 a = this->pos_start;
	ImVec2 b = ImGui::GetMousePos();

	if (a.x > b.x) {
		if (b.y < a.y)
			std::swap(a, b);
		else {
			ImVec2 a_new = { b.x,a.y };
			ImVec2 b_new = { a.x,b.y };
			a = a_new;
			b = b_new;
		}
	}
	else if (b.y < a.y) {
		ImVec2 a_new = { a.x,b.y };
		ImVec2 b_new = { b.x,a.y };
		a = a_new;
		b = b_new;
	}

	AreaSelection = { a,b };
}
void Timeline_AreaSelection::Update() {
	UpdateAreaSelection();
}


void Timeline_AreaSelection::Draw(ImDrawList* draw_list) {
	if (!status) return;

	ImRect rect_area = GetAreaSelection();

	
	draw_list->AddRect(rect_area.Min, rect_area.Max, ImColor(0.7f, 0.7f, 0.7f, 0.7f), 2.f, 0, 3.f);
	draw_list->AddRectFilled(rect_area.Min, rect_area.Max, ImColor(0.7f, 0.7f, 0.7f, 0.3f), 3.f);
}

const SelectionArea_Type& Timeline_AreaSelection::GetTypeAreaStart() {
	return this->area_start;
}
const bool& Timeline_AreaSelection::IsActive() {
	return this->status;
}
bool Timeline_AreaSelection::CheckAABB(const ImRect& rect) {
	bool overlap_x = AreaSelection.Min.x <= rect.Max.x && AreaSelection.Max.x >= rect.Min.x;
	bool overlap_y = AreaSelection.Min.y <= rect.Max.y && AreaSelection.Max.y >= rect.Min.y;

	return overlap_x && overlap_y && status;
}
bool Timeline_AreaSelection::HaveCollision(ImVec2 pos) {
	return AreaSelection.Min.x <= pos.x && AreaSelection.Max.x >= pos.x &&
		AreaSelection.Min.y <= pos.y && AreaSelection.Max.y >= pos.y && status;
}


void Timeline_AreaSelection::SetAreaStart(SelectionArea_Type area) {
	this->area_start = area;
}

void Timeline_AreaSelection::SetStatus(bool flag) {
	this->status = flag;
}

void Timeline_AreaSelection::SetStartMousePosition(const ImVec2& start) {
	this->pos_start = start;
}