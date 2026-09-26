#ifndef TIMELINE_LAYERS_LIST_TIMELINE_H
#define TIMELINE_LAYERS_LIST_TIMELINE_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"
#include "Core/Animation/Widgets/Panel/Layers/ContextMenuManager/Layers.ContextMenu.Manager.h"
#include "Core/Animation/Widgets/Panel/Layers/Keyframe/DragHandler/Keyframe.DragHandler.h"
#include "Core/Animation/Widgets/Panel/Layers/Keyframe/Renderer/Keyframe.Renderer.h"
#include "Core/Animation/Widgets/Panel/TimeCursor/Timeline.TimeCursor.h"


#include <unordered_map>

class Timeline_Layers_Viewer
{
public:
	Timeline_Layers_Viewer(
		TimelineContext* ctx,
		KeyframeDragHandler* kf_DragHandler,
		Timeline_TimeCursor* timeCursor,
		LayersContextMenu_Manager* layers_ctx_menu
	);

	~Timeline_Layers_Viewer();

	void Draw();

private:
	TimelineContext* ctx;
	KeyframeDragHandler* kf_DragHandler;
	Timeline_TimeCursor* timeCursor;
	LayersContextMenu_Manager* layers_ctx_menu;

	enum class ObjectStatus : uint8_t {
		NOTHING = 0,
		LEFT_RESIZE = 1,
		RIGHT_RESIZE = 2,
		BODY_MOVE = 3
	};

	uint64_t index_drag = UINT64_MAX;
	ObjectStatus pressed_status;
	ObjectStatus draging_status;

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

	void DrawLayerBox(const ImRect& rect, const LayerInfo& info, const bool& hovered, bool isSelected, bool isBodyDragging);
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