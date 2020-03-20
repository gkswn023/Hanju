#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<vector>
#include<queue>
#include<algorithm>
#include<string.h>

using namespace std;

int T, N, M, R, C, L;
vector<vector<int>> map;
//int pipe[8][4] = { {0,0,0,0}, {1,1,1,1}, {1,1,0,0},{0,0,1,1},{1,0,0,1},{0,1,0,1},{0,1,1,0},{1,0,1,0} };//╩С,го,аб,©Л
//int dx[4] = {-1,1,0,0};
//int dy[4] = {0,0,-1,1};
int pipe[8][4] = { {0,0,0,0}, { 1,1,1,1 },{ 0,1,0,1 },{ 1,0,1,0 },{ 0,1,1,0 },{ 0,0,1,1 },{ 1,0,0,1 },{ 1,1,0,0 } };
int dx[4] = { 0,-1,0,1 };
int dy[4] = { -1,0,1,0 };
queue<pair<int,int>> q;
pair<int, int> tmp;
int escape_time;
int possible;
int q_size;
int x, y;
vector<int> result;
vector<vector<int>> visit;

int main() {
	cin >> T;
	for (int t = 0; t < T; t++) {
		cin >> N >> M >> R >> C >> L;
		map.assign(N, vector<int>(M, 0));
		visit.assign(N, vector<int>(M, 0));
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				cin >> map[i][j];
			}
		}
		q.push(make_pair(R, C));
		while (!q.empty()) {
			if (escape_time == L) break;
			q_size = q.size();
			for (int a = 0; a < q_size; a++) {
				tmp = q.front();
				q.pop();
				
				if (visit[tmp.first][tmp.second] == 0) {
					possible++;
					visit[tmp.first][tmp.second] = 1;
					for (int i = 0; i < 4; i++) {
						if (pipe[map[tmp.first][tmp.second]][i] == 1) {
							x = tmp.first + dx[i];
							y = tmp.second + dy[i];
							if (x >= 0 && x < N && y >= 0 && y < M && map[x][y] && !visit[x][y]) {
								q.push(make_pair(x, y));
							}
						}
					}
				}
			}
			escape_time++;
		}
		result.push_back(possible);
		possible = 0;
		escape_time = 0;
		while(!q.empty()) q.pop();
	}
	for (int i = 0; i < T; i++) {
		printf("#%d %d\n", i + 1, result[i]);
	}

	cin >> T;
}