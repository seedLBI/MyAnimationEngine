#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H

#include "Utils/LerpedValue/LerpedValue.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_glfw.h"
#include "ThirdParty/ImGui/imgui_impl_opengl3.h"

#include <vector>
#include <string>
#include <cstdint>

namespace DebugOverlay {





    struct GraphSample {
        float t;
        float v;
    };

    class SampleRing {
    public:
        void Init(size_t capacityHint) {
            size_t cap = 16;
            while (cap < capacityHint) cap <<= 1;
            m_mask = cap - 1;
            m_buf.assign(cap, GraphSample{});
            m_head = m_size = 0;
        }

        inline void Push(float t, float v) {
            m_buf[m_head] = { t, v };
            m_head = (m_head + 1) & m_mask;
            if (m_size <= m_mask) ++m_size;
        }

        inline size_t Size()     const { return m_size; }
        inline size_t Capacity() const { return m_buf.size(); }
        inline bool   Full()     const { return !m_buf.empty() && m_size == m_buf.size(); }

        inline const GraphSample& At(size_t i) const {
            return m_buf[(m_head - m_size + i) & m_mask];
        }

        void Grow() {
            if (m_buf.empty()) { Init(16); return; }
            std::vector<GraphSample> nb(m_buf.size() * 2);
            const size_t n = m_size;
            for (size_t i = 0; i < n; ++i)
                nb[i] = At(i);
            m_buf.swap(nb);
            m_mask = m_buf.size() - 1;
            m_head = n;
        }

    private:
        std::vector<GraphSample> m_buf;
        size_t m_head = 0, m_size = 0, m_mask = 0;
    };

    enum class SmoothMode : uint8_t {
        Off,
        Box,
        EMA
    };

    struct DebugGraph {
        std::string id;
        float   windowSec = 5.f;
        float   sampleStep = 0.f;
        bool    autoScaleY = true;
        bool    zeroBased = false;
        float   fixedMin = 0.f;
        float   fixedMax = 1.f;
        float   vPad = 0.10f; 
        float   shrinkK = 5.f;
        int     precision = 3;
        ImColor color = ImColor(120, 220, 120, 255);

        SmoothMode smooth = SmoothMode::Off;
        float   smoothSec = 0.25f;

        bool    outlierGuard = true;
        float   outlierPct = 0.1f;

        bool    showStats = true;
        bool    statsHighTail = false;

        SampleRing ring;
        double tOrigin = 0.0;
        float  lastPushT = -1e30f;
        float  lastValue = 0.f;
        float  curMin = 0.f, curMax = 1.f;

        float  statAvg = 0.f, statP1 = 0.f, statP01 = 0.f;

        std::vector<ImVec2>   pts;
        std::vector<float>    ts;
        std::vector<float>    sm;
        std::vector<float>    colMin, colMax, colAvg;
        std::vector<double>   colSum;
        std::vector<uint32_t> colCnt;
        std::vector<uint32_t> hist;
    };


    void Plot(const char* id, float value);
    DebugGraph& Graph(const char* id);

    extern struct LineTexts;


    enum class Anchor : uint8_t {
        TopLeft, Top, TopRight,
        Left, Center, Right,
        BottomLeft, Bottom, BottomRight
    };

    void   SetAnchor(Anchor anchor);
    Anchor GetAnchor();

    namespace vars {
        extern std::vector<LineTexts> elements;
        extern LerpedValue<float> timer_show;
        extern bool isShow;
        extern std::vector<DebugGraph> graphs;
        extern double time;
        extern float  lastDt;
        extern Anchor anchor;
    }

    void Show();
    void Hide();
    void ToggleShow();

    void Update(const float& dt);

    void DrawDebug();
    void AddText(const std::string& text, const ImColor& color);
    void AddText(const std::vector<std::string>& texts, const ImColor& color);
    void AddText(const std::vector<std::pair<std::string, ImColor>>& colored_texts);
}

#endif