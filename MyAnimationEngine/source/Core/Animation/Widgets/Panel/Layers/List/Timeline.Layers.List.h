#ifndef TIMELINE_LAYERS_LIST_H
#define TIMELINE_LAYERS_LIST_H

#include "Core/Animation/Widgets/Panel/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Widgets/Panel/Transform/Vertical/Timeline.Transform.Vertical.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Timeline/Controller/Timeline.Controller.h"
#include "IconDrawer/Timeline.Layers.Icon.Drawer.h"


#include <vector>
#include <string>

class Timeline_Layers_List {
public:
	Timeline_Layers_List(
		LayersDataProvider* layersData,
		TimelineProviderData* timelineData,
		TimelineController* timelineController,
		TimelineTransformHorizontal* horizontalTransform,
		TimelineTransformVertical* verticalTransform);
	~Timeline_Layers_List();

	void Draw();

	void DrawColumnsHeader();

	float getLengthColumn();

	void Update();

private:
	LayersDataProvider* layersData;
	TimelineProviderData* timelineData;
	TimelineController* timelineController;
	TimelineTransformHorizontal*	horizontalTransform;
	TimelineTransformVertical*		verticalTransform;

	void DrawObject(LayerObject* object, uint64_t index);

	bool DrawToggle(LayerObject* object, uint64_t index);
	void DrawLayerColor(LayerObject* object, uint64_t index);
	void DrawLayerIndex(LayerObject* object, uint64_t index);
	bool DrawLayerIconType(LayerObject* object, uint64_t index);
	bool DrawLayerName(LayerObject* object, uint64_t index);

	void drawTriggerButton(bool& value, std::string index ,const char* symbol, float availableHeight);
	bool drawAnimationEnabler(bool& value, std::string index, float availableHeight);


	enum TypeColumn {
		INDEX,
		NAME,
		MARKER,
		FUNCTION_AV,
		TOGGLES,
		PARENT,
		MODES,
		RENDER_TIME,
		NOTHING
	};

	struct Column {
		std::string name;
		float min_width;
		float width;
		float startCursorPos;
		bool isRemovebal;
		bool isDragable;
		bool isResizable;
		bool isDisable;
		TypeColumn Left;
		TypeColumn Right;
	};

	std::vector<Column> columns;
	std::vector<uint8_t> index_current_columns; //order for column

	int getCountEnabledColumns();


	enum AV_SYMBOLS {
		VISIBLE,
		ONLY,
		VOLUME,
		LOCK
	};
	std::vector<float> av_symbols_cursor_pos;
};


#endif