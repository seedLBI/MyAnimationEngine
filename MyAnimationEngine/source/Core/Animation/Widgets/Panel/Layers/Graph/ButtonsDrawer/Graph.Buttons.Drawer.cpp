#include "Graph.Buttons.Drawer.h"
#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"

GraphButtonsDrawer::GraphButtonsDrawer(TimelineContext* ctx, GraphControls* graphCtrl) : ctx(ctx), graphCtrl(graphCtrl) {
	InitButtons();
}

GraphButtonsDrawer::~GraphButtonsDrawer() {

}


void GraphButtonsDrawer::Button_Make_Selected_Keyframes_Linear() {
	graphCtrl->ChangeTypeKeyframe(TypeKeyframe::LINEAR);
}
void GraphButtonsDrawer::Button_Make_Selected_Keyframes_Binary() {
	graphCtrl->ChangeTypeKeyframe(TypeKeyframe::BINARY);
}
void GraphButtonsDrawer::Button_Make_Selected_Keyframes_Bezier() {
	graphCtrl->ChangeTypeKeyframe(TypeKeyframe::BEZIER);
}
void GraphButtonsDrawer::Button_Make_Selected_Keyframes_AutoBezier() {
	graphCtrl->ChangeTypeKeyframe(TypeKeyframe::AUTOBEZIER);
}

void GraphButtonsDrawer::Button_Fit_Selected_Graph() {
	graphCtrl->FitSelectedGraph();
}
void GraphButtonsDrawer::Button_Fit_Always_Vertically() {
	graphCtrl->FitAllVertical();
}
void GraphButtonsDrawer::Button_SplitValues() {
	
}

std::shared_ptr<Texture> GraphButtonsDrawer::MakeTexture(GraphButtonsType type, const std::string& fullpath) {
	TextureSetting tex_setting;
	tex_setting.Min = TextureFilter::LINEAR_MIPMAP_LINEAR;
	tex_setting.Max = TextureFilter::LINEAR_MIPMAP_LINEAR;
	tex_setting.WrapX = TextureWrap::CLAMP_TO_BORDER;
	tex_setting.WrapY = TextureWrap::CLAMP_TO_BORDER;

	return std::make_shared<Texture>(tex_setting, fullpath);
}

void GraphButtonsDrawer::InitButtons() {


	struct ButtonsInfo {
		GraphButtonsType type;
		const char* filename;
		void(GraphButtonsDrawer::* action)();
	};

	const ButtonsInfo btnsInfo[] = {
		{BTN_ALWAYS_FIT_VERTICALLY,			 "btn_always_fit_verticle.png",		 &GraphButtonsDrawer::Button_Fit_Always_Vertically},
		{BTN_FIT_SELECTED_GRAPH,			 "btn_fit_selected_graph.png",		 &GraphButtonsDrawer::Button_Fit_Selected_Graph},
		{BTN_SPLIT_VALUES,					 "btn_split_values.png",			 &GraphButtonsDrawer::Button_SplitValues},
		{BTN_GRAPH_MAKE_KEYFRAME_LINEAR,	 "btn_make_keyframe_linear.png",	 &GraphButtonsDrawer::Button_Make_Selected_Keyframes_Linear},
		{BTN_GRAPH_MAKE_KEYFRAME_BINARY,	 "btn_make_keyframe_binary.png",	 &GraphButtonsDrawer::Button_Make_Selected_Keyframes_Binary},
		{BTN_GRAPH_MAKE_KEYFRAME_BEZIER,	 "btn_make_keyframe_bezier.png",	 &GraphButtonsDrawer::Button_Make_Selected_Keyframes_Bezier},
		{BTN_GRAPH_MAKE_KEYFRAME_AUTOBEZIER, "btn_make_keyframe_autobezier.png", &GraphButtonsDrawer::Button_Make_Selected_Keyframes_AutoBezier}
	};

	const std::string base_path = "resources/images/buttons/";

	std::vector<Button> btns(magic_enum::enum_count<GraphButtonsType>());

	for (const auto& btn : btnsInfo) {
		std::string fullpath = base_path + btn.filename;
		btns[btn.type].texture = MakeTexture(btn.type, fullpath);
		btns[btn.type].action = btn.action;
	}

	layout = {
		btns[BTN_ALWAYS_FIT_VERTICALLY],
		btns[BTN_FIT_SELECTED_GRAPH],
		LayoutType::SPACE,
		btns[BTN_SPLIT_VALUES],
		LayoutType::SPACE,
		btns[BTN_GRAPH_MAKE_KEYFRAME_LINEAR],
		btns[BTN_GRAPH_MAKE_KEYFRAME_BEZIER],
		btns[BTN_GRAPH_MAKE_KEYFRAME_AUTOBEZIER],
		btns[BTN_GRAPH_MAKE_KEYFRAME_BINARY],
	};

}

#include <Core/DebugOverlay/DebugOverlay.h>
#include "ThirdParty/LivePT/LivePT.h"

void GraphButtonsDrawer::Draw(const ImVec2& p_min, const ImVec2& p_max) {
	auto drawlist = ImGui::GetWindowDrawList();

	drawlist->AddRectFilled(p_min, p_max, COLOR_BG_LIST);

	float height = p_max.y - p_min.y;

	float size_btn = height * (0.554f);
	float padding_size_btn = size_btn * (0.12f);

	float separator_width = eval(4.f);
	float space_width = eval(6.f);
	float gap = (7.9f);

	DebugOverlay::AddText("separator_width:\t" + std::to_string(separator_width), ImColor(1.0f, 0.f, 0.f, 1.f));
	DebugOverlay::AddText("space_width:\t" + std::to_string(space_width), ImColor(1.f, 0.f, 0.f, 1.f));
	DebugOverlay::AddText("gap:\t" + std::to_string(gap), ImColor(1.f, 0.f, 0.f, 1.f));

	float full_width = 0.f;

	for (const auto& obj : layout) {
		if (std::holds_alternative<Button>(obj))
			full_width += size_btn;
		else if (const LayoutType* type = std::get_if<LayoutType>(&obj)) {
			if (*type == LayoutType::SEPARATOR)
				full_width += separator_width;
			else if (*type == LayoutType::SPACE)
				full_width += space_width;
		}

		full_width += gap;
	}
	full_width -= gap;


	ImVec2 p_c = { (p_max.x + p_min.x)/2.f - full_width/2.f, (p_max.y + p_min.y) / 2.f - size_btn/2.f };


	uint64_t count = 0;
	for (const auto& obj : layout) {
		if (const Button* btn = std::get_if<Button>(&obj)) {

			ImVec2 rect_min = { p_c.x - padding_size_btn, p_c.y - padding_size_btn };
			ImVec2 rect_max = { p_c.x + size_btn + padding_size_btn, p_c.y + size_btn + padding_size_btn };

			ImVec2 image_min = p_c;
			ImVec2 image_max = { p_c.x + size_btn, p_c.y + size_btn };

			bool btn_hovered, btn_held;

			if (MyButtonBehavior(rect_min, rect_max, ("GRAPH_BTN_" + std::to_string(count)).c_str(), btn_hovered, btn_held)) {
				(this->*(btn->action))();
			}
			count++;

			ImColor color_bg = ImColor(0.f, 0.f, 0.f, 0.f);
			ImColor color_icon = COLOR_DEFAULT;

			if (btn_held) {
				color_bg = COLOR_BG_BUTTONS_HELD;
				color_icon = ImColor(1.f, 1.f, 1.f, 1.f);
			}
			else if (btn_hovered) {
				color_bg = COLOR_BG_BUTTONS_HOVERED;
				color_icon = ImColor(1.f, 1.f, 1.f, 1.f);
			}


			drawlist->AddRectFilled(rect_min, rect_max, color_bg, 3.f);
			drawlist->AddImage(btn->texture->GetID(), image_min, image_max, ImVec2(0, 1), ImVec2(1, 0), color_icon);

			p_c.x += size_btn;
		}
		else if (const LayoutType* type = std::get_if<LayoutType>(&obj)) {
			if (*type == LayoutType::SEPARATOR) {
				// ImGui::Separator();
				p_c.x += separator_width;
			}
			else if (*type == LayoutType::SPACE) {
				// ImGui::Spacing();
				p_c.x += space_width;
			}
		}
		p_c.x += gap;
	}
}