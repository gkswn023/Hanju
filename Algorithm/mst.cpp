//2014005032_ÀÌÇÑÁÖ

#include<stdio.h>
#include<stdlib.h>
#include<algorithm>
#include<vector>

using namespace std;

typedef struct node {
	int my;
	int parent;
	int rank;
}node;

typedef struct edge {
	int u, v, w;
}edge;

bool sort_edge(edge a, edge b) {
	bool swap = false;

	if (a.w < b.w) swap = true;
	if (a.w == b.w && a.u < b.u) swap = true;
	if (a.w == b.w && a.u == b.u && a.v < b.v) swap = true;

	return swap;
}

void make_set(int x);
void union_set(int x, int y);
void link(int x, int y);
int find_set(int x);
void mst_kruskal(int num);

node* forest;
vector<edge> edges;
vector<edge> result;

int main() {
	int num, i;
	int num1, num2, num3;
	edge tmp_edge;

	scanf("%d", &num);

	forest = (node*)malloc(sizeof(node)* (num + 1));

	while (scanf("%d %d %d", &num1, &num2, &num3) != EOF) {
		if (num1 > num2) {
			tmp_edge.u = num2;
			tmp_edge.v = num1;
		}
		else {
			tmp_edge.u = num1;
			tmp_edge.v = num2;
		}
		tmp_edge.w = num3;
		edges.push_back(tmp_edge);
	}

	mst_kruskal(num);

	printf("%d\n", result.size());

	for (i = 0; i < result.size(); i++) {
		printf("%d %d %d\n", result[i].u, result[i].v, result[i].w);
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

void mst_kruskal(int num) {
	int i;

	for (i = 1; i < num + 1; i++) {
		make_set(i);
	}

	sort(edges.begin(), edges.end(), sort_edge);

	for (i = 0; i < edges.size(); i++) {
		if (find_set(edges[i].u) != find_set(edges[i].v)) {
			result.push_back(edges[i]);
			union_set(edges[i].u, edges[i].v);
		}
	}
}