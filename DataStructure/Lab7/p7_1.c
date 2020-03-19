#include<stdio.h>
#include<stdlib.h>
#define int_Max 2147483647

typedef int ElementType;
typedef struct HeapStruct Heap;

struct HeapStruct{
	int Capacity;
	int Size;
	ElementType *Elemenets;
};

Heap* CreateHeap(int heapSize);
void Insert(Heap* heap, int value);
int Find(Heap* heap, int value);
void DeleteHeap(Heap* heap);

void main(int argc, char* argv[]){
	FILE *fi = fopen(argv[1],"r");
	char cv;
	Heap* maxHeap;
	int heapSize, key;
	while(!feof(fi)){
		fscanf(fi,"%c",&cv);
		switch(cv){
			case 'n' :
				fscanf(fi,"%d",&heapSize);
				maxHeap = CreateHeap(heapSize);
				break;
			case 'i' :
				fscanf(fi,"%d",&key);
				Insert(maxHeap, key);
				break;
			/*case 'd' :
				DeleteMax(maxHeap);
				break;
			case 'p' :
				PrintHeap(maxHeap);*/
			case 'f' :
				fscanf(fi,"%d",&key);
				if(Find(maxHeap,key)==1)
					printf("%d is in the tree.\n", key);
				else 
					printf("%d is not in the tree.\n", key);
				break;	
		}
	}
	DeleteHeap(maxHeap);
	fclose(fi);
}

Heap* CreateHeap(int heapSize){
	Heap* heap;
	heap=(Heap*)malloc(sizeof(struct HeapStruct));
	if(heap==NULL)
		printf("out of space");
	heap->Elemenets=(int*)malloc(sizeof(int)*(heapSize+1));
	if(heap->Elemenets==NULL)
		printf("out of space");
	heap->Capacity=heapSize;
	heap->Size=0;
	heap->Elemenets[0]=int_Max;
	return heap;
}

void Insert(Heap* heap, int value){
	int i;
	if(heap->Capacity==heap->Size){
		printf("Tree is Full\n");
		return ;
	}
	else if(Find(heap,value)==1) {
		printf("%d is already in the tree\n",value);
		return ;
	}
	else{
	for(i=++heap->Size;heap->Elemenets[i/2]<value;i/=2){
		heap->Elemenets[i]=heap->Elemenets[i/2];
	}
	heap->Elemenets[i]=value;
	printf("insert %d\n",value);
}
}

int Find(Heap* heap, int value){
	int i=1;
	while(i<=heap->Size){
		if(heap->Elemenets[i]==value)
			return 1;
		else
			i++;
	}
	return -1;
}

void DeleteHeap(Heap* heap){
	free(heap->Elemenets);
	free(heap);
}