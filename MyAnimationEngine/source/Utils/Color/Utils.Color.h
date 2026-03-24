#ifndef UTILS_COLOR_H
#define UTILS_COLOR_H


#include <glm/glm.hpp>

/// <summary>
/// <para> r = [0, 1] </para>
/// <para> g = [0, 1] </para>
/// <para> b = [0, 1] </para>
/// </summary>
struct colorRGB {
	float r, g, b;
};

/// <summary>
/// <para> h = [0, 360] </para>
/// <para> s = [0, 1] </para>
/// <para> v = [0, 1] </para>
/// </summary>
struct colorHSV {
	float h, s, v;
};

colorHSV rgb2hsv(const colorRGB& color);
colorRGB hsv2rgb(const colorHSV& color);

float getLuminance(const colorRGB& color);

colorRGB lerp(const colorRGB& colorFrom, const colorRGB& colorTo, const float& time);

#endif