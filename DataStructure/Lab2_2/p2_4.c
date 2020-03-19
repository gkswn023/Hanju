#include <stdio.h>
#include<stdbool.h>
#include<stdlib.h>

#define INT_MAX 2147483647

int Zero();
bool IsZero(int);
bool Equal(int, int);
int Successor(int);
int Add(int, int);
int Substract(int, int);

int main(int argc, char** args)
{
	int x = atoi(args[1]);
	int y = atoi(args[2]);

	printf("Is %d zero? %s\n",x,IsZero(x) ? "True" : "False");
	printf("Is %d zoro? %s\n",y,IsZero(y) ? "True" : "False");
	printf("Does %d equal %d? %s\n",x,y,Equal(x,y) ? "True" : "False");
	printf("%d's next number is %d\n",x,Successor(x));
	printf("%d's next number is %d\n",y,Successor(y));
	printf("%d + %d = %d\n",x,y,Add(x,y));
	printf("%d - %d = %d\n",x,y,Substract(x,y));

	return 0;
}

int Zero()
{return 0;}

bool IsZero(int x){
if(x==0) return true;
else{return false;}
}

bool Equal(int x, int y){
if(x==y) return true;
else{return false;}
}

int Successor(int x){
if(x==INT_MAX) return x;
else{ return x+1;}
}

int Add(int x, int y){
if(x+y<=INT_MAX) return x+y;
else{return INT_MAX;}
}

int Substract(int x, int y){
if(x-y<0) return 0;
else{return x-y;}
}
