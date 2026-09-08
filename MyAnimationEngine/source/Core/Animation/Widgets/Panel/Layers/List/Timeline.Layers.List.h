#ifndef TIMELINE_LAYERS_LIST_H
#define TIMELINE_LAYERS_LIST_H


#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"
#include "Core/Animation/Widgets/Panel/Layers/ContextMenuManager/Layers.ContextMenu.Manager.h"
#include "IconDrawer/Timeline.Layers.Icon.Drawer.h"
#include "Utils/Timer/Timer.h"

#include <vector>
#include <string>

class Timeline_Layers_List {
public:
	Timeline_Layers_List(TimelineContext* ctx, LayersContextMenu_Manager* layers_ctx_menu);
	~Timeline_Layers_List();

	void Draw();

	void DrawColumnsHeader();

	float getLengthColumn();

	void Update();

private:
	TimelineContext* ctx;
	LayersContextMenu_Manager* layers_ctx_menu;

	void DrawLayer(LayerObject* object, uint64_t index, const ImVec2& layer_min, const ImVec2& layer_max, bool isSelectedLayer);
	bool DrawToggle(LayerObject* object, uint64_t index);
	void DrawLayerColor(LayerObject* object, uint64_t index);
	void DrawLayerIndex(LayerObject* object, uint64_t index);
	bool DrawLayerIconType(LayerObject* object, uint64_t index);
	bool DrawLayerName(LayerObject* object, uint64_t index);

	void drawTriggerButton(bool& value, std::string index ,const char* symbol, float availableHeight);
	bool drawAnimationEnabler(bool& value, std::string index, float availableHeight);

	void DrawEmptyState(const ImVec2& center);
	void HandleScroll(const ImVec2& p_min, const ImVec2& p_max, float itemHeight);
	void PrepareDragStaff();
	void HandleSelectionLayer(LayerObject* layerlayer, uint64_t index, const ImVec2& p, const ImVec2& layer_min, const ImVec2& layer_max, bool isSelectedLayer, bool isHoveredName);
	void HandleAreaSelectionStart(const ImVec2& p, const ImVec2& p_max);
	void ProcessDragLogic(float itemHeight);
	void DrawDragAndDropIndicators(uint64_t index, const ImVec2& p, const ImVec2& p_max, const ImVec2& layer_min, const ImVec2& layer_max, bool isSelectedLayer);

	void DrawLayerProperties(LayerObject* layer, uint64_t index_layer, ImVec2& p, const ImVec2& p_min, const ImVec2& p_max, float itemHeight, float& heightContent);


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

	bool InRenamingMode = false;
	uint64_t id_layer_renamingMode = UINT64_MAX;

	int getCountEnabledColumns();

	struct DragStaff {
		uint64_t count_selected = 0;
		uint64_t indexGrabbingLayer = 0;
		bool isGrabingLayer = false;
		bool isHoveredLayer = false;
		std::vector<float> offsets_selected_while_grabing;
		ImVec2 pos_grab_start;
		Timer timer_AfterPress;
		float time4enableDrag = 0.05f;
		bool DragDrawedOnce = false;
		bool IsDragUpper = false;
		bool IsDragLower = false;
		uint64_t index_layer_dragdrop = UINT64_MAX;
	} dragStaff;

	enum AV_SYMBOLS {
		VISIBLE,
		ONLY,
		VOLUME,
		LOCK
	};

	std::vector<float> av_symbols_cursor_pos;
};


#endif