#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<stack>
#include<vector>

using namespace std;

int VPSsearch(char* vps);

int main() {
	
	int num;
	char vps[51];
	vector<int> answer;

	cin >> num;

	for (int i = 0; i < num; i++) {
		cin >> vps;
		answer.push_back(VPSsearch(vps));
	}
	for (int i = 0; i < num; i++) {
		if (answer[i] == 1) printf("YES\n");
		else printf("NO\n");
	}
}

int VPSsearch(char* vps) {
	stack<char> s;
	int i = 0;

	while (1) {
		if (vps[i] == '(') {
			s.push('(');
		}
		else if (vps[i] == ')') {
			if (s.empty()) return 1;
			s.pop();
		}
		else break;
		i++;
	}
	if (s.empty()) return 1;
	else return 0;
}