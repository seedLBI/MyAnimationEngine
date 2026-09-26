#ifndef KEYFRAME_COLORS_H
#define KEYFRAME_COLORS_H

#include "ThirdParty/ImGui/imgui_internal.h"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_glfw.h"
#include "ThirdParty/ImGui/imgui_impl_opengl3.h"

#include <string>
#include <vector>


struct KeyframeColor {
	std::string name;
	ImColor color;
};

namespace KeyframeColors {

	namespace Colors {
		const ImColor KF_DEFAULT	{ .69f, .69f, .69f, 1.f };
		const ImColor KF_RED		{ .71f, .22f, .22f, 1.f };
		const ImColor KF_YELLOW		{ .89f, .85f, .3f,  1.f };
		const ImColor KF_LIGHTBLUE	{ .66f, .8f,  .78f, 1.f };
		const ImColor KF_PINK		{ .9f,  .74f, .79f, 1.f };
		const ImColor KF_LAVENDER	{ .66f, .66f, .79f, 1.f };
		const ImColor KF_PEACH		{ .91f, .76f, .62f, 1.f };
		const ImColor KF_SEAFOAM	{ .7f,  .78f, .7f,  1.f };
		const ImColor KF_BLUE		{ .4f,  .49f, .88f, 1.f };
		const ImColor KF_GREEN		{ .29f, .64f, .3f,  1.f };
		const ImColor KF_LILAC		{ .56f, .17f, .6f,  1.f };
		const ImColor KF_ORANGE		{ .91f, .57f, .05f, 1.f };
		const ImColor KF_BROWN		{ .5f,  .27f, .16f, 1.f };
		const ImColor KF_DEEPPINK	{ .96f, .43f, .84f, 1.f };
		const ImColor KF_DEEPBLUE	{ .24f, .64f, .65f, 1.f };
		const ImColor KF_WARMGRAY	{ .66f, .59f, .47f, 1.f };
		const ImColor KF_DARKGREEN	{ .12f, .25f, .12f, 1.f };
	}

	namespace Array {
		const std::vector<KeyframeColor> colors = {
			{u8"Стандартный",		KeyframeColors::Colors::KF_DEFAULT},
			{u8"Красный",			KeyframeColors::Colors::KF_RED},
			{u8"Жёлтый",			KeyframeColors::Colors::KF_YELLOW},
			{u8"Светло-голубой",	KeyframeColors::Colors::KF_LIGHTBLUE},
			{u8"Светло-розовый",	KeyframeColors::Colors::KF_PINK},
			{u8"Лавандовый",		KeyframeColors::Colors::KF_LAVENDER},
			{u8"Персиковый",		KeyframeColors::Colors::KF_PEACH},
			{u8"Морская пена",		KeyframeColors::Colors::KF_SEAFOAM},
			{u8"Синий",				KeyframeColors::Colors::KF_BLUE},
			{u8"Зелёный",			KeyframeColors::Colors::KF_GREEN},
			{u8"Лиловый",			KeyframeColors::Colors::KF_LILAC},
			{u8"Оранжевый",			KeyframeColors::Colors::KF_ORANGE},
			{u8"Коричневый",		KeyframeColors::Colors::KF_BROWN},
			{u8"Розовый",			KeyframeColors::Colors::KF_DEEPPINK},
			{u8"Голубой",			KeyframeColors::Colors::KF_DEEPBLUE},
			{u8"Известняк",			KeyframeColors::Colors::KF_WARMGRAY},
			{u8"Тёмно-зелёный",		KeyframeColors::Colors::KF_DARKGREEN}
		};
	}

	const std::vector<KeyframeColor>& getColors() {
		return Array::colors;
	}

	const KeyframeColor& getColorByID(uint8_t color_id) {
		if (color_id >= Array::colors.size())
			return Array::colors.front();
		return Array::colors[color_id];
	}

}







#endif