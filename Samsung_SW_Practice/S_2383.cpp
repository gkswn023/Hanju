#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<vector>
#include<deque>
#include<algorithm>
#include<string.h>

using namespace std;

typedef struct Stair {
	int h; //계단 높이
	int p; //내려가는 인원 수
	int e; //탈출 인원 수
	vector<int> list;//계단 타는 사람의 거리 리스트
	pair<int, int> pos;
}Stair;

typedef struct People {
	int s; //계단
	pair<int, int> pos;
}People;

void sol1(int depth);

int T, N;
vector<vector<int>> map;
Stair s[2];
vector<People> p;
People tmp;
int flag;
int min_val = 99999;
vector<int> result;

int main() {
	cin >> T;
	for (int t = 0; t < T; t++) {
		cin >> N;
		p.clear();
		memset(s, 0, sizeof(Stair) * 2);
		map.assign(N, vector<int>(N, 0));
		flag = 0;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				cin >> map[i][j];
				if (map[i][j] == 1) {
					tmp.pos = make_pair(i, j);
					p.push_back(tmp);
				}
				else if (map[i][j] != 0) {
					s[flag].pos = make_pair(i, j);
					s[flag].h = map[i][j];
					flag++;
				}
			}
		}

		sol1(0);
		result.push_back(min_val);
		min_val = 99999;

	}

	for (int i = 0; i < T; i++) {
		printf("#%d %d\n", i + 1, result[i]);
	}
	
	return 0;
}

void sol1(int depth)
{
	int distance;
	int cnt[2] = { 0, 0 };

	if (p.size() == depth) {
		for (int i = 0; i < p.size(); i++) {
			if (p[i].s == 0) {
				distance = abs(p[i].pos.first - s[0].pos.first) + abs(p[i].pos.second - s[0].pos.second);
				s[0].list.push_back(distance);
			}
			else {
				distance = abs(p[i].pos.first - s[1].pos.first) + abs(p[i].pos.second - s[1].pos.second);
				s[1].list.push_back(distance);
			}
		}

		sort(s[0].list.begin(), s[0].list.end());
		sort(s[1].list.begin(), s[1].list.end());

		for (int t = 0; t < 2; t++) {
			while (s[t].e != s[t].list.size()) {
				for (int i = 0; i < s[t].list.size(); i++) {
					if (s[t].list[i] != 0) {
						s[t].list[i]--;
					}
					else {
						if (s[t].p < 3) {
							s[t].p++;
							s[t].list[i]--;
						}
					}

					if (s[t].h * -1 == s[t].list[i] + 1) {
						s[t].e++;
						s[t].p--;
					}
				}
				cnt[t]++;
			}
		}

		if (cnt[0] > cnt[1]) {
			if (cnt[0] < min_val) min_val = cnt[0];
		}
		else {
			if (cnt[1] < min_val) min_val = cnt[1];
		}

		return;
	}
	for (int i = 0; i <= 1; i++) {
		p[depth].s = i;
		sol1(depth + 1);
	}
}