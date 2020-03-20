#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<vector>
#include<deque>
#include<algorithm>
#include<string.h>

using namespace std;

int T, D, W, K;
int map[13][20];
int origin_map[13][20];
int cnt, tmp;
vector<bool> test;
vector<int> result;
int ans = 9999;

void dfs(int depth, int cnt);
void check(int cnt);

int main() {
	cin >> T;
	for (int t = 0; t < T; t++) {
		cin >> D >> W >> K;
		memset(map, 0, sizeof(int) * 13 * 20);
		memset(origin_map, 0, sizeof(int) * 13 * 20);
		test.assign(W, false);

		for (int i = 0; i < D; i++) {
			for (int j = 0; j < W; j++) {
				cin >> map[i][j];
				origin_map[i][j] = map[i][j];
			}
		}

		dfs(0, 0);

		result.push_back(ans);
		ans = 9999;
	}

	for (int i = 0; i < T; i++) {
		printf("#%d %d\n", i + 1, result[i]);
	}

	return 0;
}

void dfs(int depth, int cnt) {
	if (cnt >= ans) return;
	if (depth == D) {
		check(cnt);
		return;
	}

	dfs(depth + 1, cnt);

	for (int i = 0; i < W; i++) map[depth][i] = 0;
	dfs(depth + 1, cnt + 1);
	for (int i = 0; i < W; i++) map[depth][i] = origin_map[depth][i];

	for (int i = 0; i < W; i++) map[depth][i] = 1;
	dfs(depth + 1, cnt + 1);
	for (int i = 0; i < W; i++) map[depth][i] = origin_map[depth][i];
}

void check(int cnt)
{
	for (int i = 0; i < W; i++) test[i] = false;

	for (int i = 0; i < W; i++) {
		for (int j = 0; j < D - 1; j++) {
			if (map[j][i] == map[j + 1][i]) tmp++;
			else tmp = 0;

			if (j == D - K && tmp == 0) break;

			if (tmp == K - 1) {
				test[i] = true;
				break;
			}
		}
		if (test[i] == false) return;
		tmp = 0;
	}

	ans = min(cnt, ans);
}