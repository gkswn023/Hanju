#include "bpt.h"

// MAIN

int main( int argc, char ** argv ) {

    int64_t input;
    int i, k, num;
    int table_id;
    int ncolumn;
    int is_open = 0;
    char instruction;
    buf_struct* header;
    int64_t* values;
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
            scanf("%s %d", pathname, &ncolumn);
            if(is_open < 10){
            i = open_table(pathname, ncolumn);
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
            i = erase(table_id, input);
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
            scanf("%d ""%" PRId64, &table_id, &input);

            header = buf_read_page(table_id, 0);
            ncolumn = header->columns_num;
            header->is_pinned--;            

            values = (int64_t*)calloc(ncolumn-1,sizeof(int64_t));            
            for(k = 0; k< ncolumn-1; k++){
                scanf("%ld", &values[k]);
            }
            i = insert(table_id, input, values);
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
            values = find(table_id, input, 0, NULL);
            header = buf_read_page(table_id, 0);
            ncolumn = header->columns_num;
            header->is_pinned--;              
            if(values){
                printf("key : %" PRId64 "/ values : ",input);
                for(k=0; k<ncolumn-1; k++){
                    printf("%" PRId64, values[k]);
                }
                printf("\n");
            }
            else{
                printf("not exist\n");
            }
            }
            else{
            printf("First open the file ! instruction 'o'\n");    
            }
            break;
        case 'j':
            if(is_open){
                int64_t result;
                char input[100];
                const char* query;
                cin >> input;
                query = input;

                result = join(query);
                printf("%" PRId64, result);
                printf("\n");
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
