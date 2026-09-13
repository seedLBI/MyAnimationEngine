#include "Timeline.Ruler.h"
#include "CursorManager/CursorManger.h"

#include "Utils/Text/Text.h"

#include "Core/DebugOverlay/DebugOverlay.h"
#include "Utils/Math/Math.Lerp.h"

Timeline_Ruler::Timeline_Ruler(
	TimelineContext* ctx,
	Timeline_TimeCursor* timeCursor,
	HorizontalTickRenderer* horizontalTickRenderer
	)
	: 
	ctx(ctx),
	timeCursor(timeCursor),
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
	ImVec2 block_max = { cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(0.f), cursor_pos.y + region_max.y };

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

	block_min = { cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(ctx->timelineData->GetData().totalTime), cursor_pos.y };
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
	lerpedColor_reaction.Value.w = ctx->horizontalTransform->getReactionToOverScale() * 0.5f;

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
	ctx->horizontalTransform->updateAvailableWidth(region_max.x);

	float mouse_real_screen = ImGui::GetMousePos().x;
	float mouse_fixed = mouse_real_screen - cursor_pos.x;
	mouse_global = ctx->horizontalTransform->getPostionGlobal_real(mouse_fixed);
	mouse_screen = ctx->horizontalTransform->getPostionScreen_real(mouse_global);


	float offsetOutOfBound = ctx->horizontalTransform->getOffset_OutOfBounds();

	const float valueForOutBounds = 250.f;

	if (offsetOutOfBound > valueForOutBounds)
		offsetOutOfBound = valueForOutBounds;

	if (offsetOutOfBound < -valueForOutBounds)
		offsetOutOfBound = -valueForOutBounds;

	offsetOutOfBound /= valueForOutBounds;

	time_left_bound.Set( offsetOutOfBound >= 0.f ?  offsetOutOfBound : 0.f);
	time_right_bound.Set(offsetOutOfBound <= 0.f ? -offsetOutOfBound : 0.f);


	time_left_bound.Update(dt * 8.f);
	time_right_bound.Update(dt * 8.f);

}
void Timeline_Ruler::UpdateZoom() {
	if (!isHovered) return;
	
	if (scrol_zoom > 0.f) {
		ctx->horizontalTransform->zoomToPointByValue(mouse_global, 1.5f, false);
	}
	else if (scrol_zoom < 0.f) {
		ctx->horizontalTransform->zoomToPointByValue(mouse_global, 0.75f, false);
	}
}

void Timeline_Ruler::FitAllTimeline() {
	ctx->horizontalTransform->fitToRanges(-0.1, ctx->timelineData->GetData().totalTime + 0.1);
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


	CursorManager::getInstance().SetCursor(TypeCursor::POINT);

	if (isHovered)
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
	if (isDraging)
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);


	static bool isOfsetDeltaApplied = false;

	if (isDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		isOfsetDeltaApplied = true;
		float delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
		ctx->horizontalTransform->setOffsetDelta(delta);
	}
	else {
		isDraging = false;
		if (isOfsetDeltaApplied)
			ctx->horizontalTransform->applyOffsetDelta();
		isOfsetDeltaApplied = false;
	}

	if (!isHovered && !isDragingTime) return;

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
		isDraging = true;
	}
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		isDragingTime = true;
	}


	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left, false)) {

		float w = ctx->timelineData->GetData().totalTime * 0.05f;

		ctx->horizontalTransform->fitToRanges(-w, ctx->timelineData->GetData().totalTime + w);
	}

	if (isDragingTime && ImGui::IsMouseDown(ImGuiMouseButton_Right, false)) {

		float step_frame = 1.f / (float)ctx->timelineData->GetData().framerate;
		float count = std::floor(mouse_global / step_frame);
		float time = count * step_frame + 0.000001f;

		if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
			time = mouse_global;


		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {

			CursorManager::getInstance().SetCursor(TypeCursor::MAGNET);

			const float snap_radius_pixels = 15.f;

			float snap_radius_global = std::abs(
				ctx->horizontalTransform->getPostionGlobal_real(snap_radius_pixels) -
				ctx->horizontalTransform->getPostionGlobal_real(0.f)
			);

			float closest_time = -1.f;
			float min_dist = snap_radius_global;

			ctx->layersData->forEach([&](LayerObject* layer, uint64_t index) {
				const auto& layerInfo = layer->GetLayerInfo();

				layer->GetProperties().forEachUnwrapped([&](const std::string& name, auto& value) {
					for (const auto& kf : value.kr.getKeyframes()) {
						float kf_global_time = layerInfo.time.Begin + kf.sample.time;

						float dist = std::abs(kf_global_time - mouse_global);

						if (dist < min_dist) {
							min_dist = dist;
							closest_time = kf_global_time;
						}
					}
					});
				});

			if (closest_time >= 0.f) {
				time = closest_time;
			}
		}


		ctx->timelineData->GetData().SetTime(
			long double(time),
			ctx->timelineController->GetState() == TimelineRunState::RUNNING_CYCLE
		);

		
	}
	else {
		isDragingTime = false;
	}
}