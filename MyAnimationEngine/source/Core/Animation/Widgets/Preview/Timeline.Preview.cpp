#include "Timeline.Preview.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"

#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"

#include <ThirdParty/LivePT/LivePT.h>

#include "WindowManager/WindowManager.h"
#include "FontManager/FontManager.h"
#include "CursorManager/CursorManger.h"

#include <Utils/ImGui/Utils.ImGui.h>

#define IMVIEWGUIZMO_IMPLEMENTATION
#include "ThirdParty/ImViewGuizmo/ImViewGuizmo.h"

Timeline_Preview::Timeline_Preview(FPS_Timer* deltaTime, RendererAnimationScene* renderer) : deltaTime(deltaTime), renderer(renderer) {

	horizontal.setBoundsScale(0.3f, 1000.f);
	vertical.setBoundsScale(0.3f, 1000.f);

	TextureSetting ts;
	ts.Min = TextureFilter::LINEAR_MIPMAP_LINEAR;
	ts.Max = TextureFilter::LINEAR_MIPMAP_LINEAR;
	ts.WrapX = TextureWrap::CLAMP_TO_BORDER;
	ts.WrapY = TextureWrap::CLAMP_TO_BORDER;
    ts.Anisotropy = 1.f;

	temp = new Texture(ts, "resources\\images\\temp.jpg");

}
Timeline_Preview::~Timeline_Preview() {

}

void Timeline_Preview::DrawBottomMenu_Scale(float& pos_cursor_draw, const ImVec2& p_min, const ImVec2& p_max) {

    auto drawlist = ImGui::GetWindowDrawList();

    enum ScaleMode { SM_PERCENT = 0, SM_ORIGINAL, SM_AUTOFIT };

    static const char* const mode_names[] = {
        u8"Процент высоты",
        u8"Оригинал (пиксель в пиксель)",
        u8"AutoFit (в размер preview)"
    };

    static int   scale_mode = SM_AUTOFIT;
    static float percent_value = 100.f;
    static bool  center_lock = false;

    static bool  need_apply = true;
    static float applied_scale = -1.f;
    static float last_avail_w = -1.f, last_avail_h = -1.f;
    static int   last_tex_w = -1, last_tex_h = -1;

    const float avail_w = horizontal.getAvailableScreenValue();
    const float avail_h = vertical.getAvailableScreenValue();
    const float aspect = renderer->getTexture().GetAspectRatio();
    const int   tex_w = renderer->getTexture().GetWidth();
    const int   tex_h = renderer->getTexture().GetHeight();

    const bool size_valid = (avail_w > 1.f && avail_h > 1.f && aspect > 0.f && tex_w > 0 && tex_h > 0);

    float live_percent = 0.f;
    if (avail_h > 1.f)
        live_percent = (vertical.getLengthTick_real() * 2.f / avail_h) * 100.f;

    auto compute_target_height = [&]() -> float {
        switch (scale_mode) {
        case SM_PERCENT:  return percent_value * 0.01f * avail_h;
        case SM_ORIGINAL: return (float)tex_h;                    
        case SM_AUTOFIT:  return ImMin(avail_h, avail_w / aspect);
        }
        return 0.f;
        };

    auto apply_scale_mode = [&]() {
        if (vertical.getBaseStep() <= 0.f || horizontal.getBaseStep() <= 0.f)
            return;

        float ppt = compute_target_height() * 0.5f;

        const float smin = 0.3f, smax = 1000.f;
        float s_v = ImClamp(ppt / vertical.getBaseStep(), smin, smax);
        float s_h = ImClamp(ppt / horizontal.getBaseStep(), smin, smax);

        vertical.getScale().Set(s_v);
        horizontal.getScale().Set(s_h);

        applied_scale = s_v;
    };

    if (size_valid) {
        float actual_scale = vertical.getScale().GetReal();
        if (applied_scale > 0.f && std::fabs(actual_scale - applied_scale) > 1e-3f * ImMax(applied_scale, 1e-6f)) {
            scale_mode = SM_PERCENT;
            percent_value = live_percent;
            applied_scale = actual_scale;
        }

        bool size_changed =
            avail_w != last_avail_w || avail_h != last_avail_h ||
            tex_w != last_tex_w || tex_h != last_tex_h;

        if (need_apply || size_changed) {
            apply_scale_mode();


            if (need_apply || scale_mode == SM_AUTOFIT) {
                horizontal.centerize(0.f);
                vertical.centerize(0.f);
            }

            last_avail_w = avail_w; last_avail_h = avail_h;
            last_tex_w = tex_w;     last_tex_h = tex_h;
            need_apply = false;
        }
    }


    if (center_lock && !mouseIsDraging) {
        horizontal.centerize(0.f);
        vertical.centerize(0.f);
    }

    char label_buf[32];
    if (scale_mode == SM_PERCENT)
        snprintf(label_buf, sizeof(label_buf), "%d%%", (int)(live_percent + 0.5f));
    else if (scale_mode == SM_ORIGINAL)
        snprintf(label_buf, sizeof(label_buf), "1:1");
    else
        snprintf(label_buf, sizeof(label_buf), "Fit");

    ImVec2 size_scale_info = ImGui::CalcTextSize(label_buf);

    ImVec2 pos_text_min = { pos_cursor_draw, p_min.y + height_bottom_menu / 2.f - size_scale_info.y / 2.f };
    ImVec2 pos_text_max = { pos_text_min.x + size_scale_info.x, pos_text_min.y + size_scale_info.y };

    drawlist->AddText(pos_text_min, ImColor(1.f, 1.f, 1.f, 1.f), label_buf);

    pos_text_min.x -= 5.f;
    pos_text_min.y -= 0.5f;
    pos_text_max.x += 5.f;

    bool scale_hovered, scale_held;
    if (MyButtonBehavior(pos_text_min, pos_text_max, "PREVIEW_BUTTON_SCALE", scale_hovered, scale_held)) {
        ImGui::OpenPopup("POPUP_PREVIEW_SCALE");
    }

    if (scale_hovered || scale_held) {
        drawlist->AddRectFilled(pos_text_min, pos_text_max, ImColor(1.f, 1.f, 1.f, 0.1f), 2.f);
    }

    pos_cursor_draw += size_scale_info.x + padding;


    const char* text_center = "C";
    ImVec2 size_center = ImGui::CalcTextSize(text_center);

    ImVec2 pos_center_min = { pos_cursor_draw, p_min.y + height_bottom_menu / 2.f - size_center.y / 2.f };
    ImVec2 pos_center_max = { pos_center_min.x + size_center.x, pos_center_min.y + size_center.y };

    drawlist->AddText(pos_center_min, ImColor(1.f, 1.f, 1.f, 1.f), text_center);

    pos_center_min.x -= 5.f;
    pos_center_min.y -= 0.5f;
    pos_center_max.x += 5.f;

    bool center_hovered, center_held;
    if (MyButtonBehavior(pos_center_min, pos_center_max, "PREVIEW_BUTTON_SCALE_CENTER", center_hovered, center_held)) {
        center_lock = !center_lock;
    }

    if (center_lock)
        drawlist->AddRectFilled(pos_center_min, pos_center_max, ImColor(1.f, 1.f, 1.f, 0.25f), 2.f);
    else if (center_hovered || center_held)
        drawlist->AddRectFilled(pos_center_min, pos_center_max, ImColor(1.f, 1.f, 1.f, 0.1f), 2.f);

    pos_cursor_draw += size_center.x + padding;


    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.f, 12.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.f, 12.f));

    {
        float text_h = ImGui::GetTextLineHeight();
        float margin_elements_height = eval(7.f);
        float btn_padding = 6.f;

        const float tick_h = 12.f;
        const float tick_gap_text = 4.f;

        float row_h = text_h + 2.f * btn_padding + margin_elements_height;
        float slider_content_h = ImMax(text_h, text_h * 0.5f + tick_h * 0.5f + tick_gap_text + text_h);
        float slider_row_h = 2.f * btn_padding + slider_content_h + margin_elements_height * 1.5f;

        float pop_h = margin_elements_height + row_h + slider_row_h + margin_elements_height + 28.f;
        ImGui::SetNextWindowSize(ImVec2(620.f, pop_h));
    }

    if (ImGui::BeginPopup("POPUP_PREVIEW_SCALE")) {

        auto drawlist_popup = ImGui::GetWindowDrawList();

        ImVec2 currentCursor = ImGui::GetCursorScreenPos();
        ImVec2 sizePopup = ImGui::GetContentRegionAvail();

        ImVec2 pop_min = currentCursor;
        ImVec2 pop_max = { currentCursor.x + sizePopup.x, currentCursor.y + sizePopup.y };

        const char* str_mode = u8"Режим";
        const char* str_percent = u8"Процент";

        float margin_elements_height = eval(7.f);

        const float btn_padding = 6.f;
        const float btn_rounding = 15.f;
        const float btn_outline_thikness = 2.f;

        float current_cursor_height = margin_elements_height;

        auto draw_selector_row = [&](const char* label, const char* value, const char* btn_id) -> bool {

            ImVec2 size_label = ImGui::CalcTextSize(label);

            ImVec2 pos_label = { pop_min.x + btn_padding, pop_min.y + current_cursor_height };
            drawlist_popup->AddText(pos_label, ImColor(1.f, 1.f, 1.f, 1.f), label);

            ImVec2 min_btn = { pop_min.x + btn_padding + size_label.x + 2.f * btn_padding, pop_min.y + current_cursor_height - btn_padding };
            ImVec2 max_btn = { pop_max.x - btn_padding, pop_min.y + current_cursor_height + size_label.y + btn_padding };

            ImVec2 pos_value = { min_btn.x + 2.f * btn_padding, pop_min.y + current_cursor_height };

            bool btn_held, btn_hovered, btn_pressed;
            btn_pressed = MyButtonBehavior(min_btn, max_btn, btn_id, btn_hovered, btn_held);

            ImColor color_outline = COLOR_CHANGEBLE_DARK;

            if (btn_held)
                color_outline = COLOR_CHANGEBLE_HELD;
            else if (btn_hovered)
                color_outline = COLOR_CHANGEBLE_HOVERED;

            drawlist_popup->AddRectFilled(min_btn, max_btn, COLOR_BG_BUTTONS, btn_rounding);
            drawlist_popup->AddRect(min_btn, max_btn, color_outline, btn_rounding, 0, btn_outline_thikness);
            drawlist_popup->AddText(pos_value, ImColor(1.f, 1.f, 1.f, 1.f), value);

            float width_trig = btn_padding * 5.f;
            float shrink_val = 2.f;

            ImVec2 min_trig = { max_btn.x - width_trig - btn_padding * 0.5f + shrink_val, min_btn.y + shrink_val };
            ImVec2 max_trig = { max_btn.x - btn_padding * 0.5f - shrink_val, max_btn.y - shrink_val };

            ImVec2 p1 = { (3.f * min_trig.x + max_trig.x) / 4.f, (3.f * min_trig.y + max_trig.y) / 4.f };
            ImVec2 p2 = { (min_trig.x + 3.f * max_trig.x) / 4.f, (3.f * min_trig.y + max_trig.y) / 4.f };
            ImVec2 p3 = { (min_trig.x + max_trig.x) / 2.f, (min_trig.y + 3.f * max_trig.y) / 4.f };

            drawlist_popup->AddTriangleFilled(p1, p2, p3, color_outline);

            current_cursor_height += btn_padding * 2.f + size_label.y + margin_elements_height;

            return btn_pressed;
            };

        if (draw_selector_row(str_mode, mode_names[scale_mode], "BTN_MODE_PREVIEW_SCALE"))
            ImGui::OpenPopup("POPUP_PREVIEW_SCALE_MODE_LIST");


        static const int percent_steps[] = { 10, 15, 25, 50, 75, 100, 150, 200, 400, 800, 1600 };
        const int count = IM_ARRAYSIZE(percent_steps);

        const float tick_h = 12.f;
        const float tick_gap_text = 4.f;
        const float tick_w = 2.f;
        const float tick_w_active = 4.f;
        const float track_thick = 2.f;
        const float text_h = ImGui::GetTextLineHeight();

        ImVec2 size_label = ImGui::CalcTextSize(str_percent);
        ImVec2 pos_label = { pop_min.x + btn_padding, pop_min.y + current_cursor_height };
        drawlist_popup->AddText(pos_label, ImColor(1.f, 1.f, 1.f, 1.f), str_percent);

        char val_buf[16];
        snprintf(val_buf, sizeof(val_buf), "%d%%", (int)(live_percent + 0.5f));
        drawlist_popup->AddText({ pos_label.x + size_label.x + 8.f, pos_label.y }, ImColor(0.55f, 0.55f, 0.55f, 1.f), val_buf);

        float label_width = size_label.x + 8.f + ImGui::CalcTextSize(val_buf).x;

        int active_idx = -1;
        if (scale_mode == SM_PERCENT) {
            float best = FLT_MAX;
            for (int i = 0; i < count; i++) {
                float d = std::fabs(percent_value - (float)percent_steps[i]);
                if (d < best) { best = d; active_idx = i; }
            }
        }

        float slider_min_x = pop_min.x + btn_padding + label_width + 2.f * btn_padding + tick_w_active * 0.5f;
        float slider_max_x = pop_max.x - btn_padding - tick_w_active * 0.5f;
        float track_y = pos_label.y + size_label.y * 0.5f;

        ImVec2 zone_min = { slider_min_x - tick_w_active, pos_label.y };
        ImVec2 zone_max = { slider_max_x + tick_w_active, track_y + tick_h * 0.5f + tick_gap_text + text_h };

        bool sl_hovered = false, sl_held = false;
        bool sl_pressed = MyButtonBehavior(zone_min, zone_max, "SLIDER_PERCENT_PREVIEW_SCALE", sl_hovered, sl_held);

        if (sl_pressed || sl_held) {
            float denom = (float)(count - 1);
            float t = (denom > 0.f) ? (ImGui::GetIO().MousePos.x - slider_min_x) / (slider_max_x - slider_min_x) : 0.f;
            t = ImClamp(t, 0.f, 1.f);
            int idx = ImClamp((int)(t * denom + 0.5f), 0, count - 1);
            percent_value = (float)percent_steps[idx];
            scale_mode = SM_PERCENT;
            need_apply = true;
            active_idx = idx;
        }

        ImColor color_track = (sl_hovered || sl_held) ? ImColor(1.f, 1.f, 1.f, 0.45f) : ImColor(1.f, 1.f, 1.f, 0.25f);
        drawlist_popup->AddLine(ImVec2(slider_min_x, track_y), ImVec2(slider_max_x, track_y), color_track, track_thick);

        for (int i = 0; i < count; i++) {

            float x = slider_min_x + (slider_max_x - slider_min_x) * (float)i / (float)(count - 1);
            bool active = (i == active_idx);

            float w = active ? tick_w_active : tick_w;
            ImVec2 tick_min = { x - w * 0.5f, track_y - tick_h * 0.5f };
            ImVec2 tick_max = { x + w * 0.5f,  track_y + tick_h * 0.5f };
            ImColor color_tick = active ? ImColor(1.f, 1.f, 1.f, 1.f) : ImColor(1.f, 1.f, 1.f, 0.40f);
            drawlist_popup->AddRectFilled(tick_min, tick_max, color_tick, w * 0.5f);

            char tick_text[16];
            snprintf(tick_text, sizeof(tick_text), "%d%%", percent_steps[i]);
            ImVec2 size_tick_text = ImGui::CalcTextSize(tick_text);
            ImVec2 pos_tick_text = { x - size_tick_text.x * 0.5f, track_y + tick_h * 0.5f + tick_gap_text };
            ImColor color_tick_text = active ? ImColor(1.f, 1.f, 1.f, 1.f) : ImColor(0.55f, 0.55f, 0.55f, 1.f);
            drawlist_popup->AddText(pos_tick_text, color_tick_text, tick_text);
        }

        auto draw_choice_popup = [&](const char* popup_id, const std::vector<std::string>& items, int selected) {
            int chosen = -1;
            if (ImGui::BeginPopup(popup_id)) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.f, 3.f));
                float item_height = ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().FramePadding.y * 2.f;
                float text_width = 0.f;
                for (const auto& s : items)
                    text_width = ImMax(text_width, ImGui::CalcTextSize(s.c_str()).x);
                float width = text_width + ImGui::GetStyle().FramePadding.x * 4.f + ImGui::GetStyle().WindowPadding.x * 2.f + ImGui::GetFontSize();
                float height = ImMin(item_height * (float)items.size() + ImGui::GetStyle().WindowPadding.y * 2.f, item_height * 10.f);
                ImGui::BeginListBox("##choice_list", ImVec2(width, height));
                for (int i = 0; i < (int)items.size(); i++)
                    if (ImGui::Selectable(items[i].c_str(), i == selected)) {
                        chosen = i;
                        ImGui::CloseCurrentPopup();
                    }
                ImGui::EndListBox();
                ImGui::PopStyleVar();
                ImGui::EndPopup();
            }
            return chosen;
            };

        {
            std::vector<std::string> items;
            for (const char* m : mode_names)
                items.push_back(m);
            int chosen = draw_choice_popup("POPUP_PREVIEW_SCALE_MODE_LIST", items, scale_mode);
            if (chosen >= 0) {
                scale_mode = chosen;
                need_apply = true;
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(2);
}

void Timeline_Preview::DrawBottomMenu_TextureSetting(float& pos_cursor_draw, const ImVec2& p_min, const ImVec2& p_max) {

    auto drawlist = ImGui::GetWindowDrawList();

    struct NamedInt {
        int value;
        const char* name;
    };

    static const std::vector<NamedInt> min_filters = {
        { (int)TextureFilter::NEAREST,					"Nearest" },
        { (int)TextureFilter::LINEAR,					"Linear [Bilinear]" },
        { (int)TextureFilter::NEAREST_MIPMAP_NEAREST,	"Nearest (mipmap nearest)" },
        { (int)TextureFilter::LINEAR_MIPMAP_NEAREST,	"Linear (mipmap nearest)" },
        { (int)TextureFilter::NEAREST_MIPMAP_LINEAR,	"Nearest (mipmap linear)" },
        { (int)TextureFilter::LINEAR_MIPMAP_LINEAR,		"Linear (mipmap linear) [Trilinear]" },
    };

    static const std::vector<NamedInt> mag_filters = {
        { (int)TextureFilter::NEAREST,	"Nearest" },
        { (int)TextureFilter::LINEAR,	"Linear" }
    };

    static const std::vector<NamedInt> wraps = {
        { (int)TextureWrap::REPEAT,			"Repeat" },
        { (int)TextureWrap::MIRRORED_REPEAT,"Mirrored repeat" },
        { (int)TextureWrap::CLAMP_TO_EDGE,	"Clamp to edge" },
        { (int)TextureWrap::CLAMP_TO_BORDER,"Clamp to border" },
    };

    static const std::vector<NamedInt> anisotropy_values = {
        { 1,	"1x" },
        { 2,	"2x" },
        { 4,	"4x" },
        { 8,	"8x" },
        { 16,	"16x" },
    };

    static TextureSetting ts;

    if (!ImGui::IsPopupOpen("POPUP_PREVIEW_TEXTURE_SETTING"))
        ts = renderer->getTexture().GetSetting();

    auto find_name = [](const std::vector<NamedInt>& list, int value) -> const char* {
        for (const auto& e : list)
            if (e.value == value)
                return e.name;
        return "-";
     };

    auto find_index = [](const std::vector<NamedInt>& list, int value) -> int {
        for (int i = 0; i < (int)list.size(); i++)
            if (list[i].value == value)
                return i;
        return -1;
    };

    auto draw_choice_popup = [&](const char* popup_id, const std::vector<std::string>& items, int selected) {
        int chosen = -1;
        if (ImGui::BeginPopup(popup_id)) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.f, 3.f));
            float item_height = ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().FramePadding.y * 2.f;
            float text_width = 0.f;
            for (const auto& s : items)
                text_width = ImMax(text_width, ImGui::CalcTextSize(s.c_str()).x);
            float width = text_width + ImGui::GetStyle().FramePadding.x * 4.f + ImGui::GetStyle().WindowPadding.x * 2.f + ImGui::GetFontSize();
            float height = ImMin(item_height * (float)items.size() + ImGui::GetStyle().WindowPadding.y * 2.f, item_height * 10.f);
            ImGui::BeginListBox("##choice_list", ImVec2(width, height));
            for (int i = 0; i < (int)items.size(); i++)
                if (ImGui::Selectable(items[i].c_str(), i == selected)) {
                    chosen = i;
                    ImGui::CloseCurrentPopup();
                }
            ImGui::EndListBox();
            ImGui::PopStyleVar();
            ImGui::EndPopup();
        }
        return chosen;
    };

    const char* text_texture_info = u8"TS";

    ImVec2 size_texture_info = ImGui::CalcTextSize(text_texture_info);

    ImVec2 pos_text_min = { pos_cursor_draw, p_min.y + height_bottom_menu / 2.f - size_texture_info.y / 2.f };
    ImVec2 pos_text_max = { pos_text_min.x + size_texture_info.x, pos_text_min.y + size_texture_info.y };

    drawlist->AddText(pos_text_min, ImColor(1.f, 1.f, 1.f, 1.f), text_texture_info);

    pos_text_min.x -= 5.f;
    pos_text_min.y -= 0.5f;
    pos_text_max.x += 5.f;

    bool hovered, held;
    if (MyButtonBehavior(pos_text_min, pos_text_max, "PREVIEW_BUTTON_TEXTURE_SETTING", hovered, held)) {
        ImGui::OpenPopup("POPUP_PREVIEW_TEXTURE_SETTING");
    }

    if (hovered || held) {
        drawlist->AddRectFilled(pos_text_min, pos_text_max, ImColor(1.f, 1.f, 1.f, 0.1f), 2.f);
    }


    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.f, 12.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.f, 12.f));

    {
        float text_h = ImGui::GetTextLineHeight();
        float margin_elements_height = eval(7.f);
        float btn_padding = 6.f;
        float btn_margin = 1.f;

        float row_h = text_h + 2.f * btn_padding + margin_elements_height;
        float apply_h = text_h + 2.f * btn_padding + btn_margin;
        float pop_h = margin_elements_height + row_h * 5.f + apply_h + margin_elements_height + 28.f;
        ImGui::SetNextWindowSize(ImVec2(460.f, pop_h));
    }

    if (ImGui::BeginPopup("POPUP_PREVIEW_TEXTURE_SETTING")) {

        auto drawlist_popup = ImGui::GetWindowDrawList();

        ImVec2 currentCursor = ImGui::GetCursorScreenPos();
        ImVec2 sizePopup = ImGui::GetContentRegionAvail();

        ImVec2 pop_min = currentCursor;
        ImVec2 pop_max = { currentCursor.x + sizePopup.x, currentCursor.y + sizePopup.y };
        ImVec2 pop_center = { (pop_min.x + pop_max.x) / 2.f, (pop_min.y + pop_max.y) / 2.f };

        const char* str_min_filter = u8"Фильтр (уменьшение)";
        const char* str_mag_filter = u8"Фильтр (увеличение)";
        const char* str_wrap_x = u8"Повтор по X";
        const char* str_wrap_y = u8"Повтор по Y";
        const char* str_anisotropy = u8"Анизантропная\n фильтрация";
        const char* str_apply = u8"Применить";

        ImVec2 size_apply_text = ImGui::CalcTextSize(str_apply);

        float margin_elements_height = eval(7.f);

        const float btn_padding = 6.f;
        const float btn_margin = 1.f;
        const float btn_rounding = 15.f;
        const float btn_outline_thikness = 2.f;

        float current_cursor_height = margin_elements_height;

        auto draw_selector_row = [&](const char* label, const char* value, const char* btn_id) -> bool {

            ImVec2 size_label = ImGui::CalcTextSize(label);

            ImVec2 pos_label = { pop_min.x + btn_padding, pop_min.y + current_cursor_height };
            drawlist_popup->AddText(pos_label, ImColor(1.f, 1.f, 1.f, 1.f), label);

            ImVec2 min_btn = { pop_min.x + btn_padding + size_label.x + 2.f * btn_padding, pop_min.y + current_cursor_height - btn_padding };
            ImVec2 max_btn = { pop_max.x - btn_padding, pop_min.y + current_cursor_height + size_label.y + btn_padding };

            ImVec2 pos_value = { min_btn.x + 2.f * btn_padding, pop_min.y + current_cursor_height };

            bool btn_held, btn_hovered, btn_pressed;
            btn_pressed = MyButtonBehavior(min_btn, max_btn, btn_id, btn_hovered, btn_held);

            ImColor color_outline = COLOR_CHANGEBLE_DARK;

            if (btn_held)
                color_outline = COLOR_CHANGEBLE_HELD;
            else if (btn_hovered)
                color_outline = COLOR_CHANGEBLE_HOVERED;

            drawlist_popup->AddRectFilled(min_btn, max_btn, COLOR_BG_BUTTONS, btn_rounding);
            drawlist_popup->AddRect(min_btn, max_btn, color_outline, btn_rounding, 0, btn_outline_thikness);
            drawlist_popup->AddText(pos_value, ImColor(1.f, 1.f, 1.f, 1.f), value);

            float width_trig = btn_padding * 5.f;
            float shrink_val = 2.f;

            ImVec2 min_trig = { max_btn.x - width_trig - btn_padding * 0.5f + shrink_val, min_btn.y + shrink_val };
            ImVec2 max_trig = { max_btn.x - btn_padding * 0.5f - shrink_val, max_btn.y - shrink_val };

            ImVec2 p1 = { (3.f * min_trig.x + max_trig.x) / 4.f, (3.f * min_trig.y + max_trig.y) / 4.f };
            ImVec2 p2 = { (min_trig.x + 3.f * max_trig.x) / 4.f, (3.f * min_trig.y + max_trig.y) / 4.f };
            ImVec2 p3 = { (min_trig.x + max_trig.x) / 2.f, (min_trig.y + 3.f * max_trig.y) / 4.f };

            drawlist_popup->AddTriangleFilled(p1, p2, p3, color_outline);

            current_cursor_height += btn_padding * 2.f + size_label.y + margin_elements_height;

            return btn_pressed;
        };


        const float aniso_tick_h = 12.f;
        const float aniso_tick_gap_text = 4.f;
        auto draw_anisotropy_slider = [&](const char* label, const char* slider_id) -> void {

            const int   count = (int)anisotropy_values.size();
            const float tick_h = aniso_tick_h;
            const float tick_w = 2.f;
            const float tick_w_active = 4.f;
            const float track_thick = 2.f;
            const float text_h = ImGui::GetTextLineHeight();

            int active_idx = find_index(anisotropy_values, (int)ts.Anisotropy);
            if (active_idx < 0)
                active_idx = 0;

            ImVec2 size_label = ImGui::CalcTextSize(label);

            ImVec2 pos_label = { pop_min.x + btn_padding, pop_min.y + current_cursor_height };
            drawlist_popup->AddText(pos_label, ImColor(1.f, 1.f, 1.f, 1.f), label);

            float slider_min_x = pop_min.x + btn_padding + size_label.x + 2.f * btn_padding + tick_w_active * 0.5f;
            float slider_max_x = pop_max.x - btn_padding - tick_w_active * 0.5f;
            float track_y = pos_label.y + size_label.y * 0.5f;

            ImVec2 zone_min = { slider_min_x - tick_w_active, pos_label.y };
            ImVec2 zone_max = { slider_max_x + tick_w_active, track_y + tick_h * 0.5f + aniso_tick_gap_text + text_h };

            bool sl_hovered = false, sl_held = false;
            bool sl_pressed = MyButtonBehavior(zone_min, zone_max, slider_id, sl_hovered, sl_held);

            if (sl_pressed || sl_held) {
                float denom = (float)(count - 1);
                float t = (denom > 0.f) ? (ImGui::GetIO().MousePos.x - slider_min_x) / (slider_max_x - slider_min_x) : 0.f;
                t = ImClamp(t, 0.f, 1.f);
                int idx = ImClamp((int)(t * denom + 0.5f), 0, count - 1);
                ts.Anisotropy = (float)anisotropy_values[idx].value;
                active_idx = idx;
            }

            ImColor color_track = (sl_hovered || sl_held) ? ImColor(1.f, 1.f, 1.f, 0.45f) : ImColor(1.f, 1.f, 1.f, 0.25f);
            drawlist_popup->AddLine(ImVec2(slider_min_x, track_y), ImVec2(slider_max_x, track_y), color_track, track_thick);

            for (int i = 0; i < count; i++) {

                float x = slider_min_x + (slider_max_x - slider_min_x) * (float)i / (float)(count - 1);
                bool active = (i == active_idx);

                float w = active ? tick_w_active : tick_w;
                ImVec2 tick_min = { x - w * 0.5f, track_y - tick_h * 0.5f };
                ImVec2 tick_max = { x + w * 0.5f,  track_y + tick_h * 0.5f };
                ImColor color_tick = active ? ImColor(1.f, 1.f, 1.f, 1.f) : ImColor(1.f, 1.f, 1.f, 0.40f);
                drawlist_popup->AddRectFilled(tick_min, tick_max, color_tick, w * 0.5f);

                const char* tick_text = anisotropy_values[i].name;
                ImVec2 size_tick_text = ImGui::CalcTextSize(tick_text);
                ImVec2 pos_tick_text = { x - size_tick_text.x * 0.5f, track_y + tick_h * 0.5f + aniso_tick_gap_text };
                ImColor color_tick_text = active ? ImColor(1.f, 1.f, 1.f, 1.f) : ImColor(0.55f, 0.55f, 0.55f, 1.f);
                drawlist_popup->AddText(pos_tick_text, color_tick_text, tick_text);
            }

            float content_h = ImMax(size_label.y, size_label.y * 0.5f + tick_h * 0.5f + aniso_tick_gap_text + text_h);
            current_cursor_height += btn_padding * 2.f + content_h + margin_elements_height*1.5f;
        };





        if (draw_selector_row(str_min_filter, find_name(min_filters, (int)ts.Min), "BTN_MIN_FILTER_PREVIEW_TEXTURE"))
            ImGui::OpenPopup("POPUP_PREVIEW_MIN_FILTER_LIST");

        if (draw_selector_row(str_mag_filter, find_name(mag_filters, (int)ts.Max), "BTN_MAG_FILTER_PREVIEW_TEXTURE"))
            ImGui::OpenPopup("POPUP_PREVIEW_MAG_FILTER_LIST");

        if (draw_selector_row(str_wrap_x, find_name(wraps, (int)ts.WrapX), "BTN_WRAP_X_PREVIEW_TEXTURE"))
            ImGui::OpenPopup("POPUP_PREVIEW_WRAP_X_LIST");

        if (draw_selector_row(str_wrap_y, find_name(wraps, (int)ts.WrapY), "BTN_WRAP_Y_PREVIEW_TEXTURE"))
            ImGui::OpenPopup("POPUP_PREVIEW_WRAP_Y_LIST");

        draw_anisotropy_slider(str_anisotropy, "SLIDER_ANISOTROPY_PREVIEW_TEXTURE");

        {
            std::vector<std::string> items;
            for (const auto& e : min_filters) items.push_back(e.name);
            int chosen = draw_choice_popup("POPUP_PREVIEW_MIN_FILTER_LIST", items, find_index(min_filters, (int)ts.Min));
            if (chosen >= 0) ts.Min = (TextureFilter)min_filters[chosen].value;
        }
        {
            std::vector<std::string> items;
            for (const auto& e : mag_filters) items.push_back(e.name);
            int chosen = draw_choice_popup("POPUP_PREVIEW_MAG_FILTER_LIST", items, find_index(mag_filters, (int)ts.Max));
            if (chosen >= 0) ts.Max = (TextureFilter)mag_filters[chosen].value;
        }
        {
            std::vector<std::string> items;
            for (const auto& e : wraps) items.push_back(e.name);
            int chosen = draw_choice_popup("POPUP_PREVIEW_WRAP_X_LIST", items, find_index(wraps, (int)ts.WrapX));
            if (chosen >= 0) ts.WrapX = (TextureWrap)wraps[chosen].value;
        }
        {
            std::vector<std::string> items;
            for (const auto& e : wraps) items.push_back(e.name);
            int chosen = draw_choice_popup("POPUP_PREVIEW_WRAP_Y_LIST", items, find_index(wraps, (int)ts.WrapY));
            if (chosen >= 0) ts.WrapY = (TextureWrap)wraps[chosen].value;
        }


        ImVec2 min_btn_apply = { pop_center.x - size_apply_text.x / 2.f - btn_padding * 2.f, pop_max.y - btn_margin - size_apply_text.y - 2.f * btn_padding };
        ImVec2 max_btn_apply = { pop_center.x + size_apply_text.x / 2.f + btn_padding * 2.f, pop_max.y - btn_margin };

        ImVec2 pos_text_btn_apply = { pop_center.x - size_apply_text.x / 2.f, pop_max.y - btn_margin - size_apply_text.y - btn_padding };

        bool btn_apply_held, btn_apply_hovered, btn_apply_pressed;
        btn_apply_pressed = MyButtonBehavior(min_btn_apply, max_btn_apply, "BTN_APPLY_PREVIEW_TEXTURE_SETTING", btn_apply_hovered, btn_apply_held);

        ImColor color_outline_apply = COLOR_CHANGEBLE_DARK;

        if (btn_apply_held)
            color_outline_apply = COLOR_CHANGEBLE_HELD;
        else if (btn_apply_hovered)
            color_outline_apply = COLOR_CHANGEBLE_HOVERED;

        drawlist_popup->AddRectFilled(min_btn_apply, max_btn_apply, COLOR_BG_BUTTONS, btn_rounding);
        drawlist_popup->AddRect(min_btn_apply, max_btn_apply, color_outline_apply, btn_rounding, 0, btn_outline_thikness);
        drawlist_popup->AddText(pos_text_btn_apply, ImColor(1.f, 1.f, 1.f, 1.f), str_apply);

        if (btn_apply_pressed) {
            renderer->getTexture().SetSetting(ts);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(2);

    pos_cursor_draw += size_texture_info.x + padding;
}

void Timeline_Preview::DrawBottomMenu_Resolution(float& pos_cursor_draw, const ImVec2& p_min, const ImVec2& p_max) {
    auto drawlist = ImGui::GetWindowDrawList();

    std::string text_texture_size_info = std::to_string(renderer->getTexture().GetWidth()) + " x " + std::to_string(renderer->getTexture().GetHeight());
    ImVec2 size_texture_size_info = ImGui::CalcTextSize(text_texture_size_info.c_str());

    ImVec2 pos_text_min = { pos_cursor_draw, p_min.y + height_bottom_menu / 2.f - size_texture_size_info.y / 2.f };
    ImVec2 pos_text_max = { pos_text_min.x + size_texture_size_info.x, pos_text_min.y + size_texture_size_info.y };

    drawlist->AddText(pos_text_min, ImColor(1.f, 1.f, 1.f, 1.f), text_texture_size_info.c_str());

    pos_text_min.x -= 5.f;
    pos_text_min.y -= 0.5f;
    pos_text_max.x += 5.f;

    bool hovered, held;
    if (MyButtonBehavior(pos_text_min, pos_text_max, "PREVIEW_BUTTON_RESOLUTION", hovered, held)) {
        ImGui::OpenPopup("POPUP_PREVIEW_RESOLUTION");
    }

    struct ResolutionWithName {
        int first_size;
        int second_size;
        std::string name;
    };

    struct Ratio {
        int num;
        int denum;
    };

    static const std::vector<ResolutionWithName> resolutions = {
        {320,	240,	"QVGA"},
        {480,	320,	"HVGA"},
        {640,	360,	"nHD"},
        {640,	480,	"VGA"},
        {720,	480,	"NTSC"},
        {800,	480,	"WVGA"},
        {800,	600,	"SVGA"},
        {854,	480,	"FWVGA"},
        {960,	540,	"qHD"},
        {1024,	768,	"XGA"},
        {1152,	864,	"XGA+"},
        {1280,	720,	"HD"},
        {1280,	800,	"WXGA"},
        {1280,	1024,	"SXGA"},
        {1366,	768,	"FWXGA"},
        {1400,	1050,	"SXGA+"},
        {1440,	900,	"WXGA+"},
        {1600,	900,	"HD+"},
        {1600,	1200,	"UXGA"},
        {1680,	1050,	"WSXGA+"},
        {1920,	1080,	"Full HD"},
        {1920,	1200,	"WUXGA"},
        {2048,	858,	"DCI 2K"},
        {2048,	1152,	"QWXGA"},
        {2048,	1536,	"QXGA"},
        {2560,	1080,	"UWHD"},
        {2560,	1440,	"QHD"},
        {2560,	1600,	"WQXGA"},
        {2560,	2048,	"QSXGA"},
        {3200,	2400,	"QUXGA"},
        {3440,	1440,	"UWQHD"},
        {3840,	1080,	"DFHD"},
        {3840,	2160,	"4K QHD"},
        {3840,	2400,	"WQUXGA"},
        {4096,	1716,	"DCI 4K"},
        {5120,	1440,	"DWQHD"},
        {5120,	2160,	"UW5K"},
        {5120,	2880,	"5K"},
        {7680,	2160,	"DWUHD"},
        {7680,	4320,	"8K UHD"},
        {1024,	1024,	"1K"},
        {2048,	2048,	"2K"},
        {4096,	4096,	"4K"}
    };

    static const std::vector<Ratio> ratios = {
        {1, 1},
        {4, 3},
        {3, 2},
        {5, 3},
        {5, 4},
        {16, 9},
        {16, 10},
        {18, 9},
        {21, 9},
        {43, 18},
        {64, 27},
        {32, 9}
    };

    static const std::vector<int> standard_heights = {
        144, 240, 288, 320, 360, 400, 480, 540, 576, 600, 640, 720, 768, 800, 864, 900, 960,
        1024, 1050, 1080, 1152, 1200, 1280, 1440, 1536, 1600, 1800, 2048, 2160, 2400, 2560,
        2880, 3200, 3600, 4096, 4320
    };

    static int sel_ratio = 0;
    static int sel_w = 0;
    static int sel_h = 0;

    auto sync_from_texture = [&]() {
        int w = renderer->getTexture().GetWidth();
        int h = renderer->getTexture().GetHeight();
        sel_w = w;
        sel_h = h;
        if (w > 0 && h > 0) {
            float best_diff = FLT_MAX;
            for (int i = 0; i < (int)ratios.size(); i++) {
                float diff = std::fabs((float)ratios[i].num / (float)ratios[i].denum - (float)w / (float)h);
                if (diff < best_diff) {
                    best_diff = diff;
                    sel_ratio = i;
                }
            }
        }
        };

    if (!ImGui::IsPopupOpen("POPUP_PREVIEW_RESOLUTION"))
        sync_from_texture();

    auto get_ratio_resolutions = [&](const Ratio& ratio) {
        std::vector<ResolutionWithName> list;
        float ratio_value = (float)ratio.num / (float)ratio.denum;
        for (const auto& r : resolutions)
            if (std::fabs((float)r.first_size / (float)r.second_size - ratio_value) / ratio_value < 0.02f)
                list.push_back(r);
        for (int h : standard_heights) {
            int w = (int)std::lround((float)h * ratio.num / (float)ratio.denum);
            w = (w + 1) / 2 * 2;
            if (w < 64)
                continue;
            bool dup = false;
            for (const auto& r : list)
                if (r.second_size == h && std::abs(r.first_size - w) <= 2) {
                    dup = true;
                    break;
                }
            if (!dup)
                list.push_back({ w, h, "" });
        }
        std::sort(list.begin(), list.end(), [](const ResolutionWithName& a, const ResolutionWithName& b) {
            long long area_a = (long long)a.first_size * a.second_size;
            long long area_b = (long long)b.first_size * b.second_size;
            if (area_a != area_b)
                return area_a < area_b;
            return a.second_size < b.second_size;
            });
        return list;
        };

    auto diag = [](int w, int h) {
        return std::sqrt((float)w * (float)w + (float)h * (float)h);
        };

    auto pick_closest = [&](const std::vector<ResolutionWithName>& list, int w, int h) {
        int best = 0;
        float best_diff = FLT_MAX;
        for (int i = 0; i < (int)list.size(); i++) {
            float diff = std::fabs(diag(list[i].first_size, list[i].second_size) - diag(w, h));
            if (diff < best_diff) {
                best_diff = diff;
                best = i;
            }
        }
        return best;
        };

    auto ratio_label = [](const Ratio& r) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d:%d (%.2f)", r.num, r.denum, (float)r.num / (float)r.denum);
        return std::string(buf);
        };

    auto res_label = [](const ResolutionWithName& r) {
        std::string s = std::to_string(r.first_size) + " x " + std::to_string(r.second_size);
        if (!r.name.empty())
            s += "  (" + r.name + ")";
        return s;
        };

    auto draw_choice_popup = [&](const char* popup_id, const std::vector<std::string>& items, int selected) {
        int chosen = -1;
        if (ImGui::BeginPopup(popup_id)) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.f, 3.f));
            float item_height = ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().FramePadding.y * 2.f;
            float text_width = 0.f;
            for (const auto& s : items)
                text_width = ImMax(text_width, ImGui::CalcTextSize(s.c_str()).x);
            float width = text_width + ImGui::GetStyle().FramePadding.x * 4.f + ImGui::GetStyle().WindowPadding.x * 2.f + ImGui::GetFontSize();
            float height = ImMin(item_height * (float)items.size() + ImGui::GetStyle().WindowPadding.y * 2.f, item_height * 10.f);
            ImGui::BeginListBox("##choice_list", ImVec2(width, height));
            for (int i = 0; i < (int)items.size(); i++)
                if (ImGui::Selectable(items[i].c_str(), i == selected)) {
                    chosen = i;
                    ImGui::CloseCurrentPopup();
                }
            ImGui::EndListBox();
            ImGui::PopStyleVar();
            ImGui::EndPopup();
        }
        return chosen;
        };

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.f, 12.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.f, 12.f));

    ImGui::SetNextWindowSize(ImVec2(300, 130));

    if (ImGui::BeginPopup("POPUP_PREVIEW_RESOLUTION")) {

        auto drawlist_popup = ImGui::GetWindowDrawList();

        ImVec2 currentCursor = ImGui::GetCursorScreenPos();
        ImVec2 sizePopup = ImGui::GetContentRegionAvail();

        ImVec2 pop_min = currentCursor;
        ImVec2 pop_max = { currentCursor.x + sizePopup.x, currentCursor.y + sizePopup.y };
        ImVec2 pop_center = { (pop_min.x + pop_max.x) / 2.f, (pop_min.y + pop_max.y) / 2.f };

        const char* str_ratio = u8"Соотношение";
        const char* str_resolution = u8"Разрешение";
        const char* str_apply = u8"Применить";

        ImVec2 size_apply_text = ImGui::CalcTextSize(str_apply);
        ImVec2 size_ratio_text = ImGui::CalcTextSize(str_ratio);
        ImVec2 size_resolution_text = ImGui::CalcTextSize(str_resolution);

        float margin_elements_height = eval(7.f);

        const float btn_padding = 6.f;
        const float btn_margin = 1.f;
        const float btn_rounding = 15.f;
        const float btn_outline_thikness = 2.f;


        float current_cursor_height = margin_elements_height;


        // RATIO selectable
        ImVec2 pos_text_btn_ratio = { pop_min.x + btn_padding, pop_min.y + current_cursor_height };
        drawlist_popup->AddText(pos_text_btn_ratio, ImColor(1.f, 1.f, 1.f, 1.f), str_ratio);

        std::vector<ResolutionWithName> cur_list = get_ratio_resolutions(ratios[sel_ratio]);
        std::string ratio_btn_label = ratio_label(ratios[sel_ratio]);

        ImVec2 min_btn_ratio = { pop_min.x + btn_padding + size_ratio_text.x + 2.f * btn_padding, pop_min.y + current_cursor_height - btn_padding };
        ImVec2 max_btn_ratio = { pop_max.x - btn_padding, pop_min.y + current_cursor_height + size_ratio_text.y + btn_padding };

        ImVec2 pos_text_btn_ratio_value = { pop_min.x + btn_padding + size_ratio_text.x + 4.f * btn_padding, pop_min.y + current_cursor_height };

        bool btn_ratio_held, btn_ratio_hovered, btn_ratio_pressed;
        btn_ratio_pressed = MyButtonBehavior(min_btn_ratio, max_btn_ratio, "BTN_RATIO_PREVIEW_RESOLUTION", btn_ratio_hovered, btn_ratio_held);

        ImColor color_outline_ratio = COLOR_CHANGEBLE_DARK;

        if (btn_ratio_held)
            color_outline_ratio = COLOR_CHANGEBLE_HELD;
        else if (btn_ratio_hovered)
            color_outline_ratio = COLOR_CHANGEBLE_HOVERED;

        drawlist_popup->AddRectFilled(min_btn_ratio, max_btn_ratio, COLOR_BG_BUTTONS, btn_rounding);
        drawlist_popup->AddRect(min_btn_ratio, max_btn_ratio, color_outline_ratio, btn_rounding, 0, btn_outline_thikness);
        drawlist_popup->AddText(pos_text_btn_ratio_value, ImColor(1.f, 1.f, 1.f, 1.f), ratio_btn_label.c_str());

        float width_trig = btn_padding * 5.f;
        float shrink_val = 2.f;

        ImVec2 min_trig = { max_btn_ratio.x - width_trig - btn_padding * 0.5f + shrink_val, min_btn_ratio.y + shrink_val };
        ImVec2 max_trig = { max_btn_ratio.x - btn_padding * 0.5f - shrink_val, max_btn_ratio.y - shrink_val };

        ImVec2 p1 = { (3.f * min_trig.x + max_trig.x) / 4.f, (3.f * min_trig.y + max_trig.y) / 4.f };
        ImVec2 p2 = { (min_trig.x + 3.f * max_trig.x) / 4.f, (3.f * min_trig.y + max_trig.y) / 4.f };
        ImVec2 p3 = { (min_trig.x + max_trig.x)/2.f, (min_trig.y + 3.f * max_trig.y) / 4.f };


        drawlist_popup->AddTriangleFilled(p1, p2, p3, color_outline_ratio);

        
        if(btn_ratio_pressed)
            ImGui::OpenPopup("POPUP_PREVIEW_RATIO_LIST");


        // RESOLUTION selectable
        current_cursor_height += btn_padding * 2.f + size_ratio_text.y + margin_elements_height;

        ImVec2 pos_text_btn_resolution = { pop_min.x + btn_padding, pop_min.y + current_cursor_height };
        drawlist_popup->AddText(pos_text_btn_resolution, ImColor(1.f, 1.f, 1.f, 1.f), str_resolution);

        std::string res_btn_label = std::to_string(sel_w) + " x " + std::to_string(sel_h);
        for (const auto& r : resolutions)
            if (r.first_size == sel_w && r.second_size == sel_h) {
                res_btn_label = res_label(r);
                break;
            }

        ImVec2 min_btn_resolution = { pop_min.x + btn_padding + size_resolution_text.x + 2.f * btn_padding,
                              pop_min.y + current_cursor_height - btn_padding };
        ImVec2 max_btn_resolution = { pop_max.x - btn_padding,
                                      pop_min.y + current_cursor_height + size_resolution_text.y + btn_padding };

        ImVec2 pos_text_btn_resolution_value = { pop_min.x + btn_padding + size_resolution_text.x + 4.f * btn_padding,
                                                 pop_min.y + current_cursor_height };

        bool btn_resolution_held, btn_resolution_hovered, btn_resolution_pressed;
        btn_resolution_pressed = MyButtonBehavior(min_btn_resolution, max_btn_resolution,
            "BTN_RESOLUTION_PREVIEW_RESOLUTION",
            btn_resolution_hovered, btn_resolution_held);

        ImColor color_outline_resolution = COLOR_CHANGEBLE_DARK;

        if (btn_resolution_held)
            color_outline_resolution = COLOR_CHANGEBLE_HELD;
        else if (btn_resolution_hovered)
            color_outline_resolution = COLOR_CHANGEBLE_HOVERED;

        drawlist_popup->AddRectFilled(min_btn_resolution, max_btn_resolution, COLOR_BG_BUTTONS, btn_rounding);
        drawlist_popup->AddRect(min_btn_resolution, max_btn_resolution, color_outline_resolution, btn_rounding, 0, btn_outline_thikness);
        drawlist_popup->AddText(pos_text_btn_resolution_value, ImColor(1.f, 1.f, 1.f, 1.f), res_btn_label.c_str());

        ImVec2 min_trig_res = { max_btn_resolution.x - width_trig - btn_padding * 0.5f + shrink_val,
                                min_btn_resolution.y + shrink_val };
        ImVec2 max_trig_res = { max_btn_resolution.x - btn_padding * 0.5f - shrink_val,
                                max_btn_resolution.y - shrink_val };

        ImVec2 res_p1 = { (3.f * min_trig_res.x + max_trig_res.x) / 4.f, (3.f * min_trig_res.y + max_trig_res.y) / 4.f };
        ImVec2 res_p2 = { (min_trig_res.x + 3.f * max_trig_res.x) / 4.f, (3.f * min_trig_res.y + max_trig_res.y) / 4.f };
        ImVec2 res_p3 = { (min_trig_res.x + max_trig_res.x) / 2.f, (min_trig_res.y + 3.f * max_trig_res.y) / 4.f };

        drawlist_popup->AddTriangleFilled(res_p1, res_p2, res_p3, color_outline_resolution);

        if (btn_resolution_pressed)
            ImGui::OpenPopup("POPUP_PREVIEW_RESOLUTION_LIST");


        std::vector<std::string> ratio_items;
        for (const auto& r : ratios)
            ratio_items.push_back(ratio_label(r));
        int chosen_ratio = draw_choice_popup("POPUP_PREVIEW_RATIO_LIST", ratio_items, sel_ratio);
        if (chosen_ratio >= 0 && chosen_ratio != sel_ratio) {
            sel_ratio = chosen_ratio;
            std::vector<ResolutionWithName> new_list = get_ratio_resolutions(ratios[sel_ratio]);
            int best = pick_closest(new_list, sel_w, sel_h);
            sel_w = new_list[best].first_size;
            sel_h = new_list[best].second_size;
            cur_list = new_list;
        }

        std::vector<std::string> res_items;
        int sel_res_index = -1;
        for (int i = 0; i < (int)cur_list.size(); i++) {
            res_items.push_back(res_label(cur_list[i]));
            if (cur_list[i].first_size == sel_w && cur_list[i].second_size == sel_h)
                sel_res_index = i;
        }
        int chosen_res = draw_choice_popup("POPUP_PREVIEW_RESOLUTION_LIST", res_items, sel_res_index);
        if (chosen_res >= 0) {
            sel_w = cur_list[chosen_res].first_size;
            sel_h = cur_list[chosen_res].second_size;
        }


        ImVec2 min_btn_apply = { pop_center.x - size_apply_text.x / 2.f - btn_padding * 2.f, pop_max.y - btn_margin - size_apply_text.y - 2.f * btn_padding };
        ImVec2 max_btn_apply = { pop_center.x + size_apply_text.x / 2.f + btn_padding * 2.f, pop_max.y - btn_margin};

        ImVec2 pos_text_btn_apply = { pop_center.x - size_apply_text.x / 2.f, pop_max.y - btn_margin - size_apply_text.y - btn_padding };

        bool btn_apply_held, btn_apply_hovered, btn_apply_pressed;
        btn_apply_pressed = MyButtonBehavior(min_btn_apply, max_btn_apply, "BTN_APPLY_PREVIEW_RESOLUTION", btn_apply_hovered, btn_apply_held);

        ImColor color_outline = COLOR_CHANGEBLE_DARK;

        if (btn_apply_held)
            color_outline = COLOR_CHANGEBLE_HELD;
        else if (btn_apply_hovered)
            color_outline = COLOR_CHANGEBLE_HOVERED;

        drawlist_popup->AddRectFilled(min_btn_apply, max_btn_apply, COLOR_BG_BUTTONS, btn_rounding);
        drawlist_popup->AddRect(min_btn_apply, max_btn_apply, color_outline, btn_rounding, 0, btn_outline_thikness);
        drawlist_popup->AddText(pos_text_btn_apply, ImColor(1.f, 1.f, 1.f, 1.f), str_apply);

        if (btn_apply_pressed) {
            renderer->Resize(sel_w, sel_h);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(2);

    if (hovered || held) {
        drawlist->AddRectFilled(pos_text_min, pos_text_max, ImColor(1.f, 1.f, 1.f, 0.1f), 2.f);
    }

    pos_cursor_draw += size_texture_size_info.x + padding;
}

void Timeline_Preview::DrawBottomMenu_ColorPickerInfo(float& pos_cursor_draw, const ImVec2& p_min, const ImVec2& p_max) {
	if (pc.isHoveredTextureFramebuffer) {

		auto drawlist = ImGui::GetWindowDrawList();

		float aspect_ratio = renderer->getTexture().GetAspectRatio();

		float size_rect_color_picker = height_bottom_menu / 1.8f;

		float pos_x_color_picker = ((pc.mouse_global_relative.x + aspect_ratio) / (2.f * aspect_ratio)) * renderer->getTexture().GetWidth();
		float pos_y_color_picker = renderer->getTexture().GetHeight() - ((pc.mouse_global_relative.y + 1.f) / 2.f) * renderer->getTexture().GetHeight();

		glm::vec3 c_pick = renderer->getColorPixel(pos_x_color_picker, pos_y_color_picker);

		ImVec2 min_color_rect = {
			std::round(pos_cursor_draw),
			std::round(p_min.y + height_bottom_menu / 2.f - size_rect_color_picker / 2.f)
		};

		ImVec2 max_color_rect = {
			std::round(pos_cursor_draw + size_rect_color_picker),
			std::round(p_min.y + height_bottom_menu / 2.f + size_rect_color_picker / 2.f)
		};

		drawlist->AddRectFilled(min_color_rect, max_color_rect, ImColor(c_pick.x, c_pick.y, c_pick.z));
		drawlist->AddRect(min_color_rect, max_color_rect, ImColor(0.f, 0.f, 0.f, 1.f));

		pos_cursor_draw += size_rect_color_picker + padding / 3.f;

		std::string text_color_info = "(" + std::to_string(int(c_pick.x * 255.f)) + ", " + std::to_string(int(c_pick.y * 255.f)) + ", " + std::to_string(int(c_pick.x * 255.f)) + ")";
		ImVec2 size_text_color_info = ImGui::CalcTextSize(text_color_info.c_str());
		drawlist->AddText({ pos_cursor_draw, p_min.y + height_bottom_menu / 2.f - size_text_color_info.y / 2.f }, ImColor(1.f, 1.f, 1.f, 1.f), text_color_info.c_str());

		pos_cursor_draw += size_text_color_info.x + padding / 3.f;
	}
}

void Timeline_Preview::DrawBottomMenu(const ImVec2& p_min, const ImVec2& p_max) {

	auto drawlist = ImGui::GetWindowDrawList();

	ImVec2 line_begin = p_min;
	ImVec2 line_end = { p_max.x, p_min.y };

	drawlist->AddRectFilled(p_min, p_max, COLOR_BG_LIST);
	drawlist->AddLine(line_begin, line_end, ImColor(0.f, 0.f, 0.f, 1.f), 1.f);

	float pos_cursor_draw = p_min.x + padding;

	Push_SmallestFont();
        DrawBottomMenu_Scale(pos_cursor_draw, p_min, p_max);
        DrawBottomMenu_TextureSetting(pos_cursor_draw, p_min, p_max);
		DrawBottomMenu_Resolution(pos_cursor_draw, p_min, p_max);
		DrawBottomMenu_ColorPickerInfo(pos_cursor_draw, p_min, p_max);
	Pop_SmallestFont();

}

void Timeline_Preview::DrawViewGizmo(const ImVec2& p_min, const ImVec2& p_max) {
    using namespace ImViewGuizmo;

    if (p_max.x - p_min.x < 220.f || p_max.y - p_min.y < 220.f)
        return;

    auto& style = GetStyle();
    Camera_Global& cam = renderer->getCamera();

    glm::vec3 gizmoPos = cam.getPos();
    glm::quat gizmoRot = cam.getGizmoRotation();

    const float margin = 14.f;
    const float radius = 80.f * style.scale;
    const ImVec2 gizmo_center(p_max.x - margin - radius, p_min.y + margin + radius);

    const glm::vec3 pivot = gizmoPos;

    bool modified = false;

    modified |= ImViewGuizmo::Rotate(gizmoPos, gizmoRot, pivot, gizmo_center);

    const float btn = style.toolButtonRadius * style.scale * 2.f;
    const ImVec2 row(p_max.x - margin - 2.f * btn - 6.f,
        p_min.y + margin + 2.f * radius + 10.f);
    modified |= Dolly(gizmoPos, gizmoRot, ImVec2(row.x + btn + 6.f, row.y), 0.05f);
    modified |= Pan(gizmoPos, gizmoRot, row, 0.01f);


    if (modified) {
        cam.setFromGizmo(gizmoPos, gizmoRot);
        renderer->Update();
    }
}

void Timeline_Preview::Draw() {
    ImViewGuizmo::BeginFrame();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, COLOR_BG_VIEWER.Value);
	ImGui::PushStyleColor(ImGuiCol_Separator, COLOR_SEPARATOR.Value);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	{
		ImGui::Begin(u8"Предпросмотр", nullptr, ImGuiWindowFlags_NoScrollWithMouse);
		auto drawlist = ImGui::GetWindowDrawList();
		ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
		ImVec2 region_max = ImGui::GetContentRegionAvail();

		height_bottom_menu = eval(22.f);

		region_max.y -= height_bottom_menu;

		horizontal.updateAvailableScreenValue(region_max.x);
		vertical.updateAvailableScreenValue(region_max.y);

		ImVec2 mouse_Screen = ImGui::GetMousePos();
		pc.mouse_screen_relative = { mouse_Screen.x - cursor_pos.x, mouse_Screen.y - cursor_pos.y };
		pc.mouse_global_relative = { horizontal.getPosGlobal_real(pc.mouse_screen_relative.x), vertical.getPosGlobal_real(pc.mouse_screen_relative.y) };

		ImVec2 p_min = cursor_pos;
		ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

		float aspect_ratio = renderer->getTexture().GetAspectRatio();

		pc.isHoveredWidgetRegion = ImGui::IsMouseHoveringRect(p_min, p_max);

		pc.isHoveredTextureFramebuffer =
			((pc.mouse_global_relative.x >= (-1.f * aspect_ratio)) && (pc.mouse_global_relative.x <= (1.f * aspect_ratio))) &&
			((pc.mouse_global_relative.y >= -1.f) && (pc.mouse_global_relative.y <= 1.f));

		static bool camera_mode = false;
        static double cam_saved_x = 0.0;
        static double cam_saved_y = 0.0;

		bool isShiftPressed = ImGui::IsKeyPressed(ImGuiKey_RightShift);
		bool isESCPressed = ImGui::IsKeyPressed(ImGuiKey_Escape);

        bool shouldExitCamera = isESCPressed && camera_mode;

        if (isShiftPressed || shouldExitCamera) {
            if (shouldExitCamera)
                camera_mode = false;
            else
                camera_mode = !camera_mode;

            GLFWwindow* window = WindowManager::getInstance().GetHandle();
            ImGuiIO& io = ImGui::GetIO();

            if (camera_mode) {
                io.ConfigFlags |= ImGuiConfigFlags_NoMouse;

                glfwGetCursorPos(window, &cam_saved_x, &cam_saved_y);

                if (glfwRawMouseMotionSupported())
                    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else {
                io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

                glfwSetCursorPos(window, cam_saved_x, cam_saved_y);
                io.MousePos = ImVec2((float)cam_saved_x, (float)cam_saved_y);

                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
            }
        }

        if (camera_mode)
            ImGui::GetIO().MousePos = ImVec2((float)cam_saved_x, (float)cam_saved_y);

        ImGuiIO& io = ImGui::GetIO();
        DebugOverlay::AddText(
            "ImGuiConfigFlags_NoMouse: " +
            std::string(((io.ConfigFlags & ImGuiConfigFlags_NoMouse) != 0) ? "TRUE" : "FALSE"),
            ImColor(1.f, 1.f, 1.f, 1.f)
        );
        
        DebugOverlay::AddText(
            "io.MousePos: (" +
            std::to_string(io.MousePos.x) + "," +
            std::to_string(io.MousePos.y) + ")",
            ImColor(1.f, 1.f, 1.f, 1.f)
        );


		static glm::vec2 prev_m = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };

		if (camera_mode) {
			float scrol_zoom = ImGui::GetIO().MouseWheel;

			glm::vec2 curr_m = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };

			glm::vec2 delta = prev_m - curr_m;
            if (ImViewGuizmo::IsUsing())
                delta = { 0,0 };

            float sense = 0.1f;

			renderer->getCamera().Update(deltaTime->GetDeltaTime(), { delta.x * sense, delta.y * sense }, scrol_zoom);
			prev_m = curr_m;
		}
		else {
			renderer->getCamera().Update(deltaTime->GetDeltaTime(), { 0.f,0.f }, 0.f);
			prev_m = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
		}

		if (pc.isHoveredWidgetRegion && !camera_mode) {
			float scrol_zoom = ImGui::GetIO().MouseWheel;

			if (scrol_zoom > 0.f) {
				horizontal.zoomToPointByValue(pc.mouse_global_relative.x, 1.2f, false);
				vertical.zoomToPointByValue(pc.mouse_global_relative.y, 1.2f, false);
			}
			else if (scrol_zoom < 0.f) {
				horizontal.zoomToPointByValue(pc.mouse_global_relative.x, 0.8f, false);
				vertical.zoomToPointByValue(pc.mouse_global_relative.y, 0.8f, false);
			}
		}

		if (pc.isHoveredWidgetRegion && !ImViewGuizmo::IsOver() && pc.isHoveredTextureFramebuffer && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			mouseIsDraging = true;
		}

		if (mouseIsDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);

			ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			horizontal.setOffsetDelta(delta.x);
			vertical.setOffsetDelta(delta.y);
		}
		else {
			mouseIsDraging = false;
			horizontal.applyOffsetDelta();
			vertical.applyOffsetDelta();
		}

		ImVec2 LU = {
		    cursor_pos.x + horizontal.getPosScreen_lerped(-1.f * aspect_ratio),
		    cursor_pos.y + vertical.getPosScreen_lerped(1.f)
		};

		ImVec2 RD = {
			cursor_pos.x + horizontal.getPosScreen_lerped(1.f * aspect_ratio),
			cursor_pos.y + vertical.getPosScreen_lerped(-1.f)
		};

		drawlist->AddImage(renderer->getTexture().GetID(), LU, RD);

        Push_BoldFont();
        DrawViewGizmo(p_min, p_max);
        Pop_BoldFont();

		p_min.y = p_max.y;
		p_max.y += height_bottom_menu;

		DrawBottomMenu(p_min, p_max);

		DrawBorderForFocusedWindow(COLOR_WINDOW_BORDER_FOCUS);

		ImGui::End();
	}

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);

}

void Timeline_Preview::Update() {
	horizontal.update(deltaTime->GetDeltaTime() * 20.f);
	vertical.update(deltaTime->GetDeltaTime() * 20.f);

	if (firstCentered == false)
	{
		horizontal.centerizeForce(0.f);
		vertical.centerizeForce(0.f);

		if (horizontal.getAvailableScreenValue() > 100.f)
			firstCentered = true;
	}
}