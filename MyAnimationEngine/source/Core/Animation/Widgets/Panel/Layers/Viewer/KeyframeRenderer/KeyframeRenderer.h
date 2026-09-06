#ifndef KEYFRAME_RENDERER_H
#define KEYFRAME_RENDERER_H

#include "ThirdParty/ImGui/imgui_internal.h"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_glfw.h"
#include "ThirdParty/ImGui/imgui_impl_opengl3.h"

#include "Core/Animation/Keyframe/Keyframe.h"

namespace KeyframeRenderer {

    struct Parameters {
        ImColor color;
        ImColor color_side;
        ImColor outline;
        bool isLeft;
        bool isMiddle;
        bool isRight;
        bool isOnlyOne;
        bool isSelected;
        float size;
    };

    namespace Private {
        struct KeyframePoints {
            ImVec2 LU, MU, RU;
            ImVec2 LM, MM, RM;
            ImVec2 LD, MD, RD;

            ImVec2 b_LU, b_RU;
            ImVec2 b_LD, b_RD;

            ImVec2 b_MM_L_U, b_MM_R_U;
            ImVec2 b_MM_L_D, b_MM_R_D;

            ImVec2 b_MM_U, b_MM_D;

            ImVec2 b_MM_U_o, b_MM_D_o;

            ImVec2 b_MM_L_U_o, b_MM_R_U_o;
            ImVec2 b_MM_L_D_o, b_MM_R_D_o;
        };

        enum class Side { LEFT, RIGHT };

        ImColor GetHalfColor(Side side, TypeKeyframe type_in, TypeKeyframe type_out, const Parameters& p);
        void DrawHalf(ImDrawList* draw_list, const Side side, const ImVec2& pos, TypeKeyframe type_in, TypeKeyframe type_out, const Parameters& params, const KeyframePoints& pts);
        void DrawOutline(ImDrawList* draw_list, TypeKeyframe type_in, TypeKeyframe type_out, const ImVec2& pos, const Parameters& params, const KeyframePoints& pts);

        KeyframePoints CalcKeyframePoints(const ImVec2& pos, const Parameters& params);
    }

    inline void Draw(ImDrawList* draw_list, TypeKeyframe type_in, TypeKeyframe type_out, const ImVec2& pos_center, const Parameters& params) {
        using namespace KeyframeRenderer::Private;

        const auto pts = CalcKeyframePoints(pos_center, params);

        if (params.isLeft)
            type_in = type_out;

        if (params.isSelected) 
            DrawOutline(draw_list, type_in, type_out, pos_center, params, pts);
        
        DrawHalf(draw_list, Side::LEFT,  pos_center, type_in, type_out, params, pts);
        DrawHalf(draw_list, Side::RIGHT, pos_center, type_in, type_out, params, pts);

    }

}

#endif