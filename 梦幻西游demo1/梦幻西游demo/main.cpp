#include<Windows.h>//windowsϵͳͷ�ļ�
//��ý��ӿڵ�ͷ�ļ��Ϳ��ļ�
#include <mmsystem.h>
#pragma comment (lib,"winmm.lib")



#define WND_WIDTH 800       //���ڵĿ�
#define WND_HEIGHT 600      //���ڵĸ�

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM Iparam);
//������
int WINAPI WinMain(HINSTANCE hlnstance, //��ǰʵ�����
	HINSTANCE hInstance,    //ǰʵ�����
	LPSTR IpCmdLine,        //�����е�ָ��
	int nShowCmd)            //��ʾ�ĸ�ʽͬ
{
	//WNDCLASS   window;Ҳ�Ǵ��ڵĽṹ�壬��������ǰ�İ汾�ģ�
	//���ڵĽṹ��
	WNDCLASSEX window = {0};     //window��������Ҫ��һ������

	//int sizeof (int);//sizeof��һ�������

	window.cbSize = sizeof(WNDCLASSEX);//�������ڴ�����Ҫ���ڴ��С
	window.cbClsExtra = 0;        //����ĸ����ڴ棨������û���ô��ˣ�ֱ��Ϊ0��
	window.cbWndExtra = 0;         //���ڵĸ����ڴ� ��������û���ô��ˣ�ֱ��Ϊ0��
	window.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);//�����Ļ�ˢ���ͱ�������ɫ
	//GetStockObject0--����Ԥ����ı���ˢ��
	/****************************
	#define WHITE_BRUSH         0//��ɫ
	#define LTGRAY_BRUSH        1
	#define GRAY_BRUSH          2
	#define DKGRAY_BRUSH        3
	#define BLACK_BRUSH         4
	#define NULL_BRUSH          5
	#define HOLLOW_BRUSH        NULL_BRUSH
	*****************************/
	window.hCursor = NULL;      //�ı�������ʽ,��ʽ�н�ͼʱ����ʽ������ʱ����ʽ���ȵȡ�����
	window.hIcon = NULL;        //���ͼ��
	window.hInstance = hInstance;// HINSTANCE hlnstance, //��ǰʵ�����,�൱��id
	window.style = CS_HREDRAW|CS_VREDRAW;     //���ڵĴ�С��ͨ��ˮƽ����ֱ����ı䴰�ڵĴ�С
	window.lpszMenuName = NULL;             //�Լ�ȥ����
	window.lpszClassName = L"for our dreams";
	window.lpfnWndProc = WndProc;     //���ڵĹ��̺���

	//ע�ᴰ��
	RegisterClassEx(&window);

	//��������
	//�����࣬���ڱ��⣬���ڷ���Լ����ڵĳ�ʼ��λ�ü���С����ѡ�ģ���
	HWND hwnd = CreateWindow(L"for our dreams",               //�����࣬
		L"ȫ���泬���޵е�ţ�ƺ��Ĵ���",        //���ڱ���
		WS_OVERLAPPEDWINDOW,                     //���ڷ��
		CW_USEDEFAULT,CW_USEDEFAULT,             //���ڵĳ�ʼλ��
		WND_WIDTH, WND_HEIGHT,                     //���ڵĴ�С
		NULL, NULL, hInstance, NULL);                //CW_USEDEFAULTĬ�ϵ�

	//4,��ʾ���� ���´���
	ShowWindow(hwnd, nShowCmd);      //��ʾһ������
	UpdateWindow(hwnd);

	mciSendString(L"open ./res/faded.mp3 alias music", 0, 0, 0); //�������������ַ�����Ϣ
	mciSendString(L"play music", 0, 0, 0);
	//�ȴ��û���ָ���Ϣ
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
// ���ڹ��̺�������û��Ĳ�ָͬ��������ͬ�ķ�Ӧ
// ������ţ�ָ�һ����������
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