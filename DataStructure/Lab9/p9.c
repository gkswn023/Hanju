#include<stdio.h>
#include<time.h>
#include<stdlib.h>

typedef struct _DisjointSet
{
	int size_maze;
	int *ptr_arr;
} DisjointSets;

void init(DisjointSets *sets, DisjointSets *maze_print,int num);
int find(DisjointSets *sets, int i);
void Union(DisjointSets *sets, int i, int j);
void createMaze(DisjointSets *sets, DisjointSets *maze_print, int num);
void printMaze(DisjointSets *sets, int num);
void freeMaze(DisjointSets *sets, DisjointSets *maze_print);

int main(int argc, char* argv[])
{
	int num, i;
	FILE *fi = fopen(argv[1],"r");
	DisjointSets *sets, *maze_print;
	fscanf(fi,"%d",&num);
	sets=(DisjointSets*)malloc(sizeof(DisjointSets));
	maze_print=(DisjointSets*)malloc(sizeof(DisjointSets));
	init(sets,maze_print,num);
	createMaze(sets,maze_print,num);
	printMaze(maze_print,num);
	freeMaze(sets,maze_print);
	fclose(fi);

	return 0;
}

void init(DisjointSets *sets, DisjointSets *maze_print,int num){
	int i;
	sets->size_maze = num;
	sets->ptr_arr=(int*)malloc(sizeof(int)*num*num);
	maze_print->size_maze = num;
	maze_print->ptr_arr=(int*)malloc(sizeof(int)*num*(num-1)*2);

	for(i=0;i<num*num;i++){
	sets->ptr_arr[i]=0;
	if(i >= num*(num-1)) continue;
	maze_print->ptr_arr[i] = -1;
	maze_print->ptr_arr[num*(num-1)+i] = -1;		
	}
}


int find(DisjointSets *sets, int i){
	while(sets->ptr_arr[i]>0)
		i=sets->ptr_arr[i];
	return i;
}
void Union(DisjointSets *sets, int i, int j){
	i=find(sets,i);
	j=find(sets,j);

	if(sets->ptr_arr[i]<sets->ptr_arr[j])
		sets->ptr_arr[j]=i;
	else{
		if(sets->ptr_arr[i]==sets->ptr_arr[j])
			sets->ptr_arr[j]--;
		sets->ptr_arr[i]=j;
	}
}
void createMaze(DisjointSets *sets, DisjointSets *maze_print, int num){

	int i, j, x, y;
	int select;
	int wall = num*(num-1)*2;

	srand((unsigned int)time(NULL));

	do {
		do{ select = rand() % wall;}
		while(maze_print->ptr_arr[select] == 0); 

		if(select<num*(num-1)){
			y=select / (num-1);
			x=select % (num-1);
			i = y*num + x;
			j = i+1;
		}
		else{
			i = select - num*(num -1); 
			j= i + num;
		}
		if(find(sets, i) != find(sets, j))
		{
			Union(sets, i, j);
			maze_print->ptr_arr[select] = 0;
		}
		else continue;

	} while(find(sets,0) != find(sets,num*num-1));
}

void printMaze(DisjointSets *sets, int num){

	int length = 2*num + 1;
	int i, j, w, z;

	for(i=0;i<num;i++) printf(" -");
		printf("\n");

	for(i=1;i<length -1;i++){
		if(i % 2){
			if(i != 1) printf("| ");
			else printf("  ");

		for(j=0; j<num-1; j++){
			w=(i/2)*(num-1)+j;
			if(sets->ptr_arr[w]) printf("| ");
			else printf("  ");
		}
		if(i != length -2) printf("|\n");
		else printf(" \n");
	}
	else{
		for(j=0;j<num;j++){
			z = ((i-1)/2)*num + j + num*(num-1);
			if(sets->ptr_arr[z]) printf(" -");
			else printf("  ");
		}
		printf(" \n");
	}
}
for(i=0;i<num;i++) printf(" -");
printf(" \n");
}

void freeMaze(DisjointSets *sets, DisjointSets *maze_print){
	free(sets->ptr_arr);
	free(sets);
	free(maze_print->ptr_arr);
	free(maze_print);
}