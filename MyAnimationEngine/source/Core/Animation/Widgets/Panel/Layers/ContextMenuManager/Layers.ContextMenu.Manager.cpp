#include "Layers.ContextMenu.Manager.h"

LayersContextMenu_Manager::LayersContextMenu_Manager(TimelineContext* ctx) : ctx(ctx) {}
LayersContextMenu_Manager::~LayersContextMenu_Manager() {}


void LayersContextMenu_Manager::OpenMenu_Creation() {
    request_open_creation = true;
}

void LayersContextMenu_Manager::OpenMenu_LayerSetting(uint64_t id_layer) {
    request_open_setting = true;
	this->id_layer_open_setting = id_layer;
}

void LayersContextMenu_Manager::Draw() {

    if (request_open_creation) {
        ImGui::OpenPopup("POPUP_LAYERS_MENU_CREATION");
        request_open_creation = false;
    }
    if (request_open_setting) {
        ImGui::OpenPopup("POPUP_LAYERS_MENU_SETTING");
        request_open_setting = false;
    }

    Push_SmallFont();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.f, 12.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.f, 12.f));
	    DrawMenu_Creation();
	    DrawMenu_LayerSetting();
    ImGui::PopStyleVar(2);
    Pop_SmallFont();
}

void LayersContextMenu_Manager::DrawMenu_Creation() {
    if (ImGui::BeginPopup("POPUP_LAYERS_MENU_CREATION")) {

        if (ImGui::BeginMenu(u8"Создать слой")) {
            if (ImGui::MenuItem(u8"Сплошной")) {
                ctx->layerFactory->CreateSolid();
            }
            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }

}
void LayersContextMenu_Manager::DrawMenu_LayerSetting() {
    if (ImGui::BeginPopup("POPUP_LAYERS_MENU_SETTING")) {

        if (ImGui::MenuItem(u8"Переименовать")) {
            this->id_layer_renamingMode = this->id_layer_open_setting;
            pressedRename = true;
        }

        if (ImGui::MenuItem(u8"Обратить выделение")) {

            ctx->layersData->forEach(
                [&](LayerObject* layer, uint64_t number_layer) {
                    const auto& id_layer = layer->GetLayerInfo().id;
                    ctx->selector->AddMoreSelectedLayer_Toggle({ id_layer });
                }
            );

        }

        ImGui::EndPopup();
    }
}

uint64_t LayersContextMenu_Manager::GetIDLayer_RenamingMode() {
    if (pressedRename) {
        pressedRename = false;
        uint64_t temp = this->id_layer_renamingMode;
        this->id_layer_renamingMode = UINT64_MAX;
        return temp;
    }
    else {
        return UINT64_MAX;
    }

}