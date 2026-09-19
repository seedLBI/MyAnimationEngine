#ifndef TIMELINE_AREA_SELECTION_H
#define TIMELINE_AREA_SELECTION_H


#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

enum SelectionArea_Type {
	SELECTION_RIGHT_HALF,
	SELECTION_LEFT_HALF
};


class Timeline_AreaSelection{
public:
	Timeline_AreaSelection() = default;
	~Timeline_AreaSelection() = default;

	void Draw(ImDrawList* draw_list);
	void Update();

	const SelectionArea_Type& GetTypeAreaStart();
	const bool& IsActive();
	bool CheckAABB(const ImRect& rect);
	bool HaveCollision(ImVec2 pos);

	const ImRect& GetAreaSelection();

	void SetStatus(bool flag);
	void SetAreaStart(SelectionArea_Type area);
	void SetStartMousePosition(const ImVec2& start);

private:
	SelectionArea_Type area_start = SELECTION_RIGHT_HALF;
	bool status = false;
	ImVec2 pos_start;

	ImRect AreaSelection;
	void UpdateAreaSelection();
};


#endif