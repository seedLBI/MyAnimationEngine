#ifndef TIMELINE_LAYERS_LIST_TIMELINE_H
#define TIMELINE_LAYERS_LIST_TIMELINE_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"
#include "Core/Animation/Widgets/Panel/TimeCursor/Timeline.TimeCursor.h"


#include <unordered_map>

class Timeline_Layers_Viewer
{
public:
	Timeline_Layers_Viewer(
		TimelineContext* ctx,
		Timeline_TimeCursor* timeCursor
	);

	~Timeline_Layers_Viewer();

	void Draw();
	void Update();

private:
	TimelineContext* ctx;
	Timeline_TimeCursor* timeCursor;

	enum class ObjectStatus : uint8_t {
		NOTHING = 0,
		LEFT_RESIZE = 1,
		RIGHT_RESIZE = 2,
		BODY_MOVE = 3
	};

	uint64_t index_drag = UINT64_MAX;
	ObjectStatus pressed_status;
	ObjectStatus draging_status;


	struct DragKeyframeInfo {
		bool drag_started = false;
		float startedDrag_global_x;
		std::unordered_map<std::string, float> original_times;
	} DragKeyframe;


	long double drag_start_mouse_global;

	void DrawLayerBox(const ImRect& rect, const LayerInfo& info, const bool& hovered, bool isSelected);

	const float width_hover = 5.f;
	bool IsHovered_LeftResize(const ImRect& rect);
	bool IsHovered_RightResize(const ImRect& rect);
	bool IsHovered_BodyMove(const ImRect& rect);

	void DrawRomb(const ImVec2& pos_center, float size, ImColor color, bool isSelected = false, ImColor colorSelected = ImColor(0.f,0.f,0.f,0.f));


};





#endif