#include "Timeline.Ruler.h"
#include "CursorManager/CursorManger.h"

#include "Utils/Text/Text.h"

#include "Core/DebugOverlay/DebugOverlay.h"
#include "Utils/Math/Math.Lerp.h"

Timeline_Ruler::Timeline_Ruler(
	Timeline_TimeCursor* timeCursor,
	TimelineController* timelineController,
	TimelineProviderData* timelineData, 
	TimelineTransformHorizontal* timelineTransformHorizontal,
	HorizontalTickRenderer* horizontalTickRenderer
)
	: timeCursor(timeCursor),
	  timelineController(timelineController),
	  timelineData(timelineData),
	  timelineTransformHorizontal(timelineTransformHorizontal),
	  horizontalTickRenderer(horizontalTickRenderer)
{

}

Timeline_Ruler::~Timeline_Ruler() {

}

bool Timeline_Ruler::OutOfView(const ImVec2& pos) {
	const ImVec2& min = cursor_pos;
	const ImVec2& max = { cursor_pos.x + region_max.x,  cursor_pos.y + region_max.y };

	float len_x = max.x - min.x;
	float len_y = max.y - min.y;

	float len_pos_x = pos.x - min.x;
	float len_pos_y = pos.y - min.y;

	return (len_pos_x < 0.f || len_pos_x > len_x) || 
		   (len_pos_y < 0.f || len_pos_y > len_y) ;
}

bool Timeline_Ruler::OutOfView(const ImVec2& pos_min, const ImVec2& pos_max) {
	return OutOfView(pos_min) && OutOfView(pos_max);
}



void Timeline_Ruler::Draw() {
	//ImGui::BeginChild(u8"Timeline_Ruler", ImVec2(0, ImGui::GetTextLineHeight() * 2.f));


	auto drawlist = ImGui::GetWindowDrawList();
	cursor_pos = ImGui::GetCursorScreenPos();
	region_max = ImGui::GetContentRegionAvail();

	ImGui::InvisibleButton("Timeline_Ruler_canvas_button", region_max, ImGuiButtonFlags_MouseButtonLeft);

	isHovered = ImGui::IsItemHovered();
	scrol_zoom = ImGui::GetIO().MouseWheel;

	ImGui::SetCursorScreenPos(cursor_pos);

	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };


	drawlist->AddRectFilled(
		p_min,
		p_max,
		color_background,
		0.f,
		0.f
	);


	float padding_x = 0.f;

	p_min.x += padding_x;
	p_max.x -= padding_x;




	ImVec2 block_min = cursor_pos;
	ImVec2 block_max = { cursor_pos.x + timelineTransformHorizontal->getPostionScreen_lerped(0.f), cursor_pos.y + region_max.y };

	ImColor color_left_bound_outOfBounds  = color_bounds;
	ImColor color_right_bound_outOfBounds = color_bounds;

	Lerp(color_left_bound_outOfBounds, color_bounds_offsetMax,  1.f - time_left_bound.GetLerped());
	Lerp(color_right_bound_outOfBounds, color_bounds_offsetMax, 1.f - time_right_bound.GetLerped());

	if (OutOfView(block_max) == false || block_max.x > p_max.x) {


		drawlist->AddRectFilledMultiColor(
			block_min,
			{ block_max.x - width_gradient_bound, block_max.y},
			color_left_bound_outOfBounds, color_bounds, color_bounds, color_left_bound_outOfBounds
		);


		drawlist->AddRectFilledMultiColor(
			{ block_max.x - width_gradient_bound,  block_min.y},
			block_max,
			color_bounds, color_background, color_background, color_bounds
		);

	}

	block_min = { cursor_pos.x + timelineTransformHorizontal->getPostionScreen_lerped(timelineData->GetData().totalTime), cursor_pos.y };
	block_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	if (OutOfView(block_min) == false || block_min.x < p_max.x) {

		drawlist->AddRectFilledMultiColor(
			block_min,
			{ block_min.x + width_gradient_bound, block_max.y },
			color_background, color_bounds, color_bounds, color_background
		);

		drawlist->AddRectFilledMultiColor(
			{ block_min.x + width_gradient_bound, block_min.y },
			block_max,
			color_bounds, color_right_bound_outOfBounds, color_right_bound_outOfBounds, color_bounds
		);

	}



	horizontalTickRenderer->DrawTicks(
		p_min,
		p_max,
		color_mainTick, color_subTick,
		width_main_tick, width_sub_tick,
		ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() / 3.f, ImGui::GetTextLineHeight() / 1.5f,
		true
	);

	ImColor lerpedColor_reaction = color_reaction_overScale;
	lerpedColor_reaction.Value.w = timelineTransformHorizontal->getReactionToOverScale() * 0.5f;

	drawlist->AddRectFilled(
		p_min,
		p_max,
		lerpedColor_reaction,
		0.f,
		0.f
	);


	//DebugOverlay::AddText("tick rendered: " + std::to_string(count_tick_rendered), ImColor(1.f, 0.3f, 0.3f, 1.f));

	timeCursor->Draw(p_min, p_max);

}

void Timeline_Ruler::UpdateVars(const float& dt) {
	timelineTransformHorizontal->updateAvailableWidth(region_max.x);

	float mouse_real_screen = ImGui::GetMousePos().x;
	float mouse_fixed = mouse_real_screen - cursor_pos.x;
	mouse_global = timelineTransformHorizontal->getPostionGlobal_real(mouse_fixed);
	mouse_screen = timelineTransformHorizontal->getPostionScreen_real(mouse_global);


	float offsetOutOfBound = timelineTransformHorizontal->getOffset_OutOfBounds();

	const float valueForOutBounds = 250.f;

	if (offsetOutOfBound > valueForOutBounds)
		offsetOutOfBound = valueForOutBounds;

	if (offsetOutOfBound < -valueForOutBounds)
		offsetOutOfBound = -valueForOutBounds;

	offsetOutOfBound /= valueForOutBounds;

	time_left_bound.Set(offsetOutOfBound >= 0.f ?   offsetOutOfBound : 0.f);
	time_right_bound.Set(offsetOutOfBound <= 0.f ? -offsetOutOfBound : 0.f);


	time_left_bound.Update(dt * 8.f);
	time_right_bound.Update(dt * 8.f);

}
void Timeline_Ruler::UpdateZoom() {
	if (!isHovered) return;
	
	if (scrol_zoom > 0.f) {
		timelineTransformHorizontal->zoomToPointByValue(mouse_global, 1.5f, false);
	}
	else if (scrol_zoom < 0.f) {
		timelineTransformHorizontal->zoomToPointByValue(mouse_global, 0.75f, false);
	}
}

void Timeline_Ruler::FitAllTimeline() {
	timelineTransformHorizontal->fitToRanges(-0.1, timelineData->GetData().totalTime + 0.1);
}


const float& Timeline_Ruler::getGlobalMouse() {
	return mouse_global;
}
const float& Timeline_Ruler::getScreenMouse() {
	return mouse_screen;
}


void Timeline_Ruler::Update(const float& dt) {
	UpdateVars(dt);
	UpdateZoom();


	if (isHovered)
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
	if (isDraging)
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);


	static bool isOfsetDeltaApplied = false;

	if (isDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		isOfsetDeltaApplied = true;
		float delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
		timelineTransformHorizontal->setOffsetDelta(delta);
	}
	else {
		isDraging = false;
		if (isOfsetDeltaApplied)
			timelineTransformHorizontal->applyOffsetDelta();
		isOfsetDeltaApplied = false;
	}

	if (!isHovered) return;

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
		isDraging = true;
	}


	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left, false)) {
		timelineTransformHorizontal->fitToRanges(-0.1, timelineData->GetData().totalTime + 0.1);
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Right, false)) {

		float step_frame = 1.f / (float)timelineData->GetData().framerate;
		float count = std::floor(mouse_global / step_frame);
		float time = count * step_frame + 0.000001f;

		if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
			time = mouse_global;

		timelineData->GetData().SetTime(
			long double(time),
			timelineController->GetState() == TimelineRunState::RUNNING_CYCLE
		);

		CursorManager::getInstance().SetCursor(TypeCursor::POINT);
	}

}