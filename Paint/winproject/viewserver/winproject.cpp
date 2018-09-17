//	-------------------------------------	organization	------------------------------------------
//		includes
//		global var
///			//program entry
//		winmain()
//			{
//			register
//			create win
//			message loop -> winproc
//			}
///			//message functions
//		OnLBD(..)
//			{
//			stuff
//			}
//		OnMMN(..)
///			//message loop function
//		winproc()
//			{
//			switch-case
//				(OnCreate,OnTimer,OnPaint, OnMM,OnLBD, OnLBU,..)
//			}
//  --------------------------------------------------------------------------------------------------



#include "ground.h"

//	defines
#define MAX_LOADSTRING 1000
#define TIMER1 111
//	global variables
HINSTANCE hInst;											//	program number = instance
TCHAR szTitle[MAX_LOADSTRING];								//	name in window title
TCHAR szWindowClass[MAX_LOADSTRING];						//	class name of window
HFONT editfont;												//	a font to write
HWND hMain = NULL;											//	number of windows = handle window = hwnd
static char MainWin[] = "MainWin";							//	class name
HBRUSH  hWinCol = CreateSolidBrush(RGB(180, 180, 180));		//	a color
int dx = 0, dy = 0;
int showcross = 1;

vector<COLORREF> colors;
int colorId = 0;

class Painted_pixels
{
public:
	int x;
	int y;
	COLORREF color;
};

vector<Painted_pixels> paintedPixels;
bool painting = false;
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);	//message loop function (containing all switch-case statements
void draw_line(HDC DC, int x, int y, int a, int b, COLORREF color);

int APIENTRY WinMain(				//	the main function in a window program. program starts here
	HINSTANCE hInstance,			//	here the program gets its own number
	HINSTANCE hPrevInstance,		//	in case this program is called from within another program
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{

	
	hInst = hInstance;												//						save in global variable for further use
	// TODO: Hier Code einfügen.
	MSG msg;
	
	// Globale Zeichenfolgen initialisieren
	LoadString(hInstance, 103, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, 104, szWindowClass, MAX_LOADSTRING);
	//register Window													<<<<<<<<<<			STEP ONE: REGISTER WINDOW						!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	WNDCLASSEX wcex;												//						=> Filling out struct WNDCLASSEX
	BOOL Result = TRUE;
	wcex.cbSize = sizeof(WNDCLASSEX);								//						size of this struct (don't know why
	wcex.style = CS_HREDRAW | CS_VREDRAW;							//						?
	wcex.lpfnWndProc = (WNDPROC)WndProc;							//						The corresponding Proc File -> Message loop switch-case file
	wcex.cbClsExtra = 0;											//
	wcex.cbWndExtra = 0;											//
	wcex.hInstance = hInstance;										//						The number of the program
	wcex.hIcon = LoadIcon(hInstance, NULL);							//
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);						//
	wcex.hbrBackground = hWinCol;									//						Background color
	wcex.lpszMenuName = NULL;										//
	wcex.lpszClassName = MainWin;									//						Name of the window (must the the same as later when opening the window)
	wcex.hIconSm = LoadIcon(wcex.hInstance, NULL);					//
	Result = (RegisterClassEx(&wcex) != 0);							//						Register this struct in the OS

				//													STEP TWO: OPENING THE WINDOW with x,y position and xlen, ylen !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	hMain = CreateWindow(MainWin, "TestWin CST 325", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 500, NULL, NULL, hInst, NULL);
	if (hMain==0)	return 0;

	ShowWindow(hMain, nCmdShow);
	UpdateWindow(hMain);

	//													STEP THREE: Going into the infinite message loop							  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	bool quit = FALSE;
	while (!quit)
	{
		//if (GetMessage(&msg, NULL, 0, 0) > 0)
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			if (msg.message == WM_QUIT){ quit = TRUE; break; }

			
				TranslateMessage(&msg);								//						IF a meessage occurs, the WinProc will be called!!!!
				DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
///////////////////////////////////////////////////
void redr_win_full(HWND hwnd, bool erase)
{
	RECT rt;
	GetClientRect(hwnd, &rt);
	InvalidateRect(hwnd, &rt, erase);
}

///////////////////////////////////
//		This Function is called every time the Left Mouse Button is down
///////////////////////////////////
void OnLBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	painting = true;
}
///////////////////////////////////
//		This Function is called every time the Right Mouse Button is down
///////////////////////////////////
void OnRBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	colorId = (colorId + 1) % colors.size();
}
///////////////////////////////////
//		This Function is called every time a character key is pressed
///////////////////////////////////
void OnChar(HWND hwnd, UINT ch, int cRepeat)
{

}
///////////////////////////////////
//		This Function is called every time the Left Mouse Button is up
///////////////////////////////////
void OnLBU(HWND hwnd, int x, int y, UINT keyFlags)
{
	painting = false;
	Painted_pixels fake;
	fake.x = -1;
	paintedPixels.push_back(fake);

	/*
	if (x > 250)
	{
		PostQuitMessage(0);
	}
	*/
}
///////////////////////////////////
//		This Function is called every time the Right Mouse Button is up
///////////////////////////////////
void OnRBU(HWND hwnd, int x, int y, UINT keyFlags)
	{
	

	}
///////////////////////////////////
//		This Function is called every time the Mouse Moves
///////////////////////////////////
void OnMM(HWND hwnd, int x, int y, UINT keyFlags)
{
	dx = x;
	dy = y;

	if (painting)
	{
		Painted_pixels pixel;
		pixel.x = x;
		pixel.y = y;
		pixel.color = colors[colorId];

		paintedPixels.push_back(pixel);
	}

	if ((keyFlags & MK_LBUTTON) == MK_LBUTTON)
	{
	}

	if ((keyFlags & MK_RBUTTON) == MK_RBUTTON)
	{
	}
}
///////////////////////////////////
//		This Function is called once at the begin of a program
///////////////////////////////////
#define TIMER1 1

BOOL OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
{
	hMain = hwnd;
	//editfont = CreateFont(-10, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, "Arial");	

	if (!SetTimer(hwnd, TIMER1, 20, NULL))
	{
		MessageBox(hwnd, "No Timers Available", "Info", MB_OK);
		return FALSE;
	}

	colors.push_back(RGB(0, 0, 0));			//Black
	colors.push_back(RGB(255, 0, 0));		//Red
	colors.push_back(RGB(0, 255, 0));		//Green
	colors.push_back(RGB(0, 0, 255));		//Blue
	colors.push_back(RGB(255, 255, 255));	//White
	
	return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HWND hwin;
	
	switch (id)
	{
	default:
		break;
	}

}
//************************************************************************
void OnTimer(HWND hwnd, UINT id)
	{
	static int count = 0;
	//count++;
	//if (count >= 10) //is called 5 times a second
	//	{
	//	count = 0;
	//	if (showcross == 1)
	//		showcross = 0;
	//	else
	//		showcross = 1;
	//	}
	redr_win_full(hwnd, FALSE); //calling the OnPaint
	}
//************************************************************************
///////////////////////////////////
//		This Function is called every time the window has to be painted again
///////////////////////////////////


void OnPaint(HWND hwnd)
	{

	//if(rglobe.EckSizing.active_sizing())return;
	PAINTSTRUCT PaintStruct;
	HDC DC_ = BeginPaint(hwnd, &PaintStruct);
	HBITMAP hbmMem, hbmOld;
	HDC DC = CreateCompatibleDC(DC_);
	RECT rc;
	GetClientRect(hwnd, &rc);
	hbmMem = CreateCompatibleBitmap(DC_, rc.right - rc.left, 2000);
	hbmOld = (HBITMAP)SelectObject(DC, hbmMem);
	COLORREF bg = RGB(180, 180, 180);
	//if(rglobe.EckSizing.active_sizing())
	//bg=RGB(255,255,0);
	HBRUSH hbrBkGnd = CreateSolidBrush(bg);
	FillRect(DC, &rc, hbrBkGnd);
	DeleteObject(hbrBkGnd);

	
	
	//draw here:
	//draw_line(DC, 0, 0, 300, 300, 255, 255, 255, 5);
	
	/*if (showcross == 1)
		{
		draw_line(DC, dx - 10, dy, dx + 10, dy, 255, 0, 0, 5);
		draw_line(DC, dx, dy - 10, dx, dy + 10, 255, 0, 0, 5);
		}
		*/

	for (int i = 1; i < paintedPixels.size(); ++i)
	{
		if (paintedPixels[i].x == -1) {
			i++;
			continue;
		}
		draw_line(DC, paintedPixels[i-1].x, paintedPixels[i-1].y, paintedPixels[i].x, paintedPixels[i].y, paintedPixels[i].color);
	}

//					DOUBLE BUFFERING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	BitBlt(DC_, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, DC, 0, 0, SRCCOPY);
	SelectObject(DC, hbmOld);
	DeleteObject(hbmMem);
	DeleteDC(DC);
	EndPaint(hwnd, &PaintStruct);
	}
//****************************************************************************

void draw_line(HDC DC,int x, int y, int a, int b, COLORREF color)
	{
	HPEN Stift = CreatePen(PS_SOLID, 3, color);
	SelectObject(DC, Stift);
	MoveToEx(DC, x, y, NULL);
	LineTo(DC, a, b);
	DeleteObject(Stift);
	}
//*************************************************************************
void OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
	{

	switch (vk)
		{
			case 65://a

				break;
			default:break;
			
		}
	}

//*************************************************************************
void OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
	{
	switch (vk)
		{
			default:break;
			
		}

	}


//**************************************************************************
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{

    /*
	#define HANDLE_MSG(hwnd, message, fn)    \
    case (message): return HANDLE_##message((hwnd), (wParam), (lParam), (fn))
	*/
		
		HANDLE_MSG(hwnd, WM_CHAR, OnChar);			// when a key is pressed and its a character
		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLBD);	// when pressing the left button
		HANDLE_MSG(hwnd, WM_LBUTTONUP, OnLBU);		// when releasing the left button
		HANDLE_MSG(hwnd, WM_MOUSEMOVE, OnMM);		// when moving the mouse inside your window
		HANDLE_MSG(hwnd, WM_RBUTTONDOWN, OnRBD);
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);		// called only once when the window is created
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);		// drawing stuff
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);	// not used
		HANDLE_MSG(hwnd, WM_KEYDOWN, OnKeyDown);	// press a keyboard key
		HANDLE_MSG(hwnd, WM_KEYUP, OnKeyUp);		// release a keyboard key
		HANDLE_MSG(hwnd, WM_TIMER, OnTimer);		// timer

	case WM_ERASEBKGND:
		return (LRESULT)1;
	
	case WM_DESTROY:		
		PostQuitMessage(0);
		break;
	
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
