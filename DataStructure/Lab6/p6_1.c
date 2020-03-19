#include<stdio.h>
#include<stdlib.h>

typedef int ElementType;
typedef struct AVLNode* AVLTree;

struct AVLNode
{
	ElementType Element;
	AVLTree Left;
	AVLTree Right;
	int Height;
}AVLNode;

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

AVLTree Insert(ElementType X, AVLTree T){
	if(T==NULL){
		T=(AVLTree)malloc(sizeof(struct AVLNode));
			if(T==NULL){
				printf("fatal error");
				return T;
			}
		T->Height=0;
		T->Left=NULL;
		T->Right=NULL;
		T->Element=X;
	}
	else if(X<T->Element){
		T->Left=Insert(X,T->Left);
		if((T->Height)-(T->Left->Height)<1){
		T->Height++;
		}
	}
	else if(X>T->Element){
		T->Right=Insert(X,T->Right);
		if((T->Height)-(T->Right->Height)<1){
		T->Height++;
		}
	}
	else{
		printf("[Error] %d already in the tree!\n",X);
	}
	return T;
}

void printInorder(AVLTree T){

	if(T)
	{
	printInorder(T->Left);
	printf("%d(%d) ",T->Element,T->Height);
	printInorder(T->Right);
	}
}

void DeleteTree(AVLTree T){
	if(T)
	{
	DeleteTree(T->Left);
	DeleteTree(T->Right);
	free(T);
	}
}
