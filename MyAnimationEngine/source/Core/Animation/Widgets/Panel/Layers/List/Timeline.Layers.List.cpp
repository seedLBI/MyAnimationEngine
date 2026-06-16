#include "Timeline.Layers.List.h"


#include "CursorManager/CursorManger.h"
#include "FontManager/FontManager.h"
#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/Encoding/UTF8/UTF8.h"
#include <cstring>

Timeline_Layers_List::Timeline_Layers_List(LayersDataProvider* layersData, TimelineTransformVertical* verticalTransform)
	: layersData(layersData),
	  verticalTransform(verticalTransform)
{

    columns.resize(7);
    av_symbols_cursor_pos.resize(4);
    columns[TypeColumn::FUNCTION_AV] = {
        u8"Функции A/V",
        70.f,
        70.f,
        0.f,
        true,
        true,
        false,
        false,
        NOTHING,
        NOTHING
    };
    columns[TypeColumn::MARKER] = {
        u8"Маркировка",
        20.f,
        20.f,
        0.f,
        true,
        false,
        true,
        false,
        NOTHING,
        INDEX
    };
    columns[TypeColumn::INDEX] = {
        u8"Индекс слоя",
        20.f,
        20.f,
        0.f,
        true,
        false,
        true,
        false,
        MARKER,
        NAME
    };

    columns[TypeColumn::NAME] = {
        u8"Имя источника",
        110.f,
        110.f,
        0.f,
        false,
        false,
        true,
        false,
        INDEX,
        NOTHING
    };
    columns[TypeColumn::TOGGLES] = {
        u8"Переключатели",
        100.f,
        100.f,
        0.f,
        true,
        true,
        true,
        false,
        NOTHING,
        NOTHING
    };
    columns[TypeColumn::PARENT] = {
        u8"Родительский элемент и ссылка",
        120.f,
        180.f,
        0.f,
        true,
        true,
        true,
        false,
        NOTHING,
        NOTHING
    };
    columns[TypeColumn::MODES] = {
        u8"Режимы",
        120.f,
        120.f,
        0.f,
        true,
        true,
        true,
        false,
        NOTHING,
        NOTHING
    };

    index_current_columns = {
        TypeColumn::FUNCTION_AV,
		TypeColumn::MARKER,
        TypeColumn::INDEX,
		TypeColumn::NAME,
		TypeColumn::TOGGLES,
        TypeColumn::MODES,
		TypeColumn::PARENT
    };
}

Timeline_Layers_List::~Timeline_Layers_List() {

}

float Timeline_Layers_List::getLengthColumn() {
    float result = 0.f;
    for (size_t i = 0; i < columns.size(); i++)
        if (columns[i].isDisable == false)
            result += columns[i].width;
    return result + 10.f * getCountEnabledColumns() + 30.f;
}

int Timeline_Layers_List::getCountEnabledColumns() {
    int count = 0;
    for (size_t i = 0; i < columns.size(); i++)
        if (columns[i].isDisable == false)
            count++;
    return count;
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

    if (strcmp(symbol,"CIRCLE") == 0) {
        draw_list->AddCircleFilled({p_min.x + size/2.f, p_min.y + size / 2.f }, 3.5f, result_color);
    }
    else {
        Push_SmallestFont();
        ImVec2 icon_size = ImGui::CalcTextSize(symbol);
        draw_list->AddText({ p_min.x + (p_max.x - p_min.x) * 0.5f - icon_size.x * 0.5f + 1.f, p_min.y + (p_max.y - p_min.y) * 0.5f - icon_size.y * 0.5f }, result_color, symbol);
        Pop_SmallestFont();
    }
}

bool Timeline_Layers_List::drawAnimationEnabler(bool& value, std::string index, float availableHeight) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const float size = availableHeight * 0.8f;

    ImVec2 p_min = { ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + (availableHeight - size) };
    ImVec2 p_max = { p_min.x + size, p_min.y + size };


    const auto save_pos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(p_min);


    bool result = ImGui::InvisibleButton(std::string(index).c_str(), ImVec2(size, size));


    ImGui::SetCursorScreenPos(save_pos);
    ImGui::Dummy(ImVec2(size, availableHeight));



    bool isHovered = ImGui::IsItemHovered();


    const ImColor default_color = ImColor(.69f, .69f, .69f, 1.f);
    const ImColor hovered_color = ImColor(1.f, 1.f, 1.f, 1.f);
    const ImColor hidden_color = ImColor(.42f, .42f, .42f, 1.f);
    const ImColor disable_color = ImColor(0.f, 0.f, 0.f, 1.f);
    const ImColor enabled_color = ImColor(.25f, .59f, .95f, 1.f);
    const ImColor enabled_hovered_color = ImColor(.37f, .67f, .97f, 1.f);

    if (value || isHovered) {
        draw_list->AddRectFilled(p_min, p_max, disable_color, 3.f);
        if (isHovered == false && value)
            draw_list->AddRect(p_min, p_max, hidden_color, 3.f, 0, 1.f);
        else
            draw_list->AddRect(p_min, p_max, default_color, 3.f, 0, 1.f);
    }



    ImColor result_color = default_color;

    if (value)
        result_color = isHovered ? enabled_hovered_color : enabled_color;
    else
        result_color = isHovered ? hovered_color : default_color;

    Push_SmallestFont();
    ImVec2 icon_size = ImGui::CalcTextSize(ICON_FA_STOPWATCH);
    draw_list->AddText({ p_min.x + (p_max.x - p_min.x) * 0.5f - icon_size.x * 0.5f + 1.f, p_min.y + (p_max.y - p_min.y) * 0.5f - icon_size.y * 0.5f }, result_color, ICON_FA_STOPWATCH);
    Pop_SmallestFont();

    return result;
}


void Timeline_Layers_List::DrawObject(std::shared_ptr<LayerObject>& object)
{
    std::string index = std::to_string(object.get()->GetLayerInfo().index);
    float height_object = verticalTransform->getScaledHeightOneObject();

    ImGui::BeginChild(("##Layer##" + index).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, height_object), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    const float margin = 5.f;


    float startY = ImGui::GetCursorScreenPos().y;

    bool toggle_drawed = false;


    for (size_t i = 0; i < index_current_columns.size(); i++) {

        auto current_col = index_current_columns[i];

        if (columns[current_col].isDisable)
            continue;



        if ((current_col == TypeColumn::MARKER || current_col == TypeColumn::INDEX || current_col == TypeColumn::NAME) && toggle_drawed == false) {
            toggle_drawed = true;
            ImGui::SetCursorScreenPos(ImVec2(columns[current_col].startCursorPos - 20.f, startY));

            DrawToggle(object);

        }

        ImGui::SetCursorScreenPos(ImVec2(columns[current_col].startCursorPos, startY));


        switch (current_col) {
        case TypeColumn::FUNCTION_AV: {

            ImGui::SetCursorScreenPos(ImVec2(av_symbols_cursor_pos[AV_SYMBOLS::VISIBLE], startY));
            drawTriggerButton(
                object.get()->GetLayerInfo().isVisible,
                ("##LAYER_VISIBLE_TRIGGER" + index).c_str(),
                ICON_FA_EYE,
                height_object
            );


            ImGui::SetCursorScreenPos(ImVec2(av_symbols_cursor_pos[AV_SYMBOLS::LOCK], startY));
            drawTriggerButton(
                object.get()->GetLayerInfo().isLocked,
                ("##LAYER_LOCKED_TRIGGER" + index).c_str(),
                ICON_FA_LOCK,
                height_object
            );


            ImGui::SetCursorScreenPos(ImVec2(av_symbols_cursor_pos[AV_SYMBOLS::ONLY], startY));
            drawTriggerButton(
                object.get()->GetLayerInfo().isOnly,
                ("##LAYER_ONLY_VISIBLE_TRIGGER" + index).c_str(),
                "CIRCLE",
                height_object
            );
            break;
        }
        case TypeColumn::MARKER: {
            DrawLayerColor(object);
            break;
        }
        case TypeColumn::INDEX: {
            DrawLayerIndex(object);
            break;
        }
        case TypeColumn::NAME: {
            DrawLayerIconType(object);
            ImGui::SameLine(0.f, margin);
            DrawLayerName(object);
            break;
        }
        case TypeColumn::TOGGLES: {
            
            break;
        }
        case TypeColumn::PARENT: {

            break;
        }
        case TypeColumn::MODES: {

            break;
        }
        case TypeColumn::NOTHING:
        default:
            break;
        }
    }

    ImGui::EndChild();
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
    float offsetY = (itemHeight - textHeight) * 0.5f - 1.f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);

    std::string str_index = std::to_string(object.get()->GetLayerInfo().index);
    //PadLeft(str_index, '0', 3);

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
    float offsetY = (itemHeight - textHeight) * 0.5f - 1.f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);

    Push_SmallFont();
    ImGui::Text(object.get()->GetLayerInfo().name.c_str());
    Pop_SmallFont();
    return false;
}



void Timeline_Layers_List::DrawColumnsHeader() {

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 region_max = ImGui::GetContentRegionAvail();
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

    ImVec2 p_min = cursor_pos;
    ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };


    float itemHeight = verticalTransform->getScaledHeightOneObject();


    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.69f, .69f, .69f, 1.f));

    Push_SmallestFont();

    float textHeight = ImGui::GetTextLineHeight();
    float offsetY = (itemHeight - textHeight) * 0.5f + 2.f;
    

    ImVec2 p = p_min;

    p.x += 10.f;

    bool toggle_offset_setted = false;

    for (size_t i = 0; i < index_current_columns.size(); i++) {

        auto& type = index_current_columns[i];
        auto& current_column = columns[index_current_columns[i]];

        if (current_column.isDisable)
            continue;

        if ((type == TypeColumn::MARKER || type == TypeColumn::INDEX || type == TypeColumn::NAME) && toggle_offset_setted == false) {
            toggle_offset_setted = true;
            p.x += 20.f;
        }


        current_column.startCursorPos = p.x;

        const char* textToDraw = "";


        float x_before = p.x;

        switch (type) {
            case TypeColumn::FUNCTION_AV: {
                
                av_symbols_cursor_pos[AV_SYMBOLS::VISIBLE] = p.x - 1.f;
                draw_list->AddText({ p.x,p.y + offsetY }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), ICON_FA_EYE);
                p.x += ImGui::CalcTextSize(ICON_FA_EYE).x;
                p.x += 5.f;

                av_symbols_cursor_pos[AV_SYMBOLS::VOLUME] = p.x - 1.f;
                draw_list->AddText({ p.x,p.y + offsetY }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), ICON_FA_VOLUME_HIGH);
                p.x += ImGui::CalcTextSize(ICON_FA_VOLUME_HIGH).x;
                p.x += 5.f;

                av_symbols_cursor_pos[AV_SYMBOLS::ONLY] = p.x - 1.f;
                draw_list->AddCircleFilled({ p.x + ImGui::CalcTextSize(ICON_FA_VOLUME_HIGH).x/2.f,p.y + itemHeight/2.f + 2.f }, 3.5f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)));
                p.x += ImGui::CalcTextSize(ICON_FA_CIRCLE).x;
                p.x += 5.f;

                av_symbols_cursor_pos[AV_SYMBOLS::LOCK] = p.x - 1.f;
                draw_list->AddText({ p.x,p.y + offsetY }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), ICON_FA_LOCK);
                p.x += ImGui::CalcTextSize(ICON_FA_LOCK).x;

                break;
            }
            case TypeColumn::INDEX: {
                draw_list->AddText({ p.x,p.y + offsetY }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), ICON_FA_HASHTAG);
                p.x += ImGui::CalcTextSize(ICON_FA_HASHTAG).x;

                break;
            }
            case TypeColumn::NAME: {
                draw_list->AddText({ p.x,p.y + offsetY }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), u8"Имя источника");
                p.x += ImGui::CalcTextSize(u8"Имя источника").x;
                break;
            }
            case TypeColumn::PARENT: {

                std::string Text = u8"Родительский элемент и ссылка";
                float widthText = ImGui::CalcTextSize(Text.c_str()).x;

                float availableWidth = columns[index_current_columns[i]].width;

                if (availableWidth < widthText)
                {
                    float oneSymbol = 7.f;

                    float dif = widthText - availableWidth;

                    int countSymbolsRemove = int(round(dif / oneSymbol) + 3.f);

                    UTF8_SPLITER_ERROR error;
                    auto splitted = utf8_splitter(Text, error);
                    splitted.erase(splitted.begin() + splitted.size() - 1 - countSymbolsRemove, splitted.end());
                    std::string result = "";
                    for (size_t i = 0; i < splitted.size(); i++){
                        result += splitted[i];
                    }
                    result += "...";
                    Text = result;
                }


                draw_list->AddText({ p.x,p.y + offsetY }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), Text.c_str());
                p.x += ImGui::CalcTextSize(Text.c_str()).x;

                break;
            }
            case TypeColumn::TOGGLES: {
                draw_list->AddText({ p.x,p.y + offsetY }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), " ");
                p.x += ImGui::CalcTextSize(" ").x;
                break;
            }
            case TypeColumn::MARKER: {
                draw_list->AddText({ p.x,p.y + offsetY }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), ICON_FA_TAG);
                p.x += ImGui::CalcTextSize(ICON_FA_TAG).x;

                break;
            }
            case TypeColumn::MODES: {
                draw_list->AddText({ p.x,p.y + offsetY }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), u8"Режим");
                p.x += ImGui::CalcTextSize(u8"Режим").x;

                break;
            }
        }
        
        // SEPARATOR BUTTON (dragable&resizable)
        float separator_pos = x_before + current_column.width + 5.f;

        ImVec2 s_min = { separator_pos ,p_min.y + 4.f };
        ImVec2 s_max = { separator_pos, p_max.y - 5.f };
        ImColor separator_color = ImColor(0.f, 0.f, 0.f, 1.f);


        if (current_column.isResizable) {
            float grab_padding = 4.0f;

            ImRect bb(separator_pos - grab_padding, p_min.y, separator_pos + grab_padding, p_max.y);

            ImGuiID id = ImGui::GetID((std::string("SEPARATOR_") + std::to_string(i)).c_str());

            bool hovered, held;

            ImGui::ItemAdd(bb, id);
            ImGui::ButtonBehavior(bb, id, &hovered, &held);

            if (hovered || held)
                CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);


            if (held) {

                current_column.width += ImGui::GetIO().MouseDelta.x;

                if (current_column.width < current_column.min_width)
                    current_column.width = current_column.min_width;
            }
        }

        draw_list->AddLine(s_min, s_max, separator_color, 2.f);


        if (current_column.isDragable) {


        }

        p.x = x_before + current_column.width + 10.f;
    }




    Pop_SmallestFont();

    ImGui::PopStyleColor();


    Push_SmallFont();

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right,false) && ImGui::IsMouseHoveringRect(p_min,p_max))
        ImGui::OpenPopup("POPUP_COLUMNS_SET");


    if (ImGui::BeginPopupContextItem("POPUP_COLUMNS_SET")) {
        for (auto& column: columns)
        {
            if (column.isRemovebal == false)
                ImGui::BeginDisabled();

            bool Status = column.isDisable == false;

            if (ImGui::Checkbox(column.name.c_str(), &Status) && column.isRemovebal) {
                column.isDisable = !column.isDisable;
            }

            if (column.isRemovebal == false)
                ImGui::EndDisabled();
        }

        ImGui::EndPopup();
    }
    Pop_SmallFont();


}

void Timeline_Layers_List::Draw() {

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 region_max = ImGui::GetContentRegionAvail();
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

    ImVec2 mouse_pos = { ImGui::GetMousePos().x - cursor_pos.x,ImGui::GetMousePos().y - cursor_pos.y };

    ImVec2 p_min = cursor_pos;
    ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

    float itemHeight = verticalTransform->getScaledHeightOneObject();

    ImVec2 p = ImGui::GetCursorScreenPos();

    auto layers = layersData->GetData().layers;
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
            
            
            properties.forEachUnwrapped([&](const std::string& name, auto& value) {

                Push_SmallFont();

                float textHeight = ImGui::GetTextLineHeight();
                float offsetY = (itemHeight - textHeight) * 0.5f;


                float offsetX = 0.f;

                if (columns[MARKER].isDisable == false)
                    offsetX = columns[MARKER].startCursorPos;
                else if (columns[INDEX].isDisable == false)
                    offsetX = columns[INDEX].startCursorPos;
                else
                    offsetX = columns[NAME].startCursorPos;

                ImGui::SetCursorScreenPos(ImVec2(p.x + offsetX, p.y - offsetY - 1.f));

                bool enable = value.kr.getKeyframes().empty() == false;


                if (drawAnimationEnabler(enable, ("##PROPERTIES_TRIGGER_" + std::to_string(layer.get()->GetLayerInfo().index) + "_" + name).c_str(), itemHeight)) {
                    if (value.kr.getKeyframes().empty())
                        value.kr.addFromDefaultValue(0.f);
                    else
                        value.kr.clear();
                }


                ImGui::SameLine(0.f, 10.f);


                draw_list->AddText({ p.x + offsetX + 22.f,p.y - offsetY + 1.f }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), name.c_str());

                Pop_SmallFont();


                p.y += itemHeight;
                draw_list->AddLine({ p_min.x, round(p.y) }, { p_max.x, round(p.y) }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);

                ImGui::SetCursorScreenPos(p);
             });



        }

	}


}


void Timeline_Layers_List::Update() {

}