#ifndef UTILS_COLOR_H
#define UTILS_COLOR_H

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include <glm/glm.hpp>

/// <summary>
/// <para> r = [0, 1] </para>
/// <para> g = [0, 1] </para>
/// <para> b = [0, 1] </para>
/// </summary>
struct colorRGB {
	float r, g, b;
	colorRGB() = default;
	colorRGB(const ImColor& color) {
		r = color.Value.x;
		g = color.Value.y;
		b = color.Value.z;
	}

	colorRGB(float red, float green, float blue) {
		this->r = red;
		this->g = green;
		this->b = blue;
	}

	colorRGB(const colorRGB& color) {
		this->r = color.r;
		this->g = color.g;
		this->b = color.b;
	}

};

/// <summary>
/// <para> h = [0, 360] </para>
/// <para> s = [0, 1] </para>
/// <para> v = [0, 1] </para>
/// </summary>
struct colorHSV {
	float h, s, v;
};

uint32_t get32uint(const colorRGB& color);

colorHSV rgb2hsv(const colorRGB& color);
colorRGB hsv2rgb(const colorHSV& color);

float getLuminance(const colorRGB& color);

colorRGB lerp(const colorRGB& colorFrom, const colorRGB& colorTo, const float& time);

#endif