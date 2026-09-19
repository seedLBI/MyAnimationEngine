#include "Keyframe.DragHandler.h"

#include "CursorManager/CursorManger.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

#include <cmath>

KeyframeDragHandler::KeyframeDragHandler(TimelineContext* ctx) : ctx(ctx) {}

void KeyframeDragHandler::BeginDrag(float start_global_x, float start_global_y, int64_t index_value, const SelectedKeyframe& current, KeyframeDragMode mode) {
    drag_started = true;
    current_mode = mode;
    startedDrag_global_x = start_global_x;
    startedDrag_global_y = start_global_y;
    dragged_value_index = index_value;
    original_times.clear();

    auto save_state = [&](const SelectedKeyframe& sel_kf) {
        auto& properties = ctx->layersData->GetLayerByID(sel_kf.property.layer.id).get()->GetProperties();
        properties.applyToKr(sel_kf.property.name, [&](auto& kr) {
            auto* kf = kr.getKeyframeByID(sel_kf.id);
            if (kf) {
                std::vector<float> orig_vals;

                if (dragged_value_index != -1 && dragged_value_index < kf->sample.value.size())
                    orig_vals.push_back(kf->sample.value[dragged_value_index]);
                else if (dragged_value_index == -1)
                    for (auto v : kf->sample.value) 
                        orig_vals.push_back(v);

                original_times.push_back({ sel_kf, kf->sample.time, orig_vals });
            }
        });
    };

    if (ctx->selector->ContainsKeyframe(current)) {
        for (const auto& sel_kf : ctx->selector->getKeyframes()) {
            save_state(sel_kf);
        }
    }
    else {
        ctx->selector->UnsetAll();
        ctx->selector->AddMoreSelectedKeyframe(current);
        ctx->selector->AddMoreSelectedProperty({ current.property.layer.id, current.property.name });
        ctx->selector->AddMoreSelectedLayer({ current.property.layer.id });

        save_state(current);
    }

    selection_min_time = FLT_MAX;
    selection_max_time = -FLT_MAX;

    selection_min_value = FLT_MAX;
    selection_max_value = -FLT_MAX;
    for (const auto& drag_keyframe : original_times) {
        selection_min_time = (std::min)(selection_min_time, drag_keyframe.original_time);
        selection_max_time = (std::max)(selection_max_time, drag_keyframe.original_time);

        for (float val : drag_keyframe.original_values) {
            selection_min_value = (std::min)(selection_min_value, val);
            selection_max_value = (std::max)(selection_max_value, val);
        }
    }
}

void KeyframeDragHandler::ProcessDrag(float current_global_x, float current_global_y) {
    if (!drag_started) return;

    bool is_mouse_down = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    bool is_shift_held = ImGui::IsKeyDown(ImGuiKey_LeftShift);

    if (is_mouse_down) {
        if (current_mode == KeyframeDragMode::ScaleLeft || current_mode == KeyframeDragMode::ScaleRight)
            CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);
        else if (current_mode == KeyframeDragMode::ScaleUp || current_mode == KeyframeDragMode::ScaleDown)
            CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_VERTICAL);
        else
            CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);


        float total_delta_x = current_global_x - startedDrag_global_x;
        float total_delta_y = current_global_y - startedDrag_global_y;

        float original_width = selection_max_time - selection_min_time;
        if (std::abs(original_width) < 0.0001f) original_width = 0.0001f;

        float original_height = selection_max_value - selection_min_value;
        if (std::abs(original_height) < 0.0001f) original_height = 0.0001f;

        for (const auto& drag_keyframe : original_times) {
            float orig_time = drag_keyframe.original_time;
            float target_time = orig_time;


            if (current_mode == KeyframeDragMode::TranslateX || current_mode == KeyframeDragMode::TranslateXY) {
                target_time = orig_time + total_delta_x;
            }
            else if (current_mode == KeyframeDragMode::ScaleLeft) {
                float new_left = selection_min_time + total_delta_x;
                float current_width = selection_max_time - new_left;
                float scale = current_width / original_width;
                target_time = selection_max_time - (selection_max_time - orig_time) * scale;
            }
            else if (current_mode == KeyframeDragMode::ScaleRight) {
                float new_right = selection_max_time + total_delta_x;
                float current_width = new_right - selection_min_time;
                float scale = current_width / original_width;
                target_time = selection_min_time + (orig_time - selection_min_time) * scale;
            }

            if (!is_shift_held) {
                const float frameTime = 1.0f / (float)ctx->timelineData->GetData().framerate;
                target_time = std::round(target_time / frameTime) * frameTime;
            }

            const auto& layer = ctx->layersData->GetLayerByID(drag_keyframe.keyframe.property.layer.id);
            auto& properties = layer.get()->GetProperties();

            properties.applyToKr(drag_keyframe.keyframe.property.name, [&](auto& kr) {
                auto* kf = kr.getKeyframeByID(drag_keyframe.keyframe.id);
                if (kf)
                {
                    bool value_changed = false;

                    auto apply_y_transformation = [&](size_t i, float orig_val) {
                        if (current_mode == KeyframeDragMode::TranslateXY) {
                            kf->sample.value[i] = orig_val + total_delta_y;
                            value_changed = true;
                        }
                        else if (current_mode == KeyframeDragMode::ScaleUp) {
                            float new_max = selection_max_value + total_delta_y;
                            float scale = (new_max - selection_min_value) / original_height;
                            kf->sample.value[i] = selection_min_value + (orig_val - selection_min_value) * scale;
                            value_changed = true;
                        }
                        else if (current_mode == KeyframeDragMode::ScaleDown) {
                            float new_min = selection_min_value + total_delta_y;
                            float scale = (selection_max_value - new_min) / original_height;
                            kf->sample.value[i] = selection_max_value - (selection_max_value - orig_val) * scale;
                            value_changed = true;
                        }

                        if (value_changed) {
                            const auto& range = kr.getRange();
                            if (range.has_value()) {
                                const auto& r = range.value();
                                kf->sample.value[i] = std::clamp(kf->sample.value[i], r.Min[i], r.Max[i]);
                            }
                        }
                    };

                    if (dragged_value_index != -1 && dragged_value_index < kf->sample.value.size()) {
                        if (!drag_keyframe.original_values.empty())
                            apply_y_transformation(dragged_value_index, drag_keyframe.original_values[0]);
                    }
                    else if (dragged_value_index == -1) {
                        for (size_t i = 0; i < kf->sample.value.size(); i++) {
                            if (i < drag_keyframe.original_values.size())
                                apply_y_transformation(i, drag_keyframe.original_values[i]);
                        }
                    }

                    if (value_changed) kr.setNeedPrecompute();

                    float delta_to_apply = target_time - kf->sample.time;
                    if (delta_to_apply != 0.f)
                        kr.setDeltaTimeForKeyframeByID(delta_to_apply, drag_keyframe.keyframe.id);


                }
            });
        }
    }
    else {
        if (drag_started) {
            for (const auto& drag_keyframe : original_times) {
                const auto& layer = ctx->layersData->GetLayerByID(drag_keyframe.keyframe.property.layer.id);
                auto& properties = layer.get()->GetProperties();
                properties.applyToKr(drag_keyframe.keyframe.property.name, [&](auto& kr) {
                        kr.applyDeltaTime();
                        kr.resolveOverlaps(drag_keyframe.keyframe.id);
                    }
                );
            }

            drag_started = false;
            original_times.clear();
        }
    }
}