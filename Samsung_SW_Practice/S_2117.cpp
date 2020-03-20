#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<vector>
#include<deque>
#include<algorithm>
#include<string.h>

using namespace std;

#define cost(K) ((K*K) + ((K-1)*(K-1)))

int T, N, M;
vector<vector<int>> map;
int num_P, max_K, val, max_val, d;
bool pass;
vector<pair<int, int>> home;
vector<int> result;

int main() {
	cin >> T;
	for (int t = 0; t < T; t++) {
		cin >> N >> M;
		map.assign(N, vector<int>(N, 0));
		home.clear();
		num_P = 0;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				cin >> map[i][j];
				if (map[i][j] == 1) {
					num_P++;
					home.push_back(make_pair(i, j));
				}
			}
		}
		max_val = 0;
		max_K = 1;
		while (cost(max_K) <= num_P * M) max_K++;
		max_K--;

		for (int m = max_K; m > 0; m--) {
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					for (int k = 0; k < num_P; k++) {
						d = abs(i - home[k].first) + abs(j - home[k].second);
						if (d < m) val++;
					}
					if (cost(m) <= val * M) max_val = max(val, max_val);
					val = 0;
				}
			}
		}
		result.push_back(max_val);
	}
	for (int i = 0; i < T; i++) {
		printf("#%d %d\n", i + 1, result[i]);
	}

	cin >> T;
}