//ͻȻ��ʶ��define�ж�ô���ã��������»�һ������
/*lsz-Amazon��־
	2019.11.24 �����ˣ��Ķ���Ϸ˵�������������̣���Ϸ�������ʵ�֣���npy�İ����¸���һ����
			   ����bug�����ڿ����ڱ��������������������
	2019.11.25 д���˵�һ��AI����������,�������ȣ�Ŀ��ֻ�ܴ��������������Լ����ܴ�Ӯ��
	2019.11.26 �ڶ���AI��ɣ����������м����� Queenmove�� Kingmove������ϵ������Ҫ����
	2019.11.27 �Ż��˲��������������е�ϵ������ֽ��ж��仯
	2019.11.28 ���������̵�һЩ���������ڵ�����û����ô��ô���ˣ����ذ����������Ի�
	2019.12.02 �����м�����position�������������ںܲ��ȶ�
	2019.12.02 20:32 �����������㣬���˼򵥵ļ�֦���ӵ�25�غϿ�ʼ�����㣬���Ա�֤�󲿷����
			   �²���ʱ
	2019.12.06 ���˱��ذ�����ж������Ƿ�Ϸ�������bug��֮ǰ���жϷ�ʽ�����㹲�����������
			   �м䣬�ᱻ�����ж�Ϊ�Ƿ�����
	2019.12.09 ��ѧ���ļ���д���˴��̶��̣����Ǵ���ֻ�ܴ�һ�Σ��ܲ��Ѻã������ټӸ���ʾ���ܣ�
			   bot��ǿ�Ȼ���Ҫ���
	2019.12.10 ������һ�ַ�ʽ���̣�����ʵ����ʱ�ֶ������ˣ��Ӽ�¼���ӷ�ʽ��Ϊ��¼��ǰ����״
			   ̬���޸��˷Ƿ�������ѭ����bug�����Ƿ����Ӻ�����������ͻ�����
	2019.12.10 22:59 ����˲˵�ѡ�������ʱ�����˳�����������ʾ���ܣ������ֶ����ò˵�����
			   ����һ���ַ��ͱ������û�������һЩ
	2019.12.14 ���Σ���������֮��bot��ǿ�����Ժ���һ��
	2019.12.17 ��ʼ�����ذ�ӹ��ܣ�ʵ�ֻ��壬�������һֱ����ǰ��Ϸģʽ�ĵ�һ����������ȫ��
			   ���ֶ�������,����˱������֣������е��
	2019.12.18 д����restart����ʵֻҪ�ٴε���playgame���������޸�ȫ�ֱ��������ˣ����ڿ���
			   ��ʱ���¿�ʼ��ֱ�ӻص���ʼ���棬��������ѡ����Ϸģʽ
	2019.12.18 13:52 �������Ѷ�ѡ���ܣ�����֮ǰ������AI��һ������汾���򲻹��հ�botҲ��
			   �Դ������һ��2333
	2019.12.18 16:10 �йܹ���ʵ�֣���Ϸ���ܱ������ɳ�񣬿����ֶ������йܾ������޸��˼�����
			   Ϸʱ�غ����͵�ǰ��һ��ҵ�bug
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

#define blackone "��"
#define whiteone "��"
#define obstacleone "�w "
#define newblackone "��"
#define newwhiteone "��"
#define newobstacleone "��"
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
int cycle = 0, me, mode, now_step, difficulty, watch = 0;//me:��ҵ���ɫ��mode:ģʽ��now_step:��¼���ӵĴ������ڻ��壬difficulty:�Ѷ�ѡ��watch:�Ƿ����ڹۿ�
int record[60][7] = { 0 };
struct ter_posi {
	int ter;
	double posi;
};

int maxx(int, int); //�������Ժ��� 
int minn(int, int); //�������Ժ��� 
void DrawTheBoard(); //�����̺���
bool inMap(int, int); //�жϸ������Ƿ��������� 
bool check(int); //���ĳ����ɫ�Ƿ�������� 
int mobility(int);//����ĳ����ɫ�������ܹ�����Ŀո������ 
void PutChess(int, int, int, int, int, int, int); //���Ӻ��� 
void turn_normal(int, int, int, int); //���������ǵ����Ӻ��ϰ�������� 
void ClearChess(int, int, int, int, int, int, int); //���ģ�����Ӻ���
bool legal(int, int, int, int, int, int, int); //�ж������Ƿ�Ϸ�
bool obey_the_rule(int, int, int, int, int, int, int); //�ж������Ƿ���Ϲ��� 
void human_decision(); //������ߣ����Ʋ˵�ѡ�Ϊ�˷���д���˺���
ter_posi king_move(int); //���� Kingmove�ƶ���ʽ�µ� territoryֵ 
double square(int, int); //�����ݴη� 
ter_posi queen_move(int); //���� Queenmove�ƶ���ʽ�µ� territoryֵ 
void determine_4(int); //AI�Ѷ�Ϊ4
void determine_3(int); //AI�Ѷ�Ϊ3
void determine_2(int); //AI�Ѷ�Ϊ2
void determine_1(int); //AI�Ѷ�Ϊ1
void AI_decision(int); //AI���ߺ���
void save(); // ���̺���
void regret(); //���庯��
void AI_play(int); //�йܺ���
void play_with_yourself(); //�Լ����Լ����� 
void new_game(); //����Ϸ��������AI��һ���� 
void continue_game(); // ������Ϸ����������һ�ֺ����е���
void last_game(); // ��һ�ֺ���
void play_game(); // ����������Ϸ���� 

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

void DrawTheBoard()//�����̺�����ֱ�ӵ��ü���
{
	system("cls");
	cout << endl << endl << "	     THE AMAZONS" << endl << endl;
	cout << "     0   1   2   3   4   5   6   7" << endl;
	cout << "  ���������Щ������Щ������Щ������Щ������Щ������Щ������Щ�������" << endl;
	for (int i = 0; i < 7; i++)
	{
		cout << " " << i;
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == 1)
				cout << "�� " << setw(2) << blackone;
			else if (board[i][j] == -1)
				cout << "�� " << setw(2) << whiteone;
			else if (board[i][j] == 2)
				cout << "�� " << setw(2) << obstacleone;
			else if (board[i][j] == 11)
				cout << "�� " << setw(2) << newblackone;
			else if (board[i][j] == 9)
				cout << "�� " << setw(2) << newwhiteone;
			else if (board[i][j] == 12)
				cout << "�� " << setw(2) << newobstacleone;
			else
				cout << "�� " << setw(2) << ' ';
		}
		cout << "�� " << i << "  ";
		cout << endl;
		cout << "  ���������੤�����੤�����੤�����੤�����੤�����੤�����੤������" << endl;
	}
	cout << " 7";
	for (int j = 0; j < 8; j++)
	{
		if (board[7][j] == 1)
			cout << "�� " << setw(2) << blackone;
		else if (board[7][j] == -1)
			cout << "�� " << setw(2) << whiteone;
		else if (board[7][j] == 2)
			cout << "�� " << setw(2) << obstacleone;
		else if (board[7][j] == 11)
			cout << "�� " << setw(2) << newblackone;
		else if (board[7][j] == 9)
			cout << "�� " << setw(2) << newwhiteone;
		else if (board[7][j] == 12)
			cout << "�� " << setw(2) << newobstacleone;
		else
			cout << "�� " << setw(2) << ' ';
	}
	cout << "�� " << 7 << "  ";
	cout << endl;
	cout << "  ���������ة������ة������ة������ة������ة������ة������ة�������" << endl;
	cout << "     0   1   2   3   4   5   6   7" << endl;
	cout << endl;

	if (cycle != 0)
	{
		cout << "	      �غ�����" << cycle << endl;
		if (mode == 1 || mode ==2)
		{
			if (me == 1)
				cout << "���Ǻڷ�" << endl;
			else if(me == -1)
				cout << "���ǰ׷�" << endl;
			if (watch)
			{
				cout << "	      �й���";
				for (int i = 0; i <= cycle % 4; i++)
					cout << ".";
				cout << endl;
			}
			else 
			{
				cout << "Press E (�˳�)	Press H (��ʾ) Press R (����)" << endl;
				cout << "Press T (���¿�ʼ) Press B (�й�)" << endl;
				cout << "����������" << endl;
			}
		}
	}
	cycle++; //ÿ��һ�����̣��غ��� +1 
}//�����̺�����ֱ�ӵ��ü���

bool inMap(int x, int y)//�жϸ������Ƿ��������� 
{
	if (x < 0 || x > 7 || y < 0 || y > 7)
		return false;
	return true;
}//�жϸ������Ƿ��������� 

bool check(int color)//���ĳ����ɫ�Ƿ�������� 
{
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			if (board[i][j] != color)
				continue;
			for (int k = 0; k < 8; k++) //�ҵ�һ�����Ӻ���˸�����������ֻҪ���߾��ܷ��ϰ�
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
}//���ĳ����ɫ�Ƿ�������� 

int mobility(int color)//����ĳ����ɫ�������ܹ�����Ŀո����������Ϊ���ȵĽ��
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
				totalmin = total_one;//������С���ȣ���ֹһ��ʼ��ʱ��ĳ�������ȱ�����
		}
	}
	return total + totalmin;
}//����ĳ����ɫ�������ܹ�����Ŀո������ 

void PutChess(int x0, int y0, int x1, int y1, int x2, int y2, int color)//���Ӻ��������ӵĹ�����ȫ�ֱ�����¼��������� 
{
	record[cycle][1] = x0; record[cycle][2] = y0;
	record[cycle][3] = x1; record[cycle][4] = y1;
	record[cycle][5] = x2; record[cycle][6] = y2;
	board[x0][y0] = 0;
	board[x1][y1] = color + 10;
	board[x2][y2] = new_obstacle;
}//���Ӻ��������ӵĹ�����ȫ�ֱ�����¼���������

void turn_normal(int x1, int y1, int x2, int y2)//�������ǵ����Ӻ��ϰ��������
{
	board[x1][y1] -= 10;
	board[x2][y2] = OBSTACLE;
}// �����ǵ����Ӻ��ϰ�������� 

void ClearChess(int x0, int y0, int x1, int y1, int x2, int y2, int color)//���ģ�����Ӻ��� 
{
	board[x2][y2] = 0;
	board[x1][y1] = 0;
	board[x0][y0] = color;//ע����ݵ�˳�򣬺���Ҫ��
}//���ģ�����Ӻ���

bool legal(int x0, int y0, int x1, int y1, int x2, int y2, int color)//�ж������Ƿ�Ϸ� 
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
}//�ж������Ƿ�Ϸ�

bool obey_the_rule(int x0, int y0, int x1, int y1, int x2, int y2)//�ж������Ƿ���Ϲ��� 
{
	if (x0 != x1 && y0 != y1 && x0 + y0 != x1 + y1 && x0 - y0 != x1 - y1)//�����Ƿ���һ��ֱ�����ƶ�
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
			if (i == y0 && x2 == x0) continue;//���Խ���ƶ�ǰ��λ�ã�ֱ��continue
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
}//�ж������Ƿ���Ϲ���

void human_decision()//������ߺ�����ѡ��ģʽ������
{
	char condition;
	int x0, y0, x1, y1, x2, y2;
	cin >> condition;//����ĵ�һ����������Ϊ�ַ��ͣ��û�����ȽϺ�
	while (true)
	{
		if (condition == 'H' || condition == 'h')//��ʾ
		{
			now_step++;
			determine_1(me);
			return;
		}
		else if (condition == 'E' || condition == 'e')//�����˳�
		{
			save();
			exit(0);
		}
		else if ((condition == 'R' || condition == 'r') && cycle >= 4 && now_step > 0)//����
		{
			regret();
			return;
		}
		else if (condition == 'T' || condition == 't')//restart���¿�ʼ������ѡ����Ϸģʽ
		{
			save();
			for (int i = 0; i < GRIDSIZE; i++)
				for (int j = 0; j < GRIDSIZE; j++)
					board[i][j] = 0;
			play_game();
		}
		else if (condition == 'B' || condition == 'b')//�й�
		{
			int your_cycle;
			cout << "��������Ҫ�йܵĻغ���" << endl;
			cin >> your_cycle;
			AI_play(2 * your_cycle);
			return;
		}
		else if (condition >= '0' && condition <= '7')//����
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
			now_step++;//���ӵĻغ���+1
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
}//������ߺ�����ѡ��ģʽ������

ter_posi king_move(int color)//Kingmove�߷���territory��positionֵ����������Ϊ�ṹ��
{
	int step[8][8][4] = { 0 };//0��ʾ��ɫ��1��ʾ��ɫ��2��ʾ territoryֵ ��3��ʾ positionֵ 
	ter_posi king_result;
	king_result.ter = 0;
	king_result.posi = 0;

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			for (int k = 0; k < 2; k++)
				step[i][j][k] = 100;//��ȫ����ʼ��Ϊ100

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
					step[i][j][which_one] = -1;//�Է�ռ�еĸ��ӣ��ҷ���Զ�޷�����
				if (board[i][j] == nowcolor)
				{
					step[i][j][which_one] = 0;//�ҷ�ռ�еĸ��ӣ���Ҫ�Ĳ���Ϊ0
					steps = 1;
					for (int k = 0; k < 8; k++)
					{
						int xx = i + dx[k];
						int yy = j + dy[k];
						if (!inMap(xx, yy) || board[xx][yy] != 0)
							continue;
						step[xx][yy][which_one] = steps;//Ѱ����Ҫ1�����Ե���ĸ���
					}
				}
			}
		}
		int mark;//����Ƿ����������Ե���ĸ���
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
					for (int k = 0; k < 8; k++)//�ӵ�ǰ�����ɵ���ĸ��ӳ���Ѱ����һ���ɵ���ĸ���
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

	for (int i = 0; i < 8; i++)//����ÿһ�����ӣ����չ�ʽ����territory��positionֵ
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
}//Kingmove�߷���territory��positionֵ����������Ϊ�ṹ��

double square(int x, int y)//����x��y����
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
} // ���� x�� y���� 

ter_posi queen_move(int color)//�������ÿ��������queen�߷�����Ĳ��������巽����king_move������ͬ
{
	double step[8][8][4] = { 0 };//0��ʾ��ɫ��1��ʾ��ɫ��2��ʾterritoryֵ��3��ʾpositionֵ
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
}//�������ÿ��������queen�߷�����Ĳ���  

void determine_4(int color)// ѡ���Ѷ�ϵ��Ϊ4������mobility��territory��position��Ϊ������25�غϺ��֦������
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
			for (int k1 = 0; k1 < 8; k1++)//�ҵ���ǰ��������λ�ã���˸������������з���
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
								PutChess(i, j, xx, yy, xxx, yyy, color);//ģ�����Ӻ���
								turn_normal(xx, yy, xxx, yyy);
								pos_begin[pos_method1][0] = i;
								pos_begin[pos_method1][1] = j;
								pos_put[pos_method1][0] = xx;
								pos_put[pos_method1][1] = yy;
								pos_obs[pos_method1][0] = xxx;
								pos_obs[pos_method1][1] = yyy;//��¼ģ������ӷ���

								if (cycle > 25)//����С����+��֦
								{
									int pos_begin2[3000][2] = { 0 }, pos_put2[3000][2] = { 0 }, pos_obs[3000][2] = { 0 };
									double minvalue = 300000;
									int flag = 1;
									//�����Է��Ŀ��з���
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
																turn_normal(xx2, yy2, xxx2, yyy2);//ģ��Է�����
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

															int n1, n2, n3, n4;//�������ջغ����仯
															if (cycle <= 12)
															{
																n1 = 18; n2 = 40; n3 = 20; n4 = 78;
															}
															else if (cycle < 42)
															{
																n1 = 7; n2 = 6; n3 = 2; n4 = 24;
															}
															else { n1 = 9; n2 = 2; n3 = 1; n4 = 30; }

															double now_value = n1 * now_queen_t + n2 * now_king_t + n3 * now_mob + n4 * now_p;//���㵱ǰ�ҷ���ֵ��ȥ�Է���ֵ
															ClearChess(ii, jj, xx2, yy2, xxx2, yyy2, -color);
															if (now_value < minvalue)
																minvalue = now_value;//�����ҷ�ÿһ���߷���Ӧ�Է����ߺ��ҵ���С��ֵ

															if (minvalue < maxvalue)//��֦
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

									if (minvalue > maxvalue)//�Է��Զ�������ķ�ʽ���ߺ��ҵ�����ֵ
									{
										maxvalue = minvalue;
										choice = pos_method1;
									}
									pos_method1++;
									ClearChess(i, j, xx, yy, xxx, yyy, color);
								}
								else//25�غ������ڿ����߷�̫�࣬Ϊ׷��ʱ��Ч�ʣ�ֻ����һ��������̰��
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
}// ѡ���Ѷ�ϵ��Ϊ4������mobility��territory��position��Ϊ������25�غϺ��֦������

void determine_3(int color)//ѡ���Ѷ�ϵ��Ϊ3������mobility��territoryΪ����������һ��
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
}//ѡ���Ѷ�ϵ��Ϊ3������mobility��territoryΪ����������һ��

void determine_2(int color)//ѡ���Ѷ�ϵ��Ϊ2������mobilityΪ����������һ��
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
}//ѡ���Ѷ�ϵ��Ϊ2������mobilityΪ����������һ��

void determine_1(int color)//ѡ���Ѷ�ϵ��Ϊ2��������ӣ�����һ��
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
	choice = rand() % pos_method;//���������
	PutChess(pos_begin[choice][0], pos_begin[choice][1], pos_put[choice][0],
		pos_put[choice][1], pos_obs[choice][0], pos_obs[choice][1], color);
	DrawTheBoard();
	turn_normal(pos_put[choice][0],
		pos_put[choice][1], pos_obs[choice][0], pos_obs[choice][1]);
}//ѡ���Ѷ�ϵ��Ϊ2��������ӣ�����һ��

void AI_decision(int color)//AI���ߺ�����AI��ǿ���뵱ǰѡ����Ѷ��й�
{
	if (difficulty == 4)	determine_4(color);
	else if (difficulty == 3)	determine_3(color);
	else if (difficulty == 2)	determine_2(color);
	else if (difficulty == 1)	determine_1(color);
}//AI���ߺ�����AI��ǿ���뵱ǰѡ����Ѷ��й�

void save()//���̺���
{
	ofstream fout("read_save.txt");
	fout << me << endl;//����ҷ���ɫ
	for (int i = 0; i < GRIDSIZE; i++)//�����ǰ����״̬
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			if(board[i][j] < 5)
				fout << board[i][j] << " ";
			else fout << board[i][j] - 10 << " ";
		}
		fout << endl;
	}
	fout << cycle - 1 << endl;//�����ǰ�غ���
	fout << difficulty;//���ѡ����Ѷ�
	fout.close();
}//���̺���

void regret()//���庯��
{
	now_step--;//�����Լ�������Ĳ�����һ
	cycle--;//ÿ�λ��峷�����غ�
	ClearChess(record[cycle][1], record[cycle][2], record[cycle][3], record[cycle][4], record[cycle][5], record[cycle][6], -me);
	cycle--;
	ClearChess(record[cycle][1], record[cycle][2], record[cycle][3], record[cycle][4], record[cycle][5], record[cycle][6], me);
	cycle--;
	if (cycle >= 2 && now_step > 0)//����һ���غ���AI�ľ������ֳ���������ǰһ�غϣ�������
	{
		board[record[cycle][3]][record[cycle][4]] += 10;
		board[record[cycle][5]][record[cycle][6]] += 10;
		DrawTheBoard();
		turn_normal(record[cycle - 1][3], record[cycle - 1][4], record[cycle - 1][5], record[cycle - 1][6]);
	}
	else DrawTheBoard();
	human_decision();
}//���庯��

void AI_play(int your_cycle)//�йܺ���
{
	int player = me;
	watch = 1;//���״̬Ϊ�й���
	while (your_cycle--)
	{
		if (player == me)
			determine_1(player);//�йܵ�ȻҪ����˵�AI��˭��������
		if (player == -me)
			AI_decision(player);

		player = player * (-1);
		if (!check(player))
		{
			cout << "Game Over" << endl;
			if (player == me)
				cout << "�����AI�� TAT" << endl;
			else cout << "��ϲ��սʤ�˴��ȵ�AI ����" << endl;
			cout << "goodbye" << endl;
			exit(0);
		}
		Sleep(500);//ÿ��һ��ͣ0.5�룬��������Ŀ���������ô����
	}
	cycle--;
	watch = 0;
	continue_game();//�йܽ�����ɼ�����Ϸ
}//�йܺ���

void play_with_yourself()//�Լ����Լ���һ����
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
}//�Լ����Լ���һ����

void new_game()//����Ϸ��������AI��һ���� 
{
	PlaySound(TEXT("D:\\Dusk.wav"), NULL, SND_FILENAME | SND_ASYNC);
	cycle = 0;
	DrawTheBoard();
	cout << "	     ��ѡ���Ѷ�" << endl;
	cout << "     ���ȣ�Press 1  ���У�Press 2" << endl;
	cout << "���ࣺPress 3	�������ѧ�ң�Press 4" << endl;
	while (true)
	{
		cin >> difficulty;//ѡ���Ѷ�
		if (difficulty == 1 || difficulty == 2 || difficulty == 3 || difficulty == 4)
			break;
		cin.clear();
		cin.ignore();
		cycle = 0;
		DrawTheBoard();
		cout << "  	   ������ѡ���Ѷ�" << endl;
		cout << "     ���ȣ�Press 1  ���У�Press 2" << endl;
		cout << "���ࣺPress 3	�������ѧ�ң�Press 4" << endl;
	}
	cycle = 0;
	DrawTheBoard();

	cout << "	please choose your color" << endl;
	cout << "  black (press B) or white (press W)" << endl;
	char choose;
	cin >> choose;//ѡ���ҷ�ִ����ɫ
	cycle = 0;
	DrawTheBoard();
	cycle++;

	while (true)
	{
		while (true)
		{
			if (choose == 'B' || choose == 'b')
			{
				cout << "	      �غ�����1" << endl << "���Ǻڷ�" << endl;
				cout << "Press E (�˳�)	Press H (��ʾ) Press R (����)" << endl;
				cout << "Press T (���¿�ʼ) Press B (�й�)" << endl;
				cout << "����������" << endl;
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
				cout << "������ѡ����Ҫִ�ӵ���ɫ" << endl;
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
					cout << "�����AI�� TAT" << endl;
				else cout << "��ϲ��սʤ�˴��ȵ�AI ����" << endl;
				cout << "goodbye" << endl;
				exit(0);
			}
		}
	}
}//����Ϸ��������AI��һ����

void continue_game()//������Ϸ����
{
	int player = -1;
	int x0, y0, x1, y1, x2, y2;
	DrawTheBoard();
	if (cycle % 2 == 0)
		player = 1;//�ж����ڵ����

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
				cout << "�����AI�� TAT" << endl;
			else cout << "��ϲ��սʤ�˴��ȵ�AI ����" << endl;
			cout << "goodbye" << endl;
			exit(0);
		}
	}
}//������Ϸ����

void last_game()//��һ����Ϸ����������
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
}//��һ����Ϸ����

void play_game()//����������Ϸ����
{
	printf("\a");
	now_step = 0;
	board[0][2] = 1; board[0][5] = 1; board[2][0] = 1; board[2][7] = 1;
	board[5][0] = -1; board[5][7] = -1; board[7][2] = -1; board[7][5] = -1;//��ʼ������
	cycle = 0;
	DrawTheBoard();
	cout << "   	  Welcome to Amazon!" << endl;
	cout << "	�� means black chess." << endl << " 	�� means white chess." << endl;
	cout << "	 �� means obstacle." << endl << endl;
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
			cout << "		������ѡ����Ϸģʽ" << endl;
			cout << "      play with yourself (press M)" << endl;
			cout << "      	  new game (press N)" << endl;
			cout << "    	 last game  (press L)" << endl;
		}
	}
}//����Ϸ����

int main()
{
	system("mode con cols=80 lines=35");//���ڿ�ȸ߶�
	system("color 0E0");//������ɫ
	play_game();
	return 0;
}