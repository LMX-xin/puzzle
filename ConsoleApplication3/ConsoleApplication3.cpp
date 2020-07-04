#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <ctime>
#include <fstream>
#include <cassert>
#include <string>



using namespace std;

bool isEqual(vector<int> start, vector<int> end);

class Node
{
public:
	vector<int> num;
	int fX, gX, hX;//hX为启发函数的值即为不在正确位的数量，fX为评估函数的值，gX为节点的深度
	int nowId, parentId;//nowid为当前状态的位置，parentid为上一个状态的位置
	Node(vector<int> num, int fX, int gX, int hX, int nowId, int parentId)
	{
		this->num = num;
		this->fX = fX;
		this->gX = gX;
		this->hX = hX;
		this->nowId = nowId;
		this->parentId = parentId;
	}
	
	bool operator==(const Node &now) const
	{
		return isEqual(num, now.num);
	}
};

vector<Node> open;
vector<Node> path;
map<vector<int>, bool> close;


int HX(vector<int> start, vector<int> end)//找出不在正确位置的数量
{
	int hX = 0;
	for (int i = 0; i < 9; i++)
	{
		if (start[i] != end[i])
		{
			hX++;
		}
	}
	return hX;
}

int FX(Node &now)//评估函数f=h+g;
{
	return now.hX + now.gX;
}

bool isEqual(vector<int> start, vector<int> end)//判断当前数字方阵位置是否与目标位置一样，是的话则为true，否的话则为false；
{
	/*
	for (int i = 0; i < 9; i++)
	{
		if (start[i] != end[i])
		{
			return false;
		}
	}
	return true;
	*/
	return HX(start, end) == 0;
}

int zeroLocation(vector<int> num)//指出零所在的位置;
{
	for (int i = 0; i < 9; i++)
	{
		if (num[i] == 0)
		{
			return i;
		}
	}
}

int getInversions(vector<int> num)//找出除0之外的逆序数之和
{
	int count = 0;
	for (int i = 0; i < 9; i++)
	{
		if (num[i] == 0)//排除零的逆序数
		{
			continue;
		}
		for (int j = i; j < 9; j++)
		{
			if (num[j] != 0 && num[i] > num[j])//坐标i之前且比num[i]大的数字，都是num[i]的逆序数（除零之外）
			{
				count++;
			}
		}
	}
	return count;
}

bool hasSolution(vector<int> start, vector<int> end)//运用前面的求逆序之和的方法，求是否有解，有则为true，没有的话则为false；
{
	int inverStart, inverEnd;
	inverStart = getInversions(start);
	inverEnd = getInversions(end);
	return inverStart % 2 == inverEnd % 2;//两组数组的逆序之和奇偶性相同，则有解，否则没有
}

void printMatrix(vector<int> num)//输出二维数组;
{
	for (int i = 0; i < 7; i += 3)
	{
		for (int j = i; j < i + 3; j++)
		{
			cout << num[j] << " ";
		}
		cout << endl;
	}
}


int AStar(vector<int> start, vector<int> end)
{
	int id = 0;
	vector<int> move = { -3, -1, 3, 1 };//创建移动步长数组
	open.push_back(Node(start, HX(start, end), 0, HX(start, end), id, id++));//保存初始状态至open表
	while (!open.empty())//如果open表中不为空，从中选取最小的fX值的节点
	{
		auto min = min_element(open.begin(), open.end(), [](const Node &x, const Node &y)-> bool
		{
			return x.fX < y.fX;
		});
		Node now = *min;//创建节点now，将min的值赋给now，到地址没有给，因为erase（）方法的参数为指针变量，所以now只能得到到值，而没有给地址；
		open.erase(min);//从open表中删除最小项，即为now节点
		path.push_back(now);//将当前的节点，保存到路径表中
		close[now.num] = true;//将close表中对应的数组标记为true，即为已经访问过
		if (isEqual(now.num, end))//当前节点的数组是否为目的数组
		{
			return now.gX;
		}
		int zeroL = zeroLocation(now.num);//找到0的方位
		int newLocation = 0;
		for (int i = 0; i < 4; i++)
		{
			newLocation = zeroL + move[i];//新的位置为0所在的位置向上下左右，四个方向移动得到的
			if (newLocation > -1 && newLocation < 9//排除几种不能移动的情况
				&& !(newLocation == 2 && zeroL == 3)
				&& !(newLocation == 3 && zeroL == 2)
				&& !(newLocation == 5 && zeroL == 6)
				&& !(newLocation == 6 && zeroL == 5))
			{
				swap(now.num[newLocation], now.num[zeroL]);//0与相邻的数字交换位置
				Node newState = Node(now.num, 0, now.gX + 1, HX(now.num, end), id++, now.nowId);//新建一个节点保存交换后的状态
				swap(now.num[newLocation], now.num[zeroL]);//因为min没有将地址也给now节点，所以不能改变now节点的信息
				newState.fX = FX(newState);
				if (isEqual(newState.num, end))//如果当状态与目标数组一样，则保存节点到path
				{
					path.push_back(newState);
					return newState.gX;//当前节点的层数
				}
				if (!close.count(newState.num))//如果close表中没有当前新节点，则保存到open表中；
				{
					open.push_back(newState);
				}
			}
		}
		
	}

}

void printPath(int parentId, int stateSize, int steps)//打印初始状态到目的状态经过的路径
{
	if (steps == 0)
	{
		cout << "step[" << steps << "] -->" << endl;
		printMatrix(path[stateSize].num);
		return;
	}
	for (int i = stateSize; i > 0; --i)
	{
		if (path[i].nowId == parentId)
		{
			printPath(path[i].parentId, i, steps - 1);
		}
	}
	cout << "step[" << steps << "] -->" << endl;
	printMatrix(path[stateSize].num);
}

int main()
{
	vector<int> start(9), end(9);
	clock_t startTime, endTime;
	ifstream infile;
	infile.open("test.txt", ios::in);
	for (int i = 0;i < 9;i++) {
		infile >> start[i];
	}
	infile.close();
	cout << "Input start:"  << endl;
	printMatrix(start);
	cout << "------------------" << endl;
	ifstream infile1;
	infile1.open("aim.txt", ios::in);
	for (int i = 0;i < 9;i++) {
		infile1 >> end[i];
	}
	infile1.close();
	cout << "Input end:" << endl;
	printMatrix(end);
	cout << "------------------" << endl;
	
	startTime = clock();
	if (hasSolution(start, end))
	{
		int steps = AStar(start, end);//将g赋值给steps
		printPath((path.end() - 1)->parentId, path.size() - 1, steps);//end()方法是指向向量最后一个元素的下一个位置，因此需要减一；
		cout << "steps:" << steps << endl;
		cout << "Has solution" << endl;
	}
	else
	{
		cout << "No solution" << endl;
	}
	endTime = clock();
	cout << "Total Time:" << endTime - startTime << "ms" << endl;
	return 0;
}
/*
Input start:
0 1 2
3 4 5
6 7 8
------------------
Input end:
1 2 3
4 5 6
7 8 0
------------------
step[1] -->
1 0 2
3 4 5
6 7 8
step[2] -->
1 4 2
3 0 5
6 7 8
step[3] -->
1 4 2
0 3 5
6 7 8
step[4] -->
1 4 2
6 3 5
0 7 8
step[5] -->
1 4 2
6 3 5
7 0 8
step[6] -->
1 4 2
6 3 5
7 8 0
step[7] -->
1 4 2
6 3 0
7 8 5
step[8] -->
1 4 2
6 0 3
7 8 5
step[9] -->
1 4 2
0 6 3
7 8 5
step[10] -->
1 4 2
7 6 3
0 8 5
step[11] -->
1 4 2
7 6 3
8 0 5
step[12] -->
1 4 2
7 0 3
8 6 5
step[13] -->
1 0 2
7 4 3
8 6 5
step[14] -->
1 2 0
7 4 3
8 6 5
step[15] -->
1 2 3
7 4 0
8 6 5
step[16] -->
1 2 3
7 4 5
8 6 0
step[17] -->
1 2 3
7 4 5
8 0 6
step[18] -->
1 2 3
7 4 5
0 8 6
step[19] -->
1 2 3
0 4 5
7 8 6
step[20] -->
1 2 3
4 0 5
7 8 6
step[21] -->
1 2 3
4 5 0
7 8 6
step[22] -->
1 2 3
4 5 6
7 8 0
steps:22
Has solution
Total Time:27930ms

Input start:
1 2 4
6 7 3
5 8 0
------------------
Input end:
1 2 3
4 5 0
8 7 6
------------------
step[0] -->
1 2 4
6 7 0
5 8 3
step[1] -->
1 2 4
6 7 0
5 8 3
step[2] -->
1 2 0
6 7 4
5 8 3
step[3] -->
1 0 2
6 7 4
5 8 3
step[4] -->
1 7 2
6 0 4
5 8 3
step[5] -->
1 7 2
6 8 4
5 0 3
step[6] -->
1 7 2
6 8 4
0 5 3
step[7] -->
1 7 2
0 8 4
6 5 3
step[8] -->
1 7 2
8 0 4
6 5 3
step[9] -->
1 7 2
8 4 0
6 5 3
step[10] -->
1 7 2
8 4 3
6 5 0
step[11] -->
1 7 2
8 4 3
6 0 5
step[12] -->
1 7 2
8 4 3
0 6 5
step[13] -->
1 7 2
0 4 3
8 6 5
step[14] -->
1 7 2
4 0 3
8 6 5
step[15] -->
1 0 2
4 7 3
8 6 5
step[16] -->
1 2 0
4 7 3
8 6 5
step[17] -->
1 2 3
4 7 0
8 6 5
step[18] -->
1 2 3
4 7 5
8 6 0
step[19] -->
1 2 3
4 7 5
8 0 6
step[20] -->
1 2 3
4 0 5
8 7 6
step[21] -->
1 2 3
4 5 0
8 7 6
steps:21
Has solution
Total Time:12003ms
*/

