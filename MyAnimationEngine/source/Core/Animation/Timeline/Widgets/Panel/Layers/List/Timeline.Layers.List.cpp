#include "Timeline.Layers.List.h"

#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"

Timeline_Layers_List::Timeline_Layers_List(LayersDataProvider* layersData, TimelineTransformVertical* verticalTransform)
	: layersData(layersData),
	  verticalTransform(verticalTransform)
{

}

Timeline_Layers_List::~Timeline_Layers_List()
{
}

void Timeline_Layers_List::DrawObject(std::shared_ptr<LayerObject>& object)
{   
    std::string index = std::to_string(object.get()->GetLayerInfo().index);

    float height_object = verticalTransform->getScaledHeightOneObject();


    ImGui::BeginChild(("##Layer##" + index).c_str(),ImVec2(ImGui::GetContentRegionAvail().x, height_object),0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        const float margin = 4.f;

        ImGui::SetCursorPosX(10.f);     DrawVisible(object);
        ImGui::SameLine(0.f, margin);   DrawToggle(object);
        ImGui::SameLine(0.f, margin);   DrawLayerColor(object);
        ImGui::SameLine(0.f, margin);   DrawLayerIndex(object);
        ImGui::SameLine(0.f, margin);   DrawLayerIconType(object);
        ImGui::SameLine(0.f, margin);   DrawLayerName(object);

    ImGui::EndChild();
}

bool Timeline_Layers_List::DrawVisible(std::shared_ptr<LayerObject>& object) {


    std::string index = std::to_string(object.get()->GetLayerInfo().index);

    std::string label = "checkbox_visible##" + index;

    ImGui::PushID(label.c_str());

    const ImGuiStyle& style = ImGui::GetStyle();
    const float box_size = verticalTransform->getScaledHeightOneObject();

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(box_size, box_size);

    ImGui::InvisibleButton(("##icon_checkbox##" + index).c_str(), size);

    bool changed = false;
    if (ImGui::IsItemClicked())
    {
        object.get()->GetLayerInfo().isVisible = !object.get()->GetLayerInfo().isVisible;
        changed = true;
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();

    const bool hovered = ImGui::IsItemHovered();
    ImU32 frame_col = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImU32 border_col = ImGui::GetColorU32(ImGuiCol_Border);
    ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);

    // box
    dl->AddRectFilled(pos, ImVec2(pos.x + box_size, pos.y + box_size), frame_col, style.FrameRounding);
    dl->AddRect(pos, ImVec2(pos.x + box_size, pos.y + box_size), border_col, style.FrameRounding);

    // icon centered in box
    bool isVisible = object.get()->GetLayerInfo().isVisible;
    const char* icon = isVisible ? ICON_FA_EYE : "";
    ImVec2 icon_size = ImGui::CalcTextSize(icon);
    dl->AddText(
        ImVec2(pos.x + (box_size - icon_size.x) * 0.5f,
            pos.y + (box_size - icon_size.y) * 0.5f - 1.0f),
        text_col,
        icon
    );


    ImGui::PopID();
    return changed;
}

bool Timeline_Layers_List::DrawToggle(std::shared_ptr<LayerObject>& object) {
    ImGui::Text(">");
    return false;
}

void Timeline_Layers_List::DrawLayerColor(std::shared_ptr<LayerObject>& object) {
    std::string index = std::to_string(object.get()->GetLayerInfo().index);

    float c[3] = { 
        object.get()->GetLayerInfo().color.r, 
        object.get()->GetLayerInfo().color.g, 
        object.get()->GetLayerInfo().color.b 
    };

    std::string button_id = "##color_picker_layer##" + index;
    std::string popup_id = "##color_picker_popup##" + index;

    ImVec4 col = ImVec4(c[0], c[1], c[2], 1.0f);

    const float size = 18.0f;
    if (ImGui::ColorButton(button_id.c_str(), col, ImGuiColorEditFlags_None, ImVec2(size, size))) {
        ImGui::OpenPopup(popup_id.c_str()); 
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("COLOR_LAYER_PAYLOAD", c, sizeof(float) * 3);
        ImGui::Text(u8"Цвет маркера");
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COLOR_LAYER_PAYLOAD");
        if (payload) {
            float* color_payload = static_cast<float*>(payload->Data);

            c[0] = color_payload[0];
            c[1] = color_payload[1];
            c[2] = color_payload[2];
        }
        ImGui::EndDragDropTarget();
    }



    if (ImGui::BeginPopup(popup_id.c_str())) {
        ImGui::ColorPicker3(("##picker##" + index).c_str(), c,
            ImGuiColorEditFlags_NoAlpha |
            ImGuiColorEditFlags_NoSidePreview |
            ImGuiColorEditFlags_NoSmallPreview);

        ImGui::EndPopup();
    }

    object.get()->GetLayerInfo().color = { c[0], c[1], c[2] };
}

void Timeline_Layers_List::DrawLayerIndex(std::shared_ptr<LayerObject>& object) {
    ImGui::Text("123");
}

bool Timeline_Layers_List::DrawLayerIconType(std::shared_ptr<LayerObject>& object) {
    object.get()->LayerList_DrawIconType();
    return false;
}

bool Timeline_Layers_List::DrawLayerName(std::shared_ptr<LayerObject>& object) {
    ImGui::Text(object.get()->GetLayerInfo().name.c_str());
    return false;
}



void Timeline_Layers_List::DrawColumnsHeader() {

}

void Timeline_Layers_List::Draw() {
	auto data = layersData->GetData().layers;

	float headerHeight = ImGui::GetTextLineHeight();

	ImVec2 p = ImGui::GetCursorScreenPos();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.f);

	for (size_t i = 0; i < data.size(); i++) {
		DrawObject(data[i]);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.f);
	}

}


void Timeline_Layers_List::Update() {

}