#include "Horizontal.Tick.Renderer.h"


HorizontalTickRenderer::HorizontalTickRenderer(
	TimelineTransformHorizontal* timelineTransformHorizontal,
	TimelineProviderData* timelineData)
	:
	timelineTransformHorizontal(timelineTransformHorizontal),
	timelineData(timelineData)
{



}

HorizontalTickRenderer::~HorizontalTickRenderer() {

}

void HorizontalTickRenderer::DrawTicks(
	const ImVec2& p_min, const ImVec2& p_max,
	ImColor colorMain, ImColor colorSub,
	float widthMain, float widthSub,
	float heightMain, float heightSubMinor, float heightSubMajor,
	bool drawText
)
{
	auto drawlist = ImGui::GetWindowDrawList();
	int count_tick_rendered = 0;

	const float height_main_tick = heightMain;
	const float height_sub_minor_tick = heightSubMinor;
	const float height_sub_major_tick = heightSubMajor;


	float step_Main = 1.f;
	float distance_between_main_ticks = timelineTransformHorizontal->getPostionScreen_lerped(step_Main) - timelineTransformHorizontal->getPostionScreen_lerped(0.f);
	int count_main_tick = timelineData->GetData().totalTime;
	float size_main_tick_text = 52.f;


	if (distance_between_main_ticks < size_main_tick_text) {
		float count = std::round(size_main_tick_text / distance_between_main_ticks);
		float countTicks = std::floor(count_main_tick / count);
		float stepTick = timelineData->GetData().totalTime / countTicks;
		step_Main = stepTick;
		count_main_tick = countTicks;
		distance_between_main_ticks = timelineTransformHorizontal->getPostionScreen_lerped(stepTick) - timelineTransformHorizontal->getPostionScreen_lerped(0.f);
	}

	int begin_index_main_tick = timelineTransformHorizontal->getLeftBoundView_global_lerped() / step_Main;
	if (begin_index_main_tick < 0) begin_index_main_tick = 0;


	float step_Sub = 1.f / (float)timelineData->GetData().framerate;
	float distance_between_sub_ticks = timelineTransformHorizontal->getPostionScreen_lerped(step_Sub) - timelineTransformHorizontal->getPostionScreen_lerped(0.f);
	int count_sub_tick = timelineData->GetData().framerate;
	float size_sub_tick_text = 16.f;
	bool draw_sub_ticks = true;

	if (distance_between_sub_ticks < size_sub_tick_text) {
		float count = (std::max)(1.0f, std::round(size_sub_tick_text / distance_between_sub_ticks));
		float countTicks = std::round((float)timelineData->GetData().framerate / count);

		if (countTicks < 1.0f) {
			draw_sub_ticks = false;
		}
		else {
			float stepTick = 1.f / countTicks;
			step_Sub = stepTick;
			count_sub_tick = countTicks;
			distance_between_sub_ticks = timelineTransformHorizontal->getPostionScreen_lerped(stepTick) - timelineTransformHorizontal->getPostionScreen_lerped(0.f);
		}
	}


	int sub_text_step = 1;
	if (draw_sub_ticks && distance_between_sub_ticks > 0.001f && distance_between_sub_ticks < 52.f) {
		sub_text_step = std::ceil(52.f / distance_between_sub_ticks);
	}


	for (size_t i = begin_index_main_tick; i <= count_main_tick; i++) {

		double tick_main_value = std::round(double(i) * double(step_Main));

		if (long double(tick_main_value) > timelineData->GetData().totalTime + 0.0001l)
			break;

		float pos_main_tick = p_min.x + timelineTransformHorizontal->getPostionScreen_lerped(tick_main_value);

		ImVec2 min_pos_main_tick = { pos_main_tick - widthMain / 2.f, p_max.y - height_main_tick };
		ImVec2 max_pos_main_tick = { pos_main_tick + widthMain / 2.f, p_max.y };

		char buf[32];
		if (i == 0)
			snprintf(buf, sizeof(buf), "0s");
		else
			snprintf(buf, sizeof(buf), "%.0fs", tick_main_value);

		ImVec2 text_size = ImGui::CalcTextSize(buf, nullptr, true);

		if (max_pos_main_tick.x + distance_between_main_ticks < p_min.x) continue;
		if (min_pos_main_tick.x - text_size.x * 0.5f > p_max.x) break;

		if (drawText) {
			float text_x = pos_main_tick - text_size.x * 0.5f;
			float text_y = min_pos_main_tick.y - text_size.y - 1.0f;


			drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(text_x, text_y), colorMain, buf);
		}

		drawlist->AddRectFilled(min_pos_main_tick, max_pos_main_tick, colorMain, 2.f, 0.f);
		count_tick_rendered++;

		if (draw_sub_ticks) {

			int subs_per_main_step = std::round(step_Main / step_Sub);

			for (int j = 1; j <= subs_per_main_step; j++) {

				double tick_sub_value = tick_main_value + (double(j) * double(step_Sub));

				if (abs(tick_sub_value - tick_main_value) < 0.000001f || abs(tick_sub_value - tick_main_value - step_Main) < 0.000001f) continue;


				if (long double(tick_sub_value) > timelineData->GetData().totalTime + 0.0001l)
					break;

				float pos_sub_tick = p_min.x + timelineTransformHorizontal->getPostionScreen_lerped(tick_sub_value);


				bool isMajorTick = (j % sub_text_step == 0) && (subs_per_main_step / sub_text_step > 1);

				ImVec2 min_pos_sub_tick = { pos_sub_tick - widthSub / 2.f, p_max.y - (isMajorTick ? height_sub_major_tick : height_sub_minor_tick) };
				ImVec2 max_pos_sub_tick = { pos_sub_tick + widthSub / 2.f, p_max.y };

				if (max_pos_sub_tick.x < p_min.x || min_pos_sub_tick.x > p_max.x) continue;

				if (isMajorTick && drawText) {
					int actual_frame = std::round((tick_sub_value - tick_main_value) * timelineData->GetData().framerate);

					char buf[32];
					snprintf(buf, sizeof(buf), "%if", actual_frame);
					ImVec2 sub_text_size = ImGui::CalcTextSize(buf, nullptr, true);
					float text_x = pos_sub_tick - sub_text_size.x * 0.5f;
					float text_y = min_pos_sub_tick.y - sub_text_size.y - 1.0f;
					drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(text_x, text_y), colorSub, buf);
				}


				drawlist->AddRectFilled(min_pos_sub_tick, max_pos_sub_tick, colorSub, 2.f, 0.f);
				count_tick_rendered++;
			}
		}

	}
}