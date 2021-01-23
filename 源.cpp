//突然意识到define有多么好用，让我重新画一个棋盘
/*lsz-Amazon日志
	2019.11.24 开工了，阅读游戏说明，画好了棋盘，游戏规则基本实现，在npy的帮助下改了一个很
			   蠢的bug，现在可以在本地让两个人类玩家下棋
	2019.11.25 写好了第一版AI，无脑搜索,计算灵活度，目测只能打过随机数，连我自己都能打赢它
	2019.11.26 第二版AI完成，评估函数中加入了 Queenmove和 Kingmove参数，系数还需要调整
	2019.11.27 优化了参数，评估函数中的系数随棋局进行而变化
	2019.11.28 调整了棋盘的一些参数，现在的棋盘没有那么那么丑了，本地版界面更加人性化
	2019.12.02 函数中加入了position参数，但是现在很不稳定
	2019.12.02 20:32 尝试搜了两层，用了简单的剪枝，从第25回合开始搜两层，可以保证大部分情况
			   下不超时
	2019.12.06 改了本地版关于判断落子是否合法的两个bug，之前的判断方式若三点共线且最初点在
			   中间，会被程序判断为非法输入
	2019.12.09 自学了文件，写好了存盘读盘，但是存盘只能存一次，很不友好；还想再加个提示功能，
			   bot的强度还需要提高
	2019.12.10 换了另一种方式存盘，可以实现随时手动存盘了，从记录落子方式变为记录当前棋盘状
			   态；修改了非法落子死循环的bug，即非法落子后清空输入流和缓冲区
	2019.12.10 22:59 添加了菜单选项，可以随时存盘退出，加入了提示功能，不用手动调用菜单，声
			   明了一个字符型变量，用户体验会好一些
	2019.12.14 调参，调整参数之后bot的强度稍稍好了一点
	2019.12.17 开始给本地版加功能，实现悔棋，悔棋可以一直到当前游戏模式的第一步，现在完全不
			   用手动下棋了,添加了背景音乐，不过有点蠢
	2019.12.18 写好了restart，其实只要再次调用playgame函数，再修改全局变量就行了，现在可以
			   随时重新开始，直接回到开始界面，可以重新选择游戏模式
	2019.12.18 13:52 加入了难度选择功能，放了之前的三版AI和一个随机版本，打不过终版bot也可
			   以打随机玩一玩2333
	2019.12.18 16:10 托管功能实现，游戏功能变得愈发沙雕，可以手动设置托管局数，修改了继续游
			   戏时回合数和当前玩家混乱的bug
*/

#include<iostream>
#include<fstream>
#include<Windows.h>
#include<mmsystem.h>
#include<iomanip> 
#include<cstdlib>
#include<cstdio>
#include<ctime>
#pragma comment(lib,"winmm.lib")

#define blackone "●"
#define whiteone "○"
#define obstacleone "╳ "
#define newblackone "◆"
#define newwhiteone "◇"
#define newobstacleone "※"
#define GRIDSIZE 8 
#define black 1
#define white -1
#define new_black 11
#define new_white 9
#define new_obstacle 12
#define OBSTACLE 2

using namespace std;
int board[8][8] = { 0 };
int dx[] = { -1,-1,-1,0,1,1,1,0 };
int dy[] = { 1,0,-1,-1,-1,0,1,1 };
int cycle = 0, me, mode, now_step, difficulty, watch = 0;//me:玩家的颜色，mode:模式，now_step:记录落子的次数便于悔棋，difficulty:难度选择，watch:是否正在观看
int record[60][7] = { 0 };
struct ter_posi {
	int ter;
	double posi;
};

int maxx(int, int); //两个无脑函数 
int minn(int, int); //两个无脑函数 
void DrawTheBoard(); //画棋盘函数
bool inMap(int, int); //判断该棋子是否在棋盘内 
bool check(int); //检查某种颜色是否有棋可走 
int mobility(int);//计算某种颜色的棋子能够到达的空格的数量 
void PutChess(int, int, int, int, int, int, int); //落子函数 
void turn_normal(int, int, int, int); //将有特殊标记的棋子和障碍变回正常 
void ClearChess(int, int, int, int, int, int, int); //清除模拟落子函数
bool legal(int, int, int, int, int, int, int); //判断落子是否合法
bool obey_the_rule(int, int, int, int, int, int, int); //判断落子是否符合规则 
void human_decision(); //人类决策，类似菜单选项，为了方便写成了函数
ter_posi king_move(int); //计算 Kingmove移动方式下的 territory值 
double square(int, int); //计算幂次方 
ter_posi queen_move(int); //计算 Queenmove移动方式下的 territory值 
void determine_4(int); //AI难度为4
void determine_3(int); //AI难度为3
void determine_2(int); //AI难度为2
void determine_1(int); //AI难度为1
void AI_decision(int); //AI决策函数
void save(); // 存盘函数
void regret(); //悔棋函数
void AI_play(int); //托管函数
void play_with_yourself(); //自己和自己下棋 
void new_game(); //新游戏函数，和AI下一盘棋 
void continue_game(); // 继续游戏函数，在上一局函数中调用
void last_game(); // 上一局函数
void play_game(); // 真正的玩游戏函数 

int maxx(int x, int y)
{
	if (x > y) return x;
	return y;
}

int minn(int x, int y)
{
	if (x > y) return y;
	return x;
}

void DrawTheBoard()//画棋盘函数，直接调用即可
{
	system("cls");
	cout << endl << endl << "	     THE AMAZONS" << endl << endl;
	cout << "     0   1   2   3   4   5   6   7" << endl;
	cout << "  ┌───┬───┬───┬───┬───┬───┬───┬───┐" << endl;
	for (int i = 0; i < 7; i++)
	{
		cout << " " << i;
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == 1)
				cout << "│ " << setw(2) << blackone;
			else if (board[i][j] == -1)
				cout << "│ " << setw(2) << whiteone;
			else if (board[i][j] == 2)
				cout << "│ " << setw(2) << obstacleone;
			else if (board[i][j] == 11)
				cout << "│ " << setw(2) << newblackone;
			else if (board[i][j] == 9)
				cout << "│ " << setw(2) << newwhiteone;
			else if (board[i][j] == 12)
				cout << "│ " << setw(2) << newobstacleone;
			else
				cout << "│ " << setw(2) << ' ';
		}
		cout << "│ " << i << "  ";
		cout << endl;
		cout << "  ├───┼───┼───┼───┼───┼───┼───┼───┤" << endl;
	}
	cout << " 7";
	for (int j = 0; j < 8; j++)
	{
		if (board[7][j] == 1)
			cout << "│ " << setw(2) << blackone;
		else if (board[7][j] == -1)
			cout << "│ " << setw(2) << whiteone;
		else if (board[7][j] == 2)
			cout << "│ " << setw(2) << obstacleone;
		else if (board[7][j] == 11)
			cout << "│ " << setw(2) << newblackone;
		else if (board[7][j] == 9)
			cout << "│ " << setw(2) << newwhiteone;
		else if (board[7][j] == 12)
			cout << "│ " << setw(2) << newobstacleone;
		else
			cout << "│ " << setw(2) << ' ';
	}
	cout << "│ " << 7 << "  ";
	cout << endl;
	cout << "  └───┴───┴───┴───┴───┴───┴───┴───┘" << endl;
	cout << "     0   1   2   3   4   5   6   7" << endl;
	cout << endl;

	if (cycle != 0)
	{
		cout << "	      回合数：" << cycle << endl;
		if (mode == 1 || mode ==2)
		{
			if (me == 1)
				cout << "您是黑方" << endl;
			else if(me == -1)
				cout << "您是白方" << endl;
			if (watch)
			{
				cout << "	      托管中";
				for (int i = 0; i <= cycle % 4; i++)
					cout << ".";
				cout << endl;
			}
			else 
			{
				cout << "Press E (退出)	Press H (提示) Press R (悔棋)" << endl;
				cout << "Press T (重新开始) Press B (托管)" << endl;
				cout << "或输入坐标" << endl;
			}
		}
	}
	cycle++; //每画一次棋盘，回合数 +1 
}//画棋盘函数，直接调用即可

bool inMap(int x, int y)//判断该棋子是否在棋盘内 
{
	if (x < 0 || x > 7 || y < 0 || y > 7)
		return false;
	return true;
}//判断该棋子是否在棋盘内 

bool check(int color)//检查某种颜色是否有棋可走 
{
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			if (board[i][j] != color)
				continue;
			for (int k = 0; k < 8; k++) //找到一个棋子后向八个方向搜索，只要能走就能放障碍
			{
				int xx = i + dx[k], yy = j + dy[k];
				if (!inMap(xx, yy))
					continue;
				if (board[xx][yy] == 0)
					return true;
			}
		}
	}
	return false;
}//检查某种颜色是否有棋可走 

int mobility(int color)//计算某种颜色的棋子能够到达的空格的数量，作为灵活度的结果
{
	int mark[GRIDSIZE][GRIDSIZE] = { 0 };
	int total = 0;
	int totalmin = 70, total_one = 0;

	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			if (board[i][j] != color)
				continue;
			total_one = 0;
			for (int k = 0; k < 8; k++)
			{
				for (int l = 1; l < GRIDSIZE; l++)
				{
					int xx = i + dx[k] * l;
					int yy = j + dy[k] * l;
					if (!inMap(xx, yy) || board[xx][yy] != 0)
						break;
					if (mark[xx][yy] == 0)
					{
						total++;
						total_one++;
						mark[xx][yy] = 1;
					}
				}
			}
			if (total_one < totalmin)
				totalmin = total_one;//计算最小灵活度，防止一开始的时候某个棋子先被堵死
		}
	}
	return total + totalmin;
}//计算某种颜色的棋子能够到达的空格的数量 

void PutChess(int x0, int y0, int x1, int y1, int x2, int y2, int color)//落子函数，落子的过程用全局变量记录，方便悔棋 
{
	record[cycle][1] = x0; record[cycle][2] = y0;
	record[cycle][3] = x1; record[cycle][4] = y1;
	record[cycle][5] = x2; record[cycle][6] = y2;
	board[x0][y0] = 0;
	board[x1][y1] = color + 10;
	board[x2][y2] = new_obstacle;
}//落子函数，落子的过程用全局变量记录，方便悔棋

void turn_normal(int x1, int y1, int x2, int y2)//将特殊标记的棋子和障碍变回正常
{
	board[x1][y1] -= 10;
	board[x2][y2] = OBSTACLE;
}// 特殊标记的棋子和障碍变回正常 

void ClearChess(int x0, int y0, int x1, int y1, int x2, int y2, int color)//清除模拟落子函数 
{
	board[x2][y2] = 0;
	board[x1][y1] = 0;
	board[x0][y0] = color;//注意回溯的顺序，很重要！
}//清除模拟落子函数

bool legal(int x0, int y0, int x1, int y1, int x2, int y2, int color)//判断落子是否合法 
{
	if (!inMap(x0, y0) || !inMap(x1, y1) || !inMap(x2, y2))
		return false;
	if (board[x0][y0] != color || board[x1][y1] != 0)
		return false;
	if (board[x2][y2] != 0 && !(x2 == x0 && y2 == y0))
		return false;
	if (x1 == x2 && y1 == y2)
		return false;
	return true;
}//判断落子是否合法

bool obey_the_rule(int x0, int y0, int x1, int y1, int x2, int y2)//判断落子是否符合规则 
{
	if (x0 != x1 && y0 != y1 && x0 + y0 != x1 + y1 && x0 - y0 != x1 - y1)//棋子是否在一条直线上移动
		return false;
	if (x1 != x2 && y1 != y2 && x1 + y1 != x2 + y2 && x1 - y1 != x2 - y2)
		return false;
	if (x0 == x1)
		for (int i = minn(y0, y1) + 1; i < maxx(y0, y1); i++)
			if (board[x0][i] != 0)
				return false;
	if (y0 == y1)
		for (int i = minn(x0, x1) + 1; i < maxx(x0, x1); i++)
			if (board[i][y0] != 0)
				return false;
	if (x0 + y0 == x1 + y1)
		for (int i = minn(x0, x1) + 1; i < maxx(x0, x1); i++)
			if (board[i][x0 + y0 - i] != 0)
				return false;
	if (x0 - y0 == x1 - y1)
		for (int i = minn(x0, x1) + 1; i < maxx(x0, x1); i++)
			if (board[i][i + y0 - x0] != 0)
				return false;

	if (x2 == x1)
		for (int i = minn(y2, y1) + 1; i < maxx(y2, y1); i++)
		{
			if (i == y0 && x2 == x0) continue;//如果越过移动前的位置，直接continue
			if (board[x2][i] != 0)
				return false;
		}
	if (y2 == y1)
		for (int i = minn(x2, x1) + 1; i < maxx(x2, x1); i++)
		{
			if (i == x0 && y2 == y0) continue;
			if (board[i][y2] != 0)
				return false;
		}
	if (x2 + y2 == x1 + y1)
		for (int i = minn(x2, x1) + 1; i < maxx(x2, x1); i++)
		{
			if (i == x0 && x2 + y2 - i == y0) continue;
			if (board[i][x2 + y2 - i] != 0)
				return false;
		}
	if (x2 - y2 == x1 - y1)
		for (int i = minn(x2, x1) + 1; i < maxx(x2, x1); i++)
		{
			if (i == x0 && i + y2 - x2 == y0) continue;
			if (board[i][i + y2 - x2] != 0)
				return false;
		}
	return true;
}//判断落子是否符合规则

void human_decision()//人类决策函数，选择模式或落子
{
	char condition;
	int x0, y0, x1, y1, x2, y2;
	cin >> condition;//读入的第一个操作声明为字符型，用户体验比较好
	while (true)
	{
		if (condition == 'H' || condition == 'h')//提示
		{
			now_step++;
			determine_1(me);
			return;
		}
		else if (condition == 'E' || condition == 'e')//存盘退出
		{
			save();
			exit(0);
		}
		else if ((condition == 'R' || condition == 'r') && cycle >= 4 && now_step > 0)//悔棋
		{
			regret();
			return;
		}
		else if (condition == 'T' || condition == 't')//restart重新开始，重新选择游戏模式
		{
			save();
			for (int i = 0; i < GRIDSIZE; i++)
				for (int j = 0; j < GRIDSIZE; j++)
					board[i][j] = 0;
			play_game();
		}
		else if (condition == 'B' || condition == 'b')//托管
		{
			int your_cycle;
			cout << "请输入需要托管的回合数" << endl;
			cin >> your_cycle;
			AI_play(2 * your_cycle);
			return;
		}
		else if (condition >= '0' && condition <= '7')//落子
		{
			x0 = condition - '0';
			cin >> y0 >> x1 >> y1 >> x2 >> y2;

			while (!legal(x0, y0, x1, y1, x2, y2, me) || !obey_the_rule(x0, y0, x1, y1, x2, y2))
			{
				cout << "illegal put,please put again" << endl;
				cin.clear();
				cin.ignore();
				cin >> x0;
				cin >> y0 >> x1 >> y1 >> x2 >> y2;
			}
			now_step++;//落子的回合数+1
			PutChess(x0, y0, x1, y1, x2, y2, me);
			DrawTheBoard();
			turn_normal(x1, y1, x2, y2);
			break;
		}
		else
		{
			cout << "illegal put, please put again" << endl;
			cin.clear();
			cin.ignore();
			cin >> condition;
		}
	}
}//人类决策函数，选择模式或落子

ter_posi king_move(int color)//Kingmove走法的territory和position值，返回类型为结构体
{
	int step[8][8][4] = { 0 };//0表示白色，1表示黑色，2表示 territory值 ，3表示 position值 
	ter_posi king_result;
	king_result.ter = 0;
	king_result.posi = 0;

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			for (int k = 0; k < 2; k++)
				step[i][j][k] = 100;//先全部初始化为100

	for (int nowcolor = 1; nowcolor >= -1; nowcolor -= 2)
	{
		int which_one = nowcolor;
		if (nowcolor == -1) which_one = 0;
		int other_one = 1 - which_one;
		int steps;

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (board[i][j] == -nowcolor)
					step[i][j][which_one] = -1;//对方占有的格子，我方永远无法到达
				if (board[i][j] == nowcolor)
				{
					step[i][j][which_one] = 0;//我方占有的格子，需要的步数为0
					steps = 1;
					for (int k = 0; k < 8; k++)
					{
						int xx = i + dx[k];
						int yy = j + dy[k];
						if (!inMap(xx, yy) || board[xx][yy] != 0)
							continue;
						step[xx][yy][which_one] = steps;//寻找需要1步可以到达的格子
					}
				}
			}
		}
		int mark;//标记是否还有其他可以到达的格子
		do
		{
			mark = 0;
			steps++;
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					if (step[i][j][which_one] != steps - 1)
						continue;
					for (int k = 0; k < 8; k++)//从当前步数可到达的格子出发寻找下一步可到达的格子
					{
						int xx = i + dx[k];
						int yy = j + dy[k];
						if (!inMap(xx, yy) || board[xx][yy] != 0)
							continue;
						if (step[xx][yy][which_one] > steps)
						{
							step[xx][yy][which_one] = steps;
							mark = 1;
						}
					}
				}
			}
		} while (mark == 1);
	}

	int this_one = color;
	if (color == -1)	this_one = 0;
	int other_one = 1 - this_one;

	for (int i = 0; i < 8; i++)//遍历每一个格子，按照公式计算territory和position值
	{
		for (int j = 0; j < 8; j++)
		{
			step[i][j][3] = minn(1, maxx(-1, (step[i][j][other_one] - step[i][j][this_one]) / 6));
			king_result.posi += step[i][j][3];
			if (step[i][j][0] == 100 && step[i][j][1] == 100)
				step[i][j][2] = 0;
			else if (step[i][j][0] == step[i][j][1] && step[i][j][0] < 100)
			{
				step[i][j][2] = color;
				king_result.ter += color;
			}
			else if (step[i][j][0] < step[i][j][1])
			{
				step[i][j][2] = -color * 2;
				king_result.ter += -color * 2;
			}
			else if (step[i][j][0] > step[i][j][1])
			{
				step[i][j][2] = color * 2;
				king_result.ter += color * 2;
			}
		}
	}
	return king_result;
}//Kingmove走法的territory和position值，返回类型为结构体

double square(int x, int y)//计算x的y次幂
{
	double result = 1;
	if (y == 0)
		return 1;
	if (y > 0)
		for (int i = 1; i <= y; i++)
			result = result * x;
	else
		for (int i = 1; i <= -y; i++)
			result = result / x;
	return result;
} // 计算 x的 y次幂 

ter_posi queen_move(int color)//标记棋盘每个格子用queen走法所需的步数，具体方法与king_move函数相同
{
	double step[8][8][4] = { 0 };//0表示白色，1表示黑色，2表示territory值，3表示position值
	ter_posi queen_result;
	queen_result.ter = 0;
	queen_result.posi = 0.0;

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			for (int k = 0; k < 2; k++)
				step[i][j][k] = 100;

	for (int nowcolor = 1; nowcolor >= -1; nowcolor -= 2)
	{
		int which_one = nowcolor;
		if (nowcolor == -1) which_one = 0;
		int other_one = 1 - which_one;
		int steps;

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (board[i][j] == -nowcolor)
					step[i][j][which_one] = -1;
				if (board[i][j] == nowcolor)
				{
					step[i][j][which_one] = 0;
					steps = 1;
					for (int k = 0; k < 8; k++)
					{
						for (int delta = 1; delta < GRIDSIZE; delta++)
						{
							int xx = i + dx[k] * delta;
							int yy = j + dy[k] * delta;
							if (!inMap(xx, yy) || board[xx][yy] != 0)
								continue;
							step[xx][yy][which_one] = steps;
						}
					}
				}
			}
		}
		int mark;
		do
		{
			mark = 0;
			steps++;
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					if (step[i][j][which_one] != steps - 1)
						continue;
					for (int k = 0; k < 8; k++)
					{
						for (int delta = 1; delta < GRIDSIZE; delta++)
						{
							int xx = i + dx[k] * delta;
							int yy = j + dy[k] * delta;
							if (!inMap(xx, yy) || board[xx][yy] != 0)
								continue;
							if (step[xx][yy][which_one] > steps)
							{
								step[xx][yy][which_one] = steps;
								mark = 1;
							}
						}
					}
				}
			}
		} while (mark == 1);
	}

	int this_one = color;
	if (color == -1)	this_one = 0;
	int other_one = 1 - this_one;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			step[i][j][3] = 2 * (square(2, -step[i][j][this_one]) - square(2, -step[i][j][other_one]));
			queen_result.posi += step[i][j][3];
			if (step[i][j][0] == 100 && step[i][j][1] == 100)
				step[i][j][2] = 0;
			else if (step[i][j][0] == step[i][j][1] && step[i][j][0] < 100)
			{
				step[i][j][2] = color;
				queen_result.ter += color;
			}
			else if (step[i][j][0] < step[i][j][1])
			{
				step[i][j][2] = -color * 2;
				queen_result.ter += -color * 2;
			}
			else if (step[i][j][0] > step[i][j][1])
			{
				step[i][j][2] = color * 2;
				queen_result.ter += color * 2;
			}
		}
	}
	return queen_result;
}//标记棋盘每个格子用queen走法所需的步数  

void determine_4(int color)// 选择难度系数为4，运用mobility，territory，position作为参数，25回合后剪枝搜两层
{
	int pos_begin[3000][2], pos_put[3000][2], pos_obs[3000][2];
	int pos_method1 = 0, choice;
	double maxvalue = -300000;
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			if (board[i][j] != color)
				continue;
			for (int k1 = 0; k1 < 8; k1++)//找到当前棋子所在位置，向八个方向搜索可行方案
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
				{
					int xx = i + dx[k1] * delta1;
					int yy = j + dy[k1] * delta1;
					if (board[xx][yy] != 0 || !inMap(xx, yy))
						break;
					for (int k2 = 0; k2 < 8; k2++)
					{
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++)
						{
							int xxx = xx + dx[k2] * delta2;
							int yyy = yy + dy[k2] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (board[xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (legal(i, j, xx, yy, xxx, yyy, color))
							{
								PutChess(i, j, xx, yy, xxx, yyy, color);//模拟落子函数
								turn_normal(xx, yy, xxx, yyy);
								pos_begin[pos_method1][0] = i;
								pos_begin[pos_method1][1] = j;
								pos_put[pos_method1][0] = xx;
								pos_put[pos_method1][1] = yy;
								pos_obs[pos_method1][0] = xxx;
								pos_obs[pos_method1][1] = yyy;//记录模拟的落子方案

								if (cycle > 25)//极大极小搜索+剪枝
								{
									int pos_begin2[3000][2] = { 0 }, pos_put2[3000][2] = { 0 }, pos_obs[3000][2] = { 0 };
									double minvalue = 300000;
									int flag = 1;
									//搜索对方的可行方案
									for (int ii = 0; ii < GRIDSIZE; ii++)
									{
										for (int jj = 0; jj < GRIDSIZE; jj++)
										{
											if (board[ii][jj] != -color)
												continue;
											for (int k3 = 0; k3 < 8; k3++)
											{
												for (int delta3 = 1; delta3 < GRIDSIZE; delta3++)
												{
													int xx2 = ii + dx[k3] * delta3;
													int yy2 = jj + dy[k3] * delta3;
													if (!inMap(xx2, yy2) || board[xx2][yy2] != 0)
														break;

													for (int k4 = 0; k4 < 8; k4++)
													{
														for (int delta4 = 1; delta4 < GRIDSIZE; delta4++)
														{
															int xxx2 = xx2 + dx[k4] * delta4;
															int yyy2 = yy2 + dy[k4] * delta4;
															if (!inMap(xxx2, yyy2))
																break;
															if (board[xxx2][yyy2] != 0 && !(ii == xxx2 && jj == yyy2))
																break;
															if (legal(ii, jj, xx2, yy2, xxx2, yyy2, -color))
															{
																PutChess(ii, jj, xx2, yy2, xxx2, yyy2, -color);
																turn_normal(xx2, yy2, xxx2, yyy2);//模拟对方落子
															}

															ter_posi now_king = king_move(color);
															ter_posi now_queen = queen_move(color);
															ter_posi other_king = king_move(-color);
															ter_posi other_queen = queen_move(-color);

															int now_mob = mobility(color) - mobility(-color);
															int now_king_t = now_king.ter - other_king.ter;
															int now_queen_t = now_queen.ter - other_queen.ter;
															double now_queen_p = now_queen.posi - other_queen.posi;
															double now_king_p = (now_king.posi - other_king.posi) / 6;
															double now_p = now_king_p + now_queen_p;

															int n1, n2, n3, n4;//参数按照回合数变化
															if (cycle <= 12)
															{
																n1 = 18; n2 = 40; n3 = 20; n4 = 78;
															}
															else if (cycle < 42)
															{
																n1 = 7; n2 = 6; n3 = 2; n4 = 24;
															}
															else { n1 = 9; n2 = 2; n3 = 1; n4 = 30; }

															double now_value = n1 * now_queen_t + n2 * now_king_t + n3 * now_mob + n4 * now_p;//计算当前我方价值减去对方价值
															ClearChess(ii, jj, xx2, yy2, xxx2, yyy2, -color);
															if (now_value < minvalue)
																minvalue = now_value;//计算我方每一种走法对应对方决策后我的最小价值

															if (minvalue < maxvalue)//剪枝
															{
																flag = 0;
																break;
															}
														}
														if (flag == 0)
															break;
													}
													if (flag == 0)
														break;
												}
												if (flag == 0)
													break;
											}
											if (flag == 0)
												break;
										}
										if (flag == 0)
											break;
									}

									if (minvalue > maxvalue)//对方以对我最不利的方式决策后我的最大价值
									{
										maxvalue = minvalue;
										choice = pos_method1;
									}
									pos_method1++;
									ClearChess(i, j, xx, yy, xxx, yyy, color);
								}
								else//25回合内由于可行走法太多，为追求时间效率，只搜索一步，运用贪心
								{
									ter_posi now_king = king_move(color);
									ter_posi now_queen = queen_move(color);
									ter_posi other_king = king_move(-color);
									ter_posi other_queen = queen_move(-color);

									int now_mob = mobility(color) - mobility(-color);
									int now_king_t = now_king.ter - other_king.ter;
									int now_queen_t = now_queen.ter - other_queen.ter;
									double now_queen_p = now_queen.posi - other_queen.posi;
									double now_king_p = (now_king.posi - other_king.posi) / 6;
									double now_p = now_king_p + now_queen_p;

									int n1, n2, n3, n4;
									if (cycle <= 12)
									{
										n1 = 18; n2 = 40; n3 = 20; n4 = 78;
									}
									else if (cycle < 42)
									{
										n1 = 7; n2 = 6; n3 = 2; n4 = 24;
									}
									else { n1 = 9; n2 = 2; n3 = 1; n4 = 30; }

									double now_value = n1 * now_queen_t + n2 * now_king_t + n3 * now_mob + n4 * now_p;
									if (now_value > maxvalue)
									{
										maxvalue = now_value;
										choice = pos_method1;
									}
									pos_method1++;
									ClearChess(i, j, xx, yy, xxx, yyy, color);
								}
							}
						}
					}
				}
			}
		}
	}
	PutChess(pos_begin[choice][0], pos_begin[choice][1], pos_put[choice][0],
		pos_put[choice][1], pos_obs[choice][0], pos_obs[choice][1], color);
	DrawTheBoard();
	turn_normal(pos_put[choice][0],
		pos_put[choice][1], pos_obs[choice][0], pos_obs[choice][1]);
}// 选择难度系数为4，运用mobility，territory，position作为参数，25回合后剪枝搜两层

void determine_3(int color)//选择难度系数为3，运用mobility和territory为参数，搜索一步
{
	int pos_begin[3000][2], pos_put[3000][2], pos_obs[3000][2];
	int pos_method1 = 0, choice;
	double maxvalue = -300000;
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			if (board[i][j] != color)
				continue;
			for (int k1 = 0; k1 < 8; k1++)
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
				{
					int xx = i + dx[k1] * delta1;
					int yy = j + dy[k1] * delta1;
					if (board[xx][yy] != 0 || !inMap(xx, yy))
						break;
					for (int k2 = 0; k2 < 8; k2++)
					{
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++)
						{
							int xxx = xx + dx[k2] * delta2;
							int yyy = yy + dy[k2] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (board[xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (legal(i, j, xx, yy, xxx, yyy, color))
							{
								PutChess(i, j, xx, yy, xxx, yyy, color);
								turn_normal(xx, yy, xxx, yyy);
								pos_begin[pos_method1][0] = i;
								pos_begin[pos_method1][1] = j;
								pos_put[pos_method1][0] = xx;
								pos_put[pos_method1][1] = yy;
								pos_obs[pos_method1][0] = xxx;
								pos_obs[pos_method1][1] = yyy;

								ter_posi now_king = king_move(color);
								ter_posi now_queen = queen_move(color);
								ter_posi other_king = king_move(-color);
								ter_posi other_queen = queen_move(-color);

								int now_mob = mobility(color) - mobility(-color);
								int now_king_t = now_king.ter - other_king.ter;
								int now_queen_t = now_queen.ter - other_queen.ter;

								int n1, n2, n3;
								if (cycle <= 12)
								{
									n1 = 14; n2 = 37; n3 = 40;
								}
								else if (cycle < 32)
								{
									n1 = 6; n2 = 5; n3 = 2;
								}
								else { n1 = 8; n2 = 1; n3 = 0; }

								double now_value = n1 * now_queen_t + n2 * now_king_t + n3 * now_mob;									if (now_value > maxvalue)
								{
									maxvalue = now_value;
									choice = pos_method1;
								}
								pos_method1++;
								ClearChess(i, j, xx, yy, xxx, yyy, color);
							}
						}
					}
				}
			}
		}
	}
	PutChess(pos_begin[choice][0], pos_begin[choice][1], pos_put[choice][0],
		pos_put[choice][1], pos_obs[choice][0], pos_obs[choice][1], color);
	DrawTheBoard();
	turn_normal(pos_put[choice][0],
		pos_put[choice][1], pos_obs[choice][0], pos_obs[choice][1]);
}//选择难度系数为3，运用mobility和territory为参数，搜索一步

void determine_2(int color)//选择难度系数为2，运用mobility为参数，搜索一步
{
	int pos_begin[3000][2], pos_put[3000][2], pos_obs[3000][2];
	int pos_method = 0, choice;
	int maxvalue = -300000;
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			if (board[i][j] != color)
				continue;
			for (int k1 = 0; k1 < 8; k1++)
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
				{
					int xx = i + dx[k1] * delta1;
					int yy = j + dy[k1] * delta1;
					if (board[xx][yy] != 0 || !inMap(xx, yy))
						break;
					for (int k2 = 0; k2 < 8; k2++)
					{
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++)
						{
							int xxx = xx + dx[k2] * delta2;
							int yyy = yy + dy[k2] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (board[xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (legal(i, j, xx, yy, xxx, yyy, color))
							{
								PutChess(i, j, xx, yy, xxx, yyy, color);
								turn_normal(xx, yy, xxx, yyy);
								pos_begin[pos_method][0] = i;
								pos_begin[pos_method][1] = j;
								pos_put[pos_method][0] = xx;
								pos_put[pos_method][1] = yy;
								pos_obs[pos_method][0] = xxx;
								pos_obs[pos_method][1] = yyy;

								int nowvalue = mobility(color) - mobility(-color);
								if (nowvalue > maxvalue)
								{
									maxvalue = nowvalue;
									choice = pos_method;
								}
								pos_method++;
								ClearChess(i, j, xx, yy, xxx, yyy, color);
							}
						}
					}
				}
			}
		}
	}
	PutChess(pos_begin[choice][0], pos_begin[choice][1], pos_put[choice][0],
		pos_put[choice][1], pos_obs[choice][0], pos_obs[choice][1], color);
	DrawTheBoard();
	turn_normal(pos_put[choice][0],
		pos_put[choice][1], pos_obs[choice][0], pos_obs[choice][1]);
}//选择难度系数为2，运用mobility为参数，搜索一步

void determine_1(int color)//选择难度系数为2，随机落子，搜索一步
{
	int pos_begin[3000][2], pos_put[3000][2], pos_obs[3000][2];
	int pos_method = 0, choice;
	for (int i = 0; i < GRIDSIZE; ++i) 
	{
		for (int j = 0; j < GRIDSIZE; ++j) 
		{
			for (int k = 0; k < 8; ++k) 
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++) 
				{
					int xx = i + dx[k] * delta1;
					int yy = j + dy[k] * delta1;
					if (board[xx][yy] != 0 || !inMap(xx, yy))
						break;
					for (int l = 0; l < 8; ++l) {
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++) 
						{
							int xxx = xx + dx[l] * delta2;
							int yyy = yy + dy[l] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (board[xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (legal(i,j,xx,yy,xxx,yyy,color))
							{
								pos_begin[pos_method][0] = i;
								pos_begin[pos_method][1] = j;
								pos_put[pos_method][0] = xx;
								pos_put[pos_method][1] = yy;
								pos_obs[pos_method][0] = xxx;
								pos_obs[pos_method][1] = yyy;
								pos_method++;
							}
						}

					}
				}

			}
		}
	}
	srand(time(0));
	choice = rand() % pos_method;//生成随机数
	PutChess(pos_begin[choice][0], pos_begin[choice][1], pos_put[choice][0],
		pos_put[choice][1], pos_obs[choice][0], pos_obs[choice][1], color);
	DrawTheBoard();
	turn_normal(pos_put[choice][0],
		pos_put[choice][1], pos_obs[choice][0], pos_obs[choice][1]);
}//选择难度系数为2，随机落子，搜索一步

void AI_decision(int color)//AI决策函数，AI的强度与当前选择的难度有关
{
	if (difficulty == 4)	determine_4(color);
	else if (difficulty == 3)	determine_3(color);
	else if (difficulty == 2)	determine_2(color);
	else if (difficulty == 1)	determine_1(color);
}//AI决策函数，AI的强度与当前选择的难度有关

void save()//存盘函数
{
	ofstream fout("read_save.txt");
	fout << me << endl;//输出我方颜色
	for (int i = 0; i < GRIDSIZE; i++)//输出当前棋盘状态
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			if(board[i][j] < 5)
				fout << board[i][j] << " ";
			else fout << board[i][j] - 10 << " ";
		}
		fout << endl;
	}
	fout << cycle - 1 << endl;//输出当前回合数
	fout << difficulty;//输出选择的难度
	fout.close();
}//存盘函数

void regret()//悔棋函数
{
	now_step--;//还可以继续悔棋的步数减一
	cycle--;//每次悔棋撤回两回合
	ClearChess(record[cycle][1], record[cycle][2], record[cycle][3], record[cycle][4], record[cycle][5], record[cycle][6], -me);
	cycle--;
	ClearChess(record[cycle][1], record[cycle][2], record[cycle][3], record[cycle][4], record[cycle][5], record[cycle][6], me);
	cycle--;
	if (cycle >= 2 && now_step > 0)//把上一个回合中AI的决策体现出来，若无前一回合，则不体现
	{
		board[record[cycle][3]][record[cycle][4]] += 10;
		board[record[cycle][5]][record[cycle][6]] += 10;
		DrawTheBoard();
		turn_normal(record[cycle - 1][3], record[cycle - 1][4], record[cycle - 1][5], record[cycle - 1][6]);
	}
	else DrawTheBoard();
	human_decision();
}//悔棋函数

void AI_play(int your_cycle)//托管函数
{
	int player = me;
	watch = 1;//标记状态为托管中
	while (your_cycle--)
	{
		if (player == me)
			determine_1(player);//托管当然要用最菜的AI，谁让你懒呢
		if (player == -me)
			AI_decision(player);

		player = player * (-1);
		if (!check(player))
		{
			cout << "Game Over" << endl;
			if (player == me)
				cout << "输给了AI呢 TAT" << endl;
			else cout << "恭喜你战胜了蠢萌的AI 撒花" << endl;
			cout << "goodbye" << endl;
			exit(0);
		}
		Sleep(500);//每走一步停0.5秒，让你清楚的看到你是怎么死的
	}
	cycle--;
	watch = 0;
	continue_game();//托管结束后可继续游戏
}//托管函数

void play_with_yourself()//自己和自己下一盘棋
{
	PlaySound(TEXT("D:\\sweet bite.wav"), NULL, SND_FILENAME | SND_ASYNC);
	DrawTheBoard();
	int player = 1;
	int x0, y0, x1, y1, x2, y2;
	cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
	while (!legal(x0, y0, x1, y1, x2, y2, player) || !obey_the_rule(x0, y0, x1, y1, x2, y2))
	{
		cout << "illegal put,please put again" << endl;
		cin.clear();
		cin.ignore();
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
	}

	while (true)
	{
		PutChess(x0, y0, x1, y1, x2, y2, player);
		DrawTheBoard();
		turn_normal(x1, y1, x2, y2);
		player = player * (-1);
		if (!check(player))
		{
			cout << "Game Over" << endl;
			cout << "The Winner is PLAYER " << endl;
			if (player == 1)
				cout << "1";
			else cout << "2";
			cout << "goodbye" << endl;
			break;
		}
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;

		while (!legal(x0, y0, x1, y1, x2, y2, player) || !obey_the_rule(x0, y0, x1, y1, x2, y2))
		{
			cout << "illegal put,please put again" << endl;
			cin.clear();
			cin.ignore();
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		}
	}
}//自己和自己下一盘棋

void new_game()//新游戏函数，和AI下一盘棋 
{
	PlaySound(TEXT("D:\\Dusk.wav"), NULL, SND_FILENAME | SND_ASYNC);
	cycle = 0;
	DrawTheBoard();
	cout << "	     请选择难度" << endl;
	cout << "     蠢萌：Press 1  休闲：Press 2" << endl;
	cout << "人类：Press 3	计算机科学家：Press 4" << endl;
	while (true)
	{
		cin >> difficulty;//选择难度
		if (difficulty == 1 || difficulty == 2 || difficulty == 3 || difficulty == 4)
			break;
		cin.clear();
		cin.ignore();
		cycle = 0;
		DrawTheBoard();
		cout << "  	   请重新选择难度" << endl;
		cout << "     蠢萌：Press 1  休闲：Press 2" << endl;
		cout << "人类：Press 3	计算机科学家：Press 4" << endl;
	}
	cycle = 0;
	DrawTheBoard();

	cout << "	please choose your color" << endl;
	cout << "  black (press B) or white (press W)" << endl;
	char choose;
	cin >> choose;//选择我方执子颜色
	cycle = 0;
	DrawTheBoard();
	cycle++;

	while (true)
	{
		while (true)
		{
			if (choose == 'B' || choose == 'b')
			{
				cout << "	      回合数：1" << endl << "您是黑方" << endl;
				cout << "Press E (退出)	Press H (提示) Press R (悔棋)" << endl;
				cout << "Press T (重新开始) Press B (托管)" << endl;
				cout << "或输入坐标" << endl;
				me = 1;		
				break;
			}
			else if (choose == 'W' || choose == 'w')
			{
				me = -1;	
				break;
			}
			else
			{
				cin.clear();
				cin.ignore();
				cout << "请重新选择您要执子的颜色" << endl;
				cout << "Black ( press B ) ; White ( press W )" << endl;
				cin >> choose;
			}
		}
		cin.clear();
		cin.ignore();
		int player = 1;

		while (true)
		{
			if (player == me)
				human_decision();
			else AI_decision(-me);

			player = player * (-1);
			if (!check(player))
			{
				cout << "Game Over" << endl;
				if (player == 1)
					cout << "输给了AI呢 TAT" << endl;
				else cout << "恭喜你战胜了蠢萌的AI 撒花" << endl;
				cout << "goodbye" << endl;
				exit(0);
			}
		}
	}
}//新游戏函数，和AI下一盘棋

void continue_game()//继续游戏函数
{
	int player = -1;
	int x0, y0, x1, y1, x2, y2;
	DrawTheBoard();
	if (cycle % 2 == 0)
		player = 1;//判断现在的玩家

	while (true)
	{
		if (player == me)
			human_decision();
		else AI_decision(-me);
		player = player * (-1);
		if (!check(player))
		{
			cout << "Game Over" << endl;
			if (player == me)
				cout << "输给了AI呢 TAT" << endl;
			else cout << "恭喜你战胜了蠢萌的AI 撒花" << endl;
			cout << "goodbye" << endl;
			exit(0);
		}
	}
}//继续游戏函数

void last_game()//上一局游戏函数，读盘
{
	PlaySound(TEXT("D:\\tide.wav"), NULL, SND_FILENAME | SND_ASYNC);
	ifstream fin("read_save.txt");
	fin >> me;
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++)
			fin >> board[i][j];
	fin >> cycle;
	fin >> difficulty;
	fin.close();
	continue_game();
	return;
}//上一局游戏函数

void play_game()//真正的玩游戏函数
{
	printf("\a");
	now_step = 0;
	board[0][2] = 1; board[0][5] = 1; board[2][0] = 1; board[2][7] = 1;
	board[5][0] = -1; board[5][7] = -1; board[7][2] = -1; board[7][5] = -1;//初始化棋盘
	cycle = 0;
	DrawTheBoard();
	cout << "   	  Welcome to Amazon!" << endl;
	cout << "	● means black chess." << endl << " 	○ means white chess." << endl;
	cout << "	 ↑ means obstacle." << endl << endl;
	cout << "      play with yourself (press M)" << endl;
	cout << "      	  new game (press N)" << endl;
	cout << "    	 last game  (press L)" << endl;

	char condition;
	while (true)
	{
		cin >> condition;
		printf("\a");
		if (condition == 'm' || condition == 'M')
		{
			mode = 0;
			play_with_yourself();
			break;
		}
		else if (condition == 'n' || condition == 'N')
		{
			mode = 1;
			new_game();
			break;
		}
		else if (condition == 'l' || condition == 'L')
		{
			mode = 2;
			last_game();
			break;
		}
		else
		{
			cycle = 0;
			DrawTheBoard();
			cout << "		请重新选择游戏模式" << endl;
			cout << "      play with yourself (press M)" << endl;
			cout << "      	  new game (press N)" << endl;
			cout << "    	 last game  (press L)" << endl;
		}
	}
}//玩游戏函数

int main()
{
	system("mode con cols=80 lines=35");//窗口宽度高度
	system("color 0E0");//背景颜色
	play_game();
	return 0;
}