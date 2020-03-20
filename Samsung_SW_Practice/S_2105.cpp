#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<vector>
#include<deque>
#include<algorithm>
#include<string.h>

using namespace std;

int T, N;
vector<vector<int>> map;
int dx[4] = { 1,1,-1,-1 };
int dy[4] = { 1,-1,-1,1 };
bool desert[101];
pair<int, int> s_pos; //start position
int way;
int max_val = -1;
int cnt;
vector<int> result;

void check(pair<int, int> pos, int way, int cnt);

int main() {
	cin >> T;
	for (int t = 0; t < T; t++) {
		cin >> N;
		map.assign(N, vector<int>(N, 0));
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				cin >> map[i][j];
			}
		}

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				s_pos = make_pair(i, j);
				desert[map[i][j]] = true;
				check(s_pos, 0, 1);
				desert[map[i][j]] = false;
			}
		}

		result.push_back(max_val);
		max_val = -1;
	}
	for (int i = 0; i < T; i++) {
		printf("#%d %d\n", i + 1, result[i]);
	}
}

void check(pair<int, int> pos, int way, int cnt) {
	int x = pos.first + dx[way];
	int y = pos.second + dy[way];

	if (way == 3 && s_pos.first == x && s_pos.second == y) {
		max_val = max(cnt, max_val);
		return;
	}

	if (x >= 0 && x < N && y >= 0 && y < N && !desert[map[x][y]]) {
		desert[map[x][y]] = true;
		check(make_pair(x, y), way, cnt + 1);
		if (way < 3) check(make_pair(x, y), way + 1, cnt + 1);
		desert[map[x][y]] = false;
	}
}