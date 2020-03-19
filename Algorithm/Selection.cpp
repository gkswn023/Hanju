//2014005032_ÀÌÇÑÁÖ
#include<stdio.h>
#include<stdlib.h>

int main() {
	int num1, num2;
	int* list;
	int min, tmp;
	int i, j;
	scanf("%d %d", &num1, &num2);

	list = (int*)malloc(sizeof(int)*num1);

	for (i = 0; i < num1; i++) {
		scanf("%d", &list[i]);
	}

	for (i = 0; i < num2; i++) {
		min = i;
		for (j = i + 1; j < num1; j++) {
			if (list[min] > list[j]) min = j;
		}
		tmp = list[i];
		list[i] = list[min];
		list[min] = tmp;
	}

	for (i = 0; i < num1; i++) {
		printf("%d\n", list[i]);
	}
	return 0;
}