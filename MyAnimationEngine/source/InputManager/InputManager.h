#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <cmath>
#include <glm/glm.hpp>
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "ThirdParty/ImGui/imgui_impl_glfw.h"
#include "ThirdParty/ImGui/imgui_impl_opengl3.h"

#ifndef _DEBUG
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "WindowManager/WindowManager.h"

namespace InputManager {

	namespace Vars {
		namespace Mouse {
			inline glm::dvec2 pos{ 0 };
			inline glm::dvec2 delta{ 0 };
			inline glm::dvec2 pos_before_raw{ 0 };
			inline bool isRaw = false;
			inline bool justTeleported = false;
		}
		namespace Portal {
			inline glm::ivec2 min{ 0 }, max{ 0 };
			inline bool isEnable = false;
		}
		inline bool isInstalled = false;
	}


	inline void TeleportCursor(GLFWwindow* window, glm::dvec2 target) {
		Vars::Mouse::pos = target;
		Vars::Mouse::justTeleported = true;
		glfwSetCursorPos(window, target.x, target.y);

		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2((float)target.x, (float)target.y);
		io.MousePosPrev = io.MousePos;
	}
	inline glm::dvec2 WrapScreenPos(const glm::dvec2& p) {
		const glm::dvec2 mn(Vars::Portal::min);
		const glm::dvec2 mx(Vars::Portal::max);
		const glm::dvec2 size = mx - mn;
		glm::dvec2 r = p;
		if (size.x > 0.0 && (r.x < mn.x || r.x >= mx.x))
			r.x = mn.x + glm::mod(r.x - mn.x, size.x);
		if (size.y > 0.0 && (r.y < mn.y || r.y >= mx.y))
			r.y = mn.y + glm::mod(r.y - mn.y, size.y);
		return r;
	}
	inline bool IsOutsidePortal(const glm::dvec2& p) {
		const glm::dvec2 mn(Vars::Portal::min);
		const glm::dvec2 mx(Vars::Portal::max);
		if (mx.x - mn.x <= 0 || mx.y - mn.y <= 0)
			return false;
		return p.x < mn.x || p.x >= mx.x || p.y < mn.y || p.y >= mx.y;
	}

	inline void WrapCursorNow(GLFWwindow* window) {
		glm::dvec2 loc;
		glfwGetCursorPos(window, &loc.x, &loc.y);
		int wx, wy;
		glfwGetWindowPos(window, &wx, &wy);
		const glm::dvec2 scr = loc + glm::dvec2{ wx, wy };

		if (!IsOutsidePortal(scr))
			return;

		const glm::dvec2 wrappedLocal = WrapScreenPos(scr) - glm::dvec2{ wx, wy };
		TeleportCursor(window, wrappedLocal);
		ImGui_ImplGlfw_CursorPosCallback(window, wrappedLocal.x, wrappedLocal.y);
	}

	inline void MyCallbackMouse(GLFWwindow* window, double xpos, double ypos) {
		const glm::dvec2 newPos{ xpos, ypos };
		const glm::dvec2 oldPos = Vars::Mouse::pos;

		if (Vars::Mouse::isRaw) {
			Vars::Mouse::delta -= newPos - oldPos;
			Vars::Mouse::pos = newPos;
			return;
		}


		if (Vars::Mouse::justTeleported) {
			const glm::dvec2 portalSize = glm::dvec2(Vars::Portal::max - Vars::Portal::min);
			const glm::dvec2 diff = glm::abs(newPos - oldPos);

			if ((portalSize.x > 0 && diff.x > portalSize.x * 0.5) ||
				(portalSize.y > 0 && diff.y > portalSize.y * 0.5)) {
				return;
			}

			Vars::Mouse::justTeleported = false;
		}

		if (Vars::Portal::isEnable) {
			int wx, wy;
			glfwGetWindowPos(window, &wx, &wy);
			const glm::dvec2 screenPos = newPos + glm::dvec2{ wx, wy };

			if (IsOutsidePortal(screenPos)) {
				Vars::Mouse::delta -= newPos - oldPos;

				const glm::dvec2 wrappedLocal = WrapScreenPos(screenPos) - glm::dvec2{ wx, wy };
				TeleportCursor(window, wrappedLocal);

				ImGui_ImplGlfw_CursorPosCallback(window, wrappedLocal.x, wrappedLocal.y);
				return;
			}
		}

		Vars::Mouse::delta -= newPos - oldPos;
		Vars::Mouse::pos = newPos;
		ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
	}


	inline void DrawBounds() {
		if (!Vars::Portal::isEnable)
			return;

		auto drawlist = ImGui::GetForegroundDrawList();
		
		float thikness = 2.f + 4.f * (std::sin(glfwGetTime() * 3.14156295f) * 0.5f + 0.5f);

		drawlist->AddRect(
			{ (float)Vars::Portal::min.x, (float)Vars::Portal::min.y }, 
			{ (float)Vars::Portal::max.x, (float)Vars::Portal::max.y }, 
			ImColor(1.f, 1.f, 0.3f, 0.5f),
			0,
			0,
			thikness
		);

	}

	inline void Init() {
		GLFWwindow* window = WindowManager::getInstance().GetHandle();
		glfwSetCursorPosCallback(window, MyCallbackMouse);
		glfwGetCursorPos(window, &Vars::Mouse::pos.x, &Vars::Mouse::pos.y);
		Vars::isInstalled = true;
	}

	inline void SetRawMode(bool flag) {
		if (flag == Vars::Mouse::isRaw)
			return;

		Vars::Mouse::isRaw = flag;

		if (!Vars::isInstalled)
			Init();

		auto* window = WindowManager::getInstance().GetHandle();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::GetIO().MousePos.x = Vars::Mouse::pos_before_raw.x;
		ImGui::GetIO().MousePos.y = Vars::Mouse::pos_before_raw.y;
		ImGui::GetIO().MouseDelta.x = 0;
		ImGui::GetIO().MouseDelta.y = 0;

		if (Vars::Mouse::isRaw) {
			Vars::Portal::isEnable = false;

			glfwGetCursorPos(window, &Vars::Mouse::pos_before_raw.x, &Vars::Mouse::pos_before_raw.y);

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

			glfwGetCursorPos(window, &Vars::Mouse::pos.x, &Vars::Mouse::pos.y);
			Vars::Mouse::delta = { 0.0, 0.0 };

			io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

			glfwSetCursorPos(window, Vars::Mouse::pos_before_raw.x, Vars::Mouse::pos_before_raw.y);
			TeleportCursor(window, Vars::Mouse::pos_before_raw);
			io.AddMousePosEvent((float)Vars::Mouse::pos_before_raw.x, (float)Vars::Mouse::pos_before_raw.y);
		}
	}
	inline void ToggleRawMode() {
		SetRawMode(!Vars::Mouse::isRaw);
	}


	/// <summary>
	/// Позиции в ЭКРАННЫХ коориданатах
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	inline void SetBoundPortal(glm::ivec2 min, glm::ivec2 max) {
		Vars::Portal::min = min;
		Vars::Portal::max = max;
	}
	inline void SetPortalMode(bool flag) {
		Vars::Portal::isEnable = flag;

		if (flag && Vars::Mouse::isRaw)
			SetRawMode(false);

		Vars::Portal::isEnable = flag;
		if (flag)
			WrapCursorNow(WindowManager::getInstance().GetHandle());
	}
	inline void UpdatePortal() {
		if (!Vars::Portal::isEnable || Vars::Mouse::isRaw)
			return;
		WrapCursorNow(WindowManager::getInstance().GetHandle());
	}

	inline glm::vec2 GetMouseDelta() { return Vars::Mouse::delta; }
	inline void ResetMouseDelta() { Vars::Mouse::delta = { 0.0, 0.0 }; }

	inline glm::vec2 GetMousePos_Window() { return Vars::Mouse::pos; }
	inline glm::vec2 GetMousePos_Screen() {
		auto* window = WindowManager::getInstance().GetHandle();
		int winX, winY;
		glfwGetWindowPos(window, &winX, &winY);
		return  GetMousePos_Window() + glm::vec2{winX, winY};
	}


}


#endif