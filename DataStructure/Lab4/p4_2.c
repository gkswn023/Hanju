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
int Pop(Stack* S);

int IsFull(Stack* S);
int IsEmpty(Stack* S);
void DeleteStack(Stack* S);


void main(int argc, char* argv[]){
	FILE* fi=fopen(argv[1],"r");

	Stack* stack;
	char input_str[101];
	int max,i=0,a,b,result;

	fgets(input_str,101,fi);
	max=strlen(input_str);
	printf("Top numbers :");

	stack = CreateStack(max);
	
	while(input_str[i]!='#'){
		if((int)input_str[i]>=49 && (int)input_str[i]<=57){
			if(Push(stack,input_str[i]-48)==-1) break;

		}//1~9 numbers
		else if(input_str[i]=='\0'){
			if(!IsFull(stack)){
				printf("\nwe can't find '#'\n");
				break;
			}//if don't have '#' 
			else{
				 Push(stack,input_str[i]);
				 break;
			}//if stack full and don't have '#'
		}
		else {
			switch(input_str[i]){
				case 43 : 
					a=Pop(stack); b=Pop(stack);
					result=a+b; Push(stack,result);
					break;//'+' operator
				case 45 :
					a=Pop(stack); b=Pop(stack);
					result=b-a; Push(stack,result);
					break;//'-' operator
				case 42 :
					a=Pop(stack); b=Pop(stack);
					result=a*b; Push(stack, result);
					break;//'*' operator
				case 47 :
					a=Pop(stack); b=Pop(stack);
					result=b/a; Push(stack, result);
					break;//'/' operator
				case 37 :
					a=Pop(stack); b=Pop(stack);
					result=b%a; Push(stack, result);
					break;//'%' operator
				default :
					printf("We haven't this operator\n");
					break;
			}			
		}
		i++;
		
}
	printf("\nevaluation result : %d\n",result);
	fclose(fi);
	DeleteStack(stack);
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
		printf("\nStack is Full\n");
		return -1;
}
	else{
		S->key[++S->top]=X;
		printf("%d  ",X);
		return 1;
	}
}

int Pop(Stack* S){
	if(IsEmpty(S)){
		printf("\nStack is Empty\n");
		return -100;
	}
	else{
		return S->key[S->top--];
	}
}

int IsFull(Stack* S)
{
	return S->top==S->max_stack_size-1;
}

int IsEmpty(Stack* S)
{
	return S->top==-1;
}

void DeleteStack(Stack* S){
	free(S->key);
	free(S);
}