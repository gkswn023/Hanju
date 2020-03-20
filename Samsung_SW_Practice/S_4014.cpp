#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<vector>
#include<algorithm>
#include<string.h>

using namespace std;

int T, N, X;
vector<vector<int>> map;
vector<vector<int>> visit;
int H; // 현재 검색한 곳의 높이
int flag;
int cnt;
int a;// 성공여부 확인을 위해 설정
vector<int> result;

int main() {
	cin >> T;

	for (int t = 0; t < T; t++) {
		cin >> N >> X;
		map.assign(N * 2, vector<int>(N, 0));
		visit.assign(N * 2, vector<int>(N, 0));
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				cin >> map[i][j];
			}
		}
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				map[i + N][j] = map[j][i];
			}
		}

		for (int n = 0; n < 2 * N; n++) {
			H = map[n][0];
			for (a = 1; a < N; a++) {
				if (map[n][a] == H) continue;
				else if (map[n][a] == H + 1) { // 지형이 올라갔을 때
					flag = 0;
					for (int x = 1; x <= X; x++) {
						if (a - x >= 0 && map[n][a - x] == H && visit[n][a - x] == 0) flag++;
					}
					if (flag == X) {
						for (int x = 1; x <= X; x++) {
							visit[n][a - x] = 1;
						}
						H = H + 1;
					}
					else break;
				}
				else if (map[n][a] == H - 1) { // 지형이 내려갔을 때
					flag = 0;
					for (int x = 0; x < X; x++) {
						if (a + x < N && map[n][a + x] == H - 1 && visit[n][a + x] == 0) flag++;
					}
					if (flag == X) {
						for (int x = 0; x < X; x++) {
							visit[n][a + x] = 1;
						}
						H = H - 1;
					}
					else break;
				}
				else break;
			}
			if (a == N) cnt++;
		}

		result.push_back(cnt);
		cnt = 0;
	}

	for (int i = 0; i < T; i++) {
		printf("#%d %d\n", i + 1, result[i]);
	}
	return 0;
}