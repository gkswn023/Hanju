#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<queue>
#include<string>
#include<vector>
#include<algorithm>

using namespace std;

string substring(string A, int start);

int main() {
		
	string input;
	vector<string> S;

	cin >> input;

	for (int i = 0; i < input.size(); i++) {
		S.push_back(substring(input, i));
	}

	sort(S.begin(), S.end());
	for (int i = 0; i < S.size(); i++) {
		cout << S[i] << endl;
	}

	return 0;
}

string substring(string A, int start) {
	string result;

	while (start < A.size()) {
		result += A[start++];
	}

	return result;
}

