#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<queue>
#include<iostream>
#include<algorithm>
#include<windows.h>

using namespace std;

int dx[4] = { 0, 0, 1, -1 };
int dy[4] = { 1, -1, 0, 0 };
int arr[4] = { 0,0,0,0 };
vector<vector<int>> block;
vector<vector<int>> old_block;
int min_val = 9999999;
vector<int> answer;

void block_game();
void erase(int x, int y,int W, int H);
void re_build(int W, int H);
int search(int W, int H);
void cal_block_num(int W, int H);

int main() {
	
	int T;
	
	cin >> T;

	for (int i = 0; i < T; i++) {
		block_game();
		answer.push_back(min_val);
		min_val = 9999999;
		block.clear();
		old_block.clear();
		for (int i = 0; i < 4; i++) arr[i] = 0;
	}

	for (int i = 0; i < answer.size(); i++) {
		printf("#%d %d\n", i + 1, answer[i]);
	}

	return 0;
}

void block_game() {
	int N, W, H;
	int x, y;

	cin >> N >> W >> H;
	block = vector<vector<int>>(H, vector<int>(W, 0));
	old_block = vector<vector<int>>(H, vector<int>(W, 0));

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			cin >> block[i][j];
			old_block[i][j] = block[i][j];
		}
	}

	while (true) {
		for (int i = 0; i < N; i++) {
			x = arr[i];
			y = search(x , H);
			if (y == H) continue;
			erase(x, y, W, H);
			re_build(W, H);
		}
		cal_block_num(W, H);
		if (min_val == 0) break;
		arr[N - 1]++;
		if (N - 1 > 0 && arr[N - 1] == W) {
			arr[N - 1] = 0;
			arr[N - 2]++;
			if (N - 2 > 0 && arr[N - 2] == W) {
				arr[N - 2] = 0;
				arr[N - 3]++;
				if (N - 3 > 0 && arr[N - 3] == W) {
					arr[N - 3] = 0;
					arr[N - 4]++;
				}
			}
		}
		if (arr[0] == W) break;

		block = old_block;
	}

}

void erase(int x, int y, int W, int H) {

	int cnt = block[y][x];
	if (block[y][x] == 0) return;
	if (block[y][x] == 1) {
		block[y][x] = 0;
		return;
	}
	block[y][x] = 0;
	for (int j = 1; j < cnt; j++) {
		for (int i = 0; i < 4; i++) {
			if (x + dx[i]*j < W && x + dx[i]*j >= 0 && y + dy[i]*j < H && y + dy[i]*j >= 0) {
				erase(x + dx[i] * j, y + dy[i] * j, W, H);
			}
		}
	}
}

void re_build(int W, int H) {
	queue<int> q;
	int num;

	for (int i = 0; i < W; i++) {
		for (int j = 0; j < H; j++) {
			if (block[j][i] != 0) {
				q.push(block[j][i]);
			}
		}
		for (int k = 0; k < H; k++) {
			block[k][i] = 0;
		}
		num = H - q.size();
		while (!q.empty()) {
			block[num][i] = q.front();
			q.pop();
			num++;
		}
	}
}

int search(int W, int H) {

	for (int i = 0; i < H; i++) {
		if (block[i][W] != 0) {
			return i;
		}
	}

	return H;
}

void cal_block_num(int W, int H) {
	int cnt = 0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (block[i][j] != 0) cnt++;
		}
	}


	if (cnt < min_val) min_val = cnt;
}


