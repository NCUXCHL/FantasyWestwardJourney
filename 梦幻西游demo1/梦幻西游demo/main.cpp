#include<Windows.h>//windowsϵͳͷ�ļ�
//��ý��ӿڵ�ͷ�ļ��Ϳ��ļ�
#include <mmsystem.h>
#include <tchar.h>  //��ʾ�ַ�
#include <time.h>    
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Msimg32.lib")   //tranparent(͸���ģ�//ΪTransparentBlt��������׼����

#define WND_WIDTH 900       //���ڵĿ�
#define WND_HEIGHT 739      //���ڵĸ�

/*********************����ȫ�ֺ���************************/

VOID HeroAction_Logic();
VOID HeroActionPaint();
VOID GamePaint(HWND hwnd);
BOOL GameInit(HWND hwnd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM Iparam);
VOID BossActionPaint();
VOID BossAction_Logic();
VOID DieCheck(int nowhp, bool ishero);
VOID Message_Insert(wchar_t* str);

/*******************ȫ�ֱ����Ķ���***************/

enum ActionType
{
	NORMAL = 0,        //��ͨ����
	CRITICAL = 1,        //����
	MAGIC = 2,        //ħ������
	MISS = 3,        //MISS
	RECOVER = 4         //�ָ�
};

HDC g_hdc, g_memdc, g_bufdc;    //���ڵ��豸���  �ڴ��豸���
HBITMAP g_bmp;          //��λͼ
HBITMAP g_hGameOver, g_hVictory;    //��Ϸʤ�� ��Ϸ������ͼƬ
HBITMAP g_backbmp,g_skillbutton1,g_skillbutton2, g_skillbutton3,g_skillbutton4;     //����ͼƬ
HBITMAP g_hero, g_boss;
HBITMAP g_heroskill1, g_heroskill2, g_heroskill3, g_criticalStrike;
HBITMAP g_bossskill1, g_bossskill2, g_bossskill3;
HBITMAP g_recover;
int     g_bossnum = 0;     //����boss������
int     g_heronum = 0;     //����hero������
DWORD   g_tPre, g_tNow;    //��ȡ��ʱ��
BOOL    g_bCanAttack,g_bGameOver;      //�Ƿ���Թ���   ��Ϸ�Ƿ���Խ���
int     g_frame = 0;       //֡��
ActionType HeroAction, BossAction;  //ö�����͵�hero��boss�ļ���
wchar_t text[8][100];           //�洢��������ֵ��ַ�������
int g_iTextNum = 0;             //���ֵ�����
//RECT g_rect;                    //��һ������������ʾ����
        
struct 
{
	int  NowMp;      //��ǰ����
	int NowHp;       //��ǰ����Ѫ
	int MaxMp;       //������
	int MaxHp;        //������Ѫ
	int level;       //�ȼ�
	int strength;     //����
	int Intell;       //����
	int Agility;       //����
}Hero,Boss;     //����Ӣ�ۺ�boss

/**********************������******************/
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
		WS_OVERLAPPEDWINDOW,                      //���ڷ��
		CW_USEDEFAULT,CW_USEDEFAULT,             //���ڵĳ�ʼλ��
		WND_WIDTH, WND_HEIGHT,                   //���ڵĴ�С
		NULL, NULL, hInstance, NULL);             //CW_USEDEFAULTĬ�ϵ�

	//4,��ʾ���� ���´���
	ShowWindow(hwnd, nShowCmd);      //��ʾһ������
	UpdateWindow(hwnd);

	mciSendString(L"open ./res/��������.mp3 alias music", 0, 0, 0); //�������������ַ�����Ϣ
	mciSendString(L"play music", 0, 0, 0);


	//�������� �������������ݵĳ�ʼ��
	if (!GameInit(hwnd))
	{
		return -1;
	}
	//�ȴ��û���ָ���Ϣ
	MSG msg = { 0 };//����һ����Ϣ
	while (msg.message != WM_QUIT)//ѭ��������ʲô������˳�������Ҽ��˳� alt+f4 �˳���ָ��
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_tNow = GetTickCount();  //2018 1 9 22 26 20 10
			//GetTickCount�����������ڻ�ȡ���ڵ�ʱ�䣨��ȷ�����룩���� 2018 1 23 21 03 10  
			if (g_tNow - g_tPre >= 80)    //80msһ��  һ֡ͼ�� g_frame=0   //��x֡--��x1֡
			{
				GamePaint(hwnd);//�ڲ��ϵ���ͼ��
			}
		}

	}
	return 0;
}
/********************��������******************************/

// ���ڹ��̺�������û��Ĳ�ָͬ��������ͬ�ķ�Ӧ
// ������ţ�ָ�һ����������
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM Iparam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN: //����������
		if (!g_bCanAttack)
		{
			int x = LOWORD(Iparam);
			int y = HIWORD(Iparam);

			if (x >= 600 && x <= 644 && y >=350  && y <= 395)
			{
				g_bCanAttack = true;
				HeroAction = RECOVER;

			}
			else if (x >= 550 && x <= 594 && y >= 350 && y <= 395)
			{
				g_bCanAttack = true;
				HeroAction = MAGIC;
			}
			else if (x >= 500 && x <= 540 && y >= 350 && y <= 390)
			{
				g_bCanAttack = true;
				HeroAction = NORMAL;
			}
		}
		break;

	case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hwnd, message, wparam, Iparam);
}

BOOL GameInit(HWND hwnd)
{
	//����
	srand((unsigned int)time(NULL));
	g_hdc = GetDC(hwnd);
	g_memdc = CreateCompatibleDC(g_hdc); //Create Compatible DC
	g_bufdc = CreateCompatibleDC(g_hdc);
	g_bmp = CreateCompatibleBitmap(g_hdc, WND_WIDTH, WND_HEIGHT);


	//�ز�
	g_backbmp = (HBITMAP)LoadImage(NULL, L"./res/����ͼ.bmp", IMAGE_BITMAP, 1024, 739, LR_LOADFROMFILE);
	g_skillbutton1 = (HBITMAP)LoadImage(NULL, L"./res/jiNeng1.bmp", IMAGE_BITMAP, 44, 45, LR_LOADFROMFILE);
	g_skillbutton2 = (HBITMAP)LoadImage(NULL, L"./res/jiNeng2.bmp", IMAGE_BITMAP, 44, 45, LR_LOADFROMFILE);
	g_skillbutton3 = (HBITMAP)LoadImage(NULL, L"./res/jiNeng3.bmp", IMAGE_BITMAP, 44, 45, LR_LOADFROMFILE);
	g_skillbutton4 = (HBITMAP)LoadImage(NULL, L"./res/jiNeng4.bmp", IMAGE_BITMAP, 44, 45, LR_LOADFROMFILE);

	g_hero = (HBITMAP)LoadImage(NULL, L"./res/hero.bmp", IMAGE_BITMAP, 542, 88, LR_LOADFROMFILE);
	g_boss = (HBITMAP)LoadImage(NULL, L"./res/boss.bmp", IMAGE_BITMAP, 542, 88, LR_LOADFROMFILE);

	g_heroskill1 = (HBITMAP)LoadImage(NULL, L"./res/heroSkill1.bmp", IMAGE_BITMAP, 142, 142, LR_LOADFROMFILE);//��ͨ����
	g_heroskill2 = (HBITMAP)LoadImage(NULL, L"./res/heroSkill2.bmp", IMAGE_BITMAP, 141, 142, LR_LOADFROMFILE);//
	g_heroskill3 = (HBITMAP)LoadImage(NULL, L"./res/heroSkill3.bmp", IMAGE_BITMAP, 141, 111, LR_LOADFROMFILE);//ħ������
	g_criticalStrike = (HBITMAP)LoadImage(NULL, L"./res/hero-criticalStrike.bmp", IMAGE_BITMAP, 148, 133, LR_LOADFROMFILE);//����

	g_bossskill1 = (HBITMAP)LoadImage(NULL, L"./res/bossSkill1.bmp", IMAGE_BITMAP, 106, 78, LR_LOADFROMFILE);//��ͨ����
	g_bossskill2 = (HBITMAP)LoadImage(NULL, L"./res/bossSkill2.bmp", IMAGE_BITMAP, 105, 70, LR_LOADFROMFILE);//ħ������//����
	g_bossskill3 = (HBITMAP)LoadImage(NULL, L"./res/boss-criticalStrike.bmp", IMAGE_BITMAP, 146, 74, LR_LOADFROMFILE);

	g_hGameOver = (HBITMAP)LoadImage(NULL, L"./res/gameOver.bmp", IMAGE_BITMAP, 1373, 523, LR_LOADFROMFILE);//��Ϸ������ͼƬ
	g_hVictory = (HBITMAP)LoadImage(NULL, L"./res/victory.bmp", IMAGE_BITMAP, 642, 484, LR_LOADFROMFILE);//Ӣ��ʤ����ͼƬ
	g_recover = (HBITMAP)LoadImage(NULL, L"./res/recover.bmp", IMAGE_BITMAP, 200, 198, LR_LOADFROMFILE);
		
	/*
		LoadImageW(
		_In_opt_HINSTANCE hInst,
		_In_ LPCWSTR name,
		_In_UINT type,
		_In_ int ex,
		_In_int cy,
		_In_ UINT fuLoad),
		*/
		/*
		���ߣ�g_hdc g_memdc g_hmp
		�زģ�g_backbmp
		���裺
		      1��bmp-->memdc
			  2, backbmp-->memdc
			  3, memdc-->hdc
         */

	//����
	//1��g_bmp --> memdc(��λͼ-->�ڴ��豸�� 
	SelectObject(g_memdc, g_bmp);    //�յ�λͼѡ��memdc

	//��ʼ��hero������ֵ
	Hero.NowMp = 60;           //��ǰ����
	Hero.NowHp = 1200;         //��ǰ����Ѫ
	Hero.MaxMp = 60;           //������
	Hero.MaxHp = 1200;         //������Ѫ
	Hero.level = 6;            //�ȼ�
	Hero.strength = 10;        //����
	Hero.Intell = 10;          //����
	Hero.Agility = 20;         //����
	

	//��ʼ��Boss������ֵ
	Boss.NowHp = 2000;           //��ǰ����Ѫ
	Boss.MaxHp = 2000;         //������Ѫ
	Boss.level = 6;            //�ȼ�
	Boss.strength = 10;        //����
	Boss.Intell = 10;          //����
	Boss.Agility = 20;         //����

	//GetClientRect(hwnd, &g_rect);��ȡһ�����εĺ���


	HFONT hfont;
hfont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, L"΢���ź�");//��������Ĵ�С����ʽ  700��ʹ�����֣�20������Ĵ�С��
SelectObject(g_memdc, hfont);
SetBkMode(g_memdc, 0);//ʹ1000/1000͸����

return TRUE;
}

//���ƺ���
VOID GamePaint(HWND hwnd)
{
	wchar_t str[100];  //����Ѫ��
	//������ͼƬ
	//2.backbmp --> bufdc
	SelectObject(g_bufdc, g_backbmp);  //ͼƬѡ��memdc

	//3.bufdc-->memdc   �豸          bmp backbmp
	//λͼ-->�豸 selectobject 
	//�豸���豸 BitBLt
	BitBlt(g_memdc,            //Ŀ���豸
		0, 0, 1024, 739,       //Ŀ���豸�ķ�Χ
		g_bufdc,               //Դ�豸
		0, 0,                  //Դ�豸�е���ʼֵ  
		SRCCOPY);              //����

	//��ʾս����Ϣ
	SetTextColor(g_memdc, RGB(0, 255, 0));
	for (int i = 0; i < g_iTextNum; i++)
	{
		TextOut(g_memdc, 20, 410 + i * 18, text[i], wcslen(text[i]));
	}

	//��BOSS
	if (Boss.NowHp > 0)      //���boss����Ѫ
	{
		SelectObject(g_bufdc, g_boss);
		TransparentBlt(g_memdc, 200, 350, 78, 88, g_bufdc, g_bossnum * 68, 0, 68, 88, RGB(0, 0, 0));
		if (++g_bossnum >= 7)
			g_bossnum = 0;

		//��ʾѪ��
		swprintf_s(str, L"%d/%d", Boss.NowHp, Boss.MaxHp);   //     1000/1000
		SetTextColor(g_memdc, RGB(255, 0, 0));            //���ã�ʹ������ɫ���
		TextOut(g_memdc, 200, 445, str, wcslen(str));
	}

	if (Hero.NowHp > 0)
	{
		//��Ӣ��
		//2.backbmp-->bufdc
		//3, bufdc -->memdc
		SelectObject(g_bufdc, g_hero);
		//BitBlt(g_memdc, 200, 350, 90, 99, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 450, 450, 78, 88, g_bufdc, g_heronum * 68, 0, 68, 88, RGB(0, 0, 0));
		//Sleep(100);
		if (++g_heronum >= 7)
			g_heronum = 0;

		//��ʾѪ��
		swprintf_s(str, L"%d/%d", Hero.NowHp, Hero.MaxHp);   //     1000/1000
		SetTextColor(g_memdc, RGB(255, 0, 0));               //���ã�ʹ������ɫ���
		TextOut(g_memdc, 460, 540, str, wcslen(str));        //480��550���������ڵ�λ��

		//��ʾ��
		swprintf_s(str, L"%d/%d", Hero.NowMp, Hero.MaxMp);   //     1000/1000
		SetTextColor(g_memdc, RGB(0, 0, 255));               //���ã�ʹ������ɫ���
		TextOut(g_memdc, 460, 560, str, wcslen(str));        //480��550���������ڵ�λ�ã����꣩
	}

	if (g_bGameOver)
	{
		if (Hero.NowHp <= 0)
		{
			//��ͼ��Ϸ������ͼƬ
			SelectObject(g_bufdc, g_hGameOver);
			//͸�����ַ�
			BitBlt(g_memdc, 45, 45, 686, 523, g_bufdc, 686, 0, SRCAND);
			BitBlt(g_memdc, 45, 45, 686, 523, g_bufdc, 0, 0, SRCPAINT);
			//TransparentBlt(g_memdc, 45, 45, 686, 523, g_bufdc, 0, 0, 686, 523, RGB(0, 0, 0));

			/*
				1.͸�����ַ�

				2.ɫ�ʷ������õ� Ч�����ã��������бߣ�
				TransparentBlt
				*/
		}
		else
		{
			//��ͼս��ʤ����ͼ��
			SelectObject(g_bufdc, g_hVictory);
			//BitBlt(g_memdc, 230, 250, 642, 484, g_bufdc, 0, 0, SRCCOPY);
			TransparentBlt(g_memdc, 230, 250, 642, 464, g_bufdc, 0, 0, 642, 464, RGB(0, 0, 0));
		}
	} else if (!g_bCanAttack)                                       //�Ƿ���Թ���
	{
		//����һ������ͼ��
		//2.backbmp-->bufdc
		//3, bufdc -->memdc
		SelectObject(g_bufdc, g_skillbutton1);
		BitBlt(g_memdc, 500, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);

		//���ڶ�������ͼ��
		//2.backbmp-->bufdc
		//3, bufdc -->memdc
		SelectObject(g_bufdc, g_skillbutton2);
		BitBlt(g_memdc, 550, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);

		//������������ͼ��
		//2.backbmp-->bufdc
		//3, bufdc -->memdc
		SelectObject(g_bufdc, g_skillbutton3);
		BitBlt(g_memdc, 600, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);

		//�����ĸ�����ͼ��
		//2.backbmp-->bufdc
		//3, bufdc -->memdc
		SelectObject(g_bufdc, g_skillbutton4);
		BitBlt(g_memdc, 650, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);
	}
	else
	{
		g_frame++;     //��Ϣѭ�� ÿһ��ѭ��������೤ʱ�䣩����һ֡
		if (g_frame >= 5 && g_frame <= 10) //5-10
		{
			if (g_frame == 5)
			{
				//���ϼ���ͼ  ��֪�� ����ļ���
				HeroAction_Logic();
				DieCheck(Boss.NowHp, false);
			}
			//���ݲ�ͬ�Ķ�������ͬ��ͼƬ
			HeroActionPaint();
		}

		if (g_frame == 15)
		{
			//������߼�
			BossAction_Logic();
		}

		//��ʾ����Ĺ���ͼƬ
		if (g_frame >= 16 && g_frame <= 30)
		{
			BossActionPaint();//��ʾ
		}

		//��30֡�غϽ���
		if (g_frame == 30)
		{
			g_bCanAttack = false;   // hero �� �� �� ��
			g_frame = 0;            // 30 * 80 = 2400ms (����) = 2.4s

			int MpRecover;          //��һ���غ���Ҫ�ָ�����
			MpRecover = 2 * rand() % Hero.Intell + 6;    //0 - 9
			Hero.NowMp += MpRecover;   //60
			if (Hero.NowMp >= Hero.MaxMp)
				Hero.NowMp = Hero.MaxMp;
		}
	}
	//4,memdc --> hdc
	BitBlt(g_hdc, 0, 0, 1024, 739, g_memdc, 0, 0, SRCCOPY);
	g_tPre = GetTickCount();  //���ڻ�ȡ���ڵ�ʱ�䣨��ȷ�����룩���� 2018 1 23 21 03 10  
}

//��ҵ��߼�����
VOID HeroAction_Logic()
{
	wchar_t str[100];
	int damage = 0;//�˺�ֵ

	switch (HeroAction)
	{
	case NORMAL:
		//����
		if (rand() % 4 == 1)         //��������25%
		{
			HeroAction = CRITICAL;
			damage = (int)4.5 * (3 * (rand() % Hero.Agility) + Hero.level*Hero.strength + 20);
			Boss.NowHp -= damage;
			swprintf_s(str, L"����� �ͷ��Ĺ�...��ħ��ʥʹ  ���[%d]�˺���Сcase", damage);
			Message_Insert(str);
		}
		//�����ͨ����
		else
		{
			damage = 3 * (rand() % Hero.Agility) + Hero.level*Hero.strength + 20;
			Boss.NowHp -= damage;
			swprintf_s(str, L"����� �ͷ���ڤ���...��ħ��ʥʹ  ���[%d]�˺���Сcase", damage);
			Message_Insert(str);
		}
		break;
	case MAGIC:    //������ 30
		if (Hero.NowMp >= 30)
		{
			damage = 5 * (2 * (rand() % Hero.Agility) + Hero.level*Hero.Intell);
			Boss.NowHp -= damage;
			swprintf_s(str, L"����� ����ħ���ͷ�С��...��ħ��ʥʹ  ���[%d]�˺���Сcase", damage);
			Message_Insert(str);
			Hero.NowMp -= 30;
		}
		else
		{
			HeroAction = MISS;
		}
		break;
	case RECOVER://������ 40
		if (Hero.NowMp >= 40)
		{
			Hero.NowMp -= 40;
			int HpRecover = 5 * (5 * (rand() % Hero.Intell) + 40);    //200 540 rand()%Hero.Intell 0 - 19
			Hero.NowHp += HpRecover;
			if (Hero.NowHp >= Hero.MaxHp)
				Hero.NowHp = Hero.MaxHp;
		}
		else
		{
			HeroAction = MISS;
		}
		break;
	}
}

//����Ӣ�۵ļ���
VOID HeroActionPaint()
{
	switch (HeroAction)
	{
	case NORMAL:
		SelectObject(g_bufdc, g_heroskill1);
		//BitBlt(g_memdc, 500, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 200, 350, 141, 141, g_bufdc, 0, 0, 141, 141, RGB(0, 0, 0));
		break;
	case CRITICAL:
		SelectObject(g_bufdc, g_criticalStrike);
		//BitBlt(g_memdc, 550, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 200, 310, 148, 133, g_bufdc, 0, 0, 148, 133, RGB(0, 0, 0));
		break;
	case MAGIC:
		SelectObject(g_bufdc, g_heroskill2);
		//BitBlt(g_memdc, 550, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 200, 350, 141, 141, g_bufdc, 0, 0, 141, 141, RGB(0, 0, 0));
		break;
	case RECOVER:
		SelectObject(g_bufdc, g_recover);
		//BitBlt(g_memdc, 550, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 400, 400, 200, 198, g_bufdc, 0, 0, 200, 198, RGB(0, 0, 0));
		break;
	}
}

//Boss���߼�����
VOID BossAction_Logic()
{
	srand((unsigned int)time(NULL));

	if (Boss.NowHp > (Boss.MaxHp / 2))//Ѫ���Ķ�����������ʲô���ܣ���Ѫ���Ƚϳ���
	{
		switch (rand() % 2)
		{
		case 0:
			BossAction = NORMAL;
			break;
		case 1:
			BossAction = CRITICAL;
			break;
		case 2:
			BossAction = MAGIC;
			break;
		}
	}
	else     //��Ѫ�Ƚϵ� ���� ��
	{
		switch (rand() % 2)
		{
		case 0:
			BossAction = CRITICAL;
			break;
		case 1:
			BossAction = MAGIC;
			break;
		case 2:
			BossAction = RECOVER;
			break;
		}
	}
}

//����Boss�ļ���
VOID BossActionPaint()
{
	wchar_t str[100];
	int damage = 0;   //80ms  ��Ϣѭ��ÿһ֡ 80������ 
	int recover = 0;
	switch (BossAction)
	{
	case NORMAL:
		SelectObject(g_bufdc, g_bossskill1);
		//BitBlt(g_memdc, 500, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 450, 450, 106, 78, g_bufdc, 0, 0, 106, 78, RGB(0, 0, 0));
		if (g_frame == 30)
		{
			damage = rand() % Boss.Agility + Boss.level*Boss.strength;
			Hero.NowHp -= damage;
			DieCheck(Hero.NowHp, true);
			swprintf_s(str, L"ħ��ʥʹ �ͷ���ڤ���...�� ����� ���[%d]�˺���Сcase",damage);
			Message_Insert(str);
		}
		break;
	case CRITICAL:
		SelectObject(g_bufdc, g_bossskill3);
		//BitBlt(g_memdc, 550, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 450, 450, 146, 74, g_bufdc, 0, 0, 146, 74, RGB(0, 0, 0));
		if (g_frame == 30)
		{
			damage = 2 * (rand() % Boss.Agility) + Boss.level*Boss.strength;
			Hero.NowHp -= damage;
			swprintf_s(str, L"ħ��ʥʹ �ͷŷ蹷...�� ����� ���[%d]�˺���Сcase", damage);
			Message_Insert(str);
			DieCheck(Hero.NowHp, true);
		}
		break;
	case MAGIC:
		SelectObject(g_bufdc, g_bossskill2);
		//BitBlt(g_memdc, 550, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 450, 450, 105, 70, g_bufdc, 0, 0, 105, 70, RGB(0, 0, 0));
		if (g_frame == 30)
		{
			damage = 2 * (2 * (rand() % Boss.Agility) + Boss.Intell*Boss.strength);
			Hero.NowHp -= damage;
			swprintf_s(str, L"ħ��ʥʹ ����ħ���ͷ�ƨƨ...�� ����� ���[%d]�˺���Сcase", damage);
			Message_Insert(str);

			//BOSS�Դ� ������Ѫ
			recover = (int)(damage*0.2);
			Boss.NowHp += recover;
		}
		break;
	case RECOVER:
		SelectObject(g_bufdc, g_recover);
		//BitBlt(g_memdc, 550, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 200, 350, 200, 198, g_bufdc, 0, 0, 200, 198, RGB(0, 0, 0));
		if (g_frame == 30)
		{
			recover = 2*Boss.Intell*Boss.Intell;
			Hero.NowHp += recover;
		}
		break;
	}
}

//�ж�����Ƿ�����
VOID DieCheck(int nowhp, bool ishero)
{
	wchar_t str[100];
	if (nowhp <= 0)  //����ֵС��0
	{
		g_bGameOver = true;   //��Ϸ����
		if (ishero)   //��Ӣ��
		{
			//����ʧ��ʱ������
			//PlaySound(L"./res/ʧ�ܵ�ϲ��.mp3", NULL, SND_FILENAME | SND_ASYNC);
			mciSendString(L"open ./res/ʧ�ܵ�ϲ��.mp3 alias music", 0, 0, 0); //�������������ַ�����Ϣ
			mciSendString(L"play music", 0, 0, 0);
			//PlaySound(L"./res/ʧ�ܵ�ϲ��.wav", NULL, SND_FILENAME | SND_ASYNC);
			swprintf_s(str, L"ʤ���˱��ҳ��£������벻Ҫ����...");
			Message_Insert(str);
		}
		else
		{
			//����ʤ��ʱ������
			PlaySound(L"./res/ʤ����ϲ��.wav", NULL, SND_FILENAME | SND_ASYNC);
			//mciSendString(L"open ./res/ʧ�ܵ�ϲ��.mp3 alias music", 0, 0, 0); //�������������ַ�����Ϣ
			//mciSendString(L"play music", 0, 0, 0);
			swprintf_s(str, L"����������ǰ�ˣ������书�����������޵У�����");
			Message_Insert(str);
		}
	}
}
VOID Message_Insert(wchar_t* str)
{
	if (g_iTextNum < 8)
	{
		swprintf_s(text[g_iTextNum], str);
		g_iTextNum++;
	}
	else
	{
		for (int i = 1; i < g_iTextNum; i++)
		{
			swprintf_s(text[i-1], text[i]);
		}
		swprintf_s(text[g_iTextNum-1], str);
	}
}