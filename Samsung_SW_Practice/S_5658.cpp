#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<stack>
#include<vector>
#include<algorithm>

using namespace std;

int T;
int N, K;
int num;
vector<char> v;
vector<int> result;
vector<int> f_result;

void calculate();
void swap_a_b();

bool desc(int a, int b) { return a > b; }

int main() {
	cin >> T;
	char tmp;

	for (int i = 0; i < T; i++) {
		cin >> N >> K;
		num = N / 4;
		for (int j = 0; j < N; j++) {
			cin >> tmp;
			v.push_back(tmp);
		}
		calculate();

		for (int k = 0; k < num - 1; k++) {
			swap_a_b();
			calculate();
		}
		sort(result.begin(), result.end(), desc);
		f_result.push_back(result[K - 1]);
		result.clear();
		v.clear();
	}

	for (int i = 0; i < T; i++) {
		printf("#%d %d", i + 1, f_result[i]);
		printf("\n");
	}

	return 0;
}

void calculate() {
	int sum = 0;
	int supply = 1;
	int end = num - 1;
	int tmp2;
	int flag = 1;

	vector<vector<char>> tmp;
	tmp = vector<vector<char>>(4, vector<char>(num, '0'));

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < num; j++) {
			tmp[i][j] = v[num*i + j];
		}
	}

	for (int i = 0; i < 4; i++) {
		while (end >= 0) {
			if (tmp[i][end] >= '0' && tmp[i][end] <= '9') tmp2 = tmp[i][end] - '0';
			else {
				tmp2 = (tmp[i][end] - 'A') + 10;
			}
			sum += tmp2 * supply;
			supply = supply * 16;
			end--;
		}
		for (int j = 0; j < result.size(); j++) {
			if (result[j] == sum) {
				flag = 0;
				break;
			}
		}
		if (flag == 1) result.push_back(sum);
		flag = 1;
		sum = 0;
		supply = 1;
		end = num - 1;
	}
}

void swap_a_b() {
	char end_pos = v[N - 1];

	for (int i = N - 1; i > 0; i--) {
		v[i] = v[i - 1];
	}
	v[0] = end_pos;
}