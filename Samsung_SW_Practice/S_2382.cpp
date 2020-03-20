#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<vector>
#include<deque>
#include<algorithm>
#include<string.h>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

using namespace std;

typedef struct Bug {
	pair<int, int> pos; //위치
	int num; // 미생물 수
	int way; //이동방향
}Bug;

int dx[5] = { 0, -1, 1, 0, 0 };
int dy[5] = { 0, 0, 0, -1, 1 };
int T, N, M, K;
vector<Bug> bug;
Bug tmp_bug;
int visit[100][100];
int map[100][100];
int bug_num[100][100];
int sum;
vector<int> result;

int main() {
	int x, y;
	cin >> T;

	for (int t = 0; t < T; t++) {
		cin >> N >> M >> K;
		bug.assign(K, tmp_bug);
		for (int k = 0; k < K; k++) {
			cin >> bug[k].pos.first >> bug[k].pos.second
				>> bug[k].num >> bug[k].way;
		}

		for (int m = 0; m < M; m++) {
			for (int i = 0; i < K; i++) {
				bug[i].pos.first += dx[bug[i].way];
				bug[i].pos.second += dy[bug[i].way];
				x = bug[i].pos.first;
				y = bug[i].pos.second;
				visit[x][y]++;

				if (x <= 0 || y <= 0 || x >= N-1 || y >= N-1) {
					bug[i].num /= 2;

					if (bug[i].num == 0) {
						bug[i--] = bug[K - 1];
						K--;
						continue;
					}

					if (bug[i].way == UP) bug[i].way = DOWN;
					else if (bug[i].way == DOWN) bug[i].way = UP;
					else if (bug[i].way == LEFT) bug[i].way = RIGHT;
					else if (bug[i].way == RIGHT) bug[i].way = LEFT;
				}//범위 밖으로 나갔을떄
				else {
					if (visit[x][y] == 1) {
						map[x][y] = i;
						bug_num[x][y] = bug[i].num;
					}
					else if (visit[x][y] > 1) {//미생물 합쳐지는경우
						bug[map[x][y]].num += bug[i].num;
						if (bug_num[x][y] < bug[i].num) {
							bug_num[x][y] = bug[i].num;
							bug[map[x][y]].way = bug[i].way;
						}
						bug[i--] = bug[K - 1];
						K--;
					}
				}
			}
			memset(visit, 0, sizeof(int) * 100 * 100);
		}
		for (int i = 0; i < K; i++) {
			sum += bug[i].num;
		}
		result.push_back(sum);
		sum = 0;
	}

	for (int t = 0; t < T; t++) {
		printf("#%d %d\n", t + 1, result[t]);
	}

	return 0;
}