#include "KeyframeRenderer.h"


#include <vector>
#include "Utils/Math/Math.h"

ImColor KeyframeRenderer::Private::GetHalfColor(KeyframeRenderer::Private::Side side, TypeKeyframe type_in, TypeKeyframe type_out, const KeyframeRenderer::Parameters& p) {
	if (p.isOnlyOne) return p.color;

	if (side == KeyframeRenderer::Private::Side::LEFT) {
		if (p.isLeft) return p.color;

		if (type_in == TypeKeyframe::BINARY && type_out != TypeKeyframe::AUTOBEZIER) {
			return p.color;
		}
		return p.color_side;
	}
	else {
		if (p.isRight) return p.color;

		if (type_out == TypeKeyframe::BINARY && p.isLeft) {
			return p.color;
		}
		return p.color_side;
	}
}

void KeyframeRenderer::Private::DrawHalf(
	ImDrawList* draw_list, 
	const KeyframeRenderer::Private::Side side,
	const ImVec2& pos, 
	TypeKeyframe type_in, TypeKeyframe type_out,
	const KeyframeRenderer::Parameters& params,
	const KeyframeRenderer::Private::KeyframePoints& pts )
{
	
	const bool isL = (side == KeyframeRenderer::Private::Side::LEFT);

	ImColor color = GetHalfColor(side, type_in, type_out, params);

	ImVec2 M = isL ? pts.LM : pts.RM;
	ImVec2 U = isL ? pts.LU : pts.RU;
	ImVec2 D = isL ? pts.LD : pts.RD;
	ImVec2 bU = isL ? pts.b_LU : pts.b_RU;
	ImVec2 bD = isL ? pts.b_LD : pts.b_RD;
	ImVec2 bMM_U = isL ? pts.b_MM_L_U : pts.b_MM_R_U;
	ImVec2 bMM_D = isL ? pts.b_MM_L_D : pts.b_MM_R_D;
	ImVec2 bMM_U_o = isL ? pts.b_MM_L_U_o : pts.b_MM_R_U_o;
	ImVec2 bMM_D_o = isL ? pts.b_MM_L_D_o : pts.b_MM_R_D_o;

	auto DrawPolyCW = [&](std::initializer_list<ImVec2> points) {
		ImVec2 arr[8];
		int count = points.size();

		int i = isL ? count - 1 : 0;


		for (auto pt : points) {
			// for fix aa
			if (pt.x == pos.x) {
				pt.x += isL ? 0.25f : -0.25f;
			}

			if (isL)
				arr[i--] = pt;
			else
				arr[i++] = pt;
		}


		draw_list->AddConvexPolyFilled(arr, count, color);
	};

	switch (isL ? type_in : type_out) {
		case TypeKeyframe::LINEAR: {
			DrawPolyCW({ pts.MU, M, pts.MD });
			break;
		}
		case TypeKeyframe::BINARY: {
			DrawPolyCW({ pts.MU, U, D, pts.MD });
			break;
		}
		case TypeKeyframe::BEZIER: {
			DrawPolyCW({ pts.MU, U, bU, bMM_U, pts.b_MM_U });
			DrawPolyCW({ pts.b_MM_U_o, bMM_U_o, bMM_D_o, pts.b_MM_D_o });
			DrawPolyCW({ pts.b_MM_D, bMM_D, bD, D, pts.MD });
			break;
		}
		case TypeKeyframe::AUTOBEZIER: {
			if (isL) {
				draw_list->PathArcTo(pos, params.size, IM_PI / 2.0f, IM_PI * 1.5f, 0);
				draw_list->PathFillConvex(color);
			}
			else {
				draw_list->PathArcTo(pos, params.size, -IM_PI / 2.0f, IM_PI / 2.0f, 0);
				draw_list->PathFillConvex(color);
			}
			break;
		}
	}

}

void KeyframeRenderer::Private::DrawOutline(ImDrawList* draw_list, TypeKeyframe type_in, TypeKeyframe type_out, const ImVec2& pos, const KeyframeRenderer::Parameters& params, const KeyframeRenderer::Private::KeyframePoints& pts) {

	if (type_in == TypeKeyframe::AUTOBEZIER || type_out == TypeKeyframe::AUTOBEZIER) {
		const float out_size = params.size + (params.size / 3.f);
		draw_list->AddCircleFilled(pos, out_size, params.outline);
		return;
	}

	ImVec2 path[16];
	int count = 0;

	path[count++] = pts.MU;

	if (type_out == TypeKeyframe::BEZIER) {
		path[count++] = pts.RU; path[count++] = pts.b_RU;
		path[count++] = pts.b_MM_R_U; path[count++] = pts.b_MM_R_D;
		path[count++] = pts.b_RD; path[count++] = pts.RD;
	}
	else if (type_out == TypeKeyframe::BINARY){
		path[count++] = pts.RU; path[count++] = pts.RD;
	}
	else if (type_out == TypeKeyframe::LINEAR) {
		path[count++] = pts.RM;
	}

	path[count++] = pts.MD;

	if (type_in == TypeKeyframe::BEZIER) {
		path[count++] = pts.LD; path[count++] = pts.b_LD;
		path[count++] = pts.b_MM_L_D; path[count++] = pts.b_MM_L_U;
		path[count++] = pts.b_LU; path[count++] = pts.LU;
	}
	else if (type_out == TypeKeyframe::BINARY) {
		path[count++] = pts.LD; path[count++] = pts.LU;
	}
	else if (type_out == TypeKeyframe::LINEAR) {
		path[count++] = pts.LM;
	}

	std::vector<glm::vec2> original_border;
	original_border.reserve(count);
	for (int i = 0; i < count; ++i)
		original_border.push_back(glm::vec2(path[i].x, path[i].y));

	float outline_radius = 1.5f;
	std::vector<glm::vec2> smoothed_border = GenerateRadiusBorder(original_border, outline_radius, false);

	std::vector<ImVec2> draw_path;
	draw_path.reserve(smoothed_border.size());
	for (const auto& p : smoothed_border)
		draw_path.push_back(ImVec2(p.x, p.y));

	if (!draw_path.empty())
		draw_list->AddPolyline(draw_path.data(), draw_path.size(), params.outline, ImDrawFlags_Closed, 1.5f);
}

KeyframeRenderer::Private::KeyframePoints
KeyframeRenderer::Private::CalcKeyframePoints(const ImVec2& pos, const KeyframeRenderer::Parameters& params) {

	KeyframeRenderer::Private::KeyframePoints pts;

	const float x_l = pos.x - params.size;
	const float x_m = pos.x;
	const float x_r = pos.x + params.size;

	const float y_u = pos.y - params.size;
	const float y_m = pos.y;
	const float y_d = pos.y + params.size;

	pts.LU = { x_l, y_u };
	pts.LM = { x_l, y_m };
	pts.LD = { x_l, y_d };

	pts.MU = { x_m, y_u };
	pts.MM = { x_m, y_m };
	pts.MD = { x_m, y_d };

	pts.RU = { x_r, y_u };
	pts.RM = { x_r, y_m };
	pts.RD = { x_r, y_d };

	const float smooth = params.size / 3.f;
	const float offset_center = smooth * 1.2f;
	const float overlap = 1.0f;

	pts.b_LU = { pts.LU.x, pts.LU.y + smooth };
	pts.b_RU = { pts.RU.x, pts.RU.y + smooth };
	pts.b_RD = { pts.RD.x, pts.RD.y - smooth };
	pts.b_LD = { pts.LD.x, pts.LD.y - smooth };

	pts.b_MM_L_U = { pts.MM.x - offset_center, pts.MM.y - smooth / 2.f };
	pts.b_MM_R_U = { pts.MM.x + offset_center, pts.MM.y - smooth / 2.f };
	pts.b_MM_L_D = { pts.MM.x - offset_center, pts.MM.y + smooth / 2.f };
	pts.b_MM_R_D = { pts.MM.x + offset_center, pts.MM.y + smooth / 2.f };

	pts.b_MM_U = { pts.MM.x, pts.MM.y - smooth / 2.f };
	pts.b_MM_D = { pts.MM.x, pts.MM.y + smooth / 2.f };

	pts.b_MM_U_o = { pts.MM.x, pts.b_MM_U.y - overlap };
	pts.b_MM_D_o = { pts.MM.x, pts.b_MM_D.y + overlap };

	pts.b_MM_L_U_o = { pts.b_MM_L_U.x, pts.b_MM_L_U.y - overlap };
	pts.b_MM_R_U_o = { pts.b_MM_R_U.x, pts.b_MM_R_U.y - overlap };
	pts.b_MM_L_D_o = { pts.b_MM_L_D.x, pts.b_MM_L_D.y + overlap };
	pts.b_MM_R_D_o = { pts.b_MM_R_D.x, pts.b_MM_R_D.y + overlap };

	return pts;

}