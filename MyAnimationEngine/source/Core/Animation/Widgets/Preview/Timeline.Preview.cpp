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

Timeline_Preview::Timeline_Preview(FPS_Timer* deltaTime, RendererAnimationScene* renderer) : deltaTime(deltaTime), renderer(renderer) {

	horizontal.setBoundsScale(0.3f, 1000.f);
	vertical.setBoundsScale(0.3f, 1000.f);

	TextureSetting ts;
	ts.Min = TextureFilter::LINEAR_MIPMAP_LINEAR;
	ts.Max = TextureFilter::LINEAR_MIPMAP_LINEAR;
	ts.WrapX = TextureWrap::CLAMP_TO_BORDER;
	ts.WrapY = TextureWrap::CLAMP_TO_BORDER;

	temp = new Texture(ts, "resources\\images\\temp.jpg");

}
Timeline_Preview::~Timeline_Preview() {

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
    if (ImGui::BeginPopup("POPUP_PREVIEW_RESOLUTION")) {

        std::vector<ResolutionWithName> cur_list = get_ratio_resolutions(ratios[sel_ratio]);

        std::string ratio_btn_label = ratio_label(ratios[sel_ratio]);
        if (ImGui::Button(ratio_btn_label.c_str()))
            ImGui::OpenPopup("POPUP_PREVIEW_RATIO_LIST");
        ImGui::SameLine();
        ImGui::Text(u8"Соотношение");

        std::string res_btn_label = std::to_string(sel_w) + " x " + std::to_string(sel_h);
        for (const auto& r : resolutions)
            if (r.first_size == sel_w && r.second_size == sel_h) {
                res_btn_label = res_label(r);
                break;
            }
        if (ImGui::Button(res_btn_label.c_str()))
            ImGui::OpenPopup("POPUP_PREVIEW_RESOLUTION_LIST");
        ImGui::SameLine();
        ImGui::Text(u8"Разрешение");

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

        ImGui::Spacing();
        if (ImGui::Button(u8"Применить")) {
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
		DrawBottomMenu_Resolution(pos_cursor_draw, p_min, p_max);
		DrawBottomMenu_ColorPickerInfo(pos_cursor_draw, p_min, p_max);
	Pop_SmallestFont();

}

void Timeline_Preview::Draw() {

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

		bool isShiftPressed = ImGui::IsKeyPressed(ImGuiKey_RightShift);
		bool isESCPressed = ImGui::IsKeyPressed(ImGuiKey_Escape);

		if (isShiftPressed || isESCPressed) {
			if (isESCPressed && camera_mode)
			{
				camera_mode = false;

				GLFWwindow* window = WindowManager::getInstance().GetHandle();

				if (camera_mode) {
					if (glfwRawMouseMotionSupported())
						glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				else {
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
				}
			}
			else {
				camera_mode = !camera_mode;

				GLFWwindow* window = WindowManager::getInstance().GetHandle();

				if (camera_mode) {
					if (glfwRawMouseMotionSupported())
						glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				else {
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
				}
			}

		}

		static glm::vec2 prev_m = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };

		if (camera_mode) {
			float scrol_zoom = ImGui::GetIO().MouseWheel;

			glm::vec2 curr_m = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };

			glm::vec2 delta = prev_m - curr_m;
			renderer->getCamera().Update(deltaTime->GetDeltaTime(), { delta.x, delta.y }, scrol_zoom);
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



		if (pc.isHoveredWidgetRegion && pc.isHoveredTextureFramebuffer && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
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