#include "bpt.h"

// MAIN

int main( int argc, char ** argv ) {

    int64_t input;
    int i;
    int is_open = 0;
    char instruction;
    char* value = (char*)calloc(1,120);
    char* pathname = (char*)calloc(1,20);


    printf("B+tree start !\n");

    printf("> ");
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 'o':
            scanf("%s", pathname);
            if(!is_open){
            i = open_db(pathname);
            if(i == -1) printf("open_db error\n");
            if(i ==  0) printf("open_db success\n");
            is_open++;
            }
            else{
            printf("data file is already opened\n");   
            }
            break;
        case 'd':
            if(is_open){
            scanf("%" PRId64, &input);
            i = delete(input);
            if(i == -1) printf("deletion error\n");
            if(i ==  0) printf("deletion success\n");
            print_tree();
            }
            else{
            printf("First open the file ! instruction 'o'\n");
            }
            break;
        case 'i':
            if(is_open){
            scanf("%" PRId64" %s", &input, value);
            i = insert(input, value);
            if( i == -1) printf("alreay exist\n");
            if( i ==  0) printf("insertion success\n");
            print_tree();
            }
            else{
            printf("First open the file ! instruction 'o'\n");                
            }
            break;
        case 'f':
            if(is_open){
            scanf("%" PRId64, &input);
            value = find(input);
            if(value){
                printf("key : %"PRId64"/ value : %s\n",input,value);
            }
            else{
                printf("not exist\n");
            }
            }
            else{
            printf("First open the file ! instruction 'o'\n");    
            }
            break;
        case 'q':
            while (getchar() != (int)'\n');
            return EXIT_SUCCESS;
            break;
        case 'p':
            if(is_open){
            print_tree();
            }
            else{
            printf("First open the file ! instruction 'o'\n");
            }
            break;
        default:
            printf("please retry\n");
            break;
        }
        while (getchar() != (int)'\n');
        printf("> ");
    }
    printf("\n");

    return EXIT_SUCCESS;
}
