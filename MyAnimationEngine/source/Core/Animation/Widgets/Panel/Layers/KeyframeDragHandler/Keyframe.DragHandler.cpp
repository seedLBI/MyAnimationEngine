#include "Keyframe.DragHandler.h"

#include "CursorManager/CursorManger.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

#include <cmath>
#include <algorithm>

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
                for (auto v : kf->sample.value) orig_vals.push_back((float)v); 

                bool has_in = kf->anchors.in.has_value();
                bool has_out = kf->anchors.out.has_value();
                std::vector<float> orig_in_vals, orig_in_times, orig_out_vals, orig_out_times;

                if (has_in) {
                    for (auto v : kf->anchors.in->value)  orig_in_vals.push_back((float)v);
                    for (auto t : kf->anchors.in->time)   orig_in_times.push_back(t);
                }
                if (has_out) {
                    for (auto v : kf->anchors.out->value) orig_out_vals.push_back((float)v);
                    for (auto t : kf->anchors.out->time)  orig_out_times.push_back(t);
                }

                float p_time = kf->sample.time;
                float n_time = kf->sample.time;
                uint64_t prev_id = 0;
                uint64_t next_id = 0;
                const auto& kfs = kr.getKeyframes();
                for (size_t i = 0; i < kfs.size(); i++) {
                    if (kfs[i].id == kf->id) {
                        if (i > 0) { p_time = kfs[i - 1].sample.time; prev_id = kfs[i - 1].id; }
                        if (i < kfs.size() - 1) { n_time = kfs[i + 1].sample.time; next_id = kfs[i + 1].id; }
                        break;
                    }
                }

                bool fix_prev_out = false;
                std::vector<float> prev_out_vals;
                if (prev_id != 0 && !ctx->selector->ContainsKeyframe({ sel_kf.property, prev_id })) {
                    auto* prev_nb = kr.getKeyframeByID(prev_id);
                    if (prev_nb && prev_nb->anchors.out.has_value()) {
                        fix_prev_out = true;
                        for (auto v : prev_nb->anchors.out->value) prev_out_vals.push_back(v);
                    }
                }

                bool fix_next_in = false;
                std::vector<float> next_in_vals;
                if (next_id != 0 && !ctx->selector->ContainsKeyframe({ sel_kf.property, next_id })) {
                    auto* next_nb = kr.getKeyframeByID(next_id);
                    if (next_nb && next_nb->anchors.in.has_value()) {
                        fix_next_in = true;
                        for (auto v : next_nb->anchors.in->value) next_in_vals.push_back(v);
                    }
                }

                original_times.push_back({ sel_kf, kf->sample.time, orig_vals, has_in, orig_in_vals, orig_in_times,
                                           has_out, orig_out_vals, orig_out_times, p_time, n_time,
                                           fix_prev_out, prev_id, prev_out_vals,
                                           fix_next_in, next_id, next_in_vals });
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

        const bool is_anchor_mode = (current_mode == KeyframeDragMode::DragAnchorIn ||
            current_mode == KeyframeDragMode::DragAnchorOut);

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

                    if (is_anchor_mode) {

                        if (kf->type == TypeKeyframe::AUTOBEZIER) {
                            kf->type = TypeKeyframe::BEZIER;
                        }

                        bool is_first_kf = std::abs(drag_keyframe.prev_kf_time - kf->sample.time) < 0.0001f;
                        bool is_last_kf = std::abs(drag_keyframe.next_kf_time - kf->sample.time) < 0.0001f;

                        float dt_prev = (std::max)(kf->sample.time - drag_keyframe.prev_kf_time, 0.0001f);
                        float dt_next = (std::max)(drag_keyframe.next_kf_time - kf->sample.time, 0.0001f);
                        const auto& range = kr.getRange();
                        
                        for (size_t i = 0; i < kf->sample.value.size(); i++) {
                            if (dragged_value_index != -1 && i != (size_t)dragged_value_index)
                                continue;

                            float kf_time = kf->sample.time;
                            float kf_val = kf->sample.value[i];

                            if (current_mode == KeyframeDragMode::DragAnchorIn && kf->anchors.in.has_value()) {
                                float orig_in_time_abs = drag_keyframe.original_time - (drag_keyframe.original_in_anchor_times[i] * dt_prev);
                                float new_in_time_abs = orig_in_time_abs + total_delta_x;
                                float raw_in_val = drag_keyframe.original_in_anchor_values[i] + total_delta_y;

                                float raw_dx_in = new_in_time_abs - kf_time;
                                float raw_dy_in = raw_in_val - kf_val;

                                float min_dx_in = -std::abs(raw_dy_in) / std::tan(max_angle_rad);
                                if (raw_dx_in > min_dx_in) {
                                    raw_dx_in = min_dx_in;
                                    new_in_time_abs = kf_time + raw_dx_in;
                                }

                                float in_time_norm = std::clamp((kf_time - new_in_time_abs) / dt_prev, 0.0f, 1.0f);
                                float actual_in_val = raw_in_val;
                                if (range.has_value()) {
                                    actual_in_val = std::clamp(actual_in_val, (float)range->Min[i], (float)range->Max[i]);
                                }

                                kf->anchors.in->time[i] = in_time_norm;
                                kf->anchors.in->value[i] = actual_in_val;

                                float actual_dx_in = -in_time_norm * dt_prev;
                                float actual_dy_in = actual_in_val - kf_val;

                                if (kf->anchors.out.has_value()) {
                                    float actual_in_len = std::sqrt(actual_dx_in * actual_dx_in + actual_dy_in * actual_dy_in);

                                    if (actual_in_len > 1e-5f) {
                                        float dir_x = -actual_dx_in / actual_in_len;
                                        float dir_y = -actual_dy_in / actual_in_len;

                                        float orig_time_norm_out = drag_keyframe.original_out_anchor_times[i];
                                        float orig_dy_out = drag_keyframe.original_out_anchor_values[i] - drag_keyframe.original_values[i];
                                        float orig_norm_len = std::sqrt(orig_time_norm_out * orig_time_norm_out + orig_dy_out * orig_dy_out);

                                        float denom = std::sqrt(dir_x * dir_x + dir_y * dir_y * dt_next * dt_next);
                                        float out_time_norm = 0.0f;
                                        float new_dy_out = 0.0f;

                                        if (denom > 1e-7f) {
                                            out_time_norm = orig_norm_len * dir_x / denom;
                                            new_dy_out = orig_norm_len * dir_y * dt_next / denom;
                                        }

                                        float new_dx_out = out_time_norm * dt_next;
                                        float scale = 1.0f;

                                        if (!is_last_kf && new_dx_out > dt_next) {
                                            scale = (std::min)(scale, dt_next / new_dx_out);
                                        }

                                        if (range.has_value()) {
                                            const auto& r = range.value();
                                            float target_val_out = kf_val + new_dy_out;
                                            if (target_val_out > r.Max[i] && new_dy_out > 1e-5f) {
                                                scale = (std::min)(scale, (r.Max[i] - kf_val) / new_dy_out);
                                            }
                                            else if (target_val_out < r.Min[i] && new_dy_out < -1e-5f) {
                                                scale = (std::min)(scale, (r.Min[i] - kf_val) / new_dy_out);
                                            }
                                        }

                                        scale = (std::max)(0.0f, scale);
                                        out_time_norm *= scale;
                                        new_dy_out *= scale;

                                        if (!is_last_kf) {
                                            kf->anchors.out->time[i] = std::clamp(out_time_norm, 0.0f, 1.0f);
                                        }
                                        kf->anchors.out->value[i] = kf_val + new_dy_out;
                                    }
                                }
                            }
                            else if (current_mode == KeyframeDragMode::DragAnchorOut && kf->anchors.out.has_value()) {
                                float orig_out_time_abs = drag_keyframe.original_time + (drag_keyframe.original_out_anchor_times[i] * dt_next);
                                float new_out_time_abs = orig_out_time_abs + total_delta_x;
                                float raw_out_val = drag_keyframe.original_out_anchor_values[i] + total_delta_y;

                                float raw_dx_out = new_out_time_abs - kf_time;
                                float raw_dy_out = raw_out_val - kf_val;

                                float min_dx_out = std::abs(raw_dy_out) / std::tan(max_angle_rad);
                                if (raw_dx_out < min_dx_out) {
                                    raw_dx_out = min_dx_out;
                                    new_out_time_abs = kf_time + raw_dx_out;
                                }

                                float out_time_norm = std::clamp((new_out_time_abs - kf_time) / dt_next, 0.0f, 1.0f);
                                float actual_out_val = raw_out_val;
                                if (range.has_value()) {
                                    actual_out_val = std::clamp(actual_out_val, (float)range->Min[i], (float)range->Max[i]);
                                }

                                kf->anchors.out->time[i] = out_time_norm;
                                kf->anchors.out->value[i] = actual_out_val;

                                float actual_dx_out = out_time_norm * dt_next;
                                float actual_dy_out = actual_out_val - kf_val;

                                if (kf->anchors.in.has_value()) {
                                    float actual_out_len = std::sqrt(actual_dx_out * actual_dx_out + actual_dy_out * actual_dy_out);

                                    if (actual_out_len > 1e-5f) {
                                        float dir_x = -actual_dx_out / actual_out_len;
                                        float dir_y = -actual_dy_out / actual_out_len;

                                        float orig_time_norm_in = drag_keyframe.original_in_anchor_times[i];
                                        float orig_dy_in = drag_keyframe.original_in_anchor_values[i] - drag_keyframe.original_values[i];
                                        float orig_norm_len = std::sqrt(orig_time_norm_in * orig_time_norm_in + orig_dy_in * orig_dy_in);

                                        float denom = std::sqrt(dir_x * dir_x + dir_y * dir_y * dt_prev * dt_prev);
                                        float in_time_norm = 0.0f;
                                        float new_dy_in = 0.0f;

                                        if (denom > 1e-7f) {
                                            in_time_norm = orig_norm_len * (-dir_x) / denom;
                                            new_dy_in = orig_norm_len * dir_y * dt_prev / denom;
                                        }

                                        float new_dx_in = -in_time_norm * dt_prev;
                                        float scale = 1.0f;

                                        if (!is_first_kf && -new_dx_in > dt_prev) {
                                            scale = (std::min)(scale, dt_prev / -new_dx_in);
                                        }

                                        if (range.has_value()) {
                                            const auto& r = range.value();
                                            float target_val_in = kf_val + new_dy_in;
                                            if (target_val_in > r.Max[i] && new_dy_in > 1e-5f) {
                                                scale = (std::min)(scale, (r.Max[i] - kf_val) / new_dy_in);
                                            }
                                            else if (target_val_in < r.Min[i] && new_dy_in < -1e-5f) {
                                                scale = (std::min)(scale, (r.Min[i] - kf_val) / new_dy_in);
                                            }
                                        }

                                        scale = (std::max)(0.0f, scale);
                                        in_time_norm *= scale;
                                        new_dy_in *= scale;

                                        if (!is_first_kf) {
                                            kf->anchors.in->time[i] = std::clamp(in_time_norm, 0.0f, 1.0f);
                                        }
                                        kf->anchors.in->value[i] = kf_val + new_dy_in;
                                    }
                                }
                            }
                        }

                        kr.setNeedPrecompute();
                        return;
                    }

                    if (current_mode == KeyframeDragMode::TranslateXY ||
                        current_mode == KeyframeDragMode::ScaleUp ||
                        current_mode == KeyframeDragMode::ScaleDown)
                    {
                        auto transform_val = [&](float v) -> float {
                            if (current_mode == KeyframeDragMode::TranslateXY) {
                                return v + total_delta_y;
                            }
                            else if (current_mode == KeyframeDragMode::ScaleUp) {
                                float new_max = selection_max_value + total_delta_y;
                                float scale = (new_max - selection_min_value) / original_height;
                                return selection_min_value + (v - selection_min_value) * scale;
                            }
                            else if (current_mode == KeyframeDragMode::ScaleDown) {
                                float new_min = selection_min_value + total_delta_y;
                                float scale = (selection_max_value - new_min) / original_height;
                                return selection_max_value - (selection_max_value - v) * scale;
                            }
                            return v;
                            };

                        const auto& range = kr.getRange();
                        for (size_t i = 0; i < kf->sample.value.size() && i < drag_keyframe.original_values.size(); i++) {
                            if (dragged_value_index != -1 && i != (size_t)dragged_value_index)
                                continue;

                            float new_val = transform_val(drag_keyframe.original_values[i]);
                            if (range.has_value()) {
                                const auto& r = range.value();
                                new_val = std::clamp(new_val, (float)r.Min[i], (float)r.Max[i]);
                            }
                            kf->sample.value[i] = new_val;
                        }
                        kr.setNeedPrecompute();
                    }

                    float delta_to_apply = target_time - kf->sample.time;
                    if (delta_to_apply != 0.f)
                        kr.setDeltaTimeForKeyframeByID(delta_to_apply, drag_keyframe.keyframe.id);


                }
            });
        }


        if (!is_anchor_mode) {
            for (const auto& drag_keyframe : original_times) {
                const auto& layer = ctx->layersData->GetLayerByID(drag_keyframe.keyframe.property.layer.id);
                auto& properties = layer.get()->GetProperties();

                properties.applyToKr(drag_keyframe.keyframe.property.name, [&](auto& kr) {
                    auto* kf = kr.getKeyframeByID(drag_keyframe.keyframe.id);
                    if (!kf) return;
                    if (!kf->anchors.in.has_value() && !kf->anchors.out.has_value()) return;

                    const auto& kfs = kr.getKeyframes();
                    uint64_t idx_u = kr.GetIndexByID(kf->id);
                    if (idx_u == UINT64_MAX || idx_u >= (uint64_t)kfs.size()) return;
                    size_t idx = (size_t)idx_u;

                    float cur_time = kfs[idx].sample.time;
                    float prev_time = (idx > 0) ? kfs[idx - 1].sample.time : cur_time;
                    float next_time = (idx + 1 < kfs.size()) ? kfs[idx + 1].sample.time : cur_time;

                    float dt_prev_new = (std::max)(cur_time - prev_time, 0.0f);
                    float dt_next_new = (std::max)(next_time - cur_time, 0.0f);

                    float dt_prev_old = (std::max)(drag_keyframe.original_time - drag_keyframe.prev_kf_time, 0.0f);
                    float dt_next_old = (std::max)(drag_keyframe.next_kf_time - drag_keyframe.original_time, 0.0f);

                    float ratio_prev = (dt_prev_old > 1e-6f) ? dt_prev_new / dt_prev_old : 1.0f;
                    float ratio_next = (dt_next_old > 1e-6f) ? dt_next_new / dt_next_old : 1.0f;

                    for (size_t i = 0; i < kf->sample.value.size() && i < drag_keyframe.original_values.size(); i++) {
                        float v_old = drag_keyframe.original_values[i];
                        float v_new = (float)kf->sample.value[i];


                        if (drag_keyframe.has_in_anchor && kf->anchors.in.has_value() &&
                            i < drag_keyframe.original_in_anchor_values.size())
                        {
                            kf->anchors.in->value[i] =
                                v_new + (drag_keyframe.original_in_anchor_values[i] - v_old) * ratio_prev;
                        }

                        if (drag_keyframe.has_out_anchor && kf->anchors.out.has_value() &&
                            i < drag_keyframe.original_out_anchor_values.size())
                        {
                            kf->anchors.out->value[i] =
                                v_new + (drag_keyframe.original_out_anchor_values[i] - v_old) * ratio_next;
                        }
                    }

                    if (drag_keyframe.fix_prev_out) {
                        auto* prev_nb = kr.getKeyframeByID(drag_keyframe.prev_neighbor_id);
                        if (prev_nb && prev_nb->anchors.out.has_value()) {
                            float nb_dt_old = (std::max)(drag_keyframe.original_time - drag_keyframe.prev_kf_time, 0.0f);
                            float nb_dt_new = kf->sample.time - prev_nb->sample.time;
                            if (nb_dt_old > 1e-6f && nb_dt_new > 1e-6f) {
                                float nb_ratio = nb_dt_new / nb_dt_old;
                                for (size_t i = 0; i < prev_nb->sample.value.size() &&
                                    i < drag_keyframe.prev_neighbor_out_values.size(); i++)
                                {
                                    float nb_v = (float)prev_nb->sample.value[i];
                                    prev_nb->anchors.out->value[i] =
                                        nb_v + (drag_keyframe.prev_neighbor_out_values[i] - nb_v) * nb_ratio;
                                }
                            }
                        }
                    }

                    if (drag_keyframe.fix_next_in) {
                        auto* next_nb = kr.getKeyframeByID(drag_keyframe.next_neighbor_id);
                        if (next_nb && next_nb->anchors.in.has_value()) {
                            float nb_dt_old = (std::max)(drag_keyframe.next_kf_time - drag_keyframe.original_time, 0.0f);
                            float nb_dt_new = next_nb->sample.time - kf->sample.time;
                            if (nb_dt_old > 1e-6f && nb_dt_new > 1e-6f) {
                                float nb_ratio = nb_dt_new / nb_dt_old;
                                for (size_t i = 0; i < next_nb->sample.value.size() &&
                                    i < drag_keyframe.next_neighbor_in_values.size(); i++)
                                {
                                    float nb_v = (float)next_nb->sample.value[i];
                                    next_nb->anchors.in->value[i] =
                                        nb_v + (drag_keyframe.next_neighbor_in_values[i] - nb_v) * nb_ratio;
                                }
                            }
                        }
                    }

                    kr.setNeedPrecompute();
                });
            }
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