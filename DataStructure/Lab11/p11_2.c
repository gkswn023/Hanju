#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct _Graph{
	int size;
	int *node;
	int **matrix;
}Graph;

typedef struct _Queue{
	int *key;
	int first;
	int rear;
	int qsize;
	int max_queue_size;
}Queue;

Graph* CreateGraph(int* nodes);
void InsertEdge(Graph* G, int a, int b);
void DeleteGraph(Graph* G);
void Topsort(Graph* G);
Queue* MakeNewQueue(int x);
int IsEmpty(Queue* Q);
int IsFull(Queue* Q);
int Dequeue(Queue* Q);
void Enqueue(Queue* Q, int x);
void DeleteQueue(Queue* Q);
void MakeEmpty(Queue* Q);

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
	/*
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
	*/
	Topsort(G);
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

void Topsort(Graph* G){
	Queue* Q;
	int i,j;
	int count=0,v,w;
	int* indegree=(int*)malloc(sizeof(int)*G->size);
	int* num=(int*)malloc(sizeof(int)*G->size);

	Q=MakeNewQueue(100);

	for(i=0;i<G->size;i++) indegree[i]=0;

	for(i=0;i<G->size;i++){
		for(j=0;j<G->size;j++){
			if(G->matrix[i][j]==1) indegree[j]++;
		}
	}

	for(i=0;i<G->size;i++){
		if(indegree[i]==0) {
			Enqueue(Q,i);
		}
	}

	while(!IsEmpty(Q)){
		v=Dequeue(Q);
		num[count++]=v;
		for(i=0;i<G->size;i++){
			if(G->matrix[v][i]==1) {
				w=i;
				if(--indegree[w]==0) Enqueue(Q,w);
		}
	}
}
	if(count!=G->size) {
		printf("Graph has a cycle\n");
		return ;
	}
	for(i=0;i<G->size;i++){
		printf("%d ",G->node[num[i]]);
	}
	printf("\n");
	free(num);
	free(indegree);
}

Queue* MakeNewQueue(int x){
	Queue* Q;
	Q=(Queue*)malloc(sizeof(Queue));
	Q->key=(int*)malloc(sizeof(int)*x);
	Q->first=0;
	Q->rear=-1;
	Q->qsize=0;
	Q->max_queue_size=x;

	return Q;
}

int IsEmpty(Queue* Q){
	if(Q->qsize==0) return 1;
	else return 0;
}

int IsFull(Queue* Q){
	if(Q->qsize==Q->max_queue_size) return 1;
	else return 0;
}

int Dequeue(Queue* Q){
	if(IsEmpty(Q)) return -1;
	else{
		Q->qsize--;
		return Q->key[Q->first++];
	}
}

void Enqueue(Queue* Q, int x){
	if(IsFull(Q)) return ;
	else{
		Q->qsize++;
		Q->key[++Q->rear]=x;
	}
}

void DeleteQueue(Queue* Q){
	free(Q->key);
	free(Q);
}

void MakeEmpty(Queue* Q){
	Q->qsize=0;
	Q->first=0;
	Q->rear=-1;
}