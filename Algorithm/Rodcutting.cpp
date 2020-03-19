//2014005032_¿Ã«—¡÷
#include<stdio.h>
#include<stdlib.h>

typedef struct node {
	int* r;
	int* s;
}node;

node* extended_bottom_up_cut_rod(int* list, int num);
void print_cut_rod_solution(int* list, int num);

int main() {
	int num;
	int* list;

	scanf("%d", &num);
	list = (int*)malloc(sizeof(int)*(num + 1));

	for (int i = 1; i <= num; i++) {
		scanf("%d", &list[i]);
	}
	list[0] = 0;
	print_cut_rod_solution(list, num);

	return 0;
}

node* extended_bottom_up_cut_rod(int* list, int num) {
	node* rod;
	rod = (node *)malloc(sizeof(node));
	rod->r = (int*)malloc(sizeof(int)*(num + 1));
	rod->s = (int*)malloc(sizeof(int)*(num + 1));
	int q;

	rod->s[0] = 0;
	rod->r[0] = 0;
	for (int j = 1; j <= num; j++) {
		q = -1;
		for (int i = 1; i <= j; i++) {
			if (q < list[i] + rod->r[j - i]) {
				q = list[i] + rod->r[j - i];
				rod->s[j] = i;
			}
		}
		rod->r[j] = q;
	}

	printf("%d\n", rod->r[num]);
	return rod;
}
void print_cut_rod_solution(int* list, int num) {
	node* rod = (node*)malloc(sizeof(node));
	rod->r = (int*)malloc(sizeof(int)*(num + 1));
	rod->s = (int*)malloc(sizeof(int)*(num + 1));

	rod = extended_bottom_up_cut_rod(list, num);
	while (num > 0) {
		printf("%d ", rod->s[num]);
		num -= rod->s[num];
	}

	return;
}