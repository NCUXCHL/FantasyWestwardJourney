#include<Windows.h>//windows系统头文件
//多媒体接口的头文件和库文件
#include <mmsystem.h>
#include <tchar.h>  //显示字符
#include <time.h>    
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Msimg32.lib")   //tranparent(透明的）//为TransparentBlt（）函数准备的

#define WND_WIDTH 900       //窗口的宽
#define WND_HEIGHT 739      //窗口的高

/*********************声明全局函数************************/

VOID HeroAction_Logic();
VOID HeroActionPaint();
VOID GamePaint(HWND hwnd);
BOOL GameInit(HWND hwnd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM Iparam);
VOID BossActionPaint();
VOID BossAction_Logic();
VOID DieCheck(int nowhp, bool ishero);
VOID Message_Insert(wchar_t* str);

/*******************全局变量的定义***************/

enum ActionType
{
	NORMAL = 0,        //普通攻击
	CRITICAL = 1,        //暴击
	MAGIC = 2,        //魔法攻击
	MISS = 3,        //MISS
	RECOVER = 4         //恢复
};

HDC g_hdc, g_memdc, g_bufdc;    //窗口的设备句柄  内存设备句柄
HBITMAP g_bmp;          //空位图
HBITMAP g_hGameOver, g_hVictory;    //游戏胜利 游戏结束的图片
HBITMAP g_backbmp,g_skillbutton1,g_skillbutton2, g_skillbutton3,g_skillbutton4;     //背景图片
HBITMAP g_hero, g_boss;
HBITMAP g_heroskill1, g_heroskill2, g_heroskill3, g_criticalStrike;
HBITMAP g_bossskill1, g_bossskill2, g_bossskill3;
HBITMAP g_recover;
int     g_bossnum = 0;     //代表boss的数量
int     g_heronum = 0;     //代表hero的数量
DWORD   g_tPre, g_tNow;    //获取的时间
BOOL    g_bCanAttack,g_bGameOver;      //是否可以攻击   游戏是否可以结束
int     g_frame = 0;       //帧数
ActionType HeroAction, BossAction;  //枚举类型的hero和boss的技能
wchar_t text[8][100];           //存储着输出文字的字符串数组
int g_iTextNum = 0;             //文字的字数
//RECT g_rect;                    //做一个矩形区域显示文字
        
struct 
{
	int  NowMp;      //当前的蓝
	int NowHp;       //当前的气血
	int MaxMp;       //最大的蓝
	int MaxHp;        //最大的气血
	int level;       //等级
	int strength;     //力量
	int Intell;       //智力
	int Agility;       //敏捷
}Hero,Boss;     //定义英雄和boss

/**********************主函数******************/
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
		WS_OVERLAPPEDWINDOW,                      //窗口风格
		CW_USEDEFAULT,CW_USEDEFAULT,             //窗口的初始位置
		WND_WIDTH, WND_HEIGHT,                   //窗口的大小
		NULL, NULL, hInstance, NULL);             //CW_USEDEFAULT默认的

	//4,显示窗口 更新窗口
	ShowWindow(hwnd, nShowCmd);      //显示一个窗口
	UpdateWindow(hwnd);

	mciSendString(L"open ./res/背景音乐.mp3 alias music", 0, 0, 0); //向驱动晨发送字符串消息
	mciSendString(L"play music", 0, 0, 0);


	//主函数中 对整个程序数据的初始化
	if (!GameInit(hwnd))
	{
		return -1;
	}
	//等待用户的指令，消息
	MSG msg = { 0 };//定义一个消息
	while (msg.message != WM_QUIT)//循环的条件什么情况下退出，点击右键退出 alt+f4 退出的指令
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_tNow = GetTickCount();  //2018 1 9 22 26 20 10
			//GetTickCount（）函数用于获取现在的时间（精确到毫秒）例如 2018 1 23 21 03 10  
			if (g_tNow - g_tPre >= 80)    //80ms一次  一帧图像 g_frame=0   //第x帧--第x1帧
			{
				GamePaint(hwnd);//在不断的贴图，
			}
		}

	}
	return 0;
}
/********************主函数底******************************/

// 窗口过程函数针对用户的不同指令做出不同的反应
// 点击播放（指令）一〉播放音乐
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM Iparam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN: //鼠标左键按下
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
	//工具
	srand((unsigned int)time(NULL));
	g_hdc = GetDC(hwnd);
	g_memdc = CreateCompatibleDC(g_hdc); //Create Compatible DC
	g_bufdc = CreateCompatibleDC(g_hdc);
	g_bmp = CreateCompatibleBitmap(g_hdc, WND_WIDTH, WND_HEIGHT);


	//素材
	g_backbmp = (HBITMAP)LoadImage(NULL, L"./res/背景图.bmp", IMAGE_BITMAP, 1024, 739, LR_LOADFROMFILE);
	g_skillbutton1 = (HBITMAP)LoadImage(NULL, L"./res/jiNeng1.bmp", IMAGE_BITMAP, 44, 45, LR_LOADFROMFILE);
	g_skillbutton2 = (HBITMAP)LoadImage(NULL, L"./res/jiNeng2.bmp", IMAGE_BITMAP, 44, 45, LR_LOADFROMFILE);
	g_skillbutton3 = (HBITMAP)LoadImage(NULL, L"./res/jiNeng3.bmp", IMAGE_BITMAP, 44, 45, LR_LOADFROMFILE);
	g_skillbutton4 = (HBITMAP)LoadImage(NULL, L"./res/jiNeng4.bmp", IMAGE_BITMAP, 44, 45, LR_LOADFROMFILE);

	g_hero = (HBITMAP)LoadImage(NULL, L"./res/hero.bmp", IMAGE_BITMAP, 542, 88, LR_LOADFROMFILE);
	g_boss = (HBITMAP)LoadImage(NULL, L"./res/boss.bmp", IMAGE_BITMAP, 542, 88, LR_LOADFROMFILE);

	g_heroskill1 = (HBITMAP)LoadImage(NULL, L"./res/heroSkill1.bmp", IMAGE_BITMAP, 142, 142, LR_LOADFROMFILE);//普通攻击
	g_heroskill2 = (HBITMAP)LoadImage(NULL, L"./res/heroSkill2.bmp", IMAGE_BITMAP, 141, 142, LR_LOADFROMFILE);//
	g_heroskill3 = (HBITMAP)LoadImage(NULL, L"./res/heroSkill3.bmp", IMAGE_BITMAP, 141, 111, LR_LOADFROMFILE);//魔法攻击
	g_criticalStrike = (HBITMAP)LoadImage(NULL, L"./res/hero-criticalStrike.bmp", IMAGE_BITMAP, 148, 133, LR_LOADFROMFILE);//暴击

	g_bossskill1 = (HBITMAP)LoadImage(NULL, L"./res/bossSkill1.bmp", IMAGE_BITMAP, 106, 78, LR_LOADFROMFILE);//普通攻击
	g_bossskill2 = (HBITMAP)LoadImage(NULL, L"./res/bossSkill2.bmp", IMAGE_BITMAP, 105, 70, LR_LOADFROMFILE);//魔法攻击//暴击
	g_bossskill3 = (HBITMAP)LoadImage(NULL, L"./res/boss-criticalStrike.bmp", IMAGE_BITMAP, 146, 74, LR_LOADFROMFILE);

	g_hGameOver = (HBITMAP)LoadImage(NULL, L"./res/gameOver.bmp", IMAGE_BITMAP, 1373, 523, LR_LOADFROMFILE);//游戏结束的图片
	g_hVictory = (HBITMAP)LoadImage(NULL, L"./res/victory.bmp", IMAGE_BITMAP, 642, 484, LR_LOADFROMFILE);//英雄胜利的图片
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
		工具：g_hdc g_memdc g_hmp
		素材：g_backbmp
		步骤：
		      1，bmp-->memdc
			  2, backbmp-->memdc
			  3, memdc-->hdc
         */

	//步骤
	//1，g_bmp --> memdc(空位图-->内存设备） 
	SelectObject(g_memdc, g_bmp);    //空的位图选入memdc

	//初始化hero的属性值
	Hero.NowMp = 60;           //当前的蓝
	Hero.NowHp = 1200;         //当前的气血
	Hero.MaxMp = 60;           //最大的蓝
	Hero.MaxHp = 1200;         //最大的气血
	Hero.level = 6;            //等级
	Hero.strength = 10;        //力量
	Hero.Intell = 10;          //智力
	Hero.Agility = 20;         //敏捷
	

	//初始化Boss的属性值
	Boss.NowHp = 2000;           //当前的气血
	Boss.MaxHp = 2000;         //最大的气血
	Boss.level = 6;            //等级
	Boss.strength = 10;        //力量
	Boss.Intell = 10;          //智力
	Boss.Agility = 20;         //敏捷

	//GetClientRect(hwnd, &g_rect);获取一个矩形的函数


	HFONT hfont;
hfont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, L"微软雅黑");//设置字体的大小和样式  700是使字体变粗，20是字体的大小，
SelectObject(g_memdc, hfont);
SetBkMode(g_memdc, 0);//使1000/1000透明化

return TRUE;
}

//绘制函数
VOID GamePaint(HWND hwnd)
{
	wchar_t str[100];  //计量血量
	//贴背景图片
	//2.backbmp --> bufdc
	SelectObject(g_bufdc, g_backbmp);  //图片选入memdc

	//3.bufdc-->memdc   设备          bmp backbmp
	//位图-->设备 selectobject 
	//设备到设备 BitBLt
	BitBlt(g_memdc,            //目标设备
		0, 0, 1024, 739,       //目标设备的范围
		g_bufdc,               //源设备
		0, 0,                  //源设备中的起始值  
		SRCCOPY);              //拷贝

	//显示战斗信息
	SetTextColor(g_memdc, RGB(0, 255, 0));
	for (int i = 0; i < g_iTextNum; i++)
	{
		TextOut(g_memdc, 20, 410 + i * 18, text[i], wcslen(text[i]));
	}

	//贴BOSS
	if (Boss.NowHp > 0)      //如果boss还有血
	{
		SelectObject(g_bufdc, g_boss);
		TransparentBlt(g_memdc, 200, 350, 78, 88, g_bufdc, g_bossnum * 68, 0, 68, 88, RGB(0, 0, 0));
		if (++g_bossnum >= 7)
			g_bossnum = 0;

		//显示血量
		swprintf_s(str, L"%d/%d", Boss.NowHp, Boss.MaxHp);   //     1000/1000
		SetTextColor(g_memdc, RGB(255, 0, 0));            //作用：使字体颜色变红
		TextOut(g_memdc, 200, 445, str, wcslen(str));
	}

	if (Hero.NowHp > 0)
	{
		//贴英雄
		//2.backbmp-->bufdc
		//3, bufdc -->memdc
		SelectObject(g_bufdc, g_hero);
		//BitBlt(g_memdc, 200, 350, 90, 99, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 450, 450, 78, 88, g_bufdc, g_heronum * 68, 0, 68, 88, RGB(0, 0, 0));
		//Sleep(100);
		if (++g_heronum >= 7)
			g_heronum = 0;

		//显示血量
		swprintf_s(str, L"%d/%d", Hero.NowHp, Hero.MaxHp);   //     1000/1000
		SetTextColor(g_memdc, RGB(255, 0, 0));               //作用：使字体颜色变红
		TextOut(g_memdc, 460, 540, str, wcslen(str));        //480和550是字体所在的位置

		//显示蓝
		swprintf_s(str, L"%d/%d", Hero.NowMp, Hero.MaxMp);   //     1000/1000
		SetTextColor(g_memdc, RGB(0, 0, 255));               //作用：使字体颜色变红
		TextOut(g_memdc, 460, 560, str, wcslen(str));        //480和550是字体所在的位置（坐标）
	}

	if (g_bGameOver)
	{
		if (Hero.NowHp <= 0)
		{
			//贴图游戏结束的图片
			SelectObject(g_bufdc, g_hGameOver);
			//透明遮罩法
			BitBlt(g_memdc, 45, 45, 686, 523, g_bufdc, 686, 0, SRCAND);
			BitBlt(g_memdc, 45, 45, 686, 523, g_bufdc, 0, 0, SRCPAINT);
			//TransparentBlt(g_memdc, 45, 45, 686, 523, g_bufdc, 0, 0, 686, 523, RGB(0, 0, 0));

			/*
				1.透明遮罩法

				2.色彩法（不用的 效果不好！！！）有边，
				TransparentBlt
				*/
		}
		else
		{
			//贴图战斗胜利的图像
			SelectObject(g_bufdc, g_hVictory);
			//BitBlt(g_memdc, 230, 250, 642, 484, g_bufdc, 0, 0, SRCCOPY);
			TransparentBlt(g_memdc, 230, 250, 642, 464, g_bufdc, 0, 0, 642, 464, RGB(0, 0, 0));
		}
	} else if (!g_bCanAttack)                                       //是否可以攻击
	{
		//贴第一个技能图标
		//2.backbmp-->bufdc
		//3, bufdc -->memdc
		SelectObject(g_bufdc, g_skillbutton1);
		BitBlt(g_memdc, 500, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);

		//贴第二个技能图标
		//2.backbmp-->bufdc
		//3, bufdc -->memdc
		SelectObject(g_bufdc, g_skillbutton2);
		BitBlt(g_memdc, 550, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);

		//贴第三个技能图标
		//2.backbmp-->bufdc
		//3, bufdc -->memdc
		SelectObject(g_bufdc, g_skillbutton3);
		BitBlt(g_memdc, 600, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);

		//贴第四个技能图标
		//2.backbmp-->bufdc
		//3, bufdc -->memdc
		SelectObject(g_bufdc, g_skillbutton4);
		BitBlt(g_memdc, 650, 350, 40, 40, g_bufdc, 0, 0, SRCCOPY);
	}
	else
	{
		g_frame++;     //消息循环 每一次循环（间隔多长时间）就是一帧
		if (g_frame >= 5 && g_frame <= 10) //5-10
		{
			if (g_frame == 5)
			{
				//贴上技能图  不知道 点击的技能
				HeroAction_Logic();
				DieCheck(Boss.NowHp, false);
			}
			//根据不同的动作贴不同的图片
			HeroActionPaint();
		}

		if (g_frame == 15)
		{
			//怪物的逻辑
			BossAction_Logic();
		}

		//显示怪物的攻击图片
		if (g_frame >= 16 && g_frame <= 30)
		{
			BossActionPaint();//显示
		}

		//第30帧回合结束
		if (g_frame == 30)
		{
			g_bCanAttack = false;   // hero 不 能 攻 击
			g_frame = 0;            // 30 * 80 = 2400ms (毫秒) = 2.4s

			int MpRecover;          //这一个回合需要恢复的蓝
			MpRecover = 2 * rand() % Hero.Intell + 6;    //0 - 9
			Hero.NowMp += MpRecover;   //60
			if (Hero.NowMp >= Hero.MaxMp)
				Hero.NowMp = Hero.MaxMp;
		}
	}
	//4,memdc --> hdc
	BitBlt(g_hdc, 0, 0, 1024, 739, g_memdc, 0, 0, SRCCOPY);
	g_tPre = GetTickCount();  //用于获取现在的时间（精确到毫秒）例如 2018 1 23 21 03 10  
}

//玩家的逻辑代码
VOID HeroAction_Logic()
{
	wchar_t str[100];
	int damage = 0;//伤害值

	switch (HeroAction)
	{
	case NORMAL:
		//暴击
		if (rand() % 4 == 1)         //暴击率是25%
		{
			HeroAction = CRITICAL;
			damage = (int)4.5 * (3 * (rand() % Hero.Agility) + Hero.level*Hero.strength + 20);
			Boss.NowHp -= damage;
			swprintf_s(str, L"神天兵 释放幽狗...对魔龙圣使  造成[%d]伤害，小case", damage);
			Message_Insert(str);
		}
		//真的普通攻击
		else
		{
			damage = 3 * (rand() % Hero.Agility) + Hero.level*Hero.strength + 20;
			Boss.NowHp -= damage;
			swprintf_s(str, L"神天兵 释放幽冥鬼火...对魔龙圣使  造成[%d]伤害，小case", damage);
			Message_Insert(str);
		}
		break;
	case MAGIC:    //消耗蓝 30
		if (Hero.NowMp >= 30)
		{
			damage = 5 * (2 * (rand() % Hero.Agility) + Hero.level*Hero.Intell);
			Boss.NowHp -= damage;
			swprintf_s(str, L"神天兵 利用魔法释放小鬼...对魔龙圣使  造成[%d]伤害，小case", damage);
			Message_Insert(str);
			Hero.NowMp -= 30;
		}
		else
		{
			HeroAction = MISS;
		}
		break;
	case RECOVER://消耗蓝 40
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

//绘制英雄的技能
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

//Boss的逻辑代码
VOID BossAction_Logic()
{
	srand((unsigned int)time(NULL));

	if (Boss.NowHp > (Boss.MaxHp / 2))//血量的多少来决定放什么技能！！血量比较充足
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
	else     //气血比较低 爆发 狂暴
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

//绘制Boss的技能
VOID BossActionPaint()
{
	wchar_t str[100];
	int damage = 0;   //80ms  消息循环每一帧 80个毫秒 
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
			swprintf_s(str, L"魔龙圣使 释放幽冥鬼火...对 神天兵 造成[%d]伤害，小case",damage);
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
			swprintf_s(str, L"魔龙圣使 释放疯狗...对 神天兵 造成[%d]伤害，小case", damage);
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
			swprintf_s(str, L"魔龙圣使 利用魔法释放屁屁...对 神天兵 造成[%d]伤害，小case", damage);
			Message_Insert(str);

			//BOSS自带 法术吸血
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

//判断玩家是否死亡
VOID DieCheck(int nowhp, bool ishero)
{
	wchar_t str[100];
	if (nowhp <= 0)  //生命值小于0
	{
		g_bGameOver = true;   //游戏结束
		if (ishero)   //是英雄
		{
			//播放失败时的音乐
			//PlaySound(L"./res/失败的喜悦.mp3", NULL, SND_FILENAME | SND_ASYNC);
			mciSendString(L"open ./res/失败的喜悦.mp3 alias music", 0, 0, 0); //向驱动晨发送字符串消息
			mciSendString(L"play music", 0, 0, 0);
			//PlaySound(L"./res/失败的喜悦.wav", NULL, SND_FILENAME | SND_ASYNC);
			swprintf_s(str, L"胜负乃兵家常事，大侠请不要气馁...");
			Message_Insert(str);
		}
		else
		{
			//播放胜利时的音乐
			PlaySound(L"./res/胜利的喜悦.wav", NULL, SND_FILENAME | SND_ASYNC);
			//mciSendString(L"open ./res/失败的喜悦.mp3 alias music", 0, 0, 0); //向驱动晨发送字符串消息
			//mciSendString(L"play music", 0, 0, 0);
			swprintf_s(str, L"长江后浪推前浪，大侠武功盖世，天下无敌！！！");
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