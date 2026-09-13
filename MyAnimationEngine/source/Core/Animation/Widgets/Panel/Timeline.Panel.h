#ifndef TIMELINE_PANEL_H
#define TIMELINE_PANEL_H

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "Utils/Timer/Timer.Framerate.h"
#include "Utils/ImGui/Utils.ImGui.h"

#include "Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Transform/Vertical/Timeline.Transform.Vertical.h"
#include "Transform/TransformScaled/Transform.Scaled.h"

#include "Core/Animation/Timeline/Controller/Timeline.Controller.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Layer/Factory/Layer.Factory.h"

#include "Selector/Timeline.Selector.h"
#include "AreaSelection/Timeline.AreaSelection.h"
#include "Ruler/Timeline.Ruler.h"
#include "Range/Timeline.Range.h"
#include "Scroll/Horizontal/Timeline.Scroll.Horizontal.h"
#include "Scroll/Vertical/Timeline.Scroll.Vertical.h"
#include "Layers/List/Timeline.Layers.List.h"
#include "Layers/Viewer/Timeline.Layers.Viewer.h"
#include "Layers/Graph/Timeline.Graph.h"
#include "Layers/ContextMenuManager/Layers.ContextMenu.Manager.h"
#include "Layers/KeyframeDragHandler/Keyframe.DragHandler.h"

#include "UpperControls/Timeline.UpperControls.h"
#include "HorizontalTickRenderer/Horizontal.Tick.Renderer.h"
#include "TimeCursor/Timeline.TimeCursor.h"
#include "Context/Timeline.Context.h"
#include "Textures/Noise/Texture.Noise.h"

#include "Core/DebugOverlay/DebugOverlay.h"


class Timeline_Panel {
public:
	Timeline_Panel(
		FPS_Timer* deltaTime, 
		TimelineController* timelineController,
		TimelineProviderData* timelineData, 
		LayersDataProvider* layersData, 
		LayerFactory* layerFactory
	);
	~Timeline_Panel();

	void Draw();
	void Update();

private:
	FPS_Timer* deltaTime;

	TimelineContext* timelineContext;

	TimelineController*			 timelineController;
	TimelineProviderData*		 timelineData;
	LayersDataProvider*			 layersData;
	LayerFactory*				 layerFactory;
	TimelineTransformHorizontal* timelineTransformHorizontal;
	TimelineTransformVertical*   timelineTransformVertical;
	TransformScaled*			 graphTransformVertical;
	Timeline_Selector*			 selector;
	LayersContextMenu_Manager*	 layers_ctx_menu;
	KeyframeDragHandler*		 keyframeDragHandler;

	// gui
	HorizontalTickRenderer*		horizontalTickRenderer;
	Timeline_TimeCursor*		timelineTimeCursor;
	Timeline_Ruler*				timelineRuler;
	Timeline_Range*				timelineRange;
	Timeline_Layers_List*		layersList;
	Timeline_Layers_Viewer*		layersViewer;
	Timeline_UpperControls*		upperControls;
	Timeline_Graph*				graph;
	Timeline_AreaSelection*		areaSelection;
	Timeline_Scroll_Horizontal* scrollHorizontal;
	Timeline_Scroll_Vertical*	scrollVertical;



	void Draw_1Level(const float& thickness_separator);
	void DrawLayerSide_1Level(const float& height);
	void DrawTimelineSide_1Level(const float& height);
	void DrawVerticalSliderBox_1Level(const float& height);


	void Draw_2Level(const float& thickness_separator);
	void DrawLayerSide_2Level(const float& height);
	void DrawTimelineSide_2Level(const float& height);
	void DrawVerticalSliderBox_2Level(const float& height);


	void Draw_3Level(const float& thickness_separator);
	void DrawLayerSide_3Level(const float& height);
	void DrawTimelineSide_3Level(const float& height);
	void DrawVerticalSliderBox_3Level(const float& height);


	void Draw_4Level(const float& thickness_separator);
	void DrawLayerSide_4Level(const float& height);
	void DrawTimelineSide_4Level(const float& height);
	void DrawVerticalSliderBox_4Level(const float& height);


};

#endif

