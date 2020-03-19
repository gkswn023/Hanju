#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<queue>
#include<string>
#include<vector>
#include<algorithm>

using namespace std;

int d[1000000] = { 0, };

int number_one(int num);

int main() {
		
	int num;

	cin >> num;

	number_one(num);

	cout << d[num] << endl;

	return 0;
}

int number_one(int num) {
	int tmp; 

	if (num == 1) return 0;
	if (d[num] > 0) return d[num];

	d[num] =  number_one(num-1) + 1;
	
	if (num % 2 == 0) {
		tmp = number_one(num / 2) + 1;
		if (tmp < d[num]) d[num] = tmp;
	}
	if (num % 3 == 0) {
		tmp = number_one(num / 3) + 1;
		if (tmp < d[num]) d[num] = tmp;
	}

	return d[num];
}


