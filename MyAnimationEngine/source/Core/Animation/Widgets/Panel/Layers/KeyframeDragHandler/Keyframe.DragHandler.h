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
    ScaleDown
};

class KeyframeDragHandler {
public:
    KeyframeDragHandler(TimelineContext* ctx);

    void BeginDrag(float start_global_x, float start_global_y, int64_t index_value, const SelectedKeyframe& current, KeyframeDragMode mode = KeyframeDragMode::TranslateX);
    void ProcessDrag(float current_global_x, float current_global_y);

    bool IsDragging() const { return drag_started; }

private:
    TimelineContext* ctx;

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
        bool has_out_anchor = false;
        std::vector<float> original_out_anchor_values;
    };

    std::vector<DragData> original_times;
};

#endif