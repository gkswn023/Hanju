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
void Delete(ElementType X, List L);
Position Find(ElementType X, List L);
Position FindPrevious(ElementType X, List L);
void DeleteList(List L);

int main(int argc, char *argv[]){

char command;
int key1, key2;
FILE *input;
Position header;
Position tmp;
tmp=(Position)malloc(sizeof(struct Node));
if(argc==1)
	input = fopen("lab3_input2","r");
else
	input = fopen(argv[1],"r");
header=MakeEmpty(header);

while(1) {

	command = fgetc(input);
	if(feof(input)) break;
	switch(command){
	case 'i':
		fscanf(input, "%d %d",&key1,&key2);
		tmp=Find(key2, header);
		if(key2==-1) {
		Insert(key1,header,header);
		break;
		}//if key2==-1 key1 is placed on header->next
		else if(tmp==NULL){
		printf("insertion(%d) Failed : cannot find the location to be inserted\n",key1);
		break;
		}//if not find key2
		else{ Insert(key1,header,tmp); }	
		
		break;
	
	case 'd':
		fscanf(input,"%d",&key1);
		Delete(key1,header);
		break;
	
	case 'f':
		fscanf(input,"%d",&key1);
		tmp = FindPrevious(key1, header);
		if(IsLast(tmp, header))
			printf("Could not find %d in the list \n", key1);

		else{
			if(tmp->element>0)
			  printf("Key of the previous node of %d is %d.\n",key1,tmp->element);
			else
			  printf("Key of the previous node of %d is header.\n",key1);
}
			break;

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
	L->element=-1;
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
	if(P->element==-1){
	tmp->element=X;	

	tmp->next=L->next;
	L->next=tmp;
	}//if X is placed on header->next
	else{
	tmp->element=X;
	tmp->next=P->next;
	P->next=tmp;
	}
}

void PrintList( List L){
	List tmp;
	tmp=(List)malloc(sizeof(struct Node));
	
	tmp=L->next;
	if(tmp==NULL) printf("this list is empty\n");//if list empty, just print message.
	else{
	  while(!IsLast(tmp,L)){
		printf("key : %d     ",tmp->element);
		tmp=tmp->next;
	}
	printf("key : %d     ",tmp->element);
}
}
void Delete( ElementType X, List L){
	Position p,tmp;
	p=(Position)malloc(sizeof(struct Node));
	tmp=(Position)malloc(sizeof(struct Node));
	p=FindPrevious(X,L);

	if(p->next==NULL) printf("Deletion failed : element %d is not in the list\n",X);
	else if(!IsLast(p,L)){
		tmp=p->next;
		p->next=tmp->next;
		free(tmp);
		}
	}	

Position Find(ElementType X, List L){
	Position P;
	
	P=L->next;
	while( P!=NULL){
		if(P->element==X) return P;
		P=P->next;
	}
	return P;
}//just find x's place

Position FindPrevious(ElementType X, List L){
	Position P;

	P=L;
	while(P->next!=NULL){
		if(P->next->element==X) return P;
		P=P->next;
	}	
	return P;

}//find x's previous place

void DeleteList( List L){
	Position tmp,P;
	
	P=L->next;
	L->next=NULL;

	while(P!=NULL){
	tmp=P->next;
	free(tmp);
	P=tmp;
	}
}//delete all list and free
			
