#include "Timeline.Layers.List.h"

#include "Core/Animation/Widgets/Panel/Colors/Panel.Colors.h"
#include "CursorManager/CursorManger.h"
#include "FontManager/FontManager.h"
#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"
#include "Utils/Text/Text.h"
#include "Utils/Text/Encoding/UTF8/UTF8.h"
#include "Utils/ImGui/Utils.ImGui.h"
#include <cstring>

Timeline_Layers_List::Timeline_Layers_List(LayersDataProvider* layersData, TimelineProviderData* timelineData, TimelineTransformHorizontal* horizontalTransform, TimelineTransformVertical* verticalTransform)
	: layersData(layersData),
      timelineData (timelineData),
      horizontalTransform(horizontalTransform),
	  verticalTransform(verticalTransform)
{

    columns.resize(8);
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
        true,
        NOTHING,
        NOTHING
    };
    columns[TypeColumn::RENDER_TIME] = {
        u8"Время рендеринга",
        120.f,
        120.f,
        0.f,
        true,
        true,
        true,
        true,
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
		TypeColumn::PARENT,
        TypeColumn::RENDER_TIME
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

    draw_list->AddRectFilled(p_min, p_max, COLOR_BG_BUTTONS, 3.f);

    ImColor result_color = COLOR_DEFAULT;
    if (value)
        result_color = isHovered ? COLOR_DEFAULT_HOVERED : COLOR_DEFAULT;
    else
        result_color = isHovered ? COLOR_DISABLED : COLOR_BG_BUTTONS;

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
        draw_list->AddRectFilled(p_min, p_max, COLOR_BG_BUTTONS, 3.f);

        if (isHovered == false && value)
            draw_list->AddRect(p_min, p_max, COLOR_DISABLED, 3.f, 0, 1.f);
        else
            draw_list->AddRect(p_min, p_max, COLOR_DEFAULT, 3.f, 0, 1.f);
    }



    ImColor result_color = default_color;

    if (value)
        result_color = isHovered ? COLOR_CHANGEBLE_HOVERED : COLOR_CHANGEBLE;
    else
        result_color = isHovered ? COLOR_DEFAULT_HOVERED : COLOR_DEFAULT;

    Push_SmallestFont();
    ImVec2 icon_size = ImGui::CalcTextSize(ICON_FA_STOPWATCH);
    draw_list->AddText({ p_min.x + (p_max.x - p_min.x) * 0.5f - icon_size.x * 0.5f + 1.f, p_min.y + (p_max.y - p_min.y) * 0.5f - icon_size.y * 0.5f }, result_color, ICON_FA_STOPWATCH);
    Pop_SmallestFont();

    return result;
}


void Timeline_Layers_List::DrawObject(LayerObject* object)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    std::string index = std::to_string(object->GetLayerInfo().index);
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
                object->GetLayerInfo().isVisible,
                ("##LAYER_VISIBLE_TRIGGER" + index).c_str(),
                ICON_FA_EYE,
                height_object
            );


            ImGui::SetCursorScreenPos(ImVec2(av_symbols_cursor_pos[AV_SYMBOLS::LOCK], startY));
            drawTriggerButton(
                object->GetLayerInfo().isLocked,
                ("##LAYER_LOCKED_TRIGGER" + index).c_str(),
                ICON_FA_LOCK,
                height_object
            );


            ImGui::SetCursorScreenPos(ImVec2(av_symbols_cursor_pos[AV_SYMBOLS::ONLY], startY));
            drawTriggerButton(
                object->GetLayerInfo().isOnly,
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

bool Timeline_Layers_List::DrawToggle(LayerObject* object) {
    std::string index = std::to_string(object->GetLayerInfo().index);
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
        object->GetLayerInfo().isExpanded = !object->GetLayerInfo().isExpanded;
        changed = true;
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const bool hovered = ImGui::IsItemHovered();

    ImU32 text_col = ImGui::GetColorU32(hovered ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    bool isExpanded = object->GetLayerInfo().isExpanded;

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
void Timeline_Layers_List::DrawLayerColor(LayerObject* object) {
    std::string index = std::to_string(object->GetLayerInfo().index);

    float c[3] = { 
        object->GetLayerInfo().color.r, 
        object->GetLayerInfo().color.g, 
        object->GetLayerInfo().color.b 
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

    if (ImGui::BeginDragDropTarget()) {
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

    object->GetLayerInfo().color = { c[0], c[1], c[2] };
}
void Timeline_Layers_List::DrawLayerIndex(LayerObject* object) {

    float itemHeight = verticalTransform->getScaledHeightOneObject();
    float textHeight = ImGui::GetTextLineHeight();
    float offsetY = (itemHeight - textHeight) * 0.5f - 1.f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);

    std::string str_index = std::to_string(object->GetLayerInfo().index);
    //PadLeft(str_index, '0', 3);

    Push_SmallFont();
    ImGui::TextColored(COLOR_DEFAULT, str_index.c_str());
    Pop_SmallFont();
}
bool Timeline_Layers_List::DrawLayerIconType(LayerObject* object) {

    float eval_time = timelineData->GetData().currentTime - object->GetLayerInfo().time.Begin;
    DrawIcon(object, verticalTransform->getScaledHeightOneObject(), eval_time);
    return false;
}
bool Timeline_Layers_List::DrawLayerName(LayerObject* object) {
    float itemHeight = verticalTransform->getScaledHeightOneObject();
    float textHeight = ImGui::GetTextLineHeight();
    float offsetY = (itemHeight - textHeight) * 0.5f - 1.f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);

    

    Push_SmallFont();
    ImGui::TextColored(COLOR_DEFAULT, object->GetLayerInfo().name.c_str());
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


    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_DEFAULT.Value);

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
            case TypeColumn::RENDER_TIME: {
                draw_list->AddText({ p.x,p.y + offsetY }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)), u8"Время рендеринга");
                p.x += ImGui::CalcTextSize(u8"Время рендеринга").x;

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
            // TODO:

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

    verticalTransform->updateAvailableHeight(region_max.y);

    ImVec2 mouse_pos = { ImGui::GetMousePos().x - cursor_pos.x,ImGui::GetMousePos().y - cursor_pos.y };


    ImVec2 p_min = cursor_pos;
    ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };
    float itemHeight = verticalTransform->getScaledHeightOneObject();

    if (ImGui::IsMouseHoveringRect(p_min, p_max)) {
        float scrol_zoom = ImGui::GetIO().MouseWheel;
        verticalTransform->addOffset(scrol_zoom * itemHeight);
    }





    ImVec2 p = ImGui::GetCursorScreenPos();

    p.y += verticalTransform->getOffset_lerped();

    float heightContent = 0.f;



    layersData->forEach([&](LayerObject* layer, uint64_t index) {
        ImGui::SetCursorScreenPos(p);
        auto& layerInfo = layer->GetLayerInfo();

        draw_list->AddRectFilled({ p_min.x, round(p.y + 1.f) }, { p_max.x, round(p.y + itemHeight) }, COLOR_BG_LAYER);

        if (index == 0) ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.f);

        DrawObject(layer);

        p.y += itemHeight;
        heightContent += itemHeight;
        draw_list->AddLine({ p_min.x, round(p.y) }, { p_max.x, round(p.y) }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);

        if (layerInfo.isExpanded) {

            auto& properties = layer->GetProperties();
            properties.forEachUnwrapped([&](const std::string& name, auto& value) {

                const auto& keyframes = value.kr.getKeyframes();

                Push_SmallFont();

                float textHeight = ImGui::GetTextLineHeight();
                float offsetY = (itemHeight - textHeight) * 0.5f;

                float offsetX = 0.f;
                bool isHaveKeyframes = keyframes.empty() == false;
                float eval_time = timelineData->GetData().currentTime - layerInfo.time.Begin;
                auto evaluated_value = value.kr.evaluate(eval_time);

                if (isHaveKeyframes) {
                    const float offset = 8.f;

                    const float ht = itemHeight / 2.3f;
                    const float wt = ht / 2.f;



                    ImVec2 T_1 = { offset + p.x + wt, p.y + itemHeight / 2.f - ht / 2.f };
                    ImVec2 T_2 = { offset + p.x + wt, p.y + itemHeight / 2.f + ht / 2.f };
                    ImVec2 T_3 = { offset + p.x, p.y + itemHeight / 2.f };
                    ImVec2 T_4 = { offset + p.x + wt * 2.f, p.y + itemHeight / 2.f };




                    ImVec2 TL[] = { T_1,T_2,T_3 };
                    ImVec2 TL_min = { T_1.x - wt - 5.f , T_1.y - 2.f };
                    ImVec2 TL_max = { T_1.x + 5.f - 2.f, T_1.y + ht + 2.f };
                    bool TL_hovered, TL_held;
                    bool TL_pressed = MyButtonBehavior(
                        TL_min,
                        TL_max,
                        (std::string("TL_PROPERTIE_") + name + std::to_string(index)).c_str(),
                        TL_hovered,
                        TL_held
                    );
                    bool TL_disabled = eval_time <= keyframes.front().sample.time;




                    T_1.x += offset * 1.5f;
                    T_2.x += offset * 1.5f;
                    T_3.x += offset * 1.5f;
                    T_4.x += offset * 1.5f;


                    const float sc = -1.f;
                    const float oo = 0.5f * 1.2f;
                    const float qq = 1.f;
                    ImVec2 ROMB[] = {
                        {T_4.x + sc, T_4.y - oo},
                        {T_4.x + sc, T_4.y + oo},
                        {T_2.x + oo, T_2.y + sc},
                        {T_2.x - oo, T_2.y + sc},
                        {T_3.x - sc, T_3.y + oo},
                        {T_3.x - sc, T_3.y - oo},
                        {T_1.x - oo, T_1.y - sc},
                        {T_1.x + oo, T_1.y - sc}
                    };
                    ImVec2 ROMB_over[] = {
                        {T_4.x + sc + qq, T_4.y - oo},
                        {T_4.x + sc + qq, T_4.y + oo},
                        {T_2.x + oo, T_2.y + sc + qq},
                        {T_2.x - oo, T_2.y + sc + qq},
                        {T_3.x - sc - qq, T_3.y + oo},
                        {T_3.x - sc - qq, T_3.y - oo},
                        {T_1.x - oo, T_1.y - sc - qq},
                        {T_1.x + oo, T_1.y - sc - qq}
                    };
                    ImVec2 ROMB_min = { T_3.x - 2.f, T_1.y - 2.f };
                    ImVec2 ROMB_max = { T_4.x + 2.f, T_2.y + 2.f };
                    bool ROMB_hovered, ROMB_held;
                    bool ROMB_pressed = MyButtonBehavior(
                        ROMB_min,
                        ROMB_max,
                        (std::string("ROMB_PROPERTIE_") + name + std::to_string(index)).c_str(),
                        ROMB_hovered,
                        ROMB_held
                    );

                    T_1.x += offset * 1.5f;
                    T_2.x += offset * 1.5f;
                    T_4.x += offset * 1.5f;



                    ImVec2 TR[] = { T_4,T_2,T_1 };
                    ImVec2 TR_min = { T_1.x - 2.f , T_1.y - 2.f };
                    ImVec2 TR_max = { T_1.x + wt + 5.f, T_1.y + ht + 2.f };
                    bool TR_hovered, TR_held;
                    bool TR_pressed = MyButtonBehavior(
                        TR_min,
                        TR_max,
                        (std::string("TR_PROPERTIE_") + name + std::to_string(index)).c_str(),
                        TR_hovered,
                        TR_held
                    );
                    bool TR_disabled = eval_time >= keyframes.back().sample.time;

                    if (TL_pressed && TL_disabled == false) {

                        for (size_t j = keyframes.size() - 1; j >= 0; j--) {
                            if (eval_time > keyframes[j].sample.time) {
                                timelineData->GetData().currentTime = keyframes[j].sample.time + layerInfo.time.Begin;
                                float left_border = horizontalTransform->getPostionScreen_real(horizontalTransform->getLeftBoundView_global_real());
                                float need_border = horizontalTransform->getPostionScreen_real(timelineData->GetData().currentTime);

                                if (need_border < left_border) {
                                    float dif = left_border - need_border;
                                    horizontalTransform->addOffset((dif + 100.f));
                                }
                                break;
                            }
                        }

                    }

                    if (TR_pressed && TR_disabled == false) {

                        for (size_t j = 0; j < keyframes.size(); j++) {
                            if (eval_time < keyframes[j].sample.time) {
                                timelineData->GetData().currentTime = keyframes[j].sample.time + layerInfo.time.Begin;

                                float right_border = horizontalTransform->getPostionScreen_real(horizontalTransform->getRightBoundView_global_real());
                                float need_border = horizontalTransform->getPostionScreen_real(timelineData->GetData().currentTime);

                                if (need_border > right_border) {
                                    float dif = need_border - right_border;
                                    horizontalTransform->addOffset(-(dif + 100.f));
                                }

                                break;
                            }
                        }

                    }

                    bool isEqual = false;
                    uint64_t indexEqual = 0;
                    for (size_t j = 0; j < keyframes.size(); j++) {
                        if (keyframes[j].sample.time == eval_time) {
                            isEqual = true;
                            indexEqual = j;
                            break;
                        }
                    }
                    if (ROMB_pressed) {
                        if (isEqual)
                            value.kr.erase(indexEqual);
                        else
                            value.kr.add(eval_time, evaluated_value);
                    }



                    ImColor color_tl = COLOR_DEFAULT;
                    if (TL_disabled)
                        color_tl = COLOR_DISABLED;
                    if (TL_hovered && !TL_disabled)
                        color_tl = COLOR_DEFAULT_HOVERED;



                    ImColor color_tr = COLOR_DEFAULT;
                    if (TR_disabled)
                        color_tr = COLOR_DISABLED;
                    if (TR_hovered && !TR_disabled)
                        color_tr = COLOR_DEFAULT_HOVERED;


                    ImColor color_romb = COLOR_DEFAULT;
                    ImColor color_romb_back = COLOR_BG_BUTTONS_HOVERED;
                    ImColor color_romb_bound = COLOR_DISABLED;


                    if (ROMB_hovered)
                        color_romb_bound = COLOR_DEFAULT_HOVERED;

                    if (isEqual) {
                        if (ROMB_hovered)
                            color_romb = COLOR_CHANGEBLE_HOVERED;
                        else
                            color_romb = COLOR_CHANGEBLE;

                        color_romb_back = color_romb;
                    }





                    if (TL_disabled == false && TL_hovered) {
                        draw_list->AddRectFilled({ TL_min.x - 0.5f, TL_min.y - 0.5f }, { TL_max.x + 0.5f, TL_max.y + 0.5f }, COLOR_DISABLED, 2.5f);
                        draw_list->AddRectFilled(TL_min, TL_max, COLOR_BG_BUTTONS, 2.5f);
                    }
                    draw_list->AddConvexPolyFilled(TL, 3, color_tl);

                    if (ROMB_hovered || isEqual) {
                        draw_list->AddRectFilled({ ROMB_min.x - 0.5f, ROMB_min.y - 0.5f }, { ROMB_max.x + 0.5f, ROMB_max.y + 0.5f }, color_romb_bound, 2.5f);
                        draw_list->AddRectFilled(ROMB_min, ROMB_max, COLOR_BG_BUTTONS, 2.5f);
                    }

                    draw_list->AddConvexPolyFilled(ROMB_over, 8, color_romb);
                    draw_list->AddConvexPolyFilled(ROMB, 8, color_romb_back);

                    if (TR_disabled == false && TR_hovered) {
                        draw_list->AddRectFilled({ TR_min.x - 0.5f, TR_min.y - 0.5f }, { TR_max.x + 0.5f, TR_max.y + 0.5f }, COLOR_DISABLED, 2.5f);
                        draw_list->AddRectFilled(TR_min, TR_max, COLOR_BG_BUTTONS, 2.5f);
                    }
                    draw_list->AddConvexPolyFilled(TR, 3, color_tr);

                }





                if (columns[MARKER].isDisable == false)
                    offsetX = columns[MARKER].startCursorPos;
                else if (columns[INDEX].isDisable == false)
                    offsetX = columns[INDEX].startCursorPos;
                else
                    offsetX = columns[NAME].startCursorPos;

                ImGui::SetCursorScreenPos(ImVec2(p.x + offsetX, p.y - offsetY - 1.f));




                if (drawAnimationEnabler(isHaveKeyframes, ("##PROPERTIES_TRIGGER_" + std::to_string(layer->GetLayerInfo().index) + "_" + name).c_str(), itemHeight)) {
                    if (keyframes.empty())
                        value.kr.addFromDefaultValue(eval_time);
                    else
                        value.kr.clear();
                }


                ImGui::SameLine(0.f, 10.f);


                draw_list->AddText({ p.x + offsetX + 22.f,p.y - offsetY + 1.f }, COLOR_DEFAULT, name.c_str());


                //draw_list->
                offsetX = columns[TOGGLES].startCursorPos;
                ImGui::SetCursorScreenPos(ImVec2(p.x + offsetX, p.y - offsetY + 1.f));


                float xxx = p.x;
                float yyy = p.y;
                p.y += offsetY + 1.f;
                p.x += offsetX;



                char text[32];
                auto AddButtonAndAdvance = [&]() {

                    for (size_t j = 0; j < evaluated_value.size(); j++) {
                        snprintf(text, sizeof(text), "%.3f", evaluated_value[j]);

                        ImVec2 sizeText = ImGui::CalcTextSize(text);

                        bool hovered, held;
                        MyButtonBehavior(
                            p,
                            { p.x + sizeText.x, p.y + sizeText.y },
                            (std::to_string(j) + std::string("PROPERTIE_") + name + std::to_string(index)).c_str(),
                            hovered,
                            held
                        );

                        ImColor color = COLOR_CHANGEBLE;

                        if (hovered || held) {
                            color = COLOR_CHANGEBLE_HOVERED;
                            CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);
                        }
                        if (held)
                            color = COLOR_CHANGEBLE_HELD;

                        if (held) {
                            float delta = ImGui::GetIO().MouseDelta.x * 0.01;

                            auto def_val = evaluated_value;
                            def_val[j] += delta;
                            value.kr.setDefaultValue(def_val);

                            if (value.kr.getKeyframes().empty() == false) {
                                value.kr.add(eval_time, def_val, TypeKeyframe::LINEAR);
                            }


                        }

                        draw_list->AddText(p, color, text);

                        p.x += ImGui::CalcTextSize(text).x + ImGui::GetStyle().ItemSpacing.x;

                        if (j != evaluated_value.size() - 1) {
                            draw_list->AddText(p, COLOR_DEFAULT, ", ");
                            p.x += ImGui::CalcTextSize(", ").x + ImGui::GetStyle().ItemSpacing.x;
                        }
                    }

                    };




                using ValueType = std::decay_t<decltype(value)>;


                static int choosed_index_for_color_picking = -1;

                if  constexpr (std::is_same_v<ValueType, TPColor>) {

                    float size_rect_color = itemHeight / 2.f;
                    p.x -= size_rect_color + 5.f;

                    ImVec2 p_rect_min = { p.x,-1.f + p.y + itemHeight / 2.f - size_rect_color / 2.f };
                    ImVec2 p_rect_max = { p.x + size_rect_color,-1.f + p.y + itemHeight / 2.f + size_rect_color / 2.f };

                    ImRect bb(p_rect_min, p_rect_max);
                    ImGuiID id = ImGui::GetID((std::to_string(0) + std::string("COLOR_BUTTON_PROPERTIE_") + name + std::to_string(index)).c_str());
                    bool hovered, held;
                    ImGui::ItemAdd(bb, id);
                    if (ImGui::ButtonBehavior(bb, id, &hovered, &held)) {
                        ImGui::OpenPopup(("POPUP_COLOR_SELECTOR_FOR_PROPERTIE" + std::to_string(index)).c_str());
                        choosed_index_for_color_picking = index;
                    }


                    ImColor color_show(evaluated_value[0], evaluated_value[1], evaluated_value[2], evaluated_value[3]);
                    ImColor color_border = COLOR_BG_BUTTONS;

                    if (hovered) {
                        color_border = COLOR_DISABLED;
                        CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
                    }

                    draw_list->AddRectFilled(p_rect_min, p_rect_max, color_show, 1.f);
                    p_rect_min.x -= 0.5f;
                    p_rect_min.y -= 0.5f;
                    p_rect_max.x += 0.5f;
                    p_rect_max.y += 0.5f;

                    draw_list->AddRect(p_rect_min, p_rect_max, color_border, 1.f, 0, 1.f);


                    p.x += size_rect_color + 5.f;
                }

                AddButtonAndAdvance();


                if (choosed_index_for_color_picking == index) {
                    if (ImGui::BeginPopupContextItem(("POPUP_COLOR_SELECTOR_FOR_PROPERTIE" + std::to_string(index)).c_str())) {

                        if (ImGui::ColorPicker4(("##picker_color_for_propertie##" + std::to_string(index)).c_str(),
                            (float*)evaluated_value.data(),
                            ImGuiColorEditFlags_NoSidePreview |
                            ImGuiColorEditFlags_NoSmallPreview |
                            ImGuiColorEditFlags_AlphaBar)) {


                            value.kr.setDefaultValue(evaluated_value);

                            if (value.kr.getKeyframes().empty() == false) {
                                value.kr.add(eval_time, evaluated_value, TypeKeyframe::LINEAR);
                            }
                        }


                        ImGui::EndPopup();
                    }
                }


                p.x = xxx;
                p.y = yyy;
                Pop_SmallFont();


                p.y += itemHeight;
                heightContent += itemHeight;
                draw_list->AddLine({ p_min.x, round(p.y) }, { p_max.x, round(p.y) }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);

                ImGui::SetCursorScreenPos(p);
                });

        }

    });

    verticalTransform->updateContentHeight(heightContent);
}


void Timeline_Layers_List::Update() {

}