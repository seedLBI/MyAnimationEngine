#include "Graph.Buttons.Drawer.h"
#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"

GraphButtonsDrawer::GraphButtonsDrawer(TimelineContext* ctx) : ctx(ctx) {
	InitButtons();
}

GraphButtonsDrawer::~GraphButtonsDrawer() {

}


void GraphButtonsDrawer::ChangeTypeKeyframe(TypeKeyframe type) {
	for (const auto& keyframe : ctx->selector->getKeyframes()) {
		const auto& id_layer = keyframe.property.layer.id;
		const auto& name_property = keyframe.property.name;

		auto* layerObject = ctx->layersData->GetLayerByID(id_layer).get();
		float time_begin = layerObject->GetLayerInfo().time.Begin;

		auto& variant_prop = layerObject->GetProperties().getVariant(name_property);


		std::visit([&](auto& propertie) {
			propertie.kr.changeTypeByID(keyframe.id, type);
			},
			variant_prop);
	}
}

void GraphButtonsDrawer::Button_Make_Selected_Keyframes_Linear() {
	ChangeTypeKeyframe(TypeKeyframe::LINEAR);
}
void GraphButtonsDrawer::Button_Make_Selected_Keyframes_Binary() {
	ChangeTypeKeyframe(TypeKeyframe::BINARY);
}
void GraphButtonsDrawer::Button_Make_Selected_Keyframes_Bezier() {
	ChangeTypeKeyframe(TypeKeyframe::BEZIER);
}
void GraphButtonsDrawer::Button_Make_Selected_Keyframes_AutoBezier() {
	ChangeTypeKeyframe(TypeKeyframe::AUTOBEZIER);
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
		{BTN_GRAPH_MAKE_KEYFRAME_LINEAR,	 "btn_make_keyframe_linear.png",	 &GraphButtonsDrawer::Button_Make_Selected_Keyframes_Linear},
		{BTN_GRAPH_MAKE_KEYFRAME_BINARY,	 "btn_make_keyframe_binary.png",	 &GraphButtonsDrawer::Button_Make_Selected_Keyframes_Binary},
		{BTN_GRAPH_MAKE_KEYFRAME_BEZIER,	 "btn_make_keyframe_bezier.png",	 &GraphButtonsDrawer::Button_Make_Selected_Keyframes_Bezier},
		{BTN_GRAPH_MAKE_KEYFRAME_AUTOBEZIER, "btn_make_keyframe_autobezier.png", &GraphButtonsDrawer::Button_Make_Selected_Keyframes_AutoBezier},
	};

	const std::string base_path = "resources/images/buttons/";

	std::vector<Button> btns(magic_enum::enum_count<GraphButtonsType>());

	for (const auto& btn : btnsInfo) {
		std::string fullpath = base_path + btn.filename;
		btns[btn.type].texture = MakeTexture(btn.type, fullpath);
		btns[btn.type].action = btn.action;
	}

	layout = {
		btns[BTN_GRAPH_MAKE_KEYFRAME_LINEAR],
		btns[BTN_GRAPH_MAKE_KEYFRAME_BEZIER],
		btns[BTN_GRAPH_MAKE_KEYFRAME_AUTOBEZIER],
		btns[BTN_GRAPH_MAKE_KEYFRAME_BINARY]
	};

}

#include "ThirdParty/LivePT/LivePT.h"

void GraphButtonsDrawer::Draw(const ImVec2& p_min, const ImVec2& p_max) {
	auto drawlist = ImGui::GetWindowDrawList();

	drawlist->AddRectFilled(p_min, p_max, COLOR_BG_LIST);

	float height = p_max.y - p_min.y;

	float size_btn = height * eval(0.554f);
	float padding_size_btn = size_btn * eval(0.12f);

	float separator_width = eval(4.0f);
	float space_width = eval(2.0f);
	float gap = eval(6.2f);

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
			drawlist->AddImage(btn->texture->GetID(), image_min, image_max, ImVec2(1, 1), ImVec2(0, 0), color_icon);

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