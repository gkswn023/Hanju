//2014005032_¿Ã«—¡÷
#include<stdio.h>
#include<stdlib.h>

int main() {
	int num;
	int *list;
	int tmp, j;
	scanf("%d", &num);

	list = (int*)malloc(sizeof(int)*num);

	for (int i = 0; i < num; i++) {
		scanf("%d", &list[i]);
	}

	for (int i = 1; i < num; i++) {
		tmp = list[i];
		j = i - 1;
		while (j >= 0 && list[j] <= tmp) {
			list[j + 1] = list[j];
			j = j - 1;
		}
		list[j + 1] = tmp;
	}
	for (int i = 0; i < num; i++) {
		printf("%d\n", list[i]);
	}

	return 0;
}