#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<stack>
#include<vector>
#include<algorithm>
#include<queue>

using namespace std;

typedef struct Pin {
	pair<int, int> pos;
	int way;
	int cnt;
}Pin;

int T;
int N;
vector<vector<int>> board;
Pin P;
int dx[4] = { 0, -1, 1, 0 };
int dy[4] = { 1, 0, 0, -1 };
int num;
vector<int> result;
int max_cnt = 0;
int flag = 0;
int flag2 = 0;
int start = 0;

int main() {
	cin >> T;

	for (int a = 0; a < T; a++) {
		cin >> N;
		board = vector<vector<int>>(N + 2, vector<int>(N + 2, 5));

		for (int i = 1; i < N + 1; i++) {
			for (int j = 1; j < N + 1; j++) {
				cin >> board[i][j];
			}
		}

		for (int i = 1; i < N + 1; i++) {
			for (int j = 1; j < N + 1; j++) {
				if (board[i][j] != 0) continue;
				else {
					for (int k = 0; k < 4; k++) {
						P.pos = make_pair(i, j);
						P.cnt = 0;
						P.way = k;

						while (1) {
							if (flag == 1) {
								flag = 0;
								break;
							}
							P.pos.first += dx[P.way];
							P.pos.second += dy[P.way];

							num = board[P.pos.first][P.pos.second];

							if (P.pos.first == i && P.pos.second == j) {
								if (P.cnt > max_cnt) max_cnt = P.cnt;
								break;
							}

							switch (num) {
							case -1:
								if (P.cnt > max_cnt) max_cnt = P.cnt;
								flag = 1;
								break;
							case 0:
								break;
							case 1:
								if (P.way == 2 || P.way == 3) P.way -= 2;
								else if (P.way == 0) P.way = 3;
								else if (P.way == 1) P.way = 2;
								P.cnt++;
								break;
							case 2:
								if (P.way == 3 || P.way == 1) P.way -= 1;
								else if (P.way == 0) P.way = 3;
								else if (P.way == 2) P.way = 1;
								P.cnt++;
								break;
							case 3:
								if (P.way == 1 || P.way == 0) P.way += 2;
								else if (P.way == 2) P.way = 1;
								else if (P.way == 3) P.way = 0;
								P.cnt++;
								break;
							case 4:
								if (P.way == 0 || P.way == 2) P.way += 1;
								else if (P.way == 1) P.way = 2;
								else if (P.way == 3) P.way = 0;
								P.cnt++;
								break;
							case 5:
								if (P.way == 0) P.way = 3;
								else if (P.way == 1) P.way = 2;
								else if (P.way == 2) P.way = 1;
								else if (P.way == 3) P.way = 0;
								P.cnt++;
								break;
							default:
								for (int x = 1; x < N + 1; x++) {
									for (int y = 1; y < N + 1; y++) {
										if (board[x][y] == num) {
											if (x == P.pos.first && y == P.pos.second) continue;
											else {
												P.pos.first = x;
												P.pos.second = y;
												flag2 = 1;
												break;
											}
										}
									}
									if (flag2 == 1) {
										flag2 = 0;
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		result.push_back(max_cnt);
		max_cnt = 0;
	}

	for (int i = 0; i < result.size(); i++) {
		printf("#%d %d\n", i + 1, result[i]);
	}
}