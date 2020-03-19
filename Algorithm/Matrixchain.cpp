//2014005032_¿Ã«—¡÷
#include<stdio.h>
#include<stdlib.h>

#define infinity 2147483647

typedef struct matrix {
	int** m;
	int** s;
}matrix;

void matrix_chain(int* p, int num, matrix* chain);
void print_matrix(int i, int j, int** s);

int main() {
	int num;
	int* p;
	matrix* chain;
	chain = (matrix *)malloc(sizeof(matrix));
	scanf("%d", &num);
	p = (int*)malloc(sizeof(int)* (num + 1));
	chain->m = (int**)malloc(sizeof(int*)*(num + 1));
	for (int i = 1; i < num + 1; i++) {
		chain->m[i] = (int*)malloc(sizeof(int)*(num + 1));
	}
	chain->s = (int**)malloc(sizeof(int*)*(num));
	for (int i = 1; i < num; i++) {
		chain->s[i] = (int*)malloc(sizeof(int)*(num + 1));
	}
	for (int i = 0; i < num + 1; i++) {
		scanf("%d", &p[i]);
	}

	matrix_chain(p, num, chain);
	printf("%d\n", chain->m[1][num]);

	print_matrix(1, num, chain->s);

	return 0;
}

void matrix_chain(int* p, int num, matrix* chain) {

	int i, j, k;
	int length, sum;

	for (i = 1; i < num + 1; i++) {
		chain->m[i][i] = 0;
	}
	for (length = 2; length < num + 1; length++) {
		for (i = 1; i < num - length + 2; i++) {
			j = i + length - 1;
			chain->m[i][j] = infinity;
			for (k = i; k < j; k++) {
				sum = chain->m[i][k] + chain->m[k + 1][j] + p[i - 1] * p[k] * p[j];
				if (sum < chain->m[i][j]) {
					chain->m[i][j] = sum;
					chain->s[i][j] = k;
				}
			}
		}
	}
}

void print_matrix(int i, int j, int** s) {
	int k;

	if (i == j) printf("%d", i);
	else {
		k = s[i][j];

		printf("(");
		print_matrix(i, k, s);
		print_matrix(k + 1, j, s);
		printf(")");
	}
}