#include "bpt.h"

// MAIN

int main( int argc, char ** argv ) {

    int64_t input;
    int i, num;
    int table_id;
    int is_open = 0;
    char instruction;
    char* value = (char*)calloc(1,120);
    char* pathname = (char*)calloc(1,20);

    printf("B+tree start !\n");
    printf("buffer size : ");
    scanf("%d", &num);

    i = init_db(num);

    if(i != 0){
        printf("init_db error\n");
    }
    scanf("%c", &instruction);
    printf("> ");
    while (1) {
        scanf("%c", &instruction);

        switch (instruction) {
        case 'o':
            scanf("%s", pathname);
            if(is_open < 10){
            i = open_table(pathname);
            if(i == -1) printf("open_db error\n");
            else{
                printf("table id : %d , file name : %s\n", i, pathname);
            }
            is_open++;
            }
            else{
                printf("table's num is alreay 10\n");
            }
            break;
        case 'd':
            if(is_open){
            scanf("%d ""%" PRId64, &table_id, &input);
            i = delete(table_id, input);
            if(i == -1) printf("deletion error\n");
            if(i ==  0) printf("deletion success\n");
            print_tree(table_id);
            }
            else{
            printf("First open the file ! instruction 'o'\n");
            }
            break;
        case 'i':
            if(is_open){
            scanf("%d ""%" PRId64" %s",&table_id, &input, value);
            i = insert(table_id, input, value);
            if( i == -1) printf("alreay exist\n");
            if( i ==  0) printf("insertion success\n");
            print_tree(table_id);
            }
            else{
            printf("First open the file ! instruction 'o'\n");                
            }
            break;
        case 'f':
            if(is_open){
            scanf("%d ""%" PRId64,&table_id, &input);
            value = find(table_id, input);
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
            shutdown_db();

            while (getchar() != (int)'\n');
            return EXIT_SUCCESS;
            break;
        case 'c':
            if(is_open){
            scanf("%d", &table_id);
            close_table(table_id);
            }
            else{
            printf("First open the file ! instruction 'o'\n");
            }    
            break;
        case 'p':
            if(is_open){
            scanf("%d", &table_id);
            print_tree(table_id);
            }
            else{
            printf("First open the file ! instruction 'o'\n");
            }
            break;
        case 'h':
            if(is_open){
            print_hash();
            }
            else{
            printf("First open the file ! instruction 'o'\n");
            }
            break;            
        case 'n':
            if(is_open){
            print_pin();
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
