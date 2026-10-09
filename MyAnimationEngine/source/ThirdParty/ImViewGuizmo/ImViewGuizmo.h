#pragma once

/*===============================================
ImViewGuizmo Single-Header Library by Marcel Kazemi

To use, do this in one (and only one) of your C++ files:
#define IMVIEWGUIZMO_IMPLEMENTATION
 #include "ImViewGuizmo.h"

In all other files, just include the header as usual:
#include "ImViewGuizmo.h"

Copyright (c) 2025 Marcel Kazemi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
======================================================*/

#ifndef IMVIEWGUIZMO_VEC3
#define IMVIEWGUIZMO_USE_GLM_DEFAULTS
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CursorManager/CursorManger.h>

#define IMVIEWGUIZMO_VEC3 glm::vec3
#define IMVIEWGUIZMO_VEC4 glm::vec4
#define IMVIEWGUIZMO_QUAT glm::quat
#define IMVIEWGUIZMO_MAT4 glm::mat4
#endif

namespace ImViewGuizmo
{
    using vec3_t = IMVIEWGUIZMO_VEC3;
    using vec4_t = IMVIEWGUIZMO_VEC4;
    using quat_t = IMVIEWGUIZMO_QUAT;
    using mat4_t = IMVIEWGUIZMO_MAT4;

#ifdef IMVIEWGUIZMO_USE_GLM_DEFAULTS
    namespace GizmoMath {
        inline float get_vec_comp(const vec3_t& v, int index) { return v[index]; }
        inline float get_vec_comp(const vec4_t& v, int index) { return v[index]; }

        inline vec3_t make_vec3(float x, float y, float z) { return vec3_t(x, y, z); }
        inline vec4_t make_vec4(float x, float y, float z, float w) { return vec4_t(x, y, z, w); }
        inline quat_t angleAxis(float angle, const vec3_t& axis) { return glm::angleAxis(angle, axis); }
        inline quat_t quatLookAt(const vec3_t& dir, const vec3_t& up) { return glm::quatLookAt(dir, up); }
        inline mat4_t mat4_identity() { return mat4_t(1.0f); }

        inline vec3_t cross(const vec3_t& a, const vec3_t& b) { return glm::cross(a, b); }
        inline float dot(const vec3_t& a, const vec3_t& b) { return glm::dot(a, b); }
        inline float dot(const quat_t& a, const quat_t& b) { return glm::dot(a, b); }
        inline float length(const vec3_t& v) { return glm::length(v); }
        inline float length2(const vec3_t& v) { return glm::length2(v); }
        inline vec3_t normalize(const vec3_t& v) { return glm::normalize(v); }
        inline vec3_t mix(const vec3_t& a, const vec3_t& b, float t) { return glm::mix(a, b, t); }
        inline vec3_t add_vv(const vec3_t& a, const vec3_t& b) { return a + b; }
        inline vec3_t subtract_vv(const vec3_t& a, const vec3_t& b) { return a - b; }
        inline vec3_t multiply_vf(const vec3_t& v, float f) { return v * f; }

        inline mat4_t mat4_cast(const quat_t& q) { return glm::mat4_cast(q); }
        inline mat4_t transpose(const mat4_t& m) { return glm::transpose(m); }
        inline vec3_t get_matrix_col(const mat4_t& m, int col) { return vec3_t(m[col]); }
        inline void set_matrix_col(mat4_t& m, int col, const vec4_t& v) { m[col] = v; }

        inline quat_t multiply_qq(const quat_t& a, const quat_t& b) { return a * b; }
        inline vec3_t multiply_qv(const quat_t& q, const vec3_t& v) { return q * v; }
        inline mat4_t multiply_mm(const mat4_t& a, const mat4_t& b) { return a * b; }
        inline vec4_t multiply_mv4(const mat4_t& m, const vec4_t& v) { return m * v; }
    }
#else
    // User must provide a GizmoMath namespace implementing all functions if using custom types
#endif

    // INTERFACE
    struct Style {
        float scale = 0.6f;

        float lineLength = 0.5f;
        float lineWidth = 4.0f;
        float circleRadius = 15.0f;

        ImU32 axisColors[3] = {
            IM_COL32(233, 55, 81, 255),  // X / -X (red)
            IM_COL32(119, 179, 22, 255), // Y / -Y (green)
            IM_COL32(49, 122, 205, 255)  // Z / -Z (blue)
        };

        float darkFactor = 0.45f;
        float backsideAlpha = 0.4f; 
        float outlineWidth = 1.0f;
        float outlineLightenFactor = 0.4f;
        float activeOutlineLightenFactor = 0.2f;

        float activeAxisThreshold = 0.99f;

        // Labels
        float labelSize = 1.3f;
        const char* axisLabels[6] = { "X", "Y", "Z", "-X", "-Y", "-Z" };
        ImU32 labelColor = IM_COL32(0, 0, 0, 255);              // black, always fully opaque
        ImU32 labelHoveredColor = IM_COL32(255, 255, 255, 255); // white while hovered

        //Big Circle
        float bigCircleRadius = 80.0f;
        ImU32 bigCircleColor = IM_COL32(255, 255, 255, 50);

        // Animation
        bool animateSnap = true;
        float snapAnimationDuration = 0.5f; // in seconds

        // Zoom/Pan Button Visuals
        float toolButtonRadius = 25.f;
        float toolButtonInnerPadding = 4.f;

        ImU32 toolButtonColor = IM_COL32(144, 144, 144, 50);
        ImU32 toolButtonHoveredColor = IM_COL32(215, 215, 215, 50);
        ImU32 toolButtonIconColor = IM_COL32(215, 215, 215, 225);
    };

    // Constants
    const vec3_t origin = GizmoMath::make_vec3(0.f, 0.f, 0.f);
    const vec3_t worldRight = GizmoMath::make_vec3(1.f, 0.f, 0.f); // +X
    const vec3_t worldUp = GizmoMath::make_vec3(0.f, -1.f, 0.f);    // -Y
    const vec3_t worldForward = GizmoMath::make_vec3(0.f, 0.f, 1.f); // +Z
    const vec3_t axisVectors[3] = { GizmoMath::make_vec3(1,0,0), GizmoMath::make_vec3(0,1,0), GizmoMath::make_vec3(0,0,1) };
    static constexpr mat4_t gizmoProjectionMatrix = {
        -1.0f,  0.0f,  0.0f,    0.0f,
         0.0f,  1.0f,  0.0f,    0.0f,
         0.0f,  0.0f, -0.01f,   0.0f,
         0.0f,  0.0f,  0.0f,    1.0f
    };

    inline Style& GetStyle() {
        static Style style;
        return style;
    }

    // Gizmo Axis Struct
    struct GizmoAxis {
        int id; // 0-5 for (+X,-X,+Y,-Y,+Z,-Z), 6=center
        int axisIndex; // 0=X, 1=Y, 2=Z
        float depth; // Screen-space depth
        vec3_t direction; // 3D vector
    };

    enum ActiveTool {
        TOOL_NONE,
        TOOL_GIZMO,
        TOOL_DOLLY,
        TOOL_PAN
    };

    struct Context {
        int hoveredAxisID = -1;
        bool isGizmoButtonHovered = false;
        bool isZoomButtonHovered = false;
        bool isPanButtonHovered = false;
        ActiveTool activeTool = TOOL_NONE;

        // Animation state
        bool isAnimating = false;
        float animationStartTime = 0.f;

        vec3_t startPos;
        vec3_t targetPos;
        vec3_t startUp;
        vec3_t targetUp;

        vec3_t animStartDir, animTargetDir;
        float animStartDist, animTargetDist;

        void Reset() {
            
            isGizmoButtonHovered = false;
            isZoomButtonHovered = false;
            isPanButtonHovered = false;
        }
    };

    // Global context
    inline Context& GetContext() {
        static Context ctx;
        return ctx;
    }

    static float ImLengthSqr(const ImVec2& v) { return v.x * v.x + v.y * v.y; }
    static float mix(float a, float b, float t) { return a * (1.0f - t) + b * t; }

    inline void BeginFrame() {
        static int lastFrame = -1;
        const int currentFrame = ImGui::GetFrameCount();
        if (lastFrame != currentFrame) {
            lastFrame = currentFrame;
            GetContext().hoveredAxisID = -1;
        }

    }

    inline bool IsUsing() {
        return GetContext().activeTool != TOOL_NONE;
    }

    inline bool IsOver() {
        const Context& ctx = GetContext();
        return ctx.isGizmoButtonHovered || ctx.isZoomButtonHovered || ctx.isPanButtonHovered;
    }

    /// @brief Renders and handles the view gizmo logic.
    /// @param cameraPos The position of the camera (will be modified).
    /// @param cameraRot The rotation of the camera (will be modified).
    /// @param pivot The position the camera will rotate around.
    /// @param position The top-left screen position where the gizmo should be rendered.
    /// @param rotationSpeed The rotation speed when dragging the gizmo.
    /// @return True if the camera was modified, false otherwise.
    bool Rotate(vec3_t& cameraPos, quat_t& cameraRot, const vec3_t& pivot, ImVec2 position, float rotationSpeed = 0.01f);

    /// @brief Renders a zoom button and handles its logic.
    /// @param cameraPos The position of the camera (will be modified).
    /// @param cameraRot The rotation of the camera (used for direction).
    /// @param position The top-left screen position of the button.
    /// @param zoomSpeed The speed/sensitivity of the zoom.
    /// @return True if the camera was modified, false otherwise.
    bool Dolly(vec3_t& cameraPos, const quat_t& cameraRot, ImVec2 position, float zoomSpeed = 0.05f);

    /// @brief Renders a pan button and handles its logic.
    /// @param cameraPos The position of the camera (will be modified).
    /// @param cameraRot The rotation of the camera (used for direction).
    /// @param position The top-left screen position of the button.
    /// @param panSpeed The speed/sensitivity of the pan.
    /// @return True if the camera was modified, false otherwise.
    bool Pan(vec3_t& cameraPos, const quat_t& cameraRot, ImVec2 position, float panSpeed = 0.01f);
}

#ifdef IMVIEWGUIZMO_IMPLEMENTATION
namespace ImViewGuizmo {

    // ---------- color helpers ----------
    inline ImU32 DarkenColor(ImU32 color, float factor) {
        const ImVec4 c = ImGui::ColorConvertU32ToFloat4(color);
        return ImGui::ColorConvertFloat4ToU32(ImVec4(c.x * factor, c.y * factor, c.z * factor, c.w));
    }

    inline ImU32 LightenColor(ImU32 color, float factor) {
        const ImVec4 c = ImGui::ColorConvertU32ToFloat4(color);
        return ImGui::ColorConvertFloat4ToU32(ImVec4(
            c.x + (1.0f - c.x) * factor,
            c.y + (1.0f - c.y) * factor,
            c.z + (1.0f - c.z) * factor, c.w));
    }

    inline ImU32 MixColor(ImU32 a, ImU32 b, float t) {
        const ImVec4 ca = ImGui::ColorConvertU32ToFloat4(a);
        const ImVec4 cb = ImGui::ColorConvertU32ToFloat4(b);
        return ImGui::ColorConvertFloat4ToU32(ImVec4(
            ca.x + (cb.x - ca.x) * t,
            ca.y + (cb.y - ca.y) * t,
            ca.z + (cb.z - ca.z) * t,
            ca.w + (cb.w - ca.w) * t));
    }

    inline ImU32 WithAlpha(ImU32 color, float alpha) {
        const ImVec4 c = ImGui::ColorConvertU32ToFloat4(color);
        return ImGui::ColorConvertFloat4ToU32(ImVec4(c.x, c.y, c.z, alpha));
    }

    /// @brief Renders and handles the view gizmo logic.
    inline bool Rotate(vec3_t& cameraPos, quat_t& cameraRot, const vec3_t& pivot, ImVec2 position, float rotationSpeed) {

        auto& io = ImGui::GetIO();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        auto& ctx = GetContext();
        auto& style = GetStyle();
        bool wasModified = false;

        // Animation 
        if (ctx.isAnimating) {
            float elapsedTime = static_cast<float>(ImGui::GetTime()) - ctx.animationStartTime;
            float t = (std::min)(1.0f, elapsedTime / style.snapAnimationDuration);
            t = 1.0f - (1.0f - t) * (1.0f - t); // ease-out quad

            vec3_t currentDir;
            vec3_t currentUp;

            // Start and target directions are (almost) opposite (X <-> -X toggle):
            // a plain mix() would pass through the zero vector, so swing the camera
            // around a fixed axis through a half-circle instead.
            if (GizmoMath::dot(ctx.animStartDir, ctx.animTargetDir) < -0.999f) {
                // Horizontal views (X<->-X, Z<->-Z): orbit around the vertical axis.
                // Vertical views (+Y<->-Y): flip around a horizontal axis.
                vec3_t swingAxis = (fabsf(GizmoMath::dot(ctx.animStartDir, worldUp)) < 0.99f)
                    ? GizmoMath::multiply_vf(worldUp, -1.0f) // vertical axis
                    : GizmoMath::normalize(GizmoMath::cross(ctx.animStartDir, ctx.targetUp));

                quat_t swing = GizmoMath::angleAxis(3.14159265f * t, swingAxis);
                currentDir = GizmoMath::normalize(GizmoMath::multiply_qv(swing, ctx.animStartDir));
                currentUp = GizmoMath::normalize(GizmoMath::multiply_qv(swing, ctx.startUp));
            }
            else {
                currentDir = GizmoMath::normalize(GizmoMath::mix(ctx.animStartDir, ctx.animTargetDir, t));
                currentUp = GizmoMath::normalize(GizmoMath::mix(ctx.startUp, ctx.targetUp, t));

                currentUp = GizmoMath::subtract_vv(
                    currentUp, GizmoMath::multiply_vf(currentDir, GizmoMath::dot(currentDir, currentUp)));
                if (GizmoMath::length2(currentUp) < 1e-6f) {
                    const vec3_t& fb = (fabsf(currentDir.y) < 0.9f) ? worldUp : worldForward;
                    currentUp = GizmoMath::cross(currentDir, fb);
                }
                currentUp = GizmoMath::normalize(currentUp);
            }

            float currentDistance = mix(ctx.animStartDist, ctx.animTargetDist, t);
            cameraPos = GizmoMath::add_vv(pivot, GizmoMath::multiply_vf(currentDir, currentDistance));

            cameraRot = GizmoMath::quatLookAt(currentDir, currentUp);

            wasModified = true;

            if (t >= 1.0f) {
                cameraPos = ctx.targetPos;
                cameraRot = GizmoMath::quatLookAt(ctx.animTargetDir, ctx.targetUp);
                ctx.isAnimating = false;
            }
        }

        // Gizmo sizes 
        const float gizmoDiameter = 256.f * style.scale;
        const float halfGizmoSize = gizmoDiameter / 2.f;
        const float scaledCircleRadius = style.circleRadius * style.scale;
        const float scaledBigCircleRadius = style.bigCircleRadius * style.scale;
        const float scaledLineWidth = style.lineWidth * style.scale;
        const float scaledOutlineWidth = (std::max)(1.0f, style.outlineWidth * style.scale);
        const float scaledFontSize = ImGui::GetFontSize() * style.scale * style.labelSize;
        const float gizmoAreaRadius = (halfGizmoSize + scaledCircleRadius) * style.scale;

        // Gizmo view matrix (transpose of rotation) 
        mat4_t rotMat4 = GizmoMath::mat4_cast(cameraRot);

        vec3_t rcol0 = GizmoMath::get_matrix_col(rotMat4, 0);
        vec3_t rcol1 = GizmoMath::get_matrix_col(rotMat4, 1);
        vec3_t rcol2 = GizmoMath::get_matrix_col(rotMat4, 2);

        mat4_t gizmoViewMatrix(1.0f);
        gizmoViewMatrix[0][0] = rcol0.x; gizmoViewMatrix[0][1] = rcol1.x; gizmoViewMatrix[0][2] = rcol2.x; gizmoViewMatrix[0][3] = 0.0f;
        gizmoViewMatrix[1][0] = rcol0.y; gizmoViewMatrix[1][1] = rcol1.y; gizmoViewMatrix[1][2] = rcol2.y; gizmoViewMatrix[1][3] = 0.0f;
        gizmoViewMatrix[2][0] = rcol0.z; gizmoViewMatrix[2][1] = rcol1.z; gizmoViewMatrix[2][2] = rcol2.z; gizmoViewMatrix[2][3] = 0.0f;
        gizmoViewMatrix[3][0] = 0.0f;    gizmoViewMatrix[3][1] = 0.0f;    gizmoViewMatrix[3][2] = 0.0f;    gizmoViewMatrix[3][3] = 1.0f;

        mat4_t gizmoMvp = GizmoMath::multiply_mm(gizmoProjectionMatrix, gizmoViewMatrix);

        // Axes (same ordering/depth calc as glm version) 
        std::array<GizmoAxis, 6> axes;

        vec3_t xAxisInView = GizmoMath::get_matrix_col(gizmoViewMatrix, 0);
        vec3_t yAxisInView = GizmoMath::get_matrix_col(gizmoViewMatrix, 1);
        vec3_t zAxisInView = GizmoMath::get_matrix_col(gizmoViewMatrix, 2);

        axes[0] = { 0, 0, xAxisInView.z, axisVectors[0] };
        axes[1] = { 1, 0, -xAxisInView.z, GizmoMath::multiply_vf(axisVectors[0], -1.0f) };
        axes[2] = { 2, 1, yAxisInView.z, axisVectors[1] };
        axes[3] = { 3, 1, -yAxisInView.z, GizmoMath::multiply_vf(axisVectors[1], -1.0f) };
        axes[4] = { 4, 2, zAxisInView.z, axisVectors[2] };
        axes[5] = { 5, 2, -zAxisInView.z, GizmoMath::multiply_vf(axisVectors[2], -1.0f) };

        std::sort(axes.begin(), axes.end(), [](const GizmoAxis& a, const GizmoAxis& b) {
            return a.depth < b.depth;
            });

        // Active handle ("current view"): the one the camera looks straight down
        // at — it sits at the center of the gizmo, facing the viewer. The check
        // uses a generous tolerance (style.activeAxisThreshold, default ~8
        // degrees) so the state engages reliably after a snap even when the
        // app rebuilds or clamps the camera rotation afterwards.
        // NOTE: clicking a handle moves the camera to the opposite side of the
        // pivot, so the handle that becomes active is the one that ends up
        // facing you (click "X" -> camera at -X -> the "-X" circle is active).
        int activeAxisID = -1;
        for (const auto& axis : axes)
            if (axis.depth > style.activeAxisThreshold) { activeAxisID = axis.id; break; }
        const bool anyAxisActive = (activeAxisID != -1);

        // world->screen 
        auto worldToScreen = [&](const vec3_t& worldPos) -> ImVec2 {
            const vec4_t clipPos = GizmoMath::multiply_mv4(gizmoMvp, GizmoMath::make_vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f));
            const float w = clipPos.w;
            if (fabsf(w) < 1e-6f)
                return { -FLT_MAX, -FLT_MAX };
            const float ndcX = clipPos.x / w;
            const float ndcY = clipPos.y / w;
            return { position.x + ndcX * halfGizmoSize, position.y - ndcY * halfGizmoSize };
            };

        const ImVec2 originScreenPos = worldToScreen(origin);

        // Hover detection 
        const bool canInteract = !(io.ConfigFlags & ImGuiConfigFlags_NoMouse);
        const ImVec2 mousePos = io.MousePos;
        const float distToCenterSq = ImViewGuizmo::ImLengthSqr(ImVec2(mousePos.x - position.x, mousePos.y - position.y));

        // The gizmo stays highlighted while the mouse is anywhere inside its area,
        // no matter what is hovered there (a handle, the center, ...) or while orbiting.
        const bool gizmoHighlighted = (ctx.activeTool == TOOL_GIZMO) ||
            (canInteract && distToCenterSq < gizmoAreaRadius * gizmoAreaRadius);

        ctx.isGizmoButtonHovered = gizmoHighlighted;

        if (canInteract && ctx.activeTool == TOOL_NONE && !ctx.isAnimating) {
            if (distToCenterSq < gizmoAreaRadius * gizmoAreaRadius) {
                const float minDistanceSq = scaledCircleRadius * scaledCircleRadius;
                // Every handle is hoverable regardless of where it sits on the sphere.
                // axes are sorted back-to-front, so the last hit wins = the one drawn on top.
                for (const auto& axis : axes) {
                    ImVec2 handlePos = worldToScreen(GizmoMath::multiply_vf(axis.direction, style.lineLength));
                    if (ImViewGuizmo::ImLengthSqr(ImVec2(handlePos.x - mousePos.x, handlePos.y - mousePos.y)) < minDistanceSq)
                        ctx.hoveredAxisID = axis.id;
                }
                if (ctx.hoveredAxisID == -1 &&
                    ImViewGuizmo::ImLengthSqr(ImVec2(originScreenPos.x - mousePos.x, originScreenPos.y - mousePos.y)) < scaledBigCircleRadius * scaledBigCircleRadius)
                    ctx.hoveredAxisID = 6;
            }
        }

        // Drawing 
        if (gizmoHighlighted)
            drawList->AddCircleFilled(originScreenPos, scaledBigCircleRadius, style.bigCircleColor);

        ImFont* font = ImGui::GetFont();
        for (const auto& axis : axes) {
            const bool isPositive = (axis.id % 2 == 0);
            const bool isActive = (axis.id == activeAxisID);
            const bool isHovered = (ctx.hoveredAxisID == axis.id);

            const ImVec2 handlePos = worldToScreen(GizmoMath::multiply_vf(axis.direction, style.lineLength));

            // 0 at the front of the sphere, 1 at the back of the sphere
            const float backness = (1.0f - axis.depth) * 0.5f;

            const ImU32 mainColor = style.axisColors[axis.axisIndex];
            const ImU32 darkColor = DarkenColor(mainColor, style.darkFactor);

            ImU32 fillColor;
            if (isPositive) {
                // X/Y/Z: opaque fill that darkens towards the back of the sphere
                fillColor = MixColor(mainColor, darkColor, backness);
            }
            else if (isActive) {
                // Active -X/-Y/-Z: filled with the main color
                fillColor = mainColor;
            }
            else {
                // -X/-Y/-Z: dark fill, becoming semi-transparent (backsideAlpha) at the back
                fillColor = WithAlpha(darkColor, mix(1.0f, style.backsideAlpha, backness));
            }

            // Lines: center -> X/Y/Z always; to every handle while the view is aligned to one
            if (isPositive || anyAxisActive) {
                ImVec2 lineDir = { handlePos.x - originScreenPos.x, handlePos.y - originScreenPos.y };
                float lineLengthVal = sqrtf(lineDir.x * lineDir.x + lineDir.y * lineDir.y) + 1e-6f;
                lineDir.x /= lineLengthVal; lineDir.y /= lineLengthVal;
                ImVec2 lineEndPos = { handlePos.x - lineDir.x * scaledCircleRadius, handlePos.y - lineDir.y * scaledCircleRadius };

                drawList->AddLine(originScreenPos, lineEndPos, fillColor, scaledLineWidth);
            }

            // Handle
            drawList->AddCircleFilled(handlePos, scaledCircleRadius, fillColor);
            if (!isPositive) {
                const float lighten = isActive ? style.activeOutlineLightenFactor : style.outlineLightenFactor;
                drawList->AddCircle(handlePos, scaledCircleRadius, LightenColor(mainColor, lighten), 0, scaledOutlineWidth);
            }

            // Label: black and always visible on X/Y/Z and on the active -handle,
            // white while hovered; -handles only show their label on hover (or while active)
            if (isPositive || isHovered || isActive) {
                const ImU32 textColor = isHovered ? style.labelHoveredColor : style.labelColor;
                const char* label = style.axisLabels[axis.axisIndex + (axis.id % 2) * 3];
                ImVec2 textSize = font->CalcTextSizeA(scaledFontSize, FLT_MAX, 0.f, label);
                drawList->AddText(font, scaledFontSize, { handlePos.x - textSize.x * 0.5f, handlePos.y - textSize.y * 0.5f }, textColor, label);
            }
        }

        // Drag start 
        if (canInteract && io.MouseDown[0] && ctx.activeTool == TOOL_NONE && ctx.hoveredAxisID == 6) {
            ctx.activeTool = TOOL_GIZMO;
            ctx.isAnimating = false;
        }

        // Active tool rotation 
        if (ctx.activeTool == TOOL_GIZMO) {
            float yawAngle = io.MouseDelta.x * rotationSpeed;
            float pitchAngle = io.MouseDelta.y * rotationSpeed;

            quat_t yawRotation = GizmoMath::angleAxis(yawAngle, worldUp);
            vec3_t rightAxis = GizmoMath::multiply_qv(cameraRot, worldRight);
            quat_t pitchRotation = GizmoMath::angleAxis(pitchAngle, rightAxis);
            quat_t totalRotation = GizmoMath::multiply_qq(yawRotation, pitchRotation);

            vec3_t relativeCamPos = GizmoMath::subtract_vv(cameraPos, pivot);
            cameraPos = GizmoMath::add_vv(pivot, GizmoMath::multiply_qv(totalRotation, relativeCamPos));
            cameraRot = GizmoMath::multiply_qq(totalRotation, cameraRot);

            wasModified = true;
        }

        // Snap on release 
        if (canInteract && ImGui::IsMouseReleased(0) && ctx.hoveredAxisID >= 0 && ctx.hoveredAxisID <= 5 && ctx.activeTool == TOOL_NONE) {
            int axisIndex = ctx.hoveredAxisID / 2;
            float sign = (ctx.hoveredAxisID % 2 == 0) ? -1.0f : 1.0f;
            vec3_t targetDir = GizmoMath::multiply_vf(axisVectors[axisIndex], sign);

            vec3_t currentDir;
            {
                const vec3_t relPos = GizmoMath::subtract_vv(cameraPos, pivot);
                if (GizmoMath::length2(relPos) > 1e-8f)
                    currentDir = GizmoMath::normalize(relPos);
                else
                    currentDir = GizmoMath::multiply_vf(
                        GizmoMath::normalize(GizmoMath::multiply_qv(cameraRot, worldForward)), -1.0f);

                if (GizmoMath::dot(currentDir, targetDir) > 0.999f) // ~2.5 deg tolerance
                    targetDir = GizmoMath::multiply_vf(targetDir, -1.0f);
            }

            float currentDistance = GizmoMath::length(GizmoMath::subtract_vv(cameraPos, pivot));
            vec3_t targetPosition = GizmoMath::add_vv(pivot, GizmoMath::multiply_vf(targetDir, currentDistance));

            vec3_t dirNormalized = GizmoMath::normalize(targetDir);

            vec3_t targetUp = -worldUp;
            // If dir is nearly parallel to worldUp, pick a different up to avoid flipping
            if (fabsf(GizmoMath::dot(dirNormalized, targetUp)) > 0.999f)
                if (dirNormalized.y > 0.0f)  // facing "up"
                    targetUp = worldForward;
                else  // facing "down"
                    targetUp = -worldForward;

            quat_t targetRotation = GizmoMath::quatLookAt(targetDir, targetUp);

            if (style.animateSnap && style.snapAnimationDuration > 0.0f) {
                bool pos_is_different = GizmoMath::length2(GizmoMath::subtract_vv(cameraPos, targetPosition)) > 0.0001f;
                bool rot_is_different = (1.0f - fabsf(GizmoMath::dot(cameraRot, targetRotation))) > 0.0001f;

                if (pos_is_different || rot_is_different) {
                    ctx.isAnimating = true;
                    ctx.animationStartTime = static_cast<float>(ImGui::GetTime());
                    ctx.startPos = cameraPos;
                    ctx.targetPos = targetPosition;
                    ctx.startUp = GizmoMath::multiply_qv(cameraRot, GizmoMath::multiply_vf(worldUp, -1.0f));
                    ctx.targetUp = targetUp;

                    ctx.animStartDist = GizmoMath::length(GizmoMath::subtract_vv(ctx.startPos, pivot));
                    ctx.animTargetDist = GizmoMath::length(GizmoMath::subtract_vv(ctx.targetPos, pivot));

                    ctx.animStartDir = currentDir;

                    if (ctx.animTargetDist > 0.0001f)
                        ctx.animTargetDir = GizmoMath::normalize(GizmoMath::subtract_vv(ctx.targetPos, pivot));
                    else
                        ctx.animTargetDir = dirNormalized;
                }
            }
            else {
                cameraRot = targetRotation;
                cameraPos = targetPosition;
                wasModified = true;
            }
        }

        if (!io.MouseDown[0] && ctx.activeTool != TOOL_NONE)
            ctx.activeTool = TOOL_NONE;

        return wasModified;
    }

    /// @brief Renders a zoom button and handles its logic.
    inline bool Dolly(vec3_t& cameraPos, const quat_t& cameraRot, const ImVec2 position, const float zoomSpeed) {

        const ImGuiIO& io = ImGui::GetIO();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        auto& ctx = GetContext();
        const Style& style = GetStyle();
        bool wasModified = false;

        const bool canInteract = !(io.ConfigFlags & ImGuiConfigFlags_NoMouse);
        const float radius = style.toolButtonRadius * style.scale;
        const ImVec2 center = { position.x + radius, position.y + radius };

        bool isHovered = false;
        if (canInteract && (ctx.activeTool == TOOL_NONE || ctx.activeTool == TOOL_DOLLY)) {
            if (ImLengthSqr({ io.MousePos.x - center.x, io.MousePos.y - center.y }) < radius * radius) {
                isHovered = true;
            }
        }
        ctx.isZoomButtonHovered = isHovered;

        if (canInteract && (isHovered || ctx.activeTool == TOOL_DOLLY)) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        }

        if (canInteract && isHovered && io.MouseDown[0] && ctx.activeTool == TOOL_NONE) {
            ctx.activeTool = TOOL_DOLLY;
            ctx.isAnimating = false;
        }

        if (ctx.activeTool == TOOL_DOLLY && io.MouseDelta.y != 0.0f) {
            vec3_t forwardMovement = GizmoMath::multiply_vf(
                GizmoMath::multiply_qv(cameraRot, worldForward),
                -io.MouseDelta.y * zoomSpeed
            );
            cameraPos = GizmoMath::add_vv(cameraPos, forwardMovement);
            wasModified = true;
        }

        if (canInteract && (isHovered || ctx.activeTool == TOOL_DOLLY)) {
            CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_VERTICAL);
        }



        const ImU32 bgColor = (ctx.activeTool == TOOL_DOLLY || isHovered) ? style.toolButtonHoveredColor : style.toolButtonColor;
        drawList->AddCircleFilled(center, radius, bgColor);

        const float p = style.toolButtonInnerPadding * style.scale;
        const float th = 3.0f * style.scale;
        const ImU32 iconColor = style.toolButtonIconColor;
        constexpr float iconScale = 0.5f;
        const float scaledP = p * iconScale;
        const float scaledRadius = radius * iconScale;

        ImVec2 glassCenter = { center.x - scaledP / 2.0f - 2.f, center.y - scaledP / 2.0f - 2.f };
        const float glassRadius = scaledRadius - scaledP;
        drawList->AddCircle(glassCenter, glassRadius, iconColor, 0, th);

        const ImVec2 handleStart = { center.x + scaledRadius / 3.0f, center.y + scaledRadius / 3.0f };
        const ImVec2 handleEnd = { center.x + scaledRadius - (p * iconScale), center.y + scaledRadius - (p * iconScale) };
        drawList->AddLine(handleStart, handleEnd, iconColor, th);

        const float plusHalfSize = glassRadius * 0.5f;

        float offset_plus = -0.5f;

        drawList->AddLine({ glassCenter.x + offset_plus, glassCenter.y - plusHalfSize + offset_plus }, { glassCenter.x + offset_plus, glassCenter.y + plusHalfSize + offset_plus }, iconColor, th);
        drawList->AddLine({ glassCenter.x - plusHalfSize + offset_plus, glassCenter.y + offset_plus }, { glassCenter.x + plusHalfSize + offset_plus, glassCenter.y + offset_plus }, iconColor, th);

        return wasModified;
    }

    /// @brief Renders a pan button and handles its logic.
    inline bool Pan(vec3_t& cameraPos, const quat_t& cameraRot, const ImVec2 position, const float panSpeed) {

        const ImGuiIO& io = ImGui::GetIO();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        auto& ctx = GetContext();
        const Style& style = GetStyle();
        bool wasModified = false;

        const vec3_t worldRight = GizmoMath::make_vec3(1.0f, 0.0f, 0.0f);
        const vec3_t worldUp = GizmoMath::make_vec3(0.0f, -1.0f, 0.0f);
        const bool canInteract = !(io.ConfigFlags & ImGuiConfigFlags_NoMouse);
        const float radius = style.toolButtonRadius * style.scale;
        const ImVec2 center = { position.x + radius, position.y + radius };

        bool isHovered = false;
        if (canInteract && (ctx.activeTool == TOOL_NONE || ctx.activeTool == TOOL_PAN)) {
            if (ImLengthSqr({ io.MousePos.x - center.x, io.MousePos.y - center.y }) < radius * radius) {
                isHovered = true;
            }
        }
        ctx.isPanButtonHovered = isHovered;

        if (canInteract && (isHovered || ctx.activeTool == TOOL_PAN)) {
            CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);
        }

        if (canInteract && isHovered && io.MouseDown[0] && ctx.activeTool == TOOL_NONE) {
            ctx.activeTool = TOOL_PAN;
            ctx.isAnimating = false;
        }

        if (ctx.activeTool == TOOL_PAN && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)) {
            vec3_t rightMovement = GizmoMath::multiply_vf(GizmoMath::multiply_qv(cameraRot, worldRight), -io.MouseDelta.x * panSpeed);
            vec3_t upMovement = GizmoMath::multiply_vf(GizmoMath::multiply_qv(cameraRot, worldUp), io.MouseDelta.y * panSpeed);
            cameraPos = GizmoMath::add_vv(cameraPos, rightMovement);
            cameraPos = GizmoMath::add_vv(cameraPos, upMovement);
            wasModified = true;
        }

        const ImU32 bgColor = (isHovered || ctx.activeTool == TOOL_PAN) ? style.toolButtonHoveredColor : style.toolButtonColor;
        drawList->AddCircleFilled(center, radius, bgColor);

        const ImU32 iconColor = style.toolButtonIconColor;
        const float th = 3.0f * style.scale;
        const float size = radius * 0.5f;
        const float arm = size * 0.35f;

        float offset_icon = -0.5f;

        const ImVec2 topTip = { center.x + offset_icon, center.y - size + offset_icon };
        drawList->AddLine({ topTip.x - arm, topTip.y + arm }, topTip, iconColor, th);
        drawList->AddLine({ topTip.x + arm, topTip.y + arm }, topTip, iconColor, th);
        const ImVec2 botTip = { center.x + offset_icon, center.y + size + offset_icon };
        drawList->AddLine({ botTip.x - arm, botTip.y - arm }, botTip, iconColor, th);
        drawList->AddLine({ botTip.x + arm, botTip.y - arm }, botTip, iconColor, th);
        const ImVec2 leftTip = { center.x - size + offset_icon, center.y + offset_icon };
        drawList->AddLine({ leftTip.x + arm, leftTip.y - arm }, leftTip, iconColor, th);
        drawList->AddLine({ leftTip.x + arm, leftTip.y + arm }, leftTip, iconColor, th);
        const ImVec2 rightTip = { center.x + size + offset_icon, center.y + offset_icon };
        drawList->AddLine({ rightTip.x - arm, rightTip.y - arm }, rightTip, iconColor, th);
        drawList->AddLine({ rightTip.x - arm, rightTip.y + arm }, rightTip, iconColor, th);

        return wasModified;
    }

} // namespace ImViewGuizmo
#endif // IMVIEWGUIZMO_IMPLEMENTATION