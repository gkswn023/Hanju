#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<vector>
#include<deque>

using namespace std;

int T, K;
pair<int,int> R[21];
deque<int> dq[4];
int cur; //현재 선택된 자석
int way;
int tmp;
vector<pair<int, int>> info;
vector<int> result;
int sum;
int p;

int main() {
	cin >> T;
	for (int t = 0; t < T; t++) {
		cin >> K;
		for (int i = 0; i < 4; i++) {
			dq[i].assign(8, 0);
			for (int j = 0; j < 8; j++) {
				cin >> dq[i][j];		
			}
		}
		for (int i = 0; i < K; i++) {
			cin >> R[i].first >> R[i].second;
		}

		for (int i = 0; i < K; i++) {
			cur = R[i].first - 1;
			way = R[i].second;
			while (cur < 3 && dq[cur][2] != dq[cur + 1][6]) {
				info.push_back(make_pair(cur + 1, way * -1));
				cur += 1;
				way *= -1;
			}
			cur = R[i].first - 1;
			way = R[i].second;
			while (cur > 0 && dq[cur][6] != dq[cur - 1][2]) {
				info.push_back(make_pair(cur - 1, way * -1));
				cur -= 1;
				way *= -1;
			}
			info.push_back(make_pair(R[i].first - 1, R[i].second));

			for (int j = 0; j < info.size(); j++) {
				if (info[j].second == 1) {
					tmp = dq[info[j].first].back();
					dq[info[j].first].pop_back();
					dq[info[j].first].push_front(tmp);
				}
				else if (info[j].second == -1) {
					tmp = dq[info[j].first].front();
					dq[info[j].first].pop_front();
					dq[info[j].first].push_back(tmp);
				}
			}
			info.clear();
		}
		p = 1;
		for (int i = 0; i < 4; i++) {
			if (dq[i][0] == 1) sum += p;
			p *= 2;
		}
		result.push_back(sum);
		sum = 0;
	}

	for (int t = 0; t < T; t++) {
		printf("#%d %d\n", t + 1, result[t]);
	}

	return 0;
}