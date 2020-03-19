#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX_INT 2147483647

typedef struct _Graph{
	int size;
	int *node;
	int **matrix;
}Graph;

/*typedef struct _Queue{
	int *key;
	int first;
	int rear;
	int qsize;
	int max_queue_size;
}Queue;
*/
Graph* CreateGraph(int* nodes);
void InsertEdge(Graph* G, int a, int b);
void DeleteGraph(Graph* G);
/*void Topsort(Graph* G);
Queue* MakeNewQueue(int X);
int IsEmpty(Queue* Q);
int IsFull(Queue* Q);
int Dequeue(Queue* Q);
void Enqueue(Queue* Q, int x);
void DeleteQueue(Queue* Q);
void MakeEmpty(Queue* Q);*/

int main(int argc, char *argv[]){
	FILE *fi=fopen(argv[1],"r");
	Graph* G;
	char node[256],buf[256];
	char* ptr;
	int* nodes=(int*)malloc(sizeof(int)*256);
	int val1,val2;
	int size=0;
	int i,j;

	val1=0;
	fgets(node,255,fi);
	ptr=strtok(node," ");
	nodes[size++]=atoi(ptr);
	while((ptr=strtok(NULL," ")) != NULL){
		nodes[size++]=atoi(ptr);
	}
	nodes[size]=-1;
	nodes=(int*)realloc(nodes,sizeof(int)*(size+1));
	G = CreateGraph(nodes);
	fgets(buf,255,fi);
	ptr=strtok(buf, " ");
	sscanf(ptr,"%d-%d",&val1,&val2);
	InsertEdge(G,val1,val2);
	while((ptr=strtok(NULL," "))!=NULL){
		sscanf(ptr,"%d-%d",&val1,&val2);
		InsertEdge(G,val1,val2);
	}

	printf("  ");
	for(i = 0; i < G->size; i++){
		printf("%d ", G->node[i]);
	}
	printf("\n");
	for(i = 1; i < G->size + 1; i++ ){
		for(j = 0; j < G->size + 1; j++){
			if(j == 0){
				printf("%d ", G->node[i - 1]);
				continue;
			}
			printf("%d ", G->matrix[i - 1][j - 1]);
		}
		printf("\n");
	}
	DeleteGraph(G);
	fclose(fi);
}

Graph* CreateGraph(int *nodes){
	int i=0, j,size=0;
	Graph* G = (Graph*)malloc(sizeof(Graph));
	while(nodes[i]!=-1){
		size++;
		i++;
	}
	G->size=size;
	G->node = (int*)realloc(nodes,sizeof(int) * G->size);
	for(i = 0; i < G->size; i++)
		G->node[i] = nodes[i];
	G->matrix = (int**)malloc(sizeof(int*) * G->size);
	for(i = 0; i < G->size; i++){
		G->matrix[i] = (int*)malloc(sizeof(int) * G->size);
		for(j = 0; j < G->size; j++)
			G->matrix[i][j] = 0;
	}


	return G;
}

void InsertEdge(Graph* G, int a, int b){
	int i, j;
	for(i = 0; i < G->size; i++){
		for(j = 0; j < G->size; j++){
			if(G->node[i] == a && G->node[j] == b)
				G->matrix[i][j] = 1;
		}
	}
	
}

void DeleteGraph(Graph* G){
	int i, j;
	for(i = 0; i < G->size; i++){
		free(G->matrix[i]);
	}
	free(G->matrix);
	free(G->node);
}