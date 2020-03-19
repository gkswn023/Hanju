#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Stack{
	int* key;
	int top;
	int max_stack_size;
}Stack;

Stack* CreateStack(int max);
int Push(Stack* S, int X);
int IsFull(Stack* S);

void main(int argc, char* argv[]){
	FILE* fi=fopen(argv[1],"r");

	Stack* stack;
	char input_str[101];
	int max,i=0,a,b,result;

	fgets(input_str,101,fi);
	max=strlen(input_str);
	printf("Pushed numbers :\n");

	stack = CreateStack(max);
	
	while(input_str[i]!='#'){
		if((int)input_str[i]>=49 && (int)input_str[i]<=57)
		if(Push(stack,input_str[i]-48)==-1) break;
		i++;
		
}
	fclose(fi);
	free(stack);
}

Stack* CreateStack(int max){
	Stack* S=NULL;
	S = (Stack*)malloc(sizeof(struct Stack));
	S->key=(int*)malloc(sizeof(int)*max);
	S->max_stack_size=max;
	S->top=-1;
	return S;
}

int Push(Stack* S,int X){
	if(IsFull(S)){
		printf("Stack is Full");
		return -1;
}
	else{
		S->key[++S->top]=X;
		printf("%d inserted\n",X);
		return 1;
	}
}

int IsFull(Stack* S)
{
	return S->top==S->max_stack_size-1;
}
