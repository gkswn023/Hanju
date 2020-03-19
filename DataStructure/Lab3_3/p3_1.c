#include<stdio.h>
#include<malloc.h>

typedef struct Node *PtrToNode;
typedef PtrToNode List;
typedef PtrToNode Position;
typedef int ElementType;

struct Node
{
	ElementType element;
	Position next;
};

List MakeEmpty(List L);
int IsEmpty(List L);
int IsLast(Position P, List L);
void Insert(ElementType X, List L, Position P);
void PrintList(List L);

int main(int argc, char *argv[]){

char command;
int key1, key2;
FILE *input;
Position header;
Position tmp;
if(argc==1)
	input = fopen("lab3_input1","r");
else
	input = fopen(argv[1],"r");
header=MakeEmpty(header);

while(1) {

	command = fgetc(input);
	if(feof(input)) break;
	switch(command){
	case 'i':
		fscanf(input, "%d %d",&key1,&key2);
		Insert(key1,header,header);
	break;
	/*
	case 'd':
		fscanf(input,"%d",&key1);
		Delete(key1,header);
		break;
	*/
	
	/*
case 'f':
		fscanf(input,"%d",&key1);
		tmp = FindPrevious(key1, header);
		if(isLast(tmp, header))
			printf("Could not find %d in the list \n", key1);

		else{
			if(tmp->element>0)
			  printf("Key of the previous node of %d is %d.\n",key1,tmp->element);
			else
			  printf("Key of the previous node of %d is header.\n",key1);
}
			break;
*/
	case 'p':
		PrintList(header);
		break;

	default:
		break;
	}
}

fclose(input);
return 0;
}
List MakeEmpty( List L){
	L=(List)malloc(sizeof(struct Node));
	L->next = NULL;
	return L;
}

int IsEmpty( List L ){
	return L->next == NULL;
}

int IsLast( Position P, List L ){
	return P->next == NULL;
}

void Insert( ElementType X, List L, Position P){
	List tmp;
	tmp=(List)malloc(sizeof(struct Node));
	
	tmp->element=X;	

	tmp->next=L->next;
	L->next=tmp;
}

void PrintList( List L){
	List tmp;
	tmp=(List)malloc(sizeof(struct Node));
	
	tmp=L->next;

	while(!IsLast(tmp,L)){
		printf("key : %d     ",tmp->element);
		tmp=tmp->next;
	}
	printf("key : %d     ",tmp->element);
}	
