#define VC_EXTRALEAN

#include <windows.h>
#include <commctrl.h>
#include <scrnsave.h>

#include <time.h>
#include <math.h>
#include <string>
#include <time.h>

#include "resource.h"

#include <gl\GLU.h>
#include <gl\GL.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#pragma comment(lib, "scrnsavw.lib")
#pragma comment (lib, "comctl32.lib")


bool SetupOpenGL();

void KillGL();

void DrawGLScene();


HDC					hDC = NULL;
HGLRC				hRC = NULL;
HWND				hWnd = NULL;
HINSTANCE			hInstance;

int uTimer;


LONG WINAPI ScreenSaverProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// 处理屏幕保护消息
	switch (message)
	{
	case WM_CREATE:
		// 创建屏幕保护窗口消息 最先执行
		hWnd = hwnd;
		SetupOpenGL();
		// 设置定时器 (间隔为16毫秒，为了每分钟刷新60次 (1000/60 = 16)
		uTimer = SetTimer(hwnd, 1, 16, NULL);
		return 0;

	case WM_ERASEBKGND:
		// 清除屏幕保护背景
		// opengl不使用
		return 0;


	case WM_TIMER:
		// 定时器消息
		DrawGLScene();
		return 0;


	case WM_DESTROY:
		// 销毁
		KillTimer(hwnd, uTimer);
		KillGL();
		PostQuitMessage(0);
		return 0;
	}
	return DefScreenSaverProc(hwnd, message, wparam, lparam);
}



BOOL WINAPI ScreenSaverConfigureDialog(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	return true;
}


BOOL WINAPI RegisterDialogClasses(HANDLE hmodule)
{
	return true;
}



bool SetupOpenGL()
{
	GLuint PixelFormat;	

	int width;
	int height;

	width = GetDeviceCaps(GetDC(NULL), HORZRES);
	height = GetDeviceCaps(GetDC(NULL), VERTRES);	

	RECT		WindowRect;				
	WindowRect.left = (long)0;			
	WindowRect.right = (long)width;		
	WindowRect.top = (long)0;				
	WindowRect.bottom = (long)height;		

	//设置opengl上下文
	static	PIXELFORMATDESCRIPTOR pfd =	
	{
		sizeof(PIXELFORMATDESCRIPTOR),		
		1,						
		PFD_DRAW_TO_WINDOW |					
		PFD_SUPPORT_OPENGL |					
		PFD_DOUBLEBUFFER,						
		PFD_TYPE_RGBA,							
		24,										
		0, 0, 0, 0, 0, 0,						
		0,										
		0,										
		0,											
		0, 0, 0, 0,								
		16,											  
		0,									
		0,										
		PFD_MAIN_PLANE,							
		0,										
		0, 0, 0									
	};

	if (!(hDC = GetDC(hWnd)))							
	{
		KillGL();							
		MessageBox(NULL, TEXT("Can't Create A GL Device Context."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;							
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	
	{
		KillGL();								
		MessageBox(NULL, TEXT("Can't Find A Suitable PixelFormat."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))	
	{
		KillGL();						
		MessageBox(NULL, TEXT("Can't Set The PixelFormat."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;							
	}

	if (!(hRC = wglCreateContext(hDC)))		
	{
		KillGL();							
		MessageBox(NULL, TEXT("Can't Create A GL Rendering Context."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;						
	}

	if (!wglMakeCurrent(hDC, hRC))					
	{
		KillGL();						
		MessageBox(NULL, TEXT("Can't Activate The GL Rendering Context."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;							
	}

	//透视矩阵
	if (height == 0){ height = 1; }
	glViewport(0, 0, width, height);					
	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();		
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();								

	//初始化opengl一些参数
	glShadeModel(GL_SMOOTH);							
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				
	glClearDepth(1.0f);								
	glEnable(GL_DEPTH_TEST);						
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

	return TRUE;
}


void KillGL(GLvoid){
	ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
	if (hRC){										// Do We Have A Rendering Context?
		wglMakeCurrent(NULL, NULL);					// Release The DC And RC Contexts
		wglDeleteContext(hRC);						// Delete The RC
		hRC = NULL;									// Set RC To NULL
	}
	ReleaseDC(hWnd, hDC);							// Release The DC
}



void DrawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();									
	glTranslatef(-1.5f, 0.0f, -6.0f);						
	glBegin(GL_TRIANGLES);								
	glVertex3f(0.0f, 1.0f, 0.0f);					
	glVertex3f(-1.0f, -1.0f, 0.0f);				
	glVertex3f(1.0f, -1.0f, 0.0f);					
	glEnd();											
	glTranslatef(3.0f, 0.0f, 0.0f);					
	glBegin(GL_QUADS);								
	glVertex3f(-1.0f, 1.0f, 0.0f);				
	glVertex3f(1.0f, 1.0f, 0.0f);				
	glVertex3f(1.0f, -1.0f, 0.0f);				
	glVertex3f(-1.0f, -1.0f, 0.0f);				
	glEnd();


	SwapBuffers(hDC);
}