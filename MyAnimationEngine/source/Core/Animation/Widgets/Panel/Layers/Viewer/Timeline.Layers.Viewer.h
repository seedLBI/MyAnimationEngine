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

		struct DragData {
			SelectedKeyframe keyframe;
			float original_time;
		};

		std::vector<DragData> original_times;
	} DragKeyframe;


	struct UIInfo {
		ImDrawList* draw_list;
		ImVec2 p_min, p_max;
		bool   isHoveredArea;
		ImVec2 relative_mouse_pos;
		float  relative_mouse_global_pos_x;
	} ui_info;


	long double drag_start_mouse_global;

	
	void DrawBackground();
	void DrawRange();

	void DrawLayerBox(const ImRect& rect, const LayerInfo& info, const bool& hovered, bool isSelected);
	void DrawLayerOverlays();

	void ProcessLayers();
	void ProcessLayerProperties();

	void ProcessLayerInput(const ImRect& rect, const LayerInfo& layerInfo, uint64_t index, bool& outHovered, bool& outActive, ObjectStatus& outStatus);
	void ProcessKeyframeInput(const SelectedKeyframe& current, const ImVec2& min_hitbox, const ImVec2& max_hitbox);


	const float width_hover = 5.f;
	bool IsHovered_LeftResize(const ImRect& rect);
	bool IsHovered_RightResize(const ImRect& rect);
	bool IsHovered_BodyMove(const ImRect& rect);
	ObjectStatus GetHoveredStatus(const ImRect& rect);

	void HandleGlobalInput();
	void HandleKeyframeDragging();
	void HandleLayerDragging();

	float SnapToGrid(float value) const;

};

#endif