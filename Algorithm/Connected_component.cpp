//2014005032_ÀÌÇÑÁÖ

#include<stdio.h>
#include<stdlib.h>

typedef struct node {
	int my;
	int parent;
	int rank;
}node;

void make_set(int x);
void union_set(int x, int y);
void link(int x, int y);
int find_set(int x);

node* forest;

int main() {
	int num, num1, num2;
	int i, j, tmp;
	int* parent_list, *tmp_list, *result;
	int component_num = 0, num3 = 0;

	scanf("%d", &num);

	forest = (node*)malloc(sizeof(node)*(num + 1));
	parent_list = (int*)malloc(sizeof(int)*(num + 1));
	tmp_list = (int*)malloc(sizeof(int)*(num + 1));

	for (i = 1; i < num + 1; i++) {
		make_set(i);
	}

	while (scanf("%d %d", &num1, &num2) != EOF) {
		union_set(num1, num2);
	}

	for (i = 1; i < num + 1; i++) {
		parent_list[i] = find_set(i);
		tmp_list[i] = find_set(i);
	}
	
	for (i = 1; i < num + 1; i++) {
		tmp = parent_list[i];
		if (tmp != 0) {
			component_num++;
			for (j = i + 1; j < num + 1; j++) {
				if(tmp == parent_list[j])
					parent_list[j] = 0;
			 }
		}
	}

	result = (int*)malloc(sizeof(int)* (component_num + 1));

	for (i = 1; i < num + 1; i++) {
		if (parent_list[i] != 0)
			result[++num3] = parent_list[i];
	}

	for (i = 1; i < num + 1; i++) {
		for (j = 1; j < num3 + 1; j++) {
			if (tmp_list[i] == result[j]) {
				parent_list[i] = j;
			}
		}
	}

	printf("%d\n", component_num);

	for (i = 1; i < num + 1; i++) {
		printf("%d\n", parent_list[i]);
	}

	return 0;
}

void make_set(int x) {
	forest[x].my = x;
	forest[x].parent = x;
	forest[x].rank = 0;
}

void union_set(int x, int y) {
	link(find_set(x), find_set(y));
}

void link(int x, int y) {
	if (forest[x].rank > forest[y].rank)
		forest[y].parent = forest[x].my;
	else {
		forest[x].parent = forest[y].my;
		if (forest[x].rank == forest[y].rank)
			forest[y].rank += 1;
	}
}

int find_set(int x) {
	if (forest[x].my != forest[x].parent) {
		forest[x].parent = find_set(forest[x].parent);
	}
	return forest[x].parent;
}
