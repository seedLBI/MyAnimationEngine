#include "Timeline.Layers.List.h"

#include "FontManager/FontManager.h"
#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"
#include "Utils/Text/Text.h"


Timeline_Layers_List::Timeline_Layers_List(LayersDataProvider* layersData, TimelineTransformVertical* verticalTransform)
	: layersData(layersData),
	  verticalTransform(verticalTransform)
{

}

Timeline_Layers_List::~Timeline_Layers_List()
{
}

void Timeline_Layers_List::drawTriggerButton(bool& value, std::string index, const char* symbol, float availableHeight) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const float size = availableHeight * 0.8f;

    ImVec2 p_min = { ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + (availableHeight - size)  };
    ImVec2 p_max = { p_min.x + size, p_min.y + size};


    const auto save_pos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(p_min);

    if (ImGui::InvisibleButton( std::string(symbol + index).c_str(), ImVec2(size,size) ))
        value = !value;

    ImGui::SetCursorScreenPos(save_pos);
    ImGui::Dummy(ImVec2(size, availableHeight));



    bool isHovered = ImGui::IsItemHovered();

    draw_list->AddRectFilled(p_min, p_max, ImColor(0.f, 0.f, 0.f, 1.f), 3.f);

    const ImColor default_color = ImColor(.69f, .69f, .69f, 1.f);
    const ImColor hovered_color = ImColor(1.f, 1.f, 1.f, 1.f);
    const ImColor hidden_color  = ImColor(.42f, .42f, .42f, 1.f);
    const ImColor disable_color = ImColor(0.f, 0.f, 0.f, 1.f);

    ImColor result_color = default_color;

    if (value)
        result_color = isHovered ? hovered_color : default_color;
    else
        result_color = isHovered ? hidden_color : disable_color;


    Push_SmallestFont();
        ImVec2 icon_size = ImGui::CalcTextSize(symbol);
        draw_list->AddText({ p_min.x + (p_max.x - p_min.x) * 0.5f - icon_size.x * 0.5f + 1.f, p_min.y + (p_max.y - p_min.y)*0.5f - icon_size.y*0.5f }, result_color, symbol);
    Pop_SmallestFont();
}

void Timeline_Layers_List::DrawObject(std::shared_ptr<LayerObject>& object)
{   
    std::string index = std::to_string(object.get()->GetLayerInfo().index);

    float height_object = verticalTransform->getScaledHeightOneObject();


    ImGui::BeginChild(("##Layer##" + index).c_str(),ImVec2(ImGui::GetContentRegionAvail().x, height_object),0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        const float margin = 5.f;

        auto pos = ImGui::GetCursorScreenPos();

        ImGui::SetCursorPosX(10.f);     
        drawTriggerButton(
            object.get()->GetLayerInfo().isVisible, 
            ("##LAYER_VISIBLE_TRIGGER" + index).c_str(), 
            ICON_FA_EYE, 
            height_object
        );
        //DrawVisible(object);

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
    std::string index = std::to_string(object.get()->GetLayerInfo().index);
    std::string label = "toggle_btn##" + index;

    ImGui::PushID(label.c_str());

    const float box_size = verticalTransform->getScaledHeightOneObject();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(box_size, box_size);

    float itemHeight = verticalTransform->getScaledHeightOneObject();
    float textHeight = ImGui::GetTextLineHeight();
    float offsetY = (itemHeight - textHeight) * 0.5f;
    pos.y -= offsetY + 1.f;

    ImGui::InvisibleButton(("##icon_toggle##" + index).c_str(), size);

    bool changed = false;
    if (ImGui::IsItemClicked())
    {
        object.get()->GetLayerInfo().isExpanded = !object.get()->GetLayerInfo().isExpanded;
        changed = true;
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const bool hovered = ImGui::IsItemHovered();

    ImU32 text_col = ImGui::GetColorU32(hovered ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    bool isExpanded = object.get()->GetLayerInfo().isExpanded;

    const char* icon = isExpanded ? ICON_FA_CHEVRON_DOWN : ICON_FA_CHEVRON_RIGHT;

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

    float itemHeight = verticalTransform->getScaledHeightOneObject();
    const float size = round(itemHeight * 0.8f);
    float offsetY = (itemHeight - size) * 0.5f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY + 1.f);



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

    float itemHeight = verticalTransform->getScaledHeightOneObject();
    float textHeight = ImGui::GetTextLineHeight();
    float offsetY = (itemHeight - textHeight) * 0.5f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);

    std::string str_index = std::to_string(object.get()->GetLayerInfo().index);
    PadLeft(str_index, '0', 3);

    Push_SmallFont();
    ImGui::Text(str_index.c_str());
    Pop_SmallFont();
}

bool Timeline_Layers_List::DrawLayerIconType(std::shared_ptr<LayerObject>& object) {
    DrawIcon(object.get(), verticalTransform->getScaledHeightOneObject());
    return false;
}

bool Timeline_Layers_List::DrawLayerName(std::shared_ptr<LayerObject>& object) {
    float itemHeight = verticalTransform->getScaledHeightOneObject();
    float textHeight = ImGui::GetTextLineHeight();
    float offsetY = (itemHeight - textHeight) * 0.5f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);

    Push_SmallFont();
    ImGui::Text(object.get()->GetLayerInfo().name.c_str());
    Pop_SmallFont();
    return false;
}

void Timeline_Layers_List::DrawColumnsHeader() {

}

void Timeline_Layers_List::Draw() {

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 region_max = ImGui::GetContentRegionAvail();
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

    ImVec2 mouse_pos = { ImGui::GetMousePos().x - cursor_pos.x,ImGui::GetMousePos().y - cursor_pos.y };

    ImVec2 p_min = cursor_pos;
    ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };



	auto layers = layersData->GetData().layers;


    float itemHeight = verticalTransform->getScaledHeightOneObject();

    ImVec2 p = ImGui::GetCursorScreenPos();

	for (size_t i = 0; i < layers.size(); i++) {

        ImGui::SetCursorScreenPos(p);

        auto layer = layers[i];
        auto& layerInfo = layer.get()->GetLayerInfo();


        draw_list->AddRectFilled({ p_min.x, round(p.y + 1.f) }, { p_max.x, round(p.y + itemHeight ) }, ImColor(.19f, .19f, .19f, 1.f));
        
        if (i==0) ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.f);
		DrawObject(layer);


        p.y += itemHeight;
        draw_list->AddLine({ p_min.x, round(p.y) }, { p_max.x, round(p.y) }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);




        if (layerInfo.isExpanded) {

            auto& properties = layer.get()->GetProperties();
            

            for (const auto& propertie : properties.getListNames()) {
                Push_SmallFont();

                float textHeight = ImGui::GetTextLineHeight();
                float offsetY = (itemHeight - textHeight) * 0.5f;
                ImGui::SetCursorScreenPos(ImVec2(p.x + 100.f, p.y + offsetY));

                bool enable = false;
                drawTriggerButton(enable, ("##PROPERTIES_TRIGGER_" + std::to_string(i)+"_"+ propertie).c_str(), ICON_FA_STOPWATCH, itemHeight);

                ImGui::SameLine(0.f,10.f);

                ImGui::Text(propertie.c_str());
                Pop_SmallFont();

                draw_list->AddLine({ p_min.x, round(p.y) }, { p_max.x, round(p.y) }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
                
                p.y += itemHeight;
                ImGui::SetCursorScreenPos(p);
            }
            //p.y += 1.f;

        }




        //ImGui::Separator();

	}


}


void Timeline_Layers_List::Update() {

}