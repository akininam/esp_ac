

#include <Windows.h>
#include <d3d11.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "Memory.h"
#include "ESP.h"
#include "WorldToScreen.h"
#include "offsets.h"
#include <dwmapi.h>

#pragma comment (lib, "d3d11.lib")

HWND hwnd = NULL;
extern ID3D11Device* g_pd3dDevice = NULL;
extern ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
extern IDXGISwapChain* g_pSwapChain = NULL;
extern ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
extern D3D_FEATURE_LEVEL level{};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void CleanupDeviceD3D()
{
	if (g_mainRenderTargetView){g_mainRenderTargetView->Release();g_mainRenderTargetView = NULL;}
	if (g_pSwapChain){g_pSwapChain->Release();g_pSwapChain = NULL;}
	if (g_pd3dDeviceContext){g_pd3dDeviceContext->Release();g_pd3dDeviceContext = NULL;}
	if (g_pd3dDevice){g_pd3dDevice->Release();g_pd3dDevice = NULL;}
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer{nullptr};
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer)
	{
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();

	}
	
	
}
	

bool CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 2U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	constexpr D3D_FEATURE_LEVEL levels[2]
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

 HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0U, levels, 2U, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &level, &g_pd3dDeviceContext);
 if (FAILED(hr))
 {
	 MessageBox(0, "FAILED to create D3D11 Device or SwapChain", "DX11 error", MB_ICONERROR);
	 return false;
 }
 return true;
}

void InitImGui(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	ImGui::StyleColorsDark();

}

void RenderLoop(Memory& mem)
{
	ViewMatrix viewMatrix = mem.Read<ViewMatrix>(Offsets::BASE_ADDRESS + Offsets::VIEWMATRIX);
	int localTeam		  = mem.Read<int>(Offsets::PLAYER_BASE + Offsets::TEAM_OFFSET);

	while (true)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				return;
		}
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		int entityList = mem.Read<int>(Offsets::entity_list);
		int entityCount = mem.Read<int>(Offsets::players_in_map);


		for (int i = 0; i < entityCount; ++i)
		{
			int entity = mem.Read<int>(entityList + i * 0x4);
			if (entity == 0) continue;

			int health = mem.Read<int>(entity + Offsets::HEALTH_OFFSET);
			int team = mem.Read<int>(entity + Offsets::TEAM_OFFSET);
			if (health <= 0 || team == localTeam) continue;

			Vector3 pos;
			pos.x = mem.Read<float>(entity + Offsets::POSx_POS);
			pos.y = mem.Read<float>(entity + Offsets::POSy_POS);
			pos.z = mem.Read<float>(entity + Offsets::POSz_POS);

			Vector2 screenPos;
			if (WorldToScreen(pos, screenPos, viewMatrix, 1920, 1080))
			{
				Vector2 bottom = screenPos;
				Vector2 top = { screenPos.x, screenPos.y - 75 };
				Renderer::DrawBox(top, bottom, IM_COL32(255, 0, 0, 255));

				std::vector<Vector2> skeleton =
				{
					top,
					{top.x, top.y + 20},
					{top.x, top.y + 40},
					{top.x, top.y + 60},
					bottom
				};
				Renderer::DrawSkeleton(skeleton, IM_COL32(0, 255, 0, 255));

			}
		}
		ImGui::Render();
		const float clear_color_width_alpha[4] = { 0, 0, 0, 0 };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_width_alpha);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		g_pSwapChain->Present(1, 0);
	}
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int cmd_show)
{
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"ESP";

	if (!RegisterClassExW(&wc))
	{
		MessageBox(0, "Failed to register window class.", "Error", MB_ICONERROR);
		return 1;
	}

	const HWND hwnd = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
		wc.lpszClassName,
		L"ESP",
		WS_POPUP,
		0, 0, 1920, 1080,
		NULL, NULL, wc.hInstance, NULL);

	if (!hwnd)
	{
		MessageBox(0, "Failed to create window.", "Error", MB_ICONERROR);
		return 1;
	}

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	RECT client_area = {};
	GetClientRect(hwnd, &client_area);
	RECT window_area = {};
	GetWindowRect(hwnd, &window_area);
	POINT diff = {};
	ClientToScreen(hwnd, &diff);

	const MARGINS margins = {
		window_area.left + (diff.x - window_area.left),
		window_area.top + (diff.y - window_area.top),
		client_area.right,
		client_area.bottom
	};

	if (FAILED(DwmExtendFrameIntoClientArea(hwnd, &margins)))
	{
		MessageBox(0, "Failed to extend frame into client area.", "Error", MB_ICONERROR);
		return 1;
	}

	if (!CreateDeviceD3D(hwnd))
	{
		MessageBox(0, "Failed to initialize D3D11 device.", "Error", MB_ICONERROR);
		return 1;
	}

	ShowWindow(hwnd, cmd_show);
	UpdateWindow(hwnd);
	CreateRenderTarget();

	InitImGui(hwnd);

	try
	{
		Memory mem("ac_client.exe");
		if (!mem.IsValid())
		{
			MessageBox(0, "Failed to attach to ac_client.exe.", "Error", MB_ICONERROR);
			return 1;
		}
		RenderLoop(mem);
	}
	catch (const std::exception& ex)
	{
		MessageBox(0, ex.what(), "Exception caught", MB_ICONERROR);
		return 1;
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(hwnd);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
	return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) return true;
	switch (msg) {
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupDeviceD3D();
			CreateRenderTarget();
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

