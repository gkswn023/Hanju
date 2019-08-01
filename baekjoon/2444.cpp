#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>

using namespace std;

int main()

{
	int num;

	cin >> num;

	for (int i = 0; i < num; i++) {
		for (int j = num - i - 1; j > 0; j--) printf(" ");
		for (int k = 0; k < 2 * i + 1; k++) printf("*");
		printf("\n");
	}
	for (int i = num - 1; i > 0; i--) {
		for (int j = num - i; j > 0 ; j--) printf(" ");
		for (int k = 0; k < 2 * i - 1; k++) printf("*");
		printf("\n");
	}

}
