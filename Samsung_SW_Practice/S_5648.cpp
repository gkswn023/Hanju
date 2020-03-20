#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<vector>

using namespace std;

typedef struct ATOM {
	pair<int, int> pos;
	int W; //way
	int E; //Energy
}ATOM;

int T, N, K, X, Y, W;
int dx[4] = { 1, -1, 0 , 0 };
int dy[4] = { 0, 0, -1, 1 };
vector<ATOM> a;
pair<int, bool> visit[4001][4001];
int num_A; //총 원자 수
int num_E; //방출된 에너지
int x, y;
vector<int> result;
int test;

int main() {
	cin >> T;
	//visit.assign(4001, vector<pair<int, bool>>(4001)); -> assign 사용시 시간초과

	for (int t = 0; t< T; t++) {
		cin >> N;
		num_A = N;
		a.resize(N);

		for (int n = 0; n < N; n++) {
			cin >> X >> Y >> W >> K;
			X = X * 2 + 2000;
			Y = Y * 2 + 2000;
			a[n].pos = make_pair(Y, X);
			a[n].W = W;
			a[n].E = K;
		}

		while (num_A != 0) {
			test++;
			for (int i = 0; i < num_A; i++) {
				a[i].pos.first += dx[a[i].W];
				a[i].pos.second += dy[a[i].W];
				x = a[i].pos.first;
				y = a[i].pos.second;
				if (x < 0 || x > 4000 || y < 0 || y > 4000) {
					a[i--] = a[num_A - 1];
					num_A--;
					continue;
				}
				visit[x][y].first++;
				if (visit[x][y].first >= 2) visit[x][y].second = true;
			}

			for (int i = 0; i < num_A; i++) {
				x = a[i].pos.first;
				y = a[i].pos.second;
				if (visit[x][y].second == true) {
					num_E += a[i].E;
					a[i--] = a[num_A - 1];
					num_A--;
				}
				visit[x][y].first--;
				if (visit[x][y].first == 0) visit[x][y].second = false;
			}
		}

		result.push_back(num_E);
		num_E = 0;
	}

	for (int i = 0; i < T; i++) {
		printf("#%d %d\n", i + 1, result[i]);
	}

	return 0;
}