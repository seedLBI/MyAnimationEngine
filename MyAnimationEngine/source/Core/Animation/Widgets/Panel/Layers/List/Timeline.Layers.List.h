#ifndef TIMELINE_LAYERS_LIST_H
#define TIMELINE_LAYERS_LIST_H


#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"
#include "IconDrawer/Timeline.Layers.Icon.Drawer.h"


#include <vector>
#include <string>

class Timeline_Layers_List {
public:
	Timeline_Layers_List(TimelineContext* ctx);
	~Timeline_Layers_List();

	void Draw();

	void DrawColumnsHeader();

	float getLengthColumn();

	void Update();

private:
	TimelineContext* ctx;

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