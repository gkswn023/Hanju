
//2014005032_¿Ã«—¡÷

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	char list1[500], list2[500], list3[500];
	int** table;
	int i, j, n, m, k;
	int num = 0;

	scanf("%s", list1);
	scanf("%s", list2);

	if (strlen(list1) > strlen(list2)) {
		table = (int**)malloc(sizeof(int*) * (strlen(list2) + 1));
		for (i = 0; i < strlen(list2) + 1; i++) {
			table[i] = (int*)malloc(sizeof(int) * (strlen(list1) + 1));
			table[i][0] = 0;
		}
		for (i = 0; i < strlen(list1) + 1; i++) {
			table[0][i] = 0;
		}
		n = strlen(list1) + 1;
		m = strlen(list2) + 1;
		k = 0;
		for (i = 1; i < m; i++) {
			for (j = k + 1; j < n; j++) {
				if (list2[i - 1] == list1[j - 1]) {
					table[i][j] = table[i - 1][j - 1] + 1;
					list3[num++] = list2[i - 1];
					k = j;
					break;
				}
				else if (table[i - 1][j] >= table[i][j - 1]) {
					table[i][j] = table[i - 1][j];
				}
				else {
					table[i][j] = table[i][j - 1];
				}
			}
		}
		for (i = 0; i < num; i++) {
			printf("%c", list3[i]);
		}
	}
	else {
		table = (int**)malloc(sizeof(int*) * (strlen(list1) + 1));
		for (i = 0; i < strlen(list1) + 1; i++) {
			table[i] = (int*)malloc(sizeof(int) * (strlen(list2) + 1));
			table[i][0] = 0;
		}
		for (i = 0; i < strlen(list2) + 1; i++) {
			table[0][i] = 0;
		}
		n = strlen(list2) + 1;
		m = strlen(list1) + 1;
		k = 0;
		for (i = 1; i < m; i++) {
			for (j = k + 1; j < n; j++) {
				if (list1[i - 1] == list2[j - 1]) {
					table[i][j] = table[i - 1][j - 1] + 1;
					list3[num++] = list1[i - 1];
					k = j;
					break;
				}
				else if (table[i - 1][j] >= table[i][j - 1]) {
					table[i][j] = table[i - 1][j];
				}
				else {
					table[i][j] = table[i][j - 1];
				}
			}
		}
		for (i = 0; i < num; i++) {
			printf("%c", list3[i]);
		}
	}
	return 0;
}