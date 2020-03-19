#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<queue>
#include<string>
#include<vector>

using namespace std;

int main() {
		
	string A,B,C,D;

	cin >> A >> B >> C >> D;

	A += B;
	C += D;

	cout << stoull(A) + stoull(C) << endl;

	return 0;
}