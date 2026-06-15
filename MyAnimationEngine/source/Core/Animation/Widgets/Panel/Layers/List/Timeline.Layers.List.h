#ifndef TIMELINE_LAYERS_LIST_H
#define TIMELINE_LAYERS_LIST_H

#include "Core/Animation/Widgets/Panel/Transform/Vertical/Timeline.Transform.Vertical.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "IconDrawer/Timeline.Layers.Icon.Drawer.h"

#include <vector>
#include <string>

class Timeline_Layers_List {
public:
	Timeline_Layers_List(LayersDataProvider* layersData, TimelineTransformVertical* verticalTransform);
	~Timeline_Layers_List();

	void Draw();

	void DrawColumnsHeader();

	float getLengthColumn();

	void Update();

private:
	LayersDataProvider* layersData;
	TimelineTransformVertical* verticalTransform;

	void DrawObject(std::shared_ptr<LayerObject>& object);

	bool DrawToggle(std::shared_ptr<LayerObject>& object);
	void DrawLayerColor(std::shared_ptr<LayerObject>& object);
	void DrawLayerIndex(std::shared_ptr<LayerObject>& object);
	bool DrawLayerIconType(std::shared_ptr<LayerObject>& object);
	bool DrawLayerName(std::shared_ptr<LayerObject>& object);

	void drawTriggerButton(bool& value, std::string index ,const char* symbol, float availableHeight);
	void drawAnimationEnabler(std::string index);


	enum TypeColumn {
		INDEX,
		NAME,
		MARKER,
		FUNCTION_AV,
		TOGGLES,
		PARENT,
		MODES,
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