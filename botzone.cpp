// 亚马逊棋（Amazons）简单交互样例程序
// 游戏信息：http://www.botzone.org/games#Amazons
// 加入了position参量 
// 搜两层 + 剪枝 


#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>


#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1

using namespace std;
struct ter_posi{
	int ter;
	double posi;
};

int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int board[GRIDSIZE][GRIDSIZE] = { 0 }; // 先x后y，记录棋盘状态
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };
int startX, startY, resultX, resultY, obstacleX, obstacleY, cycle;

int maxx(int x,int y)
{
	if(x>y) return x;
	return y;
}

int minn(int x,int y)
{
	if(x>y) return y;
	return x;
}

// 判断是否在地图内
inline bool inMap(int x, int y)
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	return true;
}


// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int x2, int y2, int color, bool check_only)
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2)))
		return false;
	if (board[x0][y0] != color || board[x1][y1] != 0)
		return false;
	if ((board[x2][y2] != 0) && !(x2 == x0 && y2 == y0))
		return false;
	if (!check_only)
	{
		board[x0][y0] = 0;
		board[x1][y1] = color;
		board[x2][y2] = OBSTACLE;
	}
	return true;
}

void PutChess(int x0,int y0,int x1,int y1,int x2,int y2,int currBotColor)//落子函数 
{
	board[x0][y0] = 0;
	board[x1][y1] = currBotColor;
	board[x2][y2] = OBSTACLE; 
}

void ClearChess(int x0,int y0,int x1,int y1,int x2,int y2,int currBotColor)//清除模拟落子函数 
{
	board[x2][y2] = 0;
	board[x1][y1] = 0;
	board[x0][y0] = currBotColor;
}//清除模拟落子函数

bool legal(int x0,int y0,int x1,int y1,int x2,int y2,int currBotColor)//判断落子是否合法 
{
	if(!inMap(x0,y0) || !inMap(x1,y1) || !inMap(x2,y2))
		return false;
	if(board[x0][y0] != currBotColor || board[x1][y1] != 0)
		return false;
	if(board[x2][y2] != 0 && !(x2 == x0 && y2 == y0))
		return false;
	return true;
}

int mobility(int color)//计算某种颜色的棋子能够到达的空格的数量 
{
	int mark[GRIDSIZE][GRIDSIZE]={0};
	int total = 0;
	int totalmin = 70, totali = 0;
	
	for(int i = 0 ; i < GRIDSIZE ; i++)
	{
		for(int j = 0 ; j < GRIDSIZE ; j++)
		{
			if(board[i][j] != color)
				continue;
			totali = 0;
			for(int k = 0 ; k < 8 ; k++)
			{
				for(int l = 1 ; l < GRIDSIZE ; l++)
				{
					int xx = i + dx[k] * l;
					int yy = j + dy[k] * l;
					if(!inMap(xx,yy) || board[xx][yy] != 0)
						break;
					if(mark[xx][yy] == 0)
					{
						total++;
						totali++;
						mark[xx][yy] = 1;
					}
				}
			}
			if(totali < totalmin)
				totalmin = totali;
		}
	}
	return total+totalmin;
}//计算某种颜色的棋子能够到达的空格的数量 

ter_posi king_move(int color)
{
	int step[8][8][4] = {0};//0表示白色，1表示黑色，2表示territory值 ，3表示position值 
	ter_posi king_result;
	king_result.ter = 0;
	king_result.posi = 0;
	
	for(int i = 0 ; i < 8 ; i++)
		for(int j = 0 ; j < 8 ; j++)
			for(int k = 0 ; k < 2 ; k++)
				step[i][j][k] = 100;
	
	for(int nowcolor = 1 ; nowcolor >= -1 ; nowcolor -= 2)
	{
		int which_one = nowcolor;
		if(nowcolor == -1) which_one = 0;
		int other_one = 1 - which_one;
		int steps;
		
		for(int i = 0 ; i < 8 ; i++)
		{
			for(int j = 0 ; j < 8 ; j++)
			{
				if(board[i][j] == -nowcolor)
					step[i][j][which_one] = -1;
				if(board[i][j] == nowcolor)
				{
					step[i][j][which_one] = 0;
					steps = 1;
					for(int k = 0 ; k < 8 ; k++)
					{
						int xx = i + dx[k];
						int yy = j + dy[k];
						if(!inMap(xx,yy) || board[xx][yy] != 0)
							continue;
						step[xx][yy][which_one] = steps;
					}
				}
			}
		}
		int mark;
		do
		{
			mark = 0;
			steps++;
			for(int i = 0 ; i < 8 ; i++)
			{
				for(int j = 0 ; j < 8 ; j++)
				{
					if(step[i][j][which_one] != steps-1)
						continue;
					for(int k = 0 ; k < 8 ; k++)
					{
						int xx = i + dx[k];
						int yy = j + dy[k];
						if(!inMap(xx,yy) || board[xx][yy] != 0)
							continue;
						if(step[xx][yy][which_one] > steps)
						{
							step[xx][yy][which_one] = steps;
							mark = 1;
						}
					}
				}
			}	
		}while(mark == 1);
	}
	
	int this_one = color;
	if(color == -1)	this_one = 0;
	int other_one = 1 - this_one;
	
	for(int i = 0 ; i < 8 ; i++)
	{
		for(int j = 0 ; j < 8 ; j++)
		{
			step[i][j][3] = minn(6 , maxx(-6 , (step[i][j][other_one] - step[i][j][this_one])));	
			king_result.posi += step[i][j][3];
			if(step[i][j][0] == 100 && step[i][j][1] == 100)
				step[i][j][2] = 0;
			else if(step[i][j][0] == step[i][j][1] && step[i][j][0] < 100)
			{
				step[i][j][2] = color;
				king_result.ter += color;
			}
			else if(step[i][j][0] < step[i][j][1])
			{
				step[i][j][2] = -color * 2;
				king_result.ter += -color * 2;
			}
			else if(step[i][j][0] > step[i][j][1])
			{
				step[i][j][2] = color * 2;
				king_result.ter += color * 2;
			}
		}
	}
	return king_result;
}

double square(int x,int y)
{
	double result = 1;
	if(y == 0)
		return 1;
	if(y > 0)
		for(int i = 1 ; i <= y ; i++)
			result = result * x;
	else
		for(int i = 1 ; i <= -y ; i++)
			result = result / x;
	return result;
} // 计算 x的 y次幂 

ter_posi queen_move(int color)//标记棋盘每个格子用queen走法所需的步数 
{
	double step[8][8][4] = {0};//0表示白色，1表示黑色
	ter_posi queen_result;
	queen_result.ter = 0;
	queen_result.posi = 0.0;
	
	for(int i = 0 ; i < 8 ; i++)
		for(int j = 0 ; j < 8 ; j++)
			for(int k = 0 ; k < 2 ; k++)
				step[i][j][k] = 100;
	
	for(int nowcolor = 1 ; nowcolor >= -1 ; nowcolor -= 2)
	{
		int which_one = nowcolor;
		if(nowcolor == -1) which_one = 0;
		int other_one = 1 - which_one;
		int steps;
		
		for(int i = 0 ; i < 8 ; i++)
		{
			for(int j = 0 ; j < 8 ; j++)
			{
				if(board[i][j] == -nowcolor)
					step[i][j][which_one] = -1;
				if(board[i][j] == nowcolor)
				{
					step[i][j][which_one] = 0;
					steps = 1;
					for(int k = 0 ; k < 8 ; k++)
					{
						for(int delta = 1 ; delta < GRIDSIZE ; delta++)
						{
							int xx = i + dx[k] *delta;
							int yy = j + dy[k] *delta;
							if(!inMap(xx,yy) || board[xx][yy] != 0)
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
			for(int i = 0 ; i < 8 ; i++)
			{
				for(int j = 0 ; j < 8 ; j++)
				{
					if(step[i][j][which_one] != steps-1)
						continue;
					for(int k = 0 ; k < 8 ; k++)
					{
						for(int delta = 1 ; delta < GRIDSIZE ; delta++)
						{
							int xx = i + dx[k] * delta;
							int yy = j + dy[k] * delta;
							if(!inMap(xx,yy) || board[xx][yy] != 0)
								continue;
							if(step[xx][yy][which_one] > steps)
							{
								step[xx][yy][which_one] = steps;
								mark = 1;
							}
						}
					}
				}
			}	
		}while(mark == 1);
	}
	
	int this_one = color;
	if(color == -1)	this_one = 0;
	int other_one = 1 - this_one;
	
	for(int i = 0 ; i < 8 ; i++)
	{
		for(int j = 0 ; j < 8 ; j++)
		{
			step[i][j][3] = square(2 , -step[i][j][this_one]) - square(2 , -step[i][j][other_one]);
			queen_result.posi += step[i][j][3];
			if(step[i][j][0] == 100 && step[i][j][1] == 100)
				step[i][j][2] = 0;
			else if(step[i][j][0] == step[i][j][1] && step[i][j][0]<100)
			{
				step[i][j][2] = color;
				queen_result.ter += color;
			}
			else if(step[i][j][0] < step[i][j][1])
			{
				step[i][j][2] = -color * 2;
				queen_result.ter += -color * 2;
			}
			else if(step[i][j][0] > step[i][j][1])
			{
				step[i][j][2] = color * 2;
				queen_result.ter += color * 2;
			}
		}
	}
	return queen_result;
}//标记棋盘每个格子用queen走法所需的步数 

void determine(int color)//AI决策 
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
								pos_begin[pos_method1][0] = i;
								pos_begin[pos_method1][1] = j;
								pos_put[pos_method1][0] = xx;
								pos_put[pos_method1][1] = yy;
								pos_obs[pos_method1][0] = xxx;
								pos_obs[pos_method1][1] = yyy;

								if (cycle > 25)
								{
									int pos_begin2[3000][2] = { 0 }, pos_put2[3000][2] = { 0 }, pos_obs[3000][2] = { 0 };
									double minvalue = 300000;
									int flag = 1;
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
																PutChess(ii, jj, xx2, yy2, xxx2, yyy2, -color);

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
															ClearChess(ii, jj, xx2, yy2, xxx2, yyy2, -color);
															if (now_value < minvalue)
																minvalue = now_value;

															if (minvalue < maxvalue)
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

									if (minvalue > maxvalue)
									{
										maxvalue = minvalue;
										choice = pos_method1;
									}
									pos_method1++;
									ClearChess(i, j, xx, yy, xxx, yyy, color);
								}
								else
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
		
	startX = pos_begin[choice][0];
	startY = pos_begin[choice][1];
	resultX = pos_put[choice][0];
	resultY = pos_put[choice][1];
	obstacleX = pos_obs[choice][0];
	obstacleY = pos_obs[choice][1];
}

int main()
{
	int x0, y0, x1, y1, x2, y2;

	// 初始化棋盘
	board[0][(GRIDSIZE - 1) / 3] = board[(GRIDSIZE - 1) / 3][0]
		= board[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][0]
		= board[GRIDSIZE - 1][(GRIDSIZE - 1) / 3] = grid_black;
	board[0][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = board[(GRIDSIZE - 1) / 3][GRIDSIZE - 1]
		= board[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][GRIDSIZE - 1]
		= board[GRIDSIZE - 1][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = grid_white;


	int turnID;
	cin >> turnID;

	// 读入到当前回合为止，自己和对手的所有行动，从而把局面恢复到当前回合
	currBotColor = grid_white; // 先假设自己是白方
	for (int i = 0; i < turnID; i++)
	{
		// 根据这些输入输出逐渐恢复状态到当前回合

		// 首先是对手行动
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		if (x0 == -1)
			currBotColor = grid_black; // 第一回合收到坐标是-1, -1，说明我是黑方
		else
			ProcStep(x0, y0, x1, y1, x2, y2, -currBotColor, false); // 模拟对方落子

		// 然后是自己当时的行动
		// 对手行动总比自己行动多一个
		if (i < turnID - 1)
		{
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 >= 0)
				ProcStep(x0, y0, x1, y1, x2, y2, currBotColor, false); // 模拟己方落子
		}
	}

	// 做出决策（你只需修改以下部分）
	cycle = 2 * turnID;
	determine(currBotColor);

	// 决策结束，输出结果（你只需修改以上部分）
	cout << startX << ' ' << startY << ' ' << resultX << ' ' << resultY << ' ' << obstacleX << ' ' << obstacleY << endl;
	return 0;
}
