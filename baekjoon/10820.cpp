#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<queue>
#include<string>
#include<vector>

using namespace std;

int main() {
		
	string S;
	int small = 0, large = 0, num = 0, space = 0;

	while (getline(cin, S)) {
		for (int i = 0; i < S.size(); i++) {
			if ((S[i] - '1') >= 0 && (S[i]- '1') <= 9) {
				num++;
			}
			else if ((S[i] - 'a') >= 0 && (S[i] - 'a') <= 25) {
				small++;
			}
			else if ((S[i] - 'A') >= 0 && (S[i] - 'A') <= 25) {
				large++;
			}
			else space++;
		}
		cout << small << " " << large << " " << num << " " << space << endl;
		small = 0; large = 0; num = 0; space = 0;
	}

	return 0;
}