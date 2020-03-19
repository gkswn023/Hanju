
//2014005032_¿Ã«—¡÷
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<vector>
#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<algorithm>

#define WHITE 0
#define GRAY 1
#define BLACK 2

using namespace std;

typedef struct Graph {
	vector< vector<int> > list;
	vector<struct node> vertex;
}graph;

typedef struct node {
	int color;
	int start;
	int end;
}node;

graph G;
vector<int> result;
int timer;

int DFS();
int DFS_VISIT(int u);

int main() {
	int num;
	int i, v1, v2, is_dag;

	scanf("%d", &num);

	G.list.resize(num + 1);
	G.vertex.resize(num + 1);

	while (scanf("%d %d", &v1, &v2) != EOF)
	{
		G.list[v1].push_back(v2);
	}

	for (i = 1; i < num + 1; i++) {
		sort(G.list[i].begin(), G.list[i].end());
	}

	is_dag = DFS();
	if (is_dag == 0) {
		printf("0\n");
	}
	else {
		printf("1\n");
		for (i = num - 1; i >= 0; i--) {
			printf("%d ", result[i]);
		}
	}
	return 0;
}

int DFS() {
	int i;

	for (i = 1; i < G.vertex.size(); i++) {
		G.vertex[i].color = WHITE;
	}
	timer = 0;
	for (i = 1; i < G.vertex.size(); i++) {
		if (G.vertex[i].color == WHITE) {
			if (DFS_VISIT(i) == 0) return 0;
		}
	}

	return 1;
}

int DFS_VISIT(int u) {
	int i;

	G.vertex[u].start = ++timer;
	G.vertex[u].color = GRAY;

	for (i = 0; i < G.list[u].size(); i++) {
		if (G.vertex[G.list[u][i]].color == WHITE) {
			if (DFS_VISIT(G.list[u][i]) == 0) return 0;
		}
		else if (G.vertex[G.list[u][i]].color == GRAY) {
			return 0;
		}
	}

	G.vertex[u].color = BLACK;
	G.vertex[u].end = ++timer;
	result.push_back(u);

	return 1;
}
