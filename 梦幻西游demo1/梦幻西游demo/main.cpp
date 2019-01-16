#include<Windows.h>//windows系统头文件
//多媒体接口的头文件和库文件
#include <mmsystem.h>
#pragma comment (lib,"winmm.lib")



#define WND_WIDTH 800       //窗口的宽
#define WND_HEIGHT 600      //窗口的高

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM Iparam);
//主函数
int WINAPI WinMain(HINSTANCE hlnstance, //当前实例句柄
	HINSTANCE hInstance,    //前实例句柄
	LPSTR IpCmdLine,        //命令行的指针
	int nShowCmd)            //显示的格式同
{
	//WNDCLASS   window;也是窗口的结构体，但是是以前的版本的，
	//窗口的结构体
	WNDCLASSEX window = {0};     //window是我们需要的一个窗口

	//int sizeof (int);//sizeof是一个运算符

	window.cbSize = sizeof(WNDCLASSEX);//窗口在内存中需要的内存大小
	window.cbClsExtra = 0;        //程序的附加内存（基本上没有用处了，直接为0）
	window.cbWndExtra = 0;         //窗口的附加内存 （基本上没有用处了，直接为0）
	window.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);//背景的画刷，和背景的颜色
	//GetStockObject0--检锁预定义的备用刷子
	/****************************
	#define WHITE_BRUSH         0//白色
	#define LTGRAY_BRUSH        1
	#define GRAY_BRUSH          2
	#define DKGRAY_BRUSH        3
	#define BLACK_BRUSH         4
	#define NULL_BRUSH          5
	#define HOLLOW_BRUSH        NULL_BRUSH
	*****************************/
	window.hCursor = NULL;      //改变鼠标的样式,样式有截图时的样式。复制时的样式，等等。。。
	window.hIcon = NULL;        //像表，图标
	window.hInstance = hInstance;// HINSTANCE hlnstance, //当前实例句柄,相当于id
	window.style = CS_HREDRAW|CS_VREDRAW;     //窗口的大小，通过水平和竖直方向改变窗口的大小
	window.lpszMenuName = NULL;             //自己去定义
	window.lpszClassName = L"for our dreams";
	window.lpfnWndProc = WndProc;     //窗口的过程函数

	//注册窗口
	RegisterClassEx(&window);

	//创建窗口
	//窗口类，窗口标题，窗口风格，以及窗口的初始化位置及大小（可选的）。
	HWND hwnd = CreateWindow(L"for our dreams",               //窗口类，
		L"全宇宙超级无敌的牛逼哄哄的窗口",        //窗口标题
		WS_OVERLAPPEDWINDOW,                     //窗口风格
		CW_USEDEFAULT,CW_USEDEFAULT,             //窗口的初始位置
		WND_WIDTH, WND_HEIGHT,                     //窗口的大小
		NULL, NULL, hInstance, NULL);                //CW_USEDEFAULT默认的

	//4,显示窗口 更新窗口
	ShowWindow(hwnd, nShowCmd);      //显示一个窗口
	UpdateWindow(hwnd);

	mciSendString(L"open ./res/faded.mp3 alias music", 0, 0, 0); //向驱动晨发送字符串消息
	mciSendString(L"play music", 0, 0, 0);
	//等待用户的指令，消息
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}

	
	return 0;
}
// 窗口过程函数针对用户的不同指令做出不同的反应
// 点击播放（指令）一〉播放音乐
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM Iparam)
{
	switch (message)
	{
	case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hwnd, message, wparam, Iparam);
}