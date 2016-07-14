#include "DxLib.h"
#include <string>
#include <chrono>
#include <thread>//std::this_thread::sleep_until
#include <unordered_map>
#include <dxlibex/basic_types.hpp>//dxle::sizei, dxle::tstring
#include <dxlibex/algorithm.hpp>//dxle::safe_dist
class Window {
	HWND windowHandle;
	MSG message;
	static std::unordered_map<HWND, dxle::sizei> windows_size_log;
public:
	Window(dxle::tstring title, dxle::tstring className) : windowHandle(nullptr), message() {
		// クラス情報を登録する
		WNDCLASS windowClass = {};
		windowClass.lpfnWndProc = Procedure;
		windowClass.hInstance = ::GetModuleHandle(nullptr);
		windowClass.lpszClassName = className.c_str();
		windowClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
		::RegisterClass(&windowClass);
		// ウィンドウを生成する
		this->windowHandle = ::CreateWindowEx(
			0, className.c_str(), title.c_str(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr, nullptr, ::GetModuleHandle(nullptr), nullptr
		);
		// 使用するウィンドウを設定
		DxLib::SetUserWindow(this->windowHandle);
		// ウインドウのメッセージ処理をＤＸライブラリが行う
		DxLib::SetUserWindowMessageProcessDXLibFlag(false);
		// 描画可能サイズをディスプレイのサイズに（最大化した時に描画できるように）
		DxLib::SetGraphMode(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN), 16);
	}
	// ウィンドウプロシージャ
	static LRESULT CALLBACK Procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message) {
			//http://stackoverflow.com/questions/9321549/handling-aerosnap-message-in-wndproc
			case WM_WINDOWPOSCHANGING: {//wParam: unused, lParam(WINDOWPOS*):information about the window's new size and position
				auto& winPos = *reinterpret_cast<WINDOWPOS*>(lParam);
				MONITORINFOEX MonitorInfoEx = {};
				MonitorInfoEx.cbSize = sizeof(MonitorInfoEx);
				::GetMonitorInfo(::MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST), &MonitorInfoEx);
				RECT& rcWorkArea = MonitorInfoEx.rcWork;
				//check for a framechange - but ignore initial draw. x,y is top left of current monitor so must be a maximise
				/*
				winPos.flags(SetWindowPos Flags) - macro is defined at WinUser.h

				binay             :hex   :dec   :: description

				//show
				//focus on
				0b0000000000000011:0x0003:3     :: SWP_NOSIZE | SWP_NOMOVE
				*note: winPos.cx and winPos.cy is equal to 0.

				//aero-snap unleft
				//aero-snap unright
				//double click title after aero-snap left
				0b0000000000100000:0x0020:32    :: SWP_FRAMECHANGED

				//init
				0b0000000001000011:0x0043:67    :: SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW
				*note: winPos.cx and winPos.cy is equal to 0.

				//resize
				//aero-snap left
				//aero-snap right
				0b0000001000000100:0x0204:516   :: SWP_NOZORDER | SWP_NOOWNERZORDER

				//aero-snap up
				//aero-snap unup
				/double click title
				//maximize
				//unmazimize
				0b1000000000100000:0x8020:32800 :: SWP_FRAMECHANGED | unknown

				//aero-snap minmize
				0b1000000100100000:0x8120:33056 :: SWP_FRAMECHANGED | SWP_NOCOPYBITS | unknown
				0b1000000101110000:0x8170:33136 :: SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOCOPYBITS | unknown


				macro definition

				0b0000000000000001:0x0001:1     :: SWP_NOSIZE
				0b0000000000000010:0x0002:2     :: SWP_NOMOVE
				0b0000000000000100:0x0004:4     :: SWP_NOZORDER
				0b0000000000001000:0x0008:8     :: SWP_NOREDRAW
				0b0000000000010000:0x0010:16    :: SWP_NOACTIVATE
				0b0000000000100000:0x0020:32    :: SWP_FRAMECHANGED
				0b0000000001000000:0x0040:64    :: SWP_SHOWWINDOW
				0b0000000010000000:0x0080:128   :: SWP_HIDEWINDOW
				0b0000000100000000:0x0100:256   :: SWP_NOCOPYBITS
				0b0000001000000000:0x0200:512   :: SWP_NOOWNERZORDER
				0b0000010000000000:0x0400:1024  :: SWP_NOSENDCHANGING

				0b0010000000000000:0x2000:8192  :: SWP_DEFERERASE
				0b0100000000000000:0x4000:16384 :: SWP_ASYNCWINDOWPOS
				0b1000000000000000:0x8000:32768 :: unknown

				*/
				if (
					((winPos.flags & SWP_FRAMECHANGED) == SWP_FRAMECHANGED)
					&& (winPos.flags & SWP_NOSIZE) != SWP_NOSIZE
					&& winPos.x == rcWorkArea.left && winPos.y == rcWorkArea.top
				) {
					//set max size to the size of the *current* monitor
					winPos.cx = static_cast<decltype(winPos.cx)>(dxle::safe_dist(rcWorkArea.right, rcWorkArea.left));
					winPos.cy = static_cast<decltype(winPos.cy)>(dxle::safe_dist(rcWorkArea.bottom, rcWorkArea.top));
				}
				dxle::sizei new_size(winPos.cx, winPos.cy);
				if (!windows_size_log.count(handle) || windows_size_log[handle] != new_size) {//window sizeが変わった
					dxle::sizei new_size(winPos.cx, winPos.cy);
					if(0 == new_size) DxLib::GetWindowSize(&new_size.width, &new_size.height);//0になることがあるのでその時はDxLibの関数を呼ぶ
					windows_size_log[handle] = new_size;
				}
				break;
			}
			case WM_DESTROY:
				::PostQuitMessage(0);
				return 0;
		}
		return ::DefWindowProc(handle, message, wParam, lParam);
	}

	// ウィンドウハンドルを取得する
	HWND get_handle() const {
		return this->windowHandle;
	}
	// ウィンドウのサイズを取得する
	const dxle::sizei& get_size() const  {
		if (!windows_size_log.count(this->windowHandle) || 0 == windows_size_log[this->windowHandle]) {
			dxle::sizei result;
			DxLib::GetWindowSize(&result.width, &result.height);
			windows_size_log[this->windowHandle] = result;
		}
		return windows_size_log[this->windowHandle];
	}

	// メッセージ処理
	bool loop() {
		while (true) {
			if (message.message == WM_QUIT) return false;
			if (!::PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) return true;
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}

	// ウィンドウを表示する
	void show() {
		::ShowWindow(windowHandle, SW_SHOW);
	}
};
std::unordered_map<HWND, dxle::sizei> Window::windows_size_log;//実体
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ウィンドウを生成
	Window window(_T("ヌッ！"), _T("DXlib"));
	// 初期化
	if (-1 == DxLib_Init()) return -1;

	// ウィンドウを表示
	window.show();

	// メインループ
	while (window.loop()) {
		const auto t = std::chrono::steady_clock::now();
		// 試しに枠を描画してみる
		ClearDrawScreen();
		auto windowSize = window.get_size();
		DrawBox(0, 0, windowSize.width, windowSize.height, DxLib::GetColor(255, 255, 255), true);
		DrawBox(windowSize.width / 4, windowSize.height / 4, windowSize.width * 3 / 4, windowSize.height * 3 / 4, DxLib::GetColor(0, 0, 0), true);
		ScreenFlip();
		std::this_thread::sleep_until(t + std::chrono::milliseconds(100));
	}
	return DxLib_End();
}