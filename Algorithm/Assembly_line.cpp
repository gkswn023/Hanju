//2014005032_¿Ã«—¡÷
#include<stdio.h>
#include<stdlib.h>

void fastest_way(int* e, int* x, int* a1, int* a2,
	int* t1, int* t2, int num);

int main() {
	int num;
	int e[2];
	int x[2];
	int* a1, *a2;
	int* t1, *t2;
	int i;

	scanf("%d", &num);
	scanf("%d %d", &e[0], &e[1]);
	scanf("%d %d", &x[0], &x[1]);
	a1 = (int*)malloc(sizeof(int)*num);
	a2 = (int*)malloc(sizeof(int)*num);
	t1 = (int*)malloc(sizeof(int)*(num - 1));
	t2 = (int*)malloc(sizeof(int)*(num - 1));
	for (i = 0; i < num; i++) {
		scanf("%d", &a1[i]);
	}
	for (i = 0; i < num; i++) {
		scanf("%d", &a2[i]);
	}
	for (i = 0; i < num - 1; i++) {
		scanf("%d", &t1[i]);
	}
	for (i = 0; i < num - 1; i++) {
		scanf("%d", &t2[i]);
	}

	fastest_way(e, x, a1, a2, t1, t2, num);

	return 0;
}

void fastest_way(int* e, int* x, int* a1, int* a2,
	int* t1, int* t2, int num) {
	int* s1, *s2;
	int* l1, *l2;
	int i, k, s_last, l_last;
	int * list;
	s1 = (int*)malloc(sizeof(int)*num);
	s2 = (int*)malloc(sizeof(int)*num);
	l1 = (int*)malloc(sizeof(int)*num);
	l2 = (int*)malloc(sizeof(int)*num);
	list = (int*)malloc(sizeof(int)*num);
	l1[0] = 0;
	l2[0] = 0;

	s1[0] = e[0] + a1[0];
	s2[0] = e[1] + a2[0];

	for (i = 1; i < num; i++) {
		if (s1[i - 1] <= s2[i - 1] + t2[i - 1]) {
			s1[i] = s1[i - 1] + a1[i];
			l1[i] = 1;
		}
		else {
			s1[i] = s2[i - 1] + t2[i - 1] + a1[i];
			l1[i] = 2;
		}
		if (s2[i - 1] < s1[i - 1] + t1[i - 1]) {
			s2[i] = s2[i - 1] + a2[i];
			l2[i] = 2;
		}
		else {
			s2[i] = s1[i - 1] + t1[i - 1] + a2[i];
			l2[i] = 1;
		}
	}
	if (s1[i - 1] + x[0] <= s2[i - 1] + x[1]) {
		s_last = s1[i - 1] + x[0];
		l_last = 1;
	}
	else {
		s_last = s2[i - 1] + x[1];
		l_last = 2;
	}

	k = l_last;
	list[num - 1] = l_last;
	for (i = num - 2; i >= 0; i--) {
		if (k == 1) {
			list[i] = l1[i + 1];
			k = l1[i + 1];
		}
		else {
			list[i] = l2[i + 1];
			k = l2[i + 1];
		}
	}
	printf("%d\n", s_last);
	for (i = 0; i < num; i++) {
		printf("%d %d\n", list[i], i + 1);
	}
}