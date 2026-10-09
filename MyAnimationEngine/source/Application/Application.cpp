#include "Application.h"


LRESULT CALLBACK custom_wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application::Application() {


	Init_OpenGL();
	Init_BorderWindow();
	Init_IconApplication();
	Init_ImGui();


	mainMenuBar = new MainMenuBar();


	FontManager::getInstance().Update();

	fps_limiter = new FPS_Timer;
	fps_limiter->SetTargetFPS(1800.01);
	glfwSwapInterval(0);

	layersData			= new LayersDataProvider();
	timelineData		= new TimelineProviderData();
	renderer			= new RendererAnimationScene(layersData);

	layersFactory		= new LayerFactory(timelineData, layersData);
	timelineController  = new TimelineController(timelineData);
	timelinePanel		= new Timeline_Panel(fps_limiter, timelineController, timelineData, layersData, layersFactory);

	playerComposition	= new Timeline_Player(timelineController);

	preview = new Timeline_Preview(fps_limiter, renderer);


	auto& g = DebugOverlay::Graph("DeltaTime");
	g.windowSec = 10.f;
	g.color = ImColor(255, 180, 0);
	//g.smooth = DebugOverlay::SmoothMode::Box;

}
Application::~Application() {

}

void Application::Init_OpenGL() {
	glfwInit();
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	WindowManager::getInstance().SetTitle(Title);
	WindowManager::getInstance().SetSize({ 1280,720 });
	WindowManager::getInstance().Centerize();
	WindowManager::getInstance().Maximize();

}
void Application::Init_BorderWindow() {
	GLFWwindow* win = WindowManager::getInstance().GetHandle();
	HWND hwnd = glfwGetWin32Window(win);
	g_hwnd_to_window[hwnd] = win;

	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	style |= WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	SetWindowLong(hwnd, GWL_STYLE, style);


	MARGINS m = { -1 };
	DwmExtendFrameIntoClientArea(hwnd, &m);


	DWM_WINDOW_CORNER_PREFERENCE pref = DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_DONOTROUND;
	DwmSetWindowAttribute(
		hwnd,
		DWMWA_WINDOW_CORNER_PREFERENCE,
		&pref,
		sizeof(pref)
	);


	orig_wndproc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)custom_wndproc);

	glfwSetWindowUserPointer(win, this);
}
void Application::Init_IconApplication() {
	if (isFileExist(Path2IconApplication)) {
		GLFWimage images[1];
		int width = 0, height = 0;
		images[0].pixels = stbi_load(Path2IconApplication.c_str(), &images[0].width, &images[0].height, 0, 4);
		glfwSetWindowIcon(WindowManager::getInstance().GetHandle(), 1, images);

#ifdef _DEBUG
		std::cout << "Loading ICON:\n\twidth: " << images[0].width << "\n\theight: " << images[0].height << std::endl;
#endif // _DEBUG

		stbi_image_free(images[0].pixels);
	}
	else {
#ifdef _DEBUG
		std::cout << "NOT FIND ICON FILE!!!!\n";
#endif // _DEBUG
	}
}
void Application::Init_ImGui() {
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(WindowManager::getInstance().GetHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	auto& s = ImGui::GetStyle();
	s.ImageBorderSize = 1.f;
	s.DockingSeparatorSize = 5.f;
	s.TabBorderSize = 0.f;
	s.TabRounding = 0.f;

	s.TabBarOverlineSize = 1.f;

	s.Colors[ImGuiCol_Border] = ImColor(0.f, 0.f, 0.f, 1.f);
	s.Colors[ImGuiCol_WindowBg] = ImColor(.11f, .11f, .11f, 1.f);
	s.Colors[ImGuiCol_PopupBg].w = 1.f;
	s.Colors[ImGuiCol_FrameBg] = ImColor(56, 56, 56, 138);
	s.Colors[ImGuiCol_Header] = ImColor(112, 178, 255, 232);

	s.Colors[ImGuiCol_TitleBg] = ImColor(.11f, .11f, .11f, 1.f);
	s.Colors[ImGuiCol_TitleBgActive] = ImColor(.11f, .11f, .11f, 1.f);
	s.Colors[ImGuiCol_ResizeGrip] = ImColor(0.f, 0.f, 0.f, 1.f);
	s.Colors[ImGuiCol_ResizeGripHovered] = ImColor(0.f, 0.f, 0.f, 1.f);
	s.Colors[ImGuiCol_ResizeGripActive] = ImColor(0.f, 0.f, 0.f, 1.f);
	
	s.Colors[ImGuiCol_TabHovered] = ImColor(0.f, 0.f, 0.f, 0.f);
	s.Colors[ImGuiCol_Tab] = ImColor(0.f, 0.f, 0.f, 0.f);
	s.Colors[ImGuiCol_TabSelected] = ImColor(0.f, 0.f, 0.f, 0.f);
	s.Colors[ImGuiCol_TabDimmed] = ImColor(0.f, 0.f, 0.f, 0.f);
	s.Colors[ImGuiCol_TabDimmedSelected] = ImColor(0.f, 0.f, 0.f, 0.f);

	s.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImColor(0.5f, 0.5f, 0.5f, 1.f);
	s.Colors[ImGuiCol_TabSelectedOverline] = ImColor(1.f, 1.f, 1.f, 1.f);


	ImGui::GetIO().IniFilename = NULL;
	ImGui::GetIO().LogFilename = NULL;
}
void Application::Init_DockLayout(ImGuiViewport* viewport, const ImGuiID& dockspace_id) {
	static bool dock_initialized = false;
	if (!dock_initialized) {

		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodePos(dockspace_id, viewport->Pos);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

		ImGuiID panel_id = 0;
		ImGuiID temp_id = 0;
		ImGuiID player_id = 0;
		ImGuiID left_id = 0;

		ImGui::DockBuilderSplitNode(
			dockspace_id,
			ImGuiDir_Down,
			0.40f,
			&panel_id,
			&temp_id
		);
		ImGui::DockBuilderSplitNode(
			temp_id,
			ImGuiDir_Right,
			0.20f,
			&player_id,
			&left_id
		);

		ImGui::DockBuilderDockWindow(u8"Временная шкала", panel_id);
		ImGui::DockBuilderDockWindow(u8"Проигрыватель", player_id);
		ImGui::DockBuilderDockWindow(u8"Предпросмотр", left_id);


		ImGui::DockBuilderFinish(dockspace_id);
		dock_initialized = true;
	}
}


void Application::Run() {

	while (WindowManager::getInstance().IsShouldClose() == false) {
#if LivePT_EditMode
		LivePT::ProcessEdit();
#endif

		Update();

		BeginDraw();
			Draw();
		EndDraw();

	}
}



void UpdateDockingCursorOnly()
{
	auto g = ImGui::GetCurrentContext();
	ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();

	bool is_resizing = (imgui_cursor == ImGuiMouseCursor_ResizeEW || imgui_cursor == ImGuiMouseCursor_ResizeNS);

	if (is_resizing && g->DebugHoveredDockNode != nullptr)
	{
		if (imgui_cursor == ImGuiMouseCursor_ResizeEW)
			CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);
		else
			CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_VERTICAL);
	}

}

void Application::Draw() {

	//DebugOverlay::AddText("FPS_imgui_io: " + std::to_string(ImGui::GetIO().Framerate), ImColor{ 0.5f,0.5f,1.f,1.f });
	DebugOverlay::AddText("FPS_limiter : " + std::to_string(1.f / fps_limiter->GetDeltaTime()), ImColor{ 0.5f,0.5f,1.f,1.f });
	DebugOverlay::Plot("DeltaTime", fps_limiter->GetDeltaTime() * 1000.f);

	mainMenuBar->Draw();

	playerComposition->Draw();
	timelinePanel->Draw();
	preview->Draw();

	//ImGui::ShowDemoWindow();

	DebugOverlay::DrawDebug();

	auto drawlist = ImGui::GetForegroundDrawList();
	drawlist->AddCircleFilled(ImGui::GetMousePos(), 1.f, ImColor(1.f, 0.f, 0.f, 1.f));
}



void Application::Update() {


	CursorManager::getInstance().Update();
	CursorManager::getInstance().SetCursor(eval(TypeCursor::HAND_OPEN));


	if (ImGui::IsKeyPressed(ImGuiKey_Tab)) {
		DebugOverlay::ToggleShow();
	}

	DebugOverlay::Update(fps_limiter->GetDeltaTime());
	timelinePanel->Update();
	timelineController->Update();
	playerComposition->Update();
	preview->Update();

	renderer->Update();
	renderer->Draw(timelineData->GetData().currentTime);

	UpdateDockingCursorOnly();
}

void Application::BeginDraw() {
	glfwPollEvents();

	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClear((unsigned int)BufferBit::COLOR | (unsigned int)BufferBit::DEPTH);

	FontManager::getInstance().Update();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(0, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

	Init_DockLayout(viewport, dockspace_id);
}

void Application::EndDraw() {

	ImGui::EndFrame();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(WindowManager::getInstance().GetHandle());

	fps_limiter->wait();
}


MainMenuBar* Application::GetPTR_MainMenuBar() {
	return mainMenuBar;
}


LRESULT CALLBACK custom_wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	auto it = g_hwnd_to_window.find(hwnd);
	GLFWwindow* win = it->second;
	Application* app = static_cast<Application*>(glfwGetWindowUserPointer(win));

	switch (msg) {
	case WM_NCCALCSIZE: {
		auto params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);

		if (IsZoomed(hwnd)) {
			HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(hMon, &mi);
			RECT rcWork = mi.rcWork;

			params->rgrc[0].left = rcWork.left;
			params->rgrc[0].top = rcWork.top;
			params->rgrc[0].right = rcWork.right;
			params->rgrc[0].bottom = rcWork.bottom;

			return WVR_REDRAW;
		}
		else {
			return 0;
		}
		break;
	}
	case WM_NCHITTEST: {

		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		ScreenToClient(hwnd, &pt);

		RECT rc;
		GetClientRect(hwnd, &rc);
		const int border = 4;

		if (!IsZoomed(hwnd) && glfwGetWindowMonitor(win) == NULL) {
			bool left = pt.x < border;
			bool right = pt.x >= rc.right - border;
			bool top = pt.y < border;
			bool bottom = pt.y >= rc.bottom - border;

			if (left && top)     return HTTOPLEFT;
			if (right && top)    return HTTOPRIGHT;
			if (left && bottom)  return HTBOTTOMLEFT;
			if (right && bottom) return HTBOTTOMRIGHT;

			if (left)   return HTLEFT;
			if (right)  return HTRIGHT;
			if (top)    return HTTOP;
			if (bottom) return HTBOTTOM;
		}




		float menuHeight = app->GetPTR_MainMenuBar()->GetHeightMenu();
		if (pt.y >= 0 && pt.y < menuHeight) {
			if (app->GetPTR_MainMenuBar()->IsPointOverTitleButton(pt)) {
				return HTCLIENT;
			}
			if (glfwGetWindowMonitor(win) == NULL)
				return HTCAPTION;
		}

		return HTCLIENT;
	}
	case WM_SETCURSOR: {

		if (!IsZoomed(hwnd) && glfwGetWindowMonitor(win) == NULL) {
			int hitTest = LOWORD(lParam);

			switch (hitTest) {
			case HTLEFT:
			case HTRIGHT:
				SetCursor(LoadCursor(NULL, IDC_SIZEWE));
				return TRUE;
			case HTTOP:
			case HTBOTTOM:
				SetCursor(LoadCursor(NULL, IDC_SIZENS));
				return TRUE;
			case HTTOPLEFT:
			case HTBOTTOMRIGHT:
				SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
				return TRUE;
			case HTTOPRIGHT:
			case HTBOTTOMLEFT:
				SetCursor(LoadCursor(NULL, IDC_SIZENESW));
				return TRUE;
			}
		}

		break;
	}
	case WM_GETMINMAXINFO: {
		auto mmi = reinterpret_cast<MINMAXINFO*>(lParam);

		HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hMon, &mi);

		RECT rcWork = mi.rcWork;

		mmi->ptMaxPosition.x = rcWork.left;
		mmi->ptMaxPosition.y = rcWork.top;
		mmi->ptMaxSize.x = rcWork.right - rcWork.left;
		mmi->ptMaxSize.y = rcWork.bottom - rcWork.top;

		return 0;
	}
	case WM_SETTINGCHANGE: {
		if (wParam == SPI_SETWORKAREA) {
			if (IsZoomed(hwnd)) {

				HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFO mi = { sizeof(mi) };
				GetMonitorInfo(hMon, &mi);
				RECT rcWork = mi.rcWork;

				SetWindowPos(hwnd, NULL, rcWork.left, rcWork.top,
					rcWork.right - rcWork.left, rcWork.bottom - rcWork.top,
					SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
		break;
	}

	}

	return CallWindowProc(orig_wndproc, hwnd, msg, wParam, lParam);
}