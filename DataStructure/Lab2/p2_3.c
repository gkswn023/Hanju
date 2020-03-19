#include<stdio.h>
#include<stdlib.h>

int main(int argc,char *argv[])
{	
	int i=atoi(argv[1]);

	char** name=(char**)malloc(i*sizeof(char*));
	
	printf("enter %d names : \n",i);

	int j;
	for(j=0;j<i;j++){
	name[j]=(char*)malloc(30*sizeof(char));
	scanf("%s",name[j]);
	}

	printf("the names you entered : \n");
	for(j=0;j<i;j++){
	printf("%s\n",name[j]);
	}
	for(j=0;j<i;j++){
	free(name[j]);
	}
	free(name);
}
	
	
		

