#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<vector>
#include<algorithm>
#include<string.h>

using namespace std;

typedef struct people {
	pair<int, int> pos;
	vector<int> W;
	bool B[8]; //사용할 수 있는 배터리
}People;

typedef struct Battery {
	pair<int, int> pos;
	int C; //충전 범위
	int P; //처리량
}Battery;

People p[2];
Battery B[8];
int dx[5] = {0, 0,1,0,-1 };
int dy[5] = {0, -1,0,1,0 };
int T, M, A;
int x, y, tmp;
int flag = 0;
int max_val;
int result;
vector<int> ans;

int main() {
	cin >> T;
	
	for (int t = 0; t < T; t++) {
		cin >> M >> A;

		memset(B, 0, sizeof(struct Battery) * 8);

		for (int n = 0; n < 2; n++) {
			p[n].W.assign(M, 0);
			for (int m = 0; m < M; m++) {
				cin >> p[n].W[m];
			}
		}
		for (int a = 0; a < A; a++) {
			cin >> B[a].pos.first >> B[a].pos.second >> B[a].C >> B[a].P;
		}

		p[0].pos = make_pair(1, 1);
		p[1].pos = make_pair(10, 10);

		for (int m = 0; m <= M; m++) { //이동 기준으로 계산
			for (int i = 0; i < 2; i++) { //사용 가능한 배터리 탐색
				for (int j = 0; j < A; j++) {
					int D = abs(p[i].pos.first - B[j].pos.first) + abs(p[i].pos.second - B[j].pos.second);
					p[i].B[j] = false; // 초기화
					if (D <= B[j].C) p[i].B[j] = true;
				}
			}

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < A; j++) {
					if (p[i].B[j] == true) {
						flag++;
						break;
					}
				}
			}	

			if (flag == 1) {
				for (int i = 0; i < 2; i++) {
					for (int j = 0; j < A; j++) {
						if (p[i].B[j] == true) max_val = max(max_val, B[j].P);
					}
				}
			}
			else if (flag == 2) {
				for (int i = 0; i < A; i++) {
					for (int j = 0; j < A; j++) {
						if (p[0].B[i] == true && p[1].B[j] == true) {
							if (i == j)	max_val = max(max_val, (B[i].P + B[j].P) / 2);
							else max_val = max(max_val, B[i].P + B[j].P);
						}
					}
				}
			}
			
			result += max_val;
			max_val = 0;
			flag = 0;
			if (m == M) break;
			else {
				for (int z = 0; z < 2; z++) {
					p[z].pos.first += dx[p[z].W[m]];
					p[z].pos.second += dy[p[z].W[m]];
				}
			}	
		}
		ans.push_back(result);
		result = 0;
	}

	for (int i = 0; i < T; i++) {
		printf("#%d %d\n", i + 1, ans[i]);
	}

	return 0;
}