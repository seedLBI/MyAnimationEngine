#ifndef TIMELINE_LAYERS_LIST_TIMELINE_H
#define TIMELINE_LAYERS_LIST_TIMELINE_H

#include "Core/Animation/Timeline/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Timeline/Transform/Vertical/Timeline.Transform.Vertical.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Timeline/Widgets/Panel/TimeCursor/Timeline.TimeCursor.h"

#include <unordered_map>

class Timeline_Layers_Viewer
{
public:
	Timeline_Layers_Viewer(
		Timeline_TimeCursor* timeCursor,
		LayersDataProvider* layersData, 
		TimelineProviderData* timelineData, 
		TimelineTransformVertical* verticalTransform, 
		TimelineTransformHorizontal* horizontalTransform
	);

	~Timeline_Layers_Viewer();

	void Draw();
	void Update();

private:
	Timeline_TimeCursor* timeCursor;

	LayersDataProvider*		layersData;
	TimelineProviderData*	timelineData;

	TimelineTransformVertical*	 verticalTransform;
	TimelineTransformHorizontal* horizontalTransform;


	enum class ObjectStatus : uint8_t {
		NOTHING = 0,
		LEFT_RESIZE = 1,
		RIGHT_RESIZE = 2,
		BODY_MOVE = 3
	};

	float drag_start_mouse_global = 0.f;
	float drag_start_time_begin = 0.f;
	float length_drag = 0.f;

	uint64_t index_drag = UINT64_MAX;
	ObjectStatus pressed_status;
	ObjectStatus draging_status;

	void DrawLayerBox(const ImRect& rect, const LayerInfo& info, const bool& hovered);

	const float width_hover = 5.f;
	bool IsHovered_LeftResize(const ImRect& rect);
	bool IsHovered_RightResize(const ImRect& rect);
	bool IsHovered_BodyMove(const ImRect& rect);

};





#endif