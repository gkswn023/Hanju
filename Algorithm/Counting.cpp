//2014005032_¿Ã«—¡÷
#include<stdio.h>
#include<stdlib.h>

int main() {
	int* count_arr;
	int* range, *list;
	int num1, num2, num3, i;

	scanf("%d %d %d", &num1, &num2, &num3);

	count_arr = (int*)calloc(num2 + 1, sizeof(int));
	list = (int*)calloc(num1, sizeof(int));
	range = (int*)calloc(num3 * 2, sizeof(int));

	for (i = 0; i < num3 * 2; i += 2) {
		scanf("%d %d", &range[i], &range[i + 1]);
	}
	for (i = 0; i < num1; i++) {
		scanf("%d", &list[i]);
	}
	for (i = 0; i < num1; i++) {
		count_arr[list[i]]++;
	}
	for (i = 1; i <= num2; i++) {
		count_arr[i] += count_arr[i - 1];
	}
	for (i = 0; i < num3 * 2; i += 2) {
		printf("%d\n", count_arr[range[i + 1]] - count_arr[range[i] - 1]);
	}
	return 0;
}