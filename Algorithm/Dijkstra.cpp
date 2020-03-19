
//2014005032_¿Ã«—¡÷
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<vector>
#include<cstdio>
#include<cstdlib>
#include<iostream>

#define MAX 2147483647
using namespace std;

typedef struct Graph {
	vector< vector<struct info> > list;
	vector<int> vertex;
}graph;

typedef struct info {
	int weight;
	int name;
}info;

typedef struct Heap {
	int id;
	int value;
}Heap;

graph G;
Heap* heap;
int heap_size = 0;
vector<int> used_table;

void initialize();
int Extract_min();
void insert(Heap h);
void Dijkstra();

int main() {
	int i, j, v, w, num, num1, su;
	int sum = 0;
	info information;


	scanf("%d", &num);
	G.list.resize(num + 1);
	G.vertex.resize(num + 1);

	for (i = 0; i < num; i++) {
		scanf("%d %d", &num1, &su);
		sum += su;
		for (j = 0; j < su; j++) {
			scanf("%d %d", &v, &w);
			information.name = v;
			information.weight = w;
			G.list[num1].push_back(information);
		}
	}
	heap = (Heap*)malloc(sizeof(Heap)*(num + sum));

	Dijkstra();

	printf("%d\n", G.vertex[used_table[num - 1]]);

	return 0;
}

void initialize() {
	int i;

	for (i = 1; i < G.vertex.size(); i++) {
		G.vertex[i] = MAX;
	}
	G.vertex[1] = 0;
}
int Extract_min() {
	int i, child;
	int min, j = 0;
	Heap tmp;

	min = heap[1].id;
	for (i = 0; i < used_table.size(); i++) {
		if (used_table[i] == min) j++;
	}
	if (j == 0) used_table.push_back(min);

	heap[1] = heap[heap_size--];

	for (i = 1; i * 2 <= heap_size; i = child) {
		child = i * 2;
		if (heap[child].value > heap[child + 1].value) {
			child++;
		}
		if (heap[i].value > heap[child].value) {
			tmp = heap[i];
			heap[i] = heap[child];
			heap[child] = tmp;
		}
	}

	return min;

}
void insert(Heap h) {
	int i, parent;
	Heap tmp;

	heap[++heap_size] = h;
	for (i = heap_size; i > 1; i = parent) {
		parent = i / 2;
		if (heap[parent].value > heap[i].value) {
			tmp = heap[parent];
			heap[parent] = heap[i];
			heap[i] = tmp;
		}
	}
}
void Dijkstra() {
	int i, num;
	Heap node;

	initialize();

	node.id = 1;
	node.value = 0;
	insert(node);

	while (heap_size > 0) {
		num = Extract_min();
		for (i = 0; i < G.list[num].size(); i++) {
			if (G.vertex[G.list[num][i].name] > G.vertex[num] + G.list[num][i].weight) {
				G.vertex[G.list[num][i].name] = G.vertex[num] + G.list[num][i].weight;
				node.id = G.list[num][i].name;
				node.value = G.vertex[G.list[num][i].name];
				insert(node);
			}
		}
	}

}
