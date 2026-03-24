#include "Utils.Color.h"


colorHSV rgb2hsv(const colorRGB& c) {
    const float& r = c.r;
    const float& g = c.g;
    const float& b = c.b;

    float h, s, v; // h:0-360.0, s:0.0-1.0, v:0.0-1.0

    float max = fmaxf(fmaxf(r, g), b);
    float min = fminf(fminf(r, g), b);

    v = max;

    if (max == 0.0f) {
        s = 0;
        h = 0;
    }
    else if (max - min == 0.0f) {
        s = 0;
        h = 0;
    }
    else {
        s = (max - min) / max;

        if (max == r) {
            h = 60 * ((g - b) / (max - min)) + 0;
        }
        else if (max == g) {
            h = 60 * ((b - r) / (max - min)) + 120;
        }
        else {
            h = 60 * ((r - g) / (max - min)) + 240;
        }
    }

    if (h < 0) h += 360.0f;

    return {h,s,v};
}
colorRGB hsv2rgb(const colorHSV& c) {
    float h = c.h;
    float s = c.s;
    float v = c.v;

    float r = 0.f, g = 0.f, b = 0.f;

    int   hi = (int)(h / 60.0f) % 6;
    float f = (h / 60.0f) - hi;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (hi) {
    case 0: r = v, g = t, b = p; break;
    case 1: r = q, g = v, b = p; break;
    case 2: r = p, g = v, b = t; break;
    case 3: r = p, g = q, b = v; break;
    case 4: r = t, g = p, b = v; break;
    case 5: r = v, g = p, b = q; break;
    }

    return {r,g,b};
}

float getLuminance(const colorRGB& color) {
    return 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;
}

colorRGB lerp(const colorRGB& colorFrom, const colorRGB& colorTo, const float& time) {
    colorRGB result;

    result.r = colorFrom.r + time * (colorTo.r - colorFrom.r);
    result.g = colorFrom.g + time * (colorTo.g - colorFrom.g);
    result.b = colorFrom.b + time * (colorTo.b - colorFrom.b);

    return result;
}