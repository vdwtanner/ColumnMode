#include "stdafx.h"
#include "Main.h"
#include "Program.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
bool g_done;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND*);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_COLUMNMODE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	HWND hwnd;
	if (!InitInstance(hInstance, nCmdShow, &hwnd))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COLUMNMODE));

	AddClipboardFormatListener(hwnd);

	MSG msg;

	LARGE_INTEGER performanceFrequency;
	VerifyBool(QueryPerformanceFrequency(&performanceFrequency));

	LARGE_INTEGER previousCounter;
	VerifyBool(QueryPerformanceCounter(&previousCounter));

	// Message loop
	g_done = false;
	while (!g_done)
	{
		while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		LARGE_INTEGER currentCounter;
		VerifyBool(QueryPerformanceCounter(&currentCounter));

		LONGLONG elapsedTicks = currentCounter.QuadPart - previousCounter.QuadPart;
		LONGLONG elapsedMilliseconds = (elapsedTicks * 1000) / performanceFrequency.QuadPart;
		if (elapsedMilliseconds >= 16)
		{
			Draw(hwnd);

			previousCounter = currentCounter;

			Update();
		}
	}

	RemoveClipboardFormatListener(hwnd);

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COLUMNMODE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_COLUMNMODE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);

}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND* returnedHwnd)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	InitGraphics(hWnd);

	ShowWindow(hWnd, nCmdShow);
	BOOL updateResult = UpdateWindow(hWnd);

	*returnedHwnd = hWnd;

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_SAVE:
			OnSave();
			break;
		case ID_FILE_OPEN:
			OnOpen(hWnd);
			break;
		case ID_EDIT_UNDO:
			OnUndo(hWnd);
			break;
		case ID_EDIT_CUT:
			OnCut(hWnd);
			break;
		case ID_EDIT_COPY:
			OnCopy(hWnd);
			break;
		case ID_EDIT_PASTE:
			OnPaste(hWnd);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		Draw(hWnd);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		OnMouseLeftButtonDown(lParam);
	}
	break;
	case WM_LBUTTONUP:
	{
		OnMouseLeftButtonUp(hWnd);
	}
	break;
	case WM_MOUSEMOVE:
	{
		OnMouseMove(hWnd, wParam, lParam);
	}
	break;
	case WM_SIZE:
	{
		OnWindowResize(hWnd);
	}
	break;
	case WM_KEYDOWN:
	{
		OnKeyDown(hWnd, wParam);
	}
	break;
	case WM_KEYUP:
	{
		OnKeyUp(hWnd, wParam);
	}
	break;
	case  WM_HSCROLL:
	{
		OnHorizontalScroll(hWnd, wParam);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		OnMouseWheel(hWnd, wParam);
	}
	case  WM_VSCROLL:
	{
		OnVerticalScroll(hWnd, wParam);
	}
	break;
	case WM_CLIPBOARDUPDATE:
	{
		OnClipboardContentsChanged(hWnd);
		return 0;
	}
	case WM_DESTROY:
		g_done = true;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
