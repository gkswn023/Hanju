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
	
	getline(cin, S);

	for (int i = 0; i < S.size(); i++) {
		if (S[i] >= 'a' && S[i] <= 'z') {
			if(S[i] - 'a' < 13)
				S[i] = S[i] + 13;
			else
				S[i] = S[i] - 13;
			
		}
		else if (S[i] - 'A' >= 0 && S[i] - 'A' <= 25) {
			if(S[i] - 'A' < 13)
				S[i] = S[i] + 13;
			else
				S[i] = S[i] - 13;
		}
	}

	cout << S << endl;

	return 0;
}