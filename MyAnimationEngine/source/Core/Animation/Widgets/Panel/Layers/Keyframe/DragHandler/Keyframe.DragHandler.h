#ifndef KEYFRAME_DRAG_HANDLER_H
#define KEYFRAME_DRAG_HANDLER_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"
#include <vector>

enum class KeyframeDragMode {
    TranslateX,
    TranslateXY,
    ScaleLeft,
    ScaleRight,
    ScaleUp,
    ScaleDown,
    DragAnchorIn,
    DragAnchorOut
};

class KeyframeDragHandler {
public:
    KeyframeDragHandler(TimelineContext* ctx);

    void BeginDrag(float start_global_x, float start_global_y, int64_t index_value, const SelectedKeyframe& current, KeyframeDragMode mode = KeyframeDragMode::TranslateX);
    void ProcessDrag(float current_global_x, float current_global_y);

    bool IsDragging() const { return drag_started; }

private:
    TimelineContext* ctx;


    const float max_angle_rad = 90.0f * 3.14159265358979323846f / 180.0f;


    bool drag_started = false;
    float startedDrag_global_x = 0.0f;
    float startedDrag_global_y = 0.0f;
    int64_t dragged_value_index = -1;

    KeyframeDragMode current_mode = KeyframeDragMode::TranslateX;
    float selection_min_time = 0.f;
    float selection_max_time = 0.f;

    float selection_min_value = 0.f;
    float selection_max_value = 0.f;

    struct DragData {
        SelectedKeyframe keyframe;
        float original_time;
        std::vector<float> original_values;

        bool has_in_anchor = false;
        std::vector<float> original_in_anchor_values;
        std::vector<float> original_in_anchor_times;

        bool has_out_anchor = false;
        std::vector<float> original_out_anchor_values;
        std::vector<float> original_out_anchor_times;

        float prev_kf_time = 0.f;
        float next_kf_time = 0.f;

        bool fix_prev_out = false;
        uint64_t prev_neighbor_id = 0;
        std::vector<float> prev_neighbor_out_values;

        bool fix_next_in = false;
        uint64_t next_neighbor_id = 0;
        std::vector<float> next_neighbor_in_values;
    };

    std::vector<DragData> original_times;
};

#endif