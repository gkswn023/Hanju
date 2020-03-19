#include<stdio.h>
#include<stdlib.h>

typedef struct Tree{
	int value;
	struct Tree* left;
	struct Tree* right;
}Tree;


Tree* insertNode(Tree *root, int key);
void printInorder(Tree* root);
void deleteTree(Tree* root);

void main(int argc, char* argv[]){
	FILE *fi = fopen(argv[1], "r");
	char cv;
	int key;
	Tree* root = NULL;
	while(!feof(fi)){
		fscanf(fi, "%c", &cv);
		switch(cv){
		case 'i' :
			fscanf(fi, "%d", &key);
			root = insertNode(root, key);
			break;
		case 'p' :
			fscanf(fi,"%c",&cv);
			if(cv== 'i')
				printInorder(root);
			printf("\n");
			break;
		}
	}
	fclose(fi);
	deleteTree(root);
}

void deleteTree(Tree* root){
	if(root==NULL){
		return;
	}
	deleteTree(root->left);
	deleteTree(root->right);
	free(root);
}

Tree* insertNode(Tree* root, int key)
{
	Tree* P,*T;// P is parent, T is present
	Tree* N;// new node

	T=root;
	P=NULL;
	while(T!=NULL){
		if(T->value==key){
			printf("insertion error : There is already %d in the tree\n",key);
			return root;
		}
		P=T;
		if(key<T->value) T=P->left;
		else T=P->right;
	}
	N= (Tree *)malloc(sizeof(struct Tree));
	N->value=key;
	N->left=NULL;
	N->right=NULL;
	if(P==NULL) {
		printf("insert %d\n",key);
		return N;
	}
	else{
		if(key<P->value) {
			P->left=N;
			printf("insert %d\n",key);
			return root;
		}
		else{
			P->right=N;
			printf("insert %d\n",key);
			return root;
		}
	}
}

void printInorder(Tree *root)
{
	if(root)
	{
	printInorder(root->left);
	printf("%d ",root->value);
	printInorder(root->right);	
	}
}
