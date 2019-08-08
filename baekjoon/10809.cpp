#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<queue>
#include<string>
#include<vector>

using namespace std;

int main() {
		
	int a[26] = { 0, };
	int place = 0;
	string S;

	for (int i = 0; i < 26; i++) {
		a[i] = -1;
	}

	cin >> S;

	for (int i = 0; i < S.size(); i++) {
		if (a[S[i] - 'a'] == -1) {
			a[S[i] - 'a'] = place;
		}
		place++;
	}

	for (int i = 0; i < 26; i++) {
		cout << a[i] << " ";
	}

	return 0;
}