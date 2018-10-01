//	-------------------------------------	organization	------------------------------------------
//		includes
//		global var
///			//program entry
//		DirectX global var						<-- NEW
//
//		winmain()
//			{
//			register
//			create win
//			InitDevice()
//				init DirectX (all global obj)	<-- NEW
//			message loop -> winproc
//				render()						<-- NEW
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





#define _USE_MATH_DEFINES // for C++  
#include <cmath> 
#include "ground.h"
//	defines
#define MAX_LOADSTRING 1000
#define TIMER1 111
//	global variables
HINSTANCE hInst;											//	program number = instance
TCHAR szTitle[MAX_LOADSTRING];								//	name in window title
TCHAR szWindowClass[MAX_LOADSTRING];						//	class name of window
HWND hMain = NULL;											//	number of windows = handle window = hwnd
static char MainWin[] = "MainWin";							//	class name
HBRUSH  hWinCol = CreateSolidBrush(RGB(180, 180, 180));		//	a color
HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

//	+++++++++++++++++++++++++++++++++++++++++++++++++
//	DIRECTX stuff follows here
//
//	first the global variables (DirectX Objects)
//
//	second the initdevice(..) function where we load all DX stuff
//
//	third the Render() function, the equivalent to the OnPaint(), but not from Windows, but from DirectX
//
//	lets go:

// GLOBALS:

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

// device context
ID3D11Device*           g_pd3dDevice = NULL;			// is for initialization and loading things (pictures, models, ...) <- InitDevice()
ID3D11DeviceContext*    g_pImmediateContext = NULL;		// is for render your models w/ pics on the screen					<- Render()
// page flipping:
IDXGISwapChain*         g_pSwapChain = NULL;
//screen <- thats our render target
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
//how a vertex looks like
ID3D11InputLayout*      g_pVertexLayout = NULL;
//our model (array of vertices on the GPU MEM)
ID3D11Buffer*           g_pVertexBuffer = NULL; //our model
//exchange of data, e.g. sending mouse coordinates to the GPU
ID3D11Buffer*			g_pConstantBuffer11 = NULL;

// function on the GPU what to do with the model exactly
ID3D11VertexShader*     g_pVertexShader = NULL;
ID3D11PixelShader*      g_pPixelShader = NULL;


const int NUM_VERTICES = 3;

//	structures we need later

struct VS_CONSTANT_BUFFER
	{
	float some_variable_a;
	float some_variable_b;
	float some_variable_c;
	float some_variable_d;
	};	//we will copy that periodically to its twin on the GPU, with some_variable_a/b for the mouse coordinates
//note: we can only copy chunks of 16 byte to the GPU
VS_CONSTANT_BUFFER VsConstData;		//gloab object of this structure

struct SimpleVertex
	{
	XMFLOAT3 Pos;
	};
//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
	{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hMain, &rc);	//getting the windows size into a RECT structure
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
		{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
		};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
		{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hMain;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
		{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
		}
	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
		{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
		}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
		{
		pVSBlob->Release();
		return hr;
		}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
		{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
		{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
		}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Create vertex buffer, the triangle
	SimpleVertex vertices[NUM_VERTICES];
	float param = 30.0f;
	float mpi = 3.1415926f;
	
	vertices[0].Pos = XMFLOAT3(cos(7.0*M_PI/6), sin(7.0*M_PI / 6), 0.0f);//
	vertices[1].Pos = XMFLOAT3(cos(M_PI), sin(M_PI), 0.0f);//
	vertices[2].Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);//O

	//vertices[3].Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);//O
	//vertices[4].Pos = XMFLOAT3(0.0f, 0.5f, 0.0f);//
	//vertices[5].Pos = XMFLOAT3(0.5f, 0.0f, 0.0f);//

	//start = finish;
	//finish += delta;
	//vertices[3].Pos = XMFLOAT3(0.0, 0.0f, 0.0f);//O
	//vertices[4].Pos = XMFLOAT3(cos(start), sin(start), 0.0f);//
	//vertices[5].Pos = XMFLOAT3(cos(finish), sin(finish), 0.0f);//



	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;


	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;

	bd.ByteWidth = sizeof(SimpleVertex) * NUM_VERTICES;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;	
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	
	

	// Supply the vertex shader constant data.

	VsConstData.some_variable_a = 0;
	VsConstData.some_variable_b = 0;
	VsConstData.some_variable_c = 1;
	VsConstData.some_variable_d = 1;

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &VsConstData;
	// Create the buffer.
	hr = g_pd3dDevice->CreateBuffer(&cbDesc, &InitData, &g_pConstantBuffer11);

	if (FAILED(hr))
		return hr;

	return S_OK;
	}

//--------------------------------------------------------------------------------------
// Render function
//--------------------------------------------------------------------------------------

	float ff = 0;
	void Render()
		{
	/*	
		ff += 0.001;

		VsConstData.some_variable_a = (sin(ff) + 1.) / 2.;
		VsConstData.some_variable_b = (cos(ff) + 1.) / 2.;
		VsConstData.some_variable_c = (sin(ff) + 1.) / 2.;
		VsConstData.some_variable_d = 1;*/
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer11, 0, 0, &VsConstData, 0, 0);	//copying it freshly into the GPU buffer from VsConstData
		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer11);					//setting it enable for the VertexShader
		g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer11);					//setting it enable for the PixelShader


		
		// Set primitive topology
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		
		// Clear the back buffer 
		float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f }; // red,green,blue,alpha
		g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

		// Render a triangle
		g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);

		// Set vertex buffer, setting the model
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
		g_pImmediateContext->Draw(NUM_VERTICES, 0);		//whatever you have set before as model (mesh, vertexbuffer) will be drawn here finally. First parameter is the vertex count, second the offset

												// Set vertex buffer, setting the model

		// Present the information rendered to the back buffer to the front buffer (the screen)
		g_pSwapChain->Present(0, 0);
		}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);	//message loop function (containing all switch-case statements

int WINAPI wWinMain(				//	the main function in a window program. program starts here
	HINSTANCE hInstance,			//	here the program gets its own number
	HINSTANCE hPrevInstance,		//	in case this program is called from within another program
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	hInst = hInstance;												//						save in global variable for further use
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
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);				//						Background color
	wcex.lpszMenuName = NULL;										//
	wcex.lpszClassName = L"TutorialWindowClass";									//						Name of the window (must the the same as later when opening the window)
	wcex.hIconSm = LoadIcon(wcex.hInstance, NULL);					//
	Result = (RegisterClassEx(&wcex) != 0);							//						Register this struct in the OS

				//													STEP TWO: OPENING THE WINDOW with x,y position and xlen, ylen !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	RECT rc = { 0, 0, 640, 480 };
	hMain = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 2: Rendering a Triangle",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (hMain==0)	return 0;

	ShowWindow(hMain, nCmdShow);
	UpdateWindow(hMain);


	if (FAILED(InitDevice()))
		{
		return 0;
		}

	//													STEP THREE: Going into the infinity message loop							  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Main message loop
	msg = { 0 };
	while (WM_QUIT != msg.message)
		{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		else
			{
			Render();
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
	if (x > 250)
		{
		PostQuitMessage(0);
		}
	
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

	}
//************************************************************************
///////////////////////////////////
//		This Function is called every time the window has to be painted again
///////////////////////////////////


void OnPaint(HWND hwnd)
	{

	
	}
//****************************************************************************

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
	
	PAINTSTRUCT ps;
	HDC hdc;
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
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);		// called only once when the window is created
		//HANDLE_MSG(hwnd, WM_PAINT, OnPaint);		// drawing stuff
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);	// not used
		HANDLE_MSG(hwnd, WM_KEYDOWN, OnKeyDown);	// press a keyboard key
		HANDLE_MSG(hwnd, WM_KEYUP, OnKeyUp);		// release a keyboard key
		HANDLE_MSG(hwnd, WM_TIMER, OnTimer);		// timer
	case WM_PAINT:
		hdc = BeginPaint(hMain, &ps);
		EndPaint(hMain, &ps);
	break;
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

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
	{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
		{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
		}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
	}