#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<stack>
#include<vector>
#include<algorithm>
#include<queue>

#define Dead	 0
#define Active   1
#define Disabled 2

using namespace std;

typedef struct Cell_Info {
	int v; //방문여부
	int l; //세포 생명
	int c; //변해가는 세포
	int s; //상태
}Cell;

int T;
int N, M, K;
int q_size;
int dx[4] = { 0, 0, -1, 1 };
int dy[4] = { 1, -1, 0, 0 };
pair<int, int> tmp;
vector<vector<Cell>> plate;
queue<pair<int, int>> q;
vector<pair<int, int>> visit;
vector<int> result;

int main() {

	cin >> T;

	for (int i = 0; i < T; i++) {
		cin >> N >> M >> K;

		plate = vector<vector<Cell>>(K + 50, vector<Cell>(K + 50));

		for (int j = 0; j < N; j++) {
			for (int k = 0; k < M; k++) {
				cin >> plate[K / 2 + j][K / 2 + k].c;
				if (plate[K / 2 + j][K / 2 + k].c != 0) {
					plate[K / 2 + j][K / 2 + k].l = plate[K / 2 + j][K / 2 + k].c;
					plate[K / 2 + j][K / 2 + k].s = Disabled;
					plate[K / 2 + j][K / 2 + k].v = 1;
					q.push(make_pair(K / 2 + j, K / 2 + k));
				}
			}
		}

		for (int a = 0; a < K; a++) {
			q_size = q.size();

			for (int k = 0; k < q_size; k++) {
				tmp = q.front();
				q.pop();
				if (plate[tmp.first][tmp.second].c == 0 && plate[tmp.first][tmp.second].s == Active) {
					for (int j = 0; j < 4; j++) {
						if (plate[tmp.first + dx[j]][tmp.second + dy[j]].v == 0
							&& plate[tmp.first][tmp.second].l > plate[tmp.first + dx[j]][tmp.second + dy[j]].l) {
							visit.push_back(make_pair(tmp.first + dx[j], tmp.second + dy[j]));
							plate[tmp.first + dx[j]][tmp.second + dy[j]].l = plate[tmp.first][tmp.second].l;
							plate[tmp.first + dx[j]][tmp.second + dy[j]].c = plate[tmp.first][tmp.second].l;
							plate[tmp.first + dx[j]][tmp.second + dy[j]].s = Disabled;
						}
					}
				}
				if (plate[tmp.first][tmp.second].s != Dead) {
					plate[tmp.first][tmp.second].c--;
					if (plate[tmp.first][tmp.second].c == 0) plate[tmp.first][tmp.second].s = Active;
					else if (plate[tmp.first][tmp.second].c == plate[tmp.first][tmp.second].l * -1)
						plate[tmp.first][tmp.second].s = Dead;
				}
				if (plate[tmp.first][tmp.second].s != Dead)
					q.push(tmp);
			}

			for (int m = 0; m < visit.size(); m++) {
				q.push(visit[m]);
				plate[visit[m].first][visit[m].second].v = 1;	
			}
			visit.clear();
		}

		result.push_back(q.size());
		while (!q.empty()) q.pop();		
	}

	for (int i = 0; i < result.size(); i++) {
		printf("#%d %d\n", i + 1, result[i]);
	}

	return 0;
}