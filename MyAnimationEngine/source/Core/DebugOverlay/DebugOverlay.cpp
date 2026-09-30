#include "DebugOverlay.h"

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace DebugOverlay {

    struct LineTexts {
        std::vector < std::pair<std::string, ImColor> > texts;
    };

    namespace vars {
        std::vector<LineTexts> elements;
        LerpedValue<float> timer_show = LerpedValue<float>(1.f);
        bool isShow = true;
        Anchor anchor = Anchor::BottomRight;
    }

    void SetAnchor(Anchor anchor) { vars::anchor = anchor; }
    Anchor GetAnchor() { return vars::anchor; }

    void Show() {
        vars::isShow = true;
        vars::timer_show.Set(1.f);
    }
    void Hide() {
        vars::isShow = false;
        vars::timer_show.Set(0.f);
    }

    void ToggleShow() {
        if (vars::isShow)
            Hide();
        else
            Show();
    }

    void Update(const float& dt) {
        vars::time += dt;
        vars::lastDt = dt;
        vars::timer_show.Update(dt * 10.f);

        if (vars::isShow) {
            vars::timer_show.Set(1.f);
        }
    }

    namespace vars {
        std::vector<DebugGraph> graphs;
        double time = 0.0;
        float  lastDt = 1.f / 60.f;
    }

    static constexpr size_t kRingInitCap = 1u << 13;
    static constexpr size_t kRingMaxCap = 1u << 18;

    static constexpr int kHistBins = 256;

    static DebugGraph* FindGraph(const char* id) {
        for (DebugGraph& g : vars::graphs)
            if (g.id == id) return &g;
        return nullptr;
    }

    DebugGraph& Graph(const char* id) {
        if (DebugGraph* g = FindGraph(id)) return *g;
        if (vars::graphs.capacity() < 32)
            vars::graphs.reserve(32);
        DebugGraph& g = vars::graphs.emplace_back();
        g.id = id;
        g.tOrigin = vars::time;
        g.ring.Init(kRingInitCap);
        return g;
    }

    void Plot(const char* id, float value) {
        DebugGraph& g = Graph(id);
        const float now = (float)(vars::time - g.tOrigin);
        if (now - g.lastPushT < g.sampleStep)
            return;
        g.lastPushT = now;
        g.lastValue = value;
        g.ring.Push(now, value);

        if (g.ring.Full() && g.ring.Capacity() < kRingMaxCap &&
            g.ring.At(0).t > now - g.windowSec)
        {
            g.ring.Grow();
        }
    }

    static float HistPercentile(const std::vector<uint32_t>& h, float lo, float hi,
        double q, size_t count)
    {
        if (count == 0 || h.empty()) return lo;
        const double rank = q * (double)(count - 1);
        const float  binw = (hi - lo) / (float)(h.size() - 1);
        double acc = 0.0;
        for (size_t i = 0; i < h.size(); ++i) {
            if (h[i] == 0) continue;
            const double next = acc + (double)h[i];
            if (next > rank) {
                const double f = (rank - acc) / (double)h[i];
                return lo + ((float)i + (float)f) * binw;
            }
            acc = next;
        }
        return hi;
    }

    static float HistTailMean(const std::vector<uint32_t>& h, float lo, float hi,
        double q, size_t count, bool low)
    {
        if (count == 0 || h.empty()) return lo;
        double k = q * (double)count;
        if (k < 1.0) k = 1.0;
        const float binw = (hi - lo) / (float)(h.size() - 1);
        double acc = 0.0, sum = 0.0;
        for (size_t ii = 0; ii < h.size() && acc < k; ++ii) {
            const size_t i = low ? ii : (h.size() - 1 - ii);
            if (h[i] == 0) continue;
            const double take = (acc + (double)h[i] <= k) ? (double)h[i] : (k - acc);
            sum += take * (double)(lo + (float)i * binw);
            acc += take;
        }
        return acc > 0.0 ? (float)(sum / acc) : lo;
    }

    static float DrawTextBlock(ImDrawList* draw_list,
        const std::pair<std::string, ImColor>& text,
        const ImVec2& pos, float alpha)
    {
        const ImVec2 sizeText = ImGui::CalcTextSize(text.first.c_str());

        ImColor color_foreground = text.second;
        ImColor color_background = ImColor(0.f, 0.f, 0.f, 0.8f * alpha);
        color_foreground.Value.w *= alpha;

        constexpr float padding_x = 6.f;

        ImVec2 pmin = { pos.x - padding_x,  pos.y };
        ImVec2 pmax = { pos.x + sizeText.x + padding_x, pos.y + sizeText.y };

        pmin.x -= 2.f; pmin.y -= 2.f;
        pmax.x += 2.f; pmax.y += 2.f;

        draw_list->AddRect(pmin, pmax, color_foreground, 10.f, 0, 2.f);

        pmin.x += 2.f; pmin.y += 2.f;
        pmax.x -= 2.f; pmax.y -= 2.f;

        draw_list->AddRectFilled(pmin, pmax, color_background, 10.f);
        draw_list->AddText(pos, color_foreground, text.first.c_str());

        return pos.x + sizeText.x + 3.f * padding_x + 3.f;
    }


    static void DrawGraph(ImDrawList* dl, DebugGraph& g, const ImVec2& pos,
        const ImVec2& size, float alpha, float dt)
    {
        const ImVec2 pmax(pos.x + size.x, pos.y + size.y);
        const int W = (int)size.x;
        if (W < 4 || size.y < 4) return;

        ImColor line_col = g.color;
        line_col.Value.w *= alpha;

        dl->AddRectFilled(pos, pmax, ImColor(0.f, 0.f, 0.f, 0.65f * alpha), 10.f);
        dl->AddRect(pos, pmax, line_col, 10.f, 0, 2.f);

        const float win = g.windowSec > 0.01f ? g.windowSec : 0.01f;
        const float now = (float)(vars::time - g.tOrigin);
        const float t0 = now - win;
        const size_t n = g.ring.Size();

        size_t lo = 0, hi = n;
        while (lo < hi) {
            const size_t mid = (lo + hi) >> 1;
            if (g.ring.At(mid).t < t0) lo = mid + 1; else hi = mid;
        }
        const size_t count = n - lo;
        if (count == 0) {
            dl->AddText(ImVec2(pos.x + 6.f, pos.y + 4.f), line_col, g.id.c_str());
            return;
        }

        const float kx = size.x / win;
        const bool  direct = count <= 2048;
        const bool  smoothOn = g.smooth != SmoothMode::Off && g.smoothSec > 0.001f;
        const float swin = g.smoothSec;

        float vmin = FLT_MAX, vmax = -FLT_MAX;
        double vsum = 0.0;

        if (direct) {
            g.pts.clear();
            g.pts.reserve(count);
            if (smoothOn) { g.ts.clear(); g.ts.reserve(count); }
            for (size_t i = lo; i < n; ++i) {
                const GraphSample& s = g.ring.At(i);
                g.pts.push_back(ImVec2(pos.x + (s.t - t0) * kx, s.v));
                if (smoothOn) g.ts.push_back(s.t);
                if (s.v < vmin) vmin = s.v;
                if (s.v > vmax) vmax = s.v;
                vsum += s.v;
            }
        }
        else {
            if (g.colCnt.size() != (size_t)W) {
                g.colMin.resize(W); g.colMax.resize(W);
                g.colSum.resize(W); g.colAvg.resize(W);
                g.colCnt.assign(W, 0u);
            }
            else {
                memset(g.colCnt.data(), 0, (size_t)W * sizeof(uint32_t));
            }
            for (size_t i = lo; i < n; ++i) {
                const GraphSample& s = g.ring.At(i);
                int c = (int)((s.t - t0) * kx);
                if (c < 0)  c = 0;
                if (c >= W) c = W - 1;
                if (g.colCnt[c]++ == 0) { g.colMin[c] = s.v; g.colMax[c] = s.v; g.colSum[c] = s.v; }
                else {
                    if (s.v < g.colMin[c]) g.colMin[c] = s.v;
                    if (s.v > g.colMax[c]) g.colMax[c] = s.v;
                    g.colSum[c] += s.v;
                }
                if (s.v < vmin) vmin = s.v;
                if (s.v > vmax) vmax = s.v;
                vsum += s.v;
            }
        }

        float smin = FLT_MAX, smax = -FLT_MAX;
        if (smoothOn) {
            if (direct) {
                if (g.smooth == SmoothMode::Box) {
                    g.sm.clear();
                    g.sm.reserve(g.pts.size());
                    size_t j = 0;
                    double sum = 0.0;
                    for (size_t i = 0; i < g.pts.size(); ++i) {
                        sum += g.pts[i].y;
                        const float edge = g.ts[i] - swin;
                        while (g.ts[j] < edge) { sum -= g.pts[j].y; ++j; }
                        const float avg = (float)(sum / (double)(i - j + 1));
                        g.sm.push_back(avg);
                        if (avg < smin) smin = avg;
                        if (avg > smax) smax = avg;
                    }
                    for (size_t i = 0; i < g.pts.size(); ++i) g.pts[i].y = g.sm[i];
                }
                else {
                    float e = g.pts[0].y;
                    for (size_t i = 0; i < g.pts.size(); ++i) {
                        if (i > 0) {
                            const float a = 1.f - expf(-(g.ts[i] - g.ts[i - 1]) / swin);
                            e += (g.pts[i].y - e) * a;
                        }
                        g.pts[i].y = e;
                        if (e < smin) smin = e;
                        if (e > smax) smax = e;
                    }
                }
            }
            else {
                const float colDt = win / (float)W;
                if (g.smooth == SmoothMode::Box) {
                    size_t j = 0;
                    double sum = 0.0;
                    uint32_t cnt = 0;
                    for (int c = 0; c < W; ++c) {
                        if (g.colCnt[c]) { sum += g.colSum[c]; cnt += g.colCnt[c]; }
                        const float edge = t0 + ((float)c + 0.5f) * colDt - swin;
                        while (j < (size_t)c && t0 + ((float)j + 0.5f) * colDt < edge) {
                            if (g.colCnt[j]) { sum -= g.colSum[j]; cnt -= g.colCnt[j]; }
                            ++j;
                        }
                        const float avg = cnt ? (float)(sum / (double)cnt) : 0.f;
                        g.colAvg[c] = avg;
                        if (cnt) {
                            if (avg < smin) smin = avg;
                            if (avg > smax) smax = avg;
                        }
                    }
                }
                else {
                    bool has = false;
                    float e = 0.f;
                    for (int c = 0; c < W; ++c) {
                        if (!g.colCnt[c]) continue;
                        const float m = (float)(g.colSum[c] / (double)g.colCnt[c]);
                        if (!has) { e = m; has = true; }
                        else e += (m - e) * (1.f - expf(-colDt / swin));
                        g.colAvg[c] = e;
                        if (e < smin) smin = e;
                        if (e > smax) smax = e;
                    }
                }
            }
        }

        const bool needHist = g.showStats || (g.autoScaleY && g.outlierGuard);
        if (needHist) {
            if (g.hist.size() != (size_t)kHistBins) g.hist.assign(kHistBins, 0u);
            else memset(g.hist.data(), 0, (size_t)kHistBins * sizeof(uint32_t));
            if (vmax - vmin > 1e-12f) {
                const float hsc = (float)(kHistBins - 1) / (vmax - vmin);
                for (size_t i = lo; i < n; ++i) {
                    int b = (int)((g.ring.At(i).v - vmin) * hsc + 0.5f);
                    if (b < 0) b = 0;
                    if (b >= kHistBins) b = kHistBins - 1;
                    ++g.hist[b];
                }
            }
            else {
                g.hist[0] = (uint32_t)count;
            }
        }

        if (g.autoScaleY) {
            float loB = vmin, hiB = vmax;
            if (g.outlierGuard) {
                float q = g.outlierPct * 0.01f;
                if (q < 0.f)   q = 0.f;
                if (q > 0.25f) q = 0.25f;
                loB = HistPercentile(g.hist, vmin, vmax, (double)q, count);
                hiB = HistPercentile(g.hist, vmin, vmax, 1.0 - (double)q, count);
            }
            if (smoothOn) {
                if (smin > loB) loB = smin;
                if (smax < hiB) hiB = smax;
            }
            if (hiB - loB < 1e-12f) { loB -= 0.5f; hiB += 0.5f; }

            const float pad = (hiB - loB) * g.vPad;
            const float tMin = g.zeroBased ? 0.f : loB - pad;
            const float tMax = hiB + pad;
            const float sm = 1.f - expf(-g.shrinkK * dt);
            g.curMin = (tMin < g.curMin) ? tMin : g.curMin + (tMin - g.curMin) * sm;
            g.curMax = (tMax > g.curMax) ? tMax : g.curMax + (tMax - g.curMax) * sm;
        }
        else {
            g.curMin = g.fixedMin;
            g.curMax = g.fixedMax;
        }

        float range = g.curMax - g.curMin;
        if (range < 1e-9f) range = 1e-9f;
        const float ky = size.y / range;
        const float yBot = pos.y + size.y;
        auto ToY = [&](float v) {
            const float y = yBot - (v - g.curMin) * ky;
            return y < pos.y ? pos.y : (y > yBot ? yBot : y);
        };

        if (direct) {
            for (ImVec2& p : g.pts) p.y = ToY(p.y);
            if (g.pts.size() == 1)
                g.pts.push_back(ImVec2(g.pts[0].x + 0.01f, g.pts[0].y));
            
        }
        else if (smoothOn) {
            g.pts.clear();
            for (int c = 0; c < W; ++c) {
                if (g.colCnt[c] == 0) continue;
                g.pts.push_back(ImVec2(pos.x + (float)c + 0.5f, ToY(g.colAvg[c])));
            }
            if (g.pts.size() == 1)
                g.pts.push_back(ImVec2(g.pts[0].x + 0.01f, g.pts[0].y));
        }
        else {
            g.pts.clear();
            for (int c = 0; c < W; ++c) {
                if (g.colCnt[c] == 0) continue;
                const float x = pos.x + (float)c + 0.5f;
                g.pts.push_back(ImVec2(x, ToY(g.colMin[c])));
                if (g.colCnt[c] > 1)
                    g.pts.push_back(ImVec2(x, ToY(g.colMax[c])));
            }
        }

        dl->AddPolyline(g.pts.data(), (int)g.pts.size(), line_col, 0, 0.9f);

        char buf[128];
        snprintf(buf, sizeof(buf), "%s: %.*f", g.id.c_str(), g.precision, g.lastValue);
        dl->AddText(ImVec2(pos.x + 6.f, pos.y + 3.f), line_col, buf);

        const ImColor dim(1.f, 1.f, 1.f, 0.45f * alpha);
        const float th = ImGui::GetTextLineHeight();

        snprintf(buf, sizeof(buf), "%.*f", g.precision, g.curMax);
        ImVec2 tsz = ImGui::CalcTextSize(buf);
        dl->AddText(ImVec2(pmax.x - tsz.x - 6.f, pos.y + 3.f), dim, buf);

        snprintf(buf, sizeof(buf), "%.*f", g.precision, g.curMin);
        tsz = ImGui::CalcTextSize(buf);
        dl->AddText(ImVec2(pmax.x - tsz.x - 6.f, pmax.y - th - 3.f), dim, buf);

        if (g.showStats) {
            const float avg = (float)(vsum / (double)count);
            const bool  low = !g.statsHighTail;
            const float p1 = HistTailMean(g.hist, vmin, vmax, 0.01, count, low);
            const float p01 = HistTailMean(g.hist, vmin, vmax, 0.001, count, low);
            g.statAvg = avg; g.statP1 = p1; g.statP01 = p01;
            if (g.statsHighTail)
                snprintf(buf, sizeof(buf), "avg %.*f | 99%% %.*f | 99.9%% %.*f",
                    g.precision, avg, g.precision, p1, g.precision, p01);
            else
                snprintf(buf, sizeof(buf), "avg %.*f | 1%% %.*f | 0.1%% %.*f",
                    g.precision, avg, g.precision, p1, g.precision, p01);
            dl->AddText(ImVec2(pos.x + 6.f, pmax.y - th - 3.f), dim, buf);
        }
    }


    static void ClearElements() {
        constexpr size_t kTextsCapLimit = 64; 
        if (vars::elements.capacity() > kTextsCapLimit)
            std::vector<LineTexts>().swap(vars::elements);
        else
            vars::elements.clear();
    }

    void DrawDebug() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();

        if (vars::timer_show.GetLerped() < 0.01f) {
            ClearElements();
            return;
        }

        const float lerp = vars::timer_show.GetLerped();

        constexpr float kMargin = 40.f;
        constexpr float kTextGap = 3.f * 6.f + 3.f;
        constexpr float kGraphW = 440.f;
        constexpr float kGraphH = 84.f;
        constexpr float kGraphStep = 64.f + 8.f;
        const     float kLineStep = ImGui::GetTextLineHeight() + 6.f;

        const Anchor a = vars::anchor;
        const bool hLeft = (a == Anchor::TopLeft || a == Anchor::Left || a == Anchor::BottomLeft);
        const bool hRight = (a == Anchor::TopRight || a == Anchor::Right || a == Anchor::BottomRight);
        const bool vTop = (a == Anchor::TopLeft || a == Anchor::Top || a == Anchor::TopRight);
        const bool vBottom = (a == Anchor::BottomLeft || a == Anchor::Bottom || a == Anchor::BottomRight);

        // анимация: слева/справа - горизонтальный выезд, по центру сверху/снизу - вертикальный
        const float slide = (1.f - lerp) * 340.f;
        float xOff = 0.f, yOff = 0.f;
        if (hLeft)   xOff = -slide;
        else if (hRight)  xOff = slide;
        else if (vTop)    yOff = -slide;
        else if (vBottom) yOff = slide;

        // полная высота контента (нужна для Bottom / Center)
        float totalH = (float)vars::elements.size() * kLineStep;
        if (!vars::graphs.empty())
            totalH += kGraphH + (float)(vars::graphs.size() - 1) * kGraphStep;

        float y = viewport->Pos.y + kMargin;
        if (vBottom)     y = viewport->Pos.y + viewport->Size.y - kMargin - totalH;
        else if (!vTop)  y = viewport->Pos.y + (viewport->Size.y - totalH) * 0.5f;
        y += yOff;

        const float vpL = viewport->Pos.x;
        const float vpW = viewport->Size.x;

        for (const auto& line : vars::elements) {

            float x;
            if (hLeft) {
                x = vpL + kMargin;
            }
            else {
                float lineWidth = 0.f;
                for (const auto& text : line.texts)
                    lineWidth += ImGui::CalcTextSize(text.first.c_str()).x;
                if (line.texts.size() > 1)
                    lineWidth += (float)(line.texts.size() - 1) * kTextGap;

                if (hRight) x = vpL + vpW - kMargin - lineWidth;
                else        x = vpL + (vpW - lineWidth) * 0.5f;
            }
            x += xOff;

            for (const auto& text : line.texts)
                x = DrawTextBlock(draw_list, text, ImVec2(x, y), lerp);

            y += kLineStep;
        }

        for (DebugGraph& g : vars::graphs) {
            float gx;
            if (hLeft)  gx = vpL + kMargin - 9.f;
            else if (hRight) gx = vpL + vpW - kMargin - kGraphW + 9.f;
            else             gx = vpL + (vpW - kGraphW) * 0.5f;
            gx += xOff;

            DrawGraph(draw_list, g, ImVec2(gx, y), ImVec2(kGraphW, kGraphH),
                lerp, vars::lastDt);
            y += kGraphStep;
        }

        ClearElements();
    }

    void AddText(const std::string& text, const ImColor& color) {
        if (vars::timer_show.GetLerped() < 0.01f)
            return;
        vars::elements.push_back(
            DebugOverlay::LineTexts{
                {
                    {text,color}
                }
            }
        );
    }

    void AddText(const std::vector<std::string>& texts, const ImColor& color) {
        if (vars::timer_show.GetLerped() < 0.01f)
            return;
        LineTexts result;
        result.texts.reserve(texts.size());
        for (const auto& text : texts) {
            result.texts.push_back({ text, color });
        }
        vars::elements.push_back(result);
    }

    void AddText(const std::vector<std::pair<std::string, ImColor>>& colored_texts) {
        if (vars::timer_show.GetLerped() < 0.01f)
            return;
        vars::elements.push_back(
            DebugOverlay::LineTexts{
                colored_texts
            }
        );
    }
}