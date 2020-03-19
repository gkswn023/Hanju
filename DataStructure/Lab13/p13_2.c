#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define infinity 2147483647

typedef struct Graph{
	int num;
	int** weight;
	int* check_visit;
}graph;

typedef struct Stack{
	int* content;
	int top;
	int max_stack_size;
}stack;

typedef struct CircularQueue{
	int* content;
	int first;
	int rear;
	int qsize;
	int max_queue_size;
}queue;

graph makeGraph(FILE* fi);
void DFS_recursive(graph g,int start,int end);
void DFS_iterative(graph g, int start,int end);
void BFS(graph g, int start, int end);
stack* create_stack(int num);
void push(stack* s, int value);
int pop(stack* s);
void close_stack(stack* s);
queue* create_queue(int size);
void enqueue(queue* q,int value);
int dequeue(queue* q);
void close_queue(queue* q);
graph init(graph g);
void error_print(graph g,int start, int end);

void main(int argc, char* argv[])
{
	FILE *fi = fopen(argv[1],"r");
	graph g=makeGraph(fi);

	int start, end;
	fscanf(fi,"%d-%d",&start,&end);

	printf("DFS recursive : ");
	DFS_recursive(g,start,end);
	error_print(g,start,end);
	init(g);
	printf("\nDFS iterative :");
	DFS_iterative(g,start,end);
	error_print(g,start,end);
	init(g);
	printf("\nBFS :");
	BFS(g,start,end);
	error_print(g,start,end);
	init(g);
	printf("\n");

	fclose(fi);
}

graph makeGraph(FILE* fi)
{
	int size,i,j;
	graph g;
	char temp = 0;
	fscanf(fi,"%d\n",&size);
	g.num=size;
	g.check_visit=(int*)malloc(sizeof(int)*(size+1));
	g.weight=(int**)malloc(sizeof(int*)*(size+1));

	for(i=1;i<size+1;i++){
		g.weight[i]=(int*)malloc(sizeof(int)*(size+1));
		for(j=1;j<size+1;j++){
			g.weight[i][j]=infinity;
		}
	}
	for(i=1;i<size+1;i++){
		g.check_visit[i]=0;
	}

	while(temp!='\n')
	{
		int node1, node2;
		fscanf(fi,"%d-%d",&node1,&node2);
		if(node1==0) break;
		g.weight[node1][node2] = 1;
		temp = fgetc(fi);
	}

	return g;
}
void DFS_recursive(graph g,int start,int end){
	int i=1;

	g.check_visit[start]=1;
	if(start==end) {
		printf("%d ", end);
		for(i=1;i<g.num+1;i++)
			g.check_visit[i]=1;
		return;
	}
	else printf("%d ",start);

	for(i=1;i<g.num+1;i++){
		if(g.weight[start][i]!=infinity){
			if(g.check_visit[i]==0){
				DFS_recursive(g,i,end);
			}
		}
	}
}
void DFS_iterative(graph g, int start,int end){
	stack* S;
	int u,i;
	S=create_stack(g.num);

	push(S,start);

	while(S->top!=-1){
		u=pop(S);
			if(g.check_visit[u]==0){
				g.check_visit[u]=1;
				printf("%d ",u);
				if(g.check_visit[end]==1) return;
					for(i=1;i<g.num+1;i++){
						if(g.weight[u][i]!=infinity){
							push(S,i);
						}
					}
			}
	}
	close_stack(S);
}
void BFS(graph g, int start, int end){
	queue* Q;
	int i,j=0;
	int* prev=(int*)malloc(sizeof(int)*(g.num+1));
	for(i=1;i<g.num+1;i++) prev[i]=0;
	Q=create_queue(g.num);
	enqueue(Q,start);
	if(g.num<start) return;
	g.check_visit[start]=0;

	while(Q->qsize!=0){
		int v = dequeue(Q);
		prev[j++]=v;
			for(i=1;i<g.num+1;i++){
				if(g.weight[v][i]!=infinity){
					if(g.check_visit[i]==0 && i!=start){
						g.check_visit[i]=g.check_visit[v]+1;
						enqueue(Q,i);
					}
				}
			}
	}
	while(prev[j-1]!=end){
		if(g.num<end) {
			break;
		}
		else if(g.check_visit[end]==0) break;
		else j--;
	}
	for(i=0;i<j;i++){
		printf("%d ",prev[i]);
	}

	close_queue(Q);
}
stack* create_stack(int num){
	stack* s;
	s=(stack*)malloc(sizeof(stack));
	s->content=(int*)malloc(sizeof(int)*num);
	s->top=-1;
	s->max_stack_size=num;

	return s;
}
void push(stack* s, int value){
	if(s->top==s->max_stack_size){
		printf("stack is full");
		return ;
	}
	else{
		s->content[++s->top]=value;
	}
}
int pop(stack* s){
	if(s->top==-1){
		printf("stack is empty");
		return -1;
	}
	else{
		return s->content[s->top--];
	}
}
void close_stack(stack* s){
	free(s->content);
	free(s);
}
queue* create_queue(int size){
	queue* Q;
	Q=(queue*)malloc(sizeof(queue));
	Q->content=(int*)malloc(sizeof(int)*size);
	Q->first=1;
	Q->rear=0;
	Q->qsize=0;
	Q->max_queue_size=size;

	return Q;
}

void enqueue(queue* q,int value){
	if(q->qsize==q->max_queue_size){
		printf("Full queue");
	}
	else{
		q->qsize++;
		q->rear=(q->rear+1) % q->max_queue_size;
		q->content[q->rear]=value;
	}
}

int dequeue(queue* q){
	if(q->qsize==0){
		printf("queue is empty");
	}
	else{
		q->qsize--;
		q->first=q->first % q->max_queue_size;
		return q->content[q->first++];
	}
}

void close_queue(queue* q){
	free(q->content);
	free(q);
}

graph init(graph g){
	int i;

	for(i=1;i<g.num+1;i++){
		g.check_visit[i]=0;
	}	
}
void error_print(graph g,int start, int end){
	if(g.num<start)
		printf("cannot find");
	else if(g.num < end)
		printf("cannot find");
	else if(g.check_visit[end]==0)
		printf("cannot find");		
}

