//2014005032_¿Ã«—¡÷

#include<stdio.h>
#include<stdlib.h>

void Merge_Sort(int* list, int* sorted_list, int left, int right);
void Merge(int* list, int* sorted_list, int left, int mid, int right);

int main() {
	int num;
	int* list;
	int* sorted_list;

	scanf("%d", &num);

	list = (int*)malloc(sizeof(int)*num);
	sorted_list = (int*)malloc(sizeof(int)*num);

	for (int i = 0; i < num; i++) {
		scanf("%d", &list[i]);
	}

	Merge_Sort(list, sorted_list, 0, num - 1);

	for (int i = 0; i < num; i++) {
		printf("%d\n", list[i]);
	}
	return 0;
}

void Merge_Sort(int* list, int* sorted_list, int left, int right) {
	int mid;

	if (left < right) {
		mid = (left + right) / 2;
		Merge_Sort(list, sorted_list, left, mid);
		Merge_Sort(list, sorted_list, mid + 1, right);
		Merge(list, sorted_list, left, mid, right);
	}
}
void Merge(int* list, int* sorted_list, int left, int mid, int right) {
	int i = left, j = left, k = mid + 1;

	while (j <= mid && k <= right) {
		if (list[j] >= list[k]) {
			sorted_list[i++] = list[j++];
		}
		else {
			sorted_list[i++] = list[k++];
		}
	}

	if (j > mid) {
		for (int a = k; a <= right; a++) {
			sorted_list[i++] = list[a];
		}
	}

	else {
		for (int a = j; a <= mid; a++) {
			sorted_list[i++] = list[a];
		}
	}

	for (int a = left; a <= right; a++) {
		list[a] = sorted_list[a];
	}
}
