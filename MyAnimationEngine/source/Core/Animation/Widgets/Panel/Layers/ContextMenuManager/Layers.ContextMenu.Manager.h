#ifndef LAYERS_CONTEXT_MENU_MANAGER_H
#define LAYERS_CONTEXT_MENU_MANAGER_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"
#include "FontManager/FontManager.h"

class LayersContextMenu_Manager{
public:
	LayersContextMenu_Manager(TimelineContext* ctx);
	~LayersContextMenu_Manager();

	void OpenMenu_Creation();
	void OpenMenu_LayerSetting(uint64_t id_layer);

	uint64_t GetIDLayer_RenamingMode();

	void Draw();

private:
	TimelineContext* ctx;
	uint64_t id_layer_open_setting;

	uint64_t id_layer_renamingMode;
	bool pressedRename = false;

	bool request_open_creation = false;
	bool request_open_setting = false;

	void DrawMenu_Creation();
	void DrawMenu_LayerSetting();
};

#endif