#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define infinity 2147483647

typedef struct Node{
	int vertex;
	int dist;
	int prev;
}Node;

typedef struct Graph{
	int size;
	int** vertices;
	Node* nodes;
}Graph;

typedef struct Heap{
	int Capacity;
	int size;
	Node* Element;
}Heap;

Graph createGraph(int size);
void printShortestPath(Graph G);
Heap* createMinHeap(int heapSize);
void insertToMinHeap(Heap* minHeap, int vertex, int distance);
Node deleteMin(Heap* minHeap);

void main(int argc, char* argv[]){

	FILE* fi= fopen(argv[1],"r");
	Graph G;
	int size,i;
	fscanf(fi,"%d\n",&size);
	G=createGraph(size+1);
	char temp = 0;

	while(temp!='\n')
	{
		int node1, node2, weight;
		fscanf(fi,"%d-%d-%d",&node1,&node2,&weight);
		if(node1==0) break;
		G.vertices[node1][node2] = weight;
		temp = fgetc(fi);
	}
	printShortestPath(G);
	fclose(fi);
}

Graph createGraph(int size){
	Graph G;
	int i,j;
	G.size=size;
	G.nodes=(Node*)malloc(sizeof(Node)*(size));
	G.vertices=(int**)malloc(sizeof(int*)*(size));
	for(i=1;i<size;i++){
		G.nodes[i].vertex=i;
	}
	for(i=1;i<size;i++){
		G.vertices[i]=(int*)malloc(sizeof(int)*(size));
		for(j=1;j<size;j++){
			G.vertices[i][j]=infinity;
		}
	}

	return G;
}

void printShortestPath(Graph G){
	Heap* heap;
	int i,j;
	Node* node;
	int* k;
	k=(int*)malloc(sizeof(int)*G.size);

	heap=createMinHeap((G.size)*(G.size));
	node=(Node*)malloc(sizeof(Node)*G.size);

	for(i=1;i<G.size;i++){
		node[i].dist=infinity;
		node[i].prev=0;
	}
	node[1].dist=0;
	for(i=2;i<G.size;i++){
		if(G.vertices[1][i]!=infinity){
			node[i].dist=G.vertices[1][i];
			node[i].prev=1;
			insertToMinHeap(heap,i,node[i].dist);
		} 
	}
	
	while(heap->size!=0){
		Node u=deleteMin(heap);
		for(i=1;i<G.size;i++){
			if(G.vertices[u.vertex][i]!=infinity){
				if(node[u.vertex].dist+G.vertices[u.vertex][i]<node[i].dist){
					node[i].dist=node[u.vertex].dist+G.vertices[u.vertex][i];
					node[i].prev=u.vertex;
					insertToMinHeap(heap,i,node[i].dist);
				}
			}
		}
	}
	for(j=2;j<G.size;j++){
		int m=0;
		for(i=j;i<G.size;i=node[i].prev){
			if(node[i].prev!=0){
				k[m]=node[i].prev;
				m++;
			}
			else break;
	}
		for(i=m-1;i>=0;i--)
		printf("%d->",k[i]);
		printf("%d (cost: %d)\n",j,node[j].dist);
}
	free(k);
	free(node);
	free(heap->Element);
	free(heap);
}

Heap* createMinHeap(int heapSize){
	Heap* heap;
	int i;
	heap=(Heap*)malloc(sizeof(Heap));
	heap->Capacity=heapSize;
	heap->size=0;
	heap->Element=(Node*)malloc(sizeof(Node)*(heapSize+1));
	heap->Element[0].dist=-1;

	return heap;
}

void insertToMinHeap(Heap* minHeap, int vertex, int distance){
	int i;
	Node node;
	node.vertex=vertex;
	node.dist=distance;
	if(minHeap->Capacity==minHeap->size) printf("Heap is Full");
	else{
		for(i=++minHeap->size;minHeap->Element[i/2].dist>distance;i/=2){
			minHeap->Element[i]=minHeap->Element[i/2];
		}
		minHeap->Element[i]=node;
	}
}

Node deleteMin(Heap* minHeap){
	int i,child;
	Node MinNode,LastNode;

	MinNode=minHeap->Element[1];
	LastNode=minHeap->Element[minHeap->size--];

	for(i=1;i*2<=minHeap->size;i=child){
		child=i*2;
		if(child!=minHeap->size && minHeap->Element[child+1].dist < minHeap->Element[child].dist)
			child++;
		if(LastNode.dist > minHeap->Element[child].dist)
			minHeap->Element[i]=minHeap->Element[child];
		else break;
	}
	minHeap->Element[i]=LastNode;
	return MinNode;
}