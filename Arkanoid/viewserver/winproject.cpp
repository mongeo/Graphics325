#include "ground.h"
#include <vector>


#define MAX_LOADSTRING 1000
// Globale Variablen:
HINSTANCE hInst;								// Aktuelle Instanz
TCHAR szTitle[MAX_LOADSTRING];					// Titelleistentext
TCHAR szWindowClass[MAX_LOADSTRING];			// Klassenname des Hauptfensters
HFONT editfont;
HWND hMain = NULL;
//-----------------------
static char MainWin[] = "MainWin";

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void draw_line(HDC DC, int x, int y, int a, int b, int red, int green, int blue, int width);
void draw_rectangle(HDC DC, int from_x, int from_y, int to_x, int to_y, int red, int green, int blue, int width);
void restart();

HBRUSH  hWinCol = CreateSolidBrush(RGB(180, 180, 180));

HWND g_hwnd=NULL;

//mouse position
int dx = 0;
int dy = 0;

//window size
int const WINDOW_WIDTH = 500;
int const WINDOW_HEIGHT = 500;

//paddle position and width
int PADDLE_HEIGHT = 400;
int PADDLE_WIDTH = 25;

//set the speed of the ball
int const SPEED = 5;

struct Ball
{
	int x, y, r;//x y coords and radius
	int bdx, bdy;//directions

	void update()
	{
		x += bdx;//ball position plus direction x
		y += bdy;//ball position plus direction y

		//Right side of window
		if (x > WINDOW_WIDTH - 25) {
			bdx = -SPEED;
		}
		//Left side of window
		if (x < 5) {
			bdx = SPEED;
		}

		//Paddle
		if (x <= dx + PADDLE_WIDTH && x >= dx - PADDLE_WIDTH && y == PADDLE_HEIGHT)
		{
			bdy = -SPEED;
			OutputDebugStringW(L"X");
		}

		//Below Paddle
		if (y > WINDOW_HEIGHT) {
			//bdy = -SPEED;//bounces
			restart();
		}
		//Top of window
		if (y < 10) {
			bdy = SPEED;
		}
	}

	void draw(HDC dc)
	{
		draw_line(dc, x, y, x, y, 0, 0, 255, r);
	}
} ball;

struct Brick
{
	int x, y;
	int width, height;
	bool exists;
	int r, g, b;

	void draw(HDC dc)
	{
		draw_line(dc, x, y, x + width, y, r, g, b, height);
	}

	bool collidesWith(Ball& b)
	{
		if (b.x > x && b.x < x + width &&
			b.y > y - (height / 2.0f) && b.y < y + (height / 2.0f))
		{
			return true;
		}
		return false;
	}
};

std::vector<Brick*> bricks; //<-- like Java's ArrayList class

void restart() {
	//set first row colors
	int rowR = 0;
	int rowG = 175;
	int rowB = 255;

	for (int j = 0; j < 5; ++j)
	{
		//change colors every iteration
		rowG += 5*(1-j);
		rowB += 25*(1-j);
		for (int i = 0; i < 15; ++i)
		{
			Brick* b = new Brick();

			b->width = 23;
			b->height = 5;

			b->x = i * (b->width + 8) + 15;
			b->y = j * (b->height + 2) + 5;

			b->r = rowR;
			b->g = rowG;
			b->b = rowB;

			b->exists = true;

			bricks.push_back(b);
		}
	}
	//initial ball position
	ball.x = 250;
	ball.y = 250;

	//randomly picks initial ball direction left or right
	srand(time(NULL));
	int randomDir = rand() % 10;
	if (randomDir < 5) {
		ball.bdx = -SPEED;
	}
	else {
		ball.bdx = SPEED;
	}
	ball.bdy = SPEED;
	ball.r = 5;
}


//timer:
#define TIMER1 111

// Vorwärtsdeklarationen der in diesem Codemodul enthaltenen Funktionen:
/*ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

*/

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
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
	hMain = CreateWindow(MainWin, "TestWin CST 320", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInst, NULL);
	if (!hMain)return 0;
	ShowWindow(hMain, nCmdShow);
	UpdateWindow(hMain);

	//													STEP THRE: Going into the infinity message loop							  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Hauptmeldungsschleife:
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

}
///////////////////////////////////
//		This Function is called every time the Right Mouse Button is down
///////////////////////////////////
void OnRBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
	{
	
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
	g_hwnd = hwnd;
	editfont = CreateFont(-10, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, "Arial");	
	if (!SetTimer(hwnd, TIMER1, 20, NULL))
	{
		MessageBox(hwnd, "No Timers Available", "Info", MB_OK);
		return FALSE;
	}
	

	restart();
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
	redr_win_full(hwnd, FALSE);

	ball.update();

	for (int i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i]->exists && bricks[i]->collidesWith(ball)) //<-- short circuit trick
		{
			bricks[i]->exists = false;
			ball.bdy = SPEED;
		}
	}
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
	COLORREF bg = RGB(255, 255, 255);
	//if(rglobe.EckSizing.active_sizing())
	bg=RGB(30,5,10);
	HBRUSH hbrBkGnd = CreateSolidBrush(bg);
	FillRect(DC, &rc, hbrBkGnd);
	DeleteObject(hbrBkGnd);

	//draw here

	//draw bricks
	for (int i = 0; i < bricks.size(); ++i)
	{
		if(bricks[i]->exists) bricks[i]->draw(DC);
	}

	//draw ball
	ball.draw(DC);

	//draw paddle
	draw_rectangle(DC, dx - PADDLE_WIDTH, PADDLE_HEIGHT, dx + PADDLE_WIDTH, PADDLE_HEIGHT, 255, 0, 0, 5);

	//end draw here

	//					DOUBLE BUFFERING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	BitBlt(DC_, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, DC, 0, 0, SRCCOPY);
	SelectObject(DC, hbmOld);
	DeleteObject(hbmMem);
	DeleteDC(DC);
	EndPaint(hwnd, &PaintStruct);
	}
//****************************************************************************

void draw_line(HDC DC,int x, int y, int a, int b, int red, int green, int blue, int width)
	{
	HPEN Stift = CreatePen(PS_SOLID, width, RGB(red, green, blue));
	SelectObject(DC, Stift);
	MoveToEx(DC, x, y, NULL);
	LineTo(DC, a, b);
	DeleteObject(Stift);
	}
//*************************************************************************

void draw_rectangle(HDC DC, int from_x, int from_y, int to_x, int to_y, int red, int green, int blue, int width)
{
	draw_line(DC, from_x, from_y, to_x, from_y, red, green, blue, width);
	draw_line(DC, to_x, from_y, to_x, to_y, red, green, blue, width);
	draw_line(DC, to_x, to_y, from_x, to_y, red, green, blue, width);
	draw_line(DC, from_x, to_y, from_x, from_y, red, green, blue, width);
}


void OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
	{

	switch (vk)
		{
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
	int wmId, wmEvent;
	SCROLLINFO si;


	
	switch (message)
	{
	/*
	#define HANDLE_MSG(hwnd, message, fn)    \
    case (message): return HANDLE_##message((hwnd), (wParam), (lParam), (fn))
	*/
		
		HANDLE_MSG(hwnd, WM_CHAR, OnChar);
		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLBD);
		HANDLE_MSG(hwnd, WM_LBUTTONUP, OnLBU);
		HANDLE_MSG(hwnd, WM_MOUSEMOVE, OnMM);	
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
		HANDLE_MSG(hwnd, WM_KEYDOWN, OnKeyDown);
		HANDLE_MSG(hwnd, WM_KEYUP, OnKeyUp);

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
