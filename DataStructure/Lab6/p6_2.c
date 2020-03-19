#include<stdio.h>
#include<stdlib.h>

#define MAX(a,b) (((a)>(b)) ? (a) : (b))

typedef int ElementType;
typedef struct AVLNode* AVLTree;
typedef struct AVLNode* Position;

struct AVLNode
{
	ElementType Element;
	AVLTree Left;
	AVLTree Right;
	int Height;
}AVLNode;

int Height(Position node);
Position SingleRotateWithLeft(Position node);
Position SingleRotateWithRight(Position node);
Position DoubleRotateWithLeft(Position node);
Position DoubleRotateWithRight(Position node);
AVLTree Insert(ElementType X, AVLTree T);
void printInorder(AVLTree T);
void DeleteTree(AVLTree T);

int main(int argc, char** argv){
	FILE *fp = fopen(argv[1],"r");
	AVLTree myTree = NULL;
	int num;

	if(fp == NULL){
		printf("There is no file : %s\n",argv[1]);
		exit(-1);
	}

	while (fscanf(fp,"%d",&num) != EOF){
		myTree = Insert(num, myTree);
		printInorder(myTree);
		printf("\n");
	}
	DeleteTree(myTree);
	fclose(fp);
	return 0;

}

int Height(Position node){
	if(node==NULL) return -1;
	else return node->Height;
}
Position SingleRotateWithLeft(Position node){
	Position tmp;

	tmp=node->Left;
	node->Left=tmp->Right;
	tmp->Right=node;

	node->Height=MAX(Height(node->Left),Height(node->Right))+1;
	tmp->Height=MAX(Height(tmp->Left),Height(tmp->Right))+1;

	return tmp;
}
Position SingleRotateWithRight(Position node){
	Position tmp;

	tmp=node->Right;
	node->Right=tmp->Left;
	tmp->Left=node;

	node->Height=MAX(Height(node->Right),Height(node->Left))+1;
	tmp->Height=MAX(Height(tmp->Right),Height(tmp->Left))+1;

	return tmp;
}
Position DoubleRotateWithLeft(Position node){
	Position tmp = node->Left;

	node->Left=SingleRotateWithRight(tmp);
 	node=SingleRotateWithLeft(node);

	return node;
}
Position DoubleRotateWithRight(Position node){
	Position tmp = node->Right;

	node->Right=SingleRotateWithLeft(tmp);
	node=SingleRotateWithRight(node);

	return node;
}
AVLTree Insert(ElementType X, AVLTree T){
	if(T==NULL){
		T=(AVLTree)malloc(sizeof(struct AVLNode));
			if(T==NULL){
				printf("Out of space");
				exit(1);
			}
		T->Element=X;
		T->Height=0;
		T->Left=NULL;
		T->Right=NULL;

		return T;
	}
	else if(X < T->Element){
		T->Left=Insert(X,T->Left);
		if(Height(T->Left)-Height(T->Right)==2){
		if(X<T->Left->Element) 
			T= SingleRotateWithLeft(T);
		else 
			T= DoubleRotateWithLeft(T);
		}
	}
	else if(X > T->Element){
		T->Right=Insert(X,T->Right);
		if(Height(T->Right)-Height(T->Left)==2){
		if(X>T->Right->Element) 
			T= SingleRotateWithRight(T);
		else 
			T= DoubleRotateWithRight(T);
		}
	}
	else{
		printf("[Error] %d already in the tree\n",X);
		return T;
	}

	T->Height=MAX(Height(T->Left),Height(T->Right)) + 1;
	return T;
}

void printInorder(AVLTree T){

	if(T != NULL)
	{
	printInorder(T->Left);
	printf("%d(%d) ",T->Element,T->Height);
	printInorder(T->Right);
	}
	else
		return;
}
void DeleteTree(AVLTree T){
	if(T != NULL)
	{
	DeleteTree(T->Left);
	DeleteTree(T->Right);
	free(T);
	}
	else
		return;
}
