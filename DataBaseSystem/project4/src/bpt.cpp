/*
 *  bpt.c  
 */
#define Version "1.14"
#include "bpt.h"

node* queue = NULL;
buf_manager manager;
Pre_table pre_table[11];
thread_result result_group[96];
vector<hash_tab> hash_tables;
int min_size = 2147483647;
int min_table;
int table_sum = 0;

int init_db(int num_buf){
    int i;
    for(i=0;i<11;i++){
        pre_table[i].table_size = 0;
        pre_table[i].col_size = 0;
    }    
    manager.size = num_buf;
    manager.cur_size = 0;
    manager.table_id = 1;
    manager.first_page = NULL;
    manager.last_page = NULL;
    manager.buffer_pool = (buf_struct*)calloc(num_buf,sizeof(buf_struct));
    manager.hash_table = (hash_struct*)calloc(num_buf,sizeof(hash_struct));
    for(i=0;i<11;i++){
        manager.table_info[i]=-1;
    }

    return 0;
}

/* open_db if we create new file, use O_CREAT | O_EXCL
else just open(pathname,O_RDWR | O_SYNC, 0777)
if open() is success return fd, else return -1  
*/

int open_table(char *pathname, int num_column){
    int fd;

    if(manager.table_id == 11){
        printf("maximum allocated id is set to 10\n");
        return -1;
    }
	fd = open(pathname,O_RDWR | O_CREAT | O_EXCL | O_SYNC, 0777);// if file already existed, return -1;
	page* header = (page*)calloc(1,PAGESIZE);
    page** free_page;
    int free_page_num = 100;
    int i;

	if(fd > 0){
        if(num_column<2 || num_column>16){
            printf("num_column's range is 2<=num<=16\n");
            close(fd);
            return -1;
        }

        free_page = (page**)calloc(free_page_num,sizeof(page*));

        for(i = 0; i < free_page_num; i++){
            free_page[i]=(page*)calloc(1,PAGESIZE);
        }
                
        manager.table_info[manager.table_id++] = fd;
        //header_page setting
		header->pages_num = 1;
        header->columns_num = num_column;
        pwrite(fd,header,PAGESIZE,SEEK_SET);

        //create free page list
        for(i = 0; i < free_page_num; i++){
            free_page[i]->ppo = header->fpo;
            header->fpo = PAGESIZE * header->pages_num;           
            pwrite(fd,free_page[i],PAGESIZE,PAGESIZE * header->pages_num);
            header->pages_num++;
            pwrite(fd,header,PAGESIZE,SEEK_SET);
        }
        for(i =0; i< free_page_num; i++){
            free(free_page[i]);
        }
        free(free_page);
        free(header);

		return manager.table_id-1;
	}//new file created

	fd = open(pathname,O_RDWR | O_SYNC, 0777);
	if(fd > 0){
        manager.table_info[manager.table_id] = fd;
        pre_table[manager.table_id].table_info = get_table(manager.table_id);
        pre_table[manager.table_id].table_size = pre_table[manager.table_id].table_info.size();        
        pre_table[manager.table_id].col_size = pre_table[manager.table_id].table_info[0].size();
        table_sum += pre_table[manager.table_id].table_size;
        if(pre_table[manager.table_id].table_size < min_size){
            min_size = pre_table[manager.table_id].table_size;
            min_table = manager.table_id;
        }

        return manager.table_id++;
	}

	printf("file open error!\n");

	return -1;
}

// Allocate an on-disk page from the free page list
buf_struct* buf_alloc_page(int table_id){
	buf_struct* header;
    buf_struct* new_page;
    page* free_page;
    header = buf_read_page(table_id, 0);

	if(header->fpo==0){
        free_page = (page*)calloc(1,PAGESIZE);
        pwrite(manager.table_info[table_id], free_page, PAGESIZE, PAGESIZE * header->pages_num);
        new_page = buf_read_page(table_id, header->pages_num);
        header->pages_num++;
        header->is_pinned--;
        header->is_dirty = 1;
	}//free page not existed

	else{
        new_page = buf_read_page(table_id, header->fpo / PAGESIZE);
        header->fpo = new_page->ppo;
        header->is_pinned--;
        header->is_dirty = 1;
	}

    return new_page;
}

// Free an on-disk page to the free page list
void buf_free_page(int table_id, pagenum_t pagenum){
    buf_struct* header;
	buf_struct* free_page;

    header = buf_read_page(table_id, 0);
    free_page = buf_read_page(table_id, pagenum);
    
    memset(free_page, 0, PAGESIZE);
	free_page->ppo = header->fpo;
    header->fpo = PAGESIZE * pagenum;

    header->is_pinned--;
    header->is_dirty = 1;
    free_page->is_pinned--;
    free_page->is_dirty = 1;
}

// Read an on-disk page into the in-memory page structure(dest)
buf_struct* buf_read_page(int table_id, pagenum_t pagenum){
    int hashing;
    int location;
    location = find_location(table_id, pagenum);
    if(location != -1){
        manager.buffer_pool[location].is_pinned++;
        insert_LRU_List(&manager.buffer_pool[location]);
        return &manager.buffer_pool[location];
    }

    hash_struct* old_hash;
    hash_struct* new_hash = (hash_struct*)calloc(1, sizeof(hash_struct));
    hash_struct* tmp;

    if(manager.cur_size == manager.size){
        buf_struct* modify_page = (buf_struct*)calloc(1,sizeof(buf_struct));
        modify_page = manager.last_page;

        while(modify_page->is_pinned != 0){
            modify_page= modify_page->next;
            //if not exist is_pinned != 0 ??
        }
        hashing = modify_page->pagenum % manager.size;
        location = find_location(modify_page->table_id, modify_page->pagenum);

        if(manager.buffer_pool[location].is_dirty){
            buf_write_page(&manager.buffer_pool[location]);
        }
        //delete hash
        old_hash = &manager.hash_table[hashing];
        if(old_hash->next == NULL){
            manager.hash_table[hashing].table_id = 0;
            manager.hash_table[hashing].pagenum = 0;
            manager.hash_table[hashing].buf_loc = 0;
        }
        else{
            if(old_hash->table_id == modify_page->table_id && old_hash->pagenum == modify_page->pagenum){
            manager.hash_table[hashing].table_id = old_hash->next->table_id;
            manager.hash_table[hashing].pagenum = old_hash->next->pagenum;
            manager.hash_table[hashing].buf_loc = old_hash->next->buf_loc;
            manager.hash_table[hashing].next = old_hash->next->next;                
            }
            else{
            while(1){
            if(old_hash->next->table_id == modify_page->table_id && old_hash->next->pagenum == modify_page->pagenum){
                tmp = old_hash->next;
                old_hash->next = old_hash->next->next;
                free(tmp);
                break;
            }
            old_hash = old_hash->next;
            }                
            }
        }
        //insert hash
        hashing = pagenum % manager.size;
        new_hash->table_id = table_id;
        new_hash->pagenum = pagenum;
        new_hash->buf_loc = location;
        if(manager.hash_table[hashing].table_id == 0){
            manager.hash_table[hashing].table_id = table_id;
            manager.hash_table[hashing].pagenum = pagenum;
            manager.hash_table[hashing].buf_loc = location;
        }
        else{
            new_hash->next = manager.hash_table[hashing].next;
            manager.hash_table[hashing].next = new_hash;
        }
        pread(manager.table_info[table_id], &manager.buffer_pool[location], PAGESIZE, pagenum * PAGESIZE);
        manager.buffer_pool[location].table_id = table_id;
        manager.buffer_pool[location].pagenum = pagenum;
        manager.buffer_pool[location].is_dirty = 0;
        manager.buffer_pool[location].is_pinned = 1;
        insert_LRU_List(&manager.buffer_pool[location]);
    } 
    else{
        hashing = pagenum % manager.size;
        new_hash->table_id = table_id;
        new_hash->pagenum = pagenum;
        new_hash->buf_loc = manager.cur_size;
        if(manager.hash_table[hashing].table_id == 0){
            manager.hash_table[hashing].table_id = table_id;
            manager.hash_table[hashing].pagenum = pagenum;
            manager.hash_table[hashing].buf_loc = manager.cur_size;
        }
        else{
            new_hash->next = manager.hash_table[hashing].next;
            manager.hash_table[hashing].next = new_hash;
        }
        pread(manager.table_info[table_id], &manager.buffer_pool[manager.cur_size],PAGESIZE,pagenum * PAGESIZE);
        manager.buffer_pool[manager.cur_size].table_id = table_id;
        manager.buffer_pool[manager.cur_size].pagenum = pagenum;
        manager.buffer_pool[manager.cur_size].is_dirty = 0;
        manager.buffer_pool[manager.cur_size].is_pinned = 1;
        insert_LRU_List(&manager.buffer_pool[manager.cur_size]);
        location = manager.cur_size;
        manager.cur_size++;
    }
   
    return &manager.buffer_pool[location];    
}

int find_location(int table_id, pagenum_t pagenum){
    int num;
    int location = -1;
    hash_struct* new_info;

    num = pagenum % manager.size;
    new_info = &manager.hash_table[num];
    if(new_info->table_id == 0){
        location = -1;
        return location;
    }
    while(1){
        if(new_info == NULL){
            location = -1;
            break;
        }
        if(new_info->table_id == table_id && new_info->pagenum == pagenum){
            location = new_info->buf_loc;
            break;
        }
        new_info = new_info->next;
    } 

    return location;   
}
// Write an in-memory page(src) to the on-disk page
// Write buffer page to on-disk page
void buf_write_page(const buf_struct* src){
    int count;
    count = pwrite(manager.table_info[src->table_id],src,PAGESIZE,PAGESIZE * src->pagenum);
    if(count != PAGESIZE){
        printf("file_write_page error!");
        exit(1);
    }
    return;
}
void insert_LRU_List(buf_struct* new_page){
    if(manager.first_page == NULL && manager.last_page == NULL){
        manager.first_page = new_page;
        manager.last_page = new_page;
        return;
    }    
    else{
        if(new_page->next == NULL && new_page->prev == NULL){
            manager.first_page->next = new_page;
            new_page->prev = manager.first_page;
            manager.first_page = new_page;
            manager.first_page->next = NULL;
        }
        else if(new_page->table_id == manager.first_page->table_id && 
            new_page->pagenum == manager.first_page->pagenum){
            return;
        }
        else if(new_page->table_id == manager.last_page->table_id && 
            new_page->pagenum == manager.last_page->pagenum){
            manager.first_page->next = new_page;
            new_page->prev = manager.first_page;
            manager.first_page = new_page;
            manager.last_page = manager.last_page->next;
            manager.first_page->next = NULL;
            manager.last_page->prev = NULL;
        }
        else{
            manager.first_page->next = new_page;
            new_page->prev->next = new_page->next;
            new_page->next->prev = new_page->prev;
            new_page->prev = manager.first_page;
            manager.first_page = new_page;
            manager.first_page->next = NULL;
        }       
        return;
    }        
}
int close_table(int table_id){
    int i;

    if(manager.table_info[table_id] == -1){
        return -1;
    }
    for(i = 0; i< manager.cur_size;i++){
        if(manager.buffer_pool[i].table_id == table_id){
            if(manager.buffer_pool[i].is_dirty == 1){
                buf_write_page(&manager.buffer_pool[i]);
            }
            manager.buffer_pool[i].is_dirty = 0;
            manager.buffer_pool[i].is_pinned = 0;
        }
    }
    close(manager.table_info[table_id]);
    manager.table_info[table_id] = -1;

    return 0;
}

int shutdown_db(void){
    int i;

    for(i = 0; i < manager.cur_size; i++){
        if(manager.buffer_pool[i].is_dirty){
            buf_write_page(&manager.buffer_pool[i]);
        }
    }
    for(i = 1; i < 11; i++){
        if(manager.table_info[i] != -1){
            close(manager.table_info[i]);
        }
    }    

    free(manager.buffer_pool);
    free(manager.hash_table);

    return 0;
}
/*
find the pagenum where enter the key
*/
pagenum_t find_leaf (int table_id, int64_t key) {
	int i = -1;
    off_t po;
    buf_struct* c;
    buf_struct* header;

    header = buf_read_page(table_id, 0); 

    po = header->rpo;
    //if header->rpo == 0 , this is empty tree. 
    if (header->rpo == 0) {
        header->is_pinned--; 
        printf("Empty tree.\n");
        return 0;
    }
    header->is_pinned--;
    //if header->rpo !=0 , read the data from root_page  
    c = buf_read_page(table_id, po / PAGESIZE);

    while (!c->is_leaf) {
        i = -1;
        while (i < c->num_keys-1) {
            if (key >= c->entrys[i+1].key) i++;
            else break;   
        }
        if(i == -1){
        	po = c->ompo;
        }
        else{
        	po = c->entrys[i].po;
        }

        c->is_pinned--;
        c = buf_read_page(table_id, po / PAGESIZE);
    }

    c->is_pinned--;
    return (po / PAGESIZE);
}
//find the value of the key and return value
int64_t* find (int table_id, int64_t key) {
	int i = 0;
	pagenum_t pagenum;
    int ncolumn;

    if(manager.table_info[table_id] == -1){
        return NULL;
    }

    buf_struct* header;

    header = buf_read_page(table_id, 0);

    ncolumn = header->columns_num;

    if(header->rpo == 0){
        header->is_pinned--;
        return NULL;
    }
    header->is_pinned--;

	pagenum = find_leaf(table_id, key);

    if(pagenum == 0){
        return NULL;
    }

    buf_struct* c;
    int64_t* values = (int64_t*)calloc(ncolumn-1,sizeof(int64_t));
    c = buf_read_page(table_id, pagenum);

    for (i = 0; i < c->num_keys; i++)
        if (c->records[i].key == key) break;
    if (i == c->num_keys){
        c->is_pinned--;
        return NULL;
    }
    else{
        memcpy(values, c->records[i].value, sizeof(int64_t)*(ncolumn-1));
        c->is_pinned--;

        return values;
    }
}

int cut( int length ) {
    if (length % 2 == 0)
        return length/2;
    else
        return length/2 + 1;
}


// INSERTION

int get_left_index(buf_struct* parent, pagenum_t left_pagenum) {

    int left_index = 0;

    if(parent->ompo == (left_pagenum * PAGESIZE)) 
        return -1;

    while (left_index < parent->num_keys - 1 && 
            parent->entrys[left_index].po != (left_pagenum * PAGESIZE))
        left_index++;
    return left_index;
}

void insert_into_leaf( buf_struct* leaf, int64_t key, int64_t* values, int ncolumn ) {
    int i, insertion_point;

    insertion_point = 0;
    while (insertion_point < leaf->num_keys && leaf->records[insertion_point].key < key)
        insertion_point++;

    for (i = leaf->num_keys; i > insertion_point; i--) {
        leaf->records[i].key = leaf->records[i - 1].key;
        memcpy(leaf->records[i].value, leaf->records[i-1].value, sizeof(int64_t) * (ncolumn-1));   
    }
    leaf->records[insertion_point].key = key;
    memcpy(leaf->records[insertion_point].value, values, sizeof(int64_t)*(ncolumn-1));
    leaf->num_keys++;
    
    leaf->is_dirty = 1;
    leaf->is_pinned--;

    return ;
}

void insert_into_leaf_after_splitting(buf_struct* leaf, int64_t key, int64_t* values,int ncolumn) {

    buf_struct* new_leaf;
    int64_t* temp_keys;
    int64_t new_key;
    int64_t** temp_values;
    int insertion_index, split, i, j, k;

    new_leaf = buf_alloc_page(leaf->table_id);
    new_leaf->is_leaf = 1;

    temp_keys = (int64_t*)calloc(1,Leaf_ORDER * sizeof(int64_t));
    if (temp_keys == NULL) {
        perror("Temporary keys array.");
        exit(EXIT_FAILURE);
    }

    temp_values = (int64_t**)calloc(1, Leaf_ORDER * sizeof(int64_t*));
    for(k=0;k<Leaf_ORDER;k++){
        temp_values[k]=(int64_t*)calloc(ncolumn-1,sizeof(int64_t));
    }
    if (temp_values == NULL) {
        perror("Temporary pointers array.");
        exit(EXIT_FAILURE);
    }

    insertion_index = 0;
    while (insertion_index < Leaf_ORDER - 1 && leaf->records[insertion_index].key < key)
        insertion_index++;

    for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
        if (j == insertion_index) j++;
        temp_keys[j] = leaf->records[i].key;
        memcpy(temp_values[j],leaf->records[i].value, sizeof(int64_t)*(ncolumn-1));
    }

    temp_keys[insertion_index] = key;
    memcpy(temp_values[insertion_index], values, sizeof(int64_t)*(ncolumn-1));

    leaf->num_keys = 0;

    split = cut( Leaf_ORDER );

    for (i = 0; i < split; i++) {
        memcpy(leaf->records[i].value, temp_values[i], sizeof(int64_t)*(ncolumn-1));
        leaf->records[i].key = temp_keys[i];
        leaf->num_keys++;
    }

    for (i = split, j = 0; i < Leaf_ORDER; i++, j++) {
        memcpy(new_leaf->records[j].value, temp_values[i], sizeof(int64_t)*(ncolumn-1));
        new_leaf->records[j].key = temp_keys[i];
        new_leaf->num_keys++;
    }

    for(k = 0; k < Leaf_ORDER; k++){
        free(temp_values[k]);
    }
    free(temp_values);
    free(temp_keys);

    new_leaf->ompo = leaf->ompo;
    leaf->ompo = PAGESIZE * (new_leaf->pagenum);

    for (i = leaf->num_keys; i < Leaf_ORDER - 1; i++){
        leaf->records[i].key = 0;
        memset(leaf->records[i].value, 0, 120);
    }
    for (i = new_leaf->num_keys; i < Leaf_ORDER - 1; i++){
        new_leaf->records[i].key = 0;
        memset(new_leaf->records[i].value, 0, 120);
    }

    new_leaf->ppo = leaf->ppo;
    new_key = new_leaf->records[0].key;

    leaf->is_dirty = 1;
    new_leaf->is_dirty = 1;

    insert_into_parent(leaf, new_key, new_leaf);

    return ;
}

void insert_into_page(buf_struct* parent, int left_index, int64_t key, buf_struct* right) {
    int i;

    for (i = parent->num_keys - 1; i > left_index; i--) {
        parent->entrys[i + 1].po = parent->entrys[i].po;
        parent->entrys[i + 1].key = parent->entrys[i].key;
    }
    parent->entrys[left_index + 1].po = (right->pagenum * PAGESIZE);
    parent->entrys[left_index + 1].key = key;
    parent->num_keys++;
 
    right->ppo = (parent->pagenum * PAGESIZE);

    parent->is_dirty = 1;
    right->is_dirty = 1;
    parent->is_pinned--;
    right->is_pinned--;

    return ;
}

void insert_into_page_after_splitting(buf_struct* old_page, int left_index, int64_t key, buf_struct* right) {

    int i, j, split;
    int64_t k_prime;
    buf_struct* new_page, * child;
    pagenum_t new_pagenum;
    int64_t * temp_keys;
    off_t * temp_off;

    temp_keys = (int64_t*)calloc(1,Internal_ORDER * sizeof(int64_t));
    if (temp_keys == NULL) {
        perror("Temporary keys array.");
        exit(EXIT_FAILURE);
    }

    temp_off = (off_t*)calloc(1,Internal_ORDER * sizeof(off_t));
    if (temp_off == NULL) {
        perror("Temporary pointers array.");
        exit(EXIT_FAILURE);
    }

    for (i = 0, j = 0; i < old_page->num_keys; i++, j++) {
        if (j == left_index + 1) j++;
        temp_off[j] = old_page->entrys[i].po;
    }

    for (i = 0, j = 0; i < old_page->num_keys; i++, j++) {
        if (j == left_index + 1) j++;
        temp_keys[j] = old_page->entrys[i].key;
    }

    temp_off[left_index + 1] = (right->pagenum * PAGESIZE);
    temp_keys[left_index + 1] = key;

    right->is_dirty = 1;
    right->is_pinned--;

    split = cut(Internal_ORDER - 1);

    new_page = buf_alloc_page(old_page->table_id);

    old_page->num_keys = 0;
    for (i = 0; i < split ; i++) {
        old_page->entrys[i].po = temp_off[i];
        old_page->entrys[i].key = temp_keys[i];
        old_page->num_keys++;
    }
    new_page->ompo = temp_off[i];
    k_prime = temp_keys[i];
    for (++i, j = 0; i < Internal_ORDER; i++, j++) {
        new_page->entrys[j].po = temp_off[i];
        new_page->entrys[j].key = temp_keys[i];
        new_page->num_keys++;
    }
    free(temp_off);
    free(temp_keys);
    new_page->ppo = old_page->ppo;

    child = buf_read_page(old_page->table_id, (new_page->ompo) / PAGESIZE);
    child->ppo = (new_page->pagenum * PAGESIZE);
    child->is_dirty = 1;
    child->is_pinned--;
    for (i = 0; i < new_page->num_keys; i++) {
        child = buf_read_page(old_page->table_id, (new_page->entrys[i].po) / PAGESIZE);
        child->ppo = (new_page->pagenum * PAGESIZE);
        child->is_dirty = 1;
        child->is_pinned--;        
    }
    old_page->is_dirty = 1;
    new_page->is_dirty = 1;

    insert_into_parent(old_page, k_prime, new_page);

    return ;
}

void insert_into_parent(buf_struct* left, int64_t key, buf_struct* right) {

    int left_index;
    pagenum_t parent_pagenum;
    buf_struct* parent;

    parent_pagenum = (left->ppo) / PAGESIZE;

    if (left->ppo == 0){
       insert_into_new_root(left, key, right);
       return ;
    }

    parent = buf_read_page(left->table_id, parent_pagenum);

    left_index = get_left_index(parent, left->pagenum);

    left->is_dirty = 1;
    left->is_pinned--;
   
    if (parent->num_keys < Internal_ORDER - 1){
        insert_into_page(parent, left_index, key, right);
        return ;
    }

    else{
        insert_into_page_after_splitting(parent, left_index, key, right);
        return ;    
    }
}
//create new root page
void insert_into_new_root(buf_struct* left, int64_t key, buf_struct* right) {

    buf_struct* root_page;
    buf_struct* header;

    root_page = buf_alloc_page(left->table_id);

    root_page->entrys[0].key = key;
    root_page->ompo = (PAGESIZE * left->pagenum);
    root_page->entrys[0].po = (PAGESIZE * right->pagenum);

    left->ppo = (PAGESIZE * root_page->pagenum);
    right->ppo = (PAGESIZE * root_page->pagenum);
    root_page->ppo = 0;
    root_page->num_keys++;

    header = buf_read_page(left->table_id, 0);
    header->rpo = (PAGESIZE * root_page->pagenum);

    header->is_dirty = 1;
    left->is_dirty = 1;
    right->is_dirty = 1;
    root_page->is_dirty = 1;
    header->is_pinned--;    
    left->is_pinned--;
    right->is_pinned--;
    root_page->is_pinned--;

    return ;
}
//create new tree 
void start_new_tree(int table_id, int64_t key, int64_t* values) {
    buf_struct* new_page;
    buf_struct* header;

	new_page = buf_alloc_page(table_id);
    header = buf_read_page(table_id, 0);

	new_page->is_leaf = 1;
	new_page->records[0].key = key;
    memcpy(new_page->records[0].value, values, sizeof(int64_t)* (header->columns_num-1));
    new_page->num_keys++;
    new_page->ppo = 0;

    new_page->is_dirty = 1;
    new_page->is_pinned--;

	header->rpo = (PAGESIZE * new_page->pagenum);
    header->is_dirty = 1;
    header->is_pinned--;

	return;
}

int insert(int table_id, int64_t key, int64_t* values ) {
    buf_struct* leaf;
    buf_struct* header;
    pagenum_t pagenum;
    int ncolumn;

    if (find(table_id, key) != NULL)
        return -1;

    header = buf_read_page(table_id, 0);
    ncolumn = header->columns_num;

    if (header->rpo == 0){
        header->is_pinned--;
        start_new_tree(table_id, key, values);
    	return 0;
    }
    header->is_pinned--;

    pagenum = find_leaf(table_id, key); //find the page where enter the key

    leaf = buf_read_page(table_id, pagenum);
    //just enter the key in the leaf page
    if (leaf->num_keys < Leaf_ORDER - 1) {
        insert_into_leaf(leaf, key, values, ncolumn);
        return 0;
    }
    //need to splitting to enter the key
    insert_into_leaf_after_splitting(leaf, key, values, ncolumn);

    return 0;
}


// DELETION.

//• Find the matching records and delete it if found.
//• If success, return 0. Otherwise, return non-zero value.

int get_neighbor_index( pagenum_t pagenum, buf_struct* parent_page) {

    int i;

    if(parent_page->ompo == PAGESIZE * pagenum){
        return -2;
    }
    for (i = 0; i < parent_page->num_keys; i++){
        if (parent_page->entrys[i].po == PAGESIZE * pagenum){
            return i - 1;
        }
    }
    // Error state.
    printf("Search for nonexistent page_offset to page in parent.\n");
    exit(EXIT_FAILURE);
}

buf_struct* remove_entry_from_page(buf_struct* n, int64_t key, int ncolumn) {

    int i;

    if(n->is_leaf){
        i = 0;

        while(n->records[i].key != key){
            i++;
        }
        for(++i; i < n->num_keys; i++){
            n->records[i - 1].key = n->records[i].key;
            memcpy(n->records[i - 1].value, n->records[i].value, sizeof(int64_t)*(ncolumn-1));   
        }

        n->num_keys--;

        for (i = n->num_keys; i < Leaf_ORDER - 1; i++){
        n->records[i].key = 0;
        memset(n->records[i].value, 0, 120);
        }
        
        n->is_dirty = 1;

        return n;
    }
    else{
        i = 0;

        while(n->entrys[i].key != key){
            i++;
        }
        for(++i; i < n->num_keys; i++){
            n->entrys[i - 1].key = n->entrys[i].key; 
            n->entrys[i - 1].po  = n->entrys[i].po; 
        }

        n->num_keys--;

        for(i = n->num_keys; i < Internal_ORDER - 1; i++){
            n->entrys[i].key = 0;
            n->entrys[i].po  = 0;
        }
        
        n->is_dirty = 1;

        return n;
    }
}

void adjust_root(buf_struct* header, buf_struct* root) {

    if (root->num_keys > 0){
        root->is_dirty = 1;
        root->is_pinned--;
        return ;
    }

    if (!root->is_leaf) {
        buf_struct* new_root;
        new_root = buf_read_page(root->table_id, root->ompo / PAGESIZE);
        header->rpo = root->ompo;
        new_root->ppo = 0;
        buf_free_page(root->table_id, root->pagenum);
        new_root->is_dirty = 1;
        new_root->is_pinned--;
    }

    else{
        header->rpo = 0;
        buf_free_page(root->table_id, root->pagenum);
    }

    root->is_dirty = 1;
    root->is_pinned--;

    return;
}

void coalesce_nodes(buf_struct* n, buf_struct* neighbor_page, buf_struct* parent,
                     int neighbor_index, int64_t k_prime, int ncolumn) {

    int i, j, neighbor_insertion_index, n_end;
    buf_struct* tmp;
    pagenum_t tmp_pagenum;
    pagenum_t cur_pagenum = n->pagenum;
    pagenum_t neighbor_pagenum = neighbor_page->pagenum;

    if (neighbor_index == -2) {
        tmp_pagenum = cur_pagenum;
        cur_pagenum = neighbor_pagenum;
        neighbor_pagenum = tmp_pagenum;
        neighbor_page = buf_read_page(n->table_id, neighbor_pagenum);
        n = buf_read_page(n->table_id, cur_pagenum);
        n->is_pinned--;
        neighbor_page->is_pinned--;
    }

    neighbor_insertion_index = neighbor_page->num_keys;

    if (!n->is_leaf) {

        neighbor_page->entrys[neighbor_insertion_index].key = k_prime;    
        neighbor_page->num_keys++;
        n_end = n->num_keys;

        neighbor_page->entrys[neighbor_insertion_index].po = n->ompo;

        for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            neighbor_page->entrys[i].key = n->entrys[j].key;
            neighbor_page->entrys[i].po = n->entrys[j].po;
            neighbor_page->num_keys++;
            n->num_keys--;
        }

        for (i = neighbor_insertion_index; i < neighbor_page->num_keys ; i++) {
            tmp = buf_read_page(n->table_id, neighbor_page->entrys[i].po / PAGESIZE);
            tmp->ppo = neighbor_page->pagenum * PAGESIZE;
            tmp->is_dirty = 1;
            tmp->is_pinned--;
        }

        buf_free_page(n->table_id, n->pagenum);
        n->is_dirty = 1;
        n->is_pinned--;
        neighbor_page->is_dirty = 1;
        neighbor_page->is_pinned--;
    }

    else {
        neighbor_page->ompo = n->ompo;
        n_end = n->num_keys;
        for(i = neighbor_insertion_index, j = 0; j < n_end; i++, j++){
            neighbor_page->records[i].key = n->records[j].key;
            memcpy(neighbor_page->records[i].value, n->records[j].value, sizeof(int64_t)*(ncolumn-1));
            neighbor_page->num_keys++;
            n->num_keys--;
        }
        buf_free_page(n->table_id, n->pagenum);
        n->is_dirty = 1;
        n->is_pinned--;
        neighbor_page->is_dirty = 1;
        neighbor_page->is_pinned--;
    }
    delete_entry(parent, k_prime);

    return ;
}

void delete_entry(buf_struct* n, int64_t key) {

    int min_keys;
    off_t neighbor;
    int neighbor_index;
    int k_prime_index;
    int64_t k_prime;
    buf_struct* header;
    int ncolumn;

    header = buf_read_page(n->table_id, 0);

    ncolumn = header->columns_num;

    n = remove_entry_from_page(n, key, ncolumn);

    if (n->pagenum * PAGESIZE == header->rpo){
        adjust_root(header, n);
        header->is_dirty = 1;
        header->is_pinned--;
        return ;
    }
    header->is_pinned--;

    min_keys = 1;

    if (n->num_keys >= min_keys){
        n->is_dirty = 1;
        n->is_pinned--;
        return ;
    }

    //modification
    buf_struct* parent;
    buf_struct* neighbor_page;

    parent = buf_read_page(n->table_id, n->ppo / PAGESIZE);

    neighbor_index = get_neighbor_index(n->pagenum, parent);
    k_prime_index = neighbor_index == -2 ? 0 : neighbor_index + 1;
    k_prime = parent->entrys[k_prime_index].key;
    if(neighbor_index == -2){
        neighbor = parent->entrys[0].po;
    }
    else{
        neighbor = neighbor_index == -1 ? parent->ompo : parent->entrys[neighbor_index].po;
    }

    neighbor_page = buf_read_page(n->table_id, neighbor / PAGESIZE);

    coalesce_nodes(n, neighbor_page, parent, neighbor_index, k_prime, ncolumn);
}

int erase(int table_id, int64_t key) {

    pagenum_t key_leaf;
    int64_t* values;

    values = find(table_id, key);
    key_leaf = find_leaf(table_id, key);
    if (values != NULL && key_leaf != 0) {

        buf_struct* cur_page;

        cur_page = buf_read_page(table_id, key_leaf);
        delete_entry(cur_page, key);
        
        return 0;
    }

    return -1;
}

//join
int64_t join(const char* query){
    vector<PredicationInfo> Operations = parse(query);
    vector<int> used_tables;
    //vector< vector<int64_t> > result_table;
    //vector< vector<int64_t> > left_table;
    //vector< vector<int64_t> > right_table;
    int left_exist, right_exist;
    int64_t sum_keys;
    int j;
    PredicationInfo tmp;
    Op tmp_op;

    for(j = 0; j < Operations.size(); j++){
        if(Operations[j].left.table_id == min_table || Operations[j].right.table_id == min_table){
            tmp = Operations[0];
            Operations[0] = Operations[j];
            Operations[j] = tmp;

            if(pre_table[Operations[0].left.table_id].table_size > pre_table[Operations[0].right.table_id].table_size){
            tmp_op = Operations[0].left;
            Operations[0].left = Operations[0].right;
            Operations[0].right = tmp_op;
            }
            break;
        }
    }    

    used_tables.push_back(Operations[0].left.table_id);
    used_tables.push_back(Operations[0].right.table_id);

    //left_table = pre_table[Operations[0].left.table_id].table_info;
    //right_table = pre_table[Operations[0].right.table_id].table_info;
    int left_col = 16*(Operations[0].left.table_id-1) + Operations[0].left.col;
    int right_col = 16*(Operations[0].right.table_id-1) + Operations[0].right.col;

    if(pre_table[Operations[0].left.table_id].table_size > pre_table[Operations[0].right.table_id].table_size){
        Run(Operations[0].right.table_id, right_col, Operations[0].left.table_id, left_col);    
    }
    else{
        Run(Operations[0].left.table_id, left_col, Operations[0].right.table_id, right_col);
    }

    for(int i = 1; i < Operations.size(); i++){
        left_exist = is_used_table(used_tables, Operations[i].left.table_id);
        right_exist = is_used_table(used_tables, Operations[i].right.table_id);
        j = i + 1;
        while(left_exist == 0 && right_exist == 0){
            tmp = Operations[i];
            Operations[i] = Operations[j];
            Operations[j] = tmp;

            left_exist = is_used_table(used_tables, Operations[i].left.table_id);
            right_exist = is_used_table(used_tables, Operations[i].right.table_id);
            j++;
        }

        left_col = 16*(Operations[i].left.table_id-1) + Operations[i].left.col;
        right_col = 16*(Operations[i].right.table_id-1) + Operations[i].right.col;
        
        if(left_exist == 1 && right_exist == 0){
            used_tables.push_back(Operations[i].right.table_id);
            //right_table = pre_table[Operations[i].right.table_id].table_info;
            Run(0, left_col, Operations[i].right.table_id, right_col);
        }
        else if(left_exist == 0 && right_exist == 1){
            used_tables.push_back(Operations[i].left.table_id);
            //left_table = pre_table[Operations[i].left.table_id].table_info;
            Run(0, right_col, Operations[i].left.table_id, left_col);
        }
        else{
            printf("error\n");
            break;
        }

        if(pre_table[0].table_size == 1){
            return 0;
        }
    }

    sum_keys = result_sum(used_tables);

    return sum_keys;
}

int is_used_table(vector<int> used_tables, int table_id){
    int i;

    for(i = 0; i < used_tables.size(); i++){
        if(used_tables[i] == table_id) 
            return 1;
    }    

    return 0;
}

int64_t result_sum(vector<int> used_tables){
    int col, spot;
    int i, j;
    int64_t sum = 0;

    for(i = 0; i < used_tables.size(); i++){
        spot = 0;
        col = 16*(used_tables[i]-1) + 1;
  
        while(pre_table[0].table_info[0][spot] != col) spot++;

        for(j = 1; j < pre_table[0].table_size; j++){
            sum += pre_table[0].table_info[j][spot];
        }
    }

    return sum;
}

void Run(int left_id, int left_col, int right_id, int right_col){

    Pre_table tmp_result;
    //vector<int64_t> left_value;
    //vector<int64_t> right_value;
    vector<int64_t> category;
    //pthread_t threads[96];
    //pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    //thread_struct thread_data[96];

    int i = 0, j = 0;
    int k,m;
    
    while(pre_table[left_id].table_info[0][i] != left_col) i++;
    while(pre_table[right_id].table_info[0][j] != right_col) j++;

    for(k = 0; k < pre_table[left_id].col_size; k++){
        category.push_back(pre_table[left_id].table_info[0][k]);
    }
    for(k = 0; k < pre_table[right_id].col_size; k++){
        category.push_back(pre_table[right_id].table_info[0][k]);
    }

    tmp_result.table_info.push_back(category);

    build_hash(left_id, i);

    //if(table_sum < 60000){
        vector<int64_t> left_value;
        vector<int64_t> right_value;
        hash_tab* tmp_hash;
        int64_t key_data;
        int hashing;

        for(k = 1; k < pre_table[right_id].table_size; k++){
            key_data = pre_table[right_id].table_info[k][j];
            hashing = key_data % (pre_table[left_id].table_size * 2);
            tmp_hash = hash_tables[hashing].next;
            while(tmp_hash != NULL){
                if(tmp_hash->hash_value == key_data){
                    left_value = pre_table[left_id].table_info[tmp_hash->row];
                    right_value = pre_table[right_id].table_info[k];

                    left_value.insert(left_value.end(), right_value.begin(), right_value.end());

                    tmp_result.table_info.push_back(left_value);
                }
                tmp_hash = tmp_hash->next;
            }
        }
    //}
    /*else if(table_sum >=60000 || table_sum < 150000){
    for(k = 0; k < 5; k++){
    thread_data[k].thread_size = 5;
    thread_data[k].cur_num = k;    
    thread_data[k].right_col = j;
    thread_data[k].left_tab = left_id;
    thread_data[k].right_tab = right_id;
    thread_data[k].size_hash = pre_table[left_id].table_size * 2;
    }    

    for(k = 0; k < 5; k++){        
        if(pthread_create(&threads[k], NULL, thread_run, (void*)&thread_data[k]) < 0){          
            printf("pthread create error!\n");
            return ;
        }
    }
    for(k = 0; k < 5; k++){
        pthread_join(threads[k], NULL);
    }

    for(k = 0; k < 5; k++){
        if(result_group[k].result_tab.size() != 0){
        tmp_result.table_info.insert(tmp_result.table_info.end(), result_group[k].result_tab.begin(), result_group[k].result_tab.end());
        result_group[k].result_tab.clear();
        }
    }
    }*/
    /*else{
    for(k = 0; k < 30; k++){
    thread_data[k].thread_size = 30;
    thread_data[k].cur_num = k;    
    thread_data[k].right_col = j;
    thread_data[k].left_tab = left_id;
    thread_data[k].right_tab = right_id;
    thread_data[k].size_hash = pre_table[left_id].table_size * 2;
    }    

    for(k = 0; k < 30; k++){        
        if(pthread_create(&threads[k], NULL, thread_run, (void*)&thread_data[k]) < 0){          
            printf("pthread create error!\n");
            return ;
        }
    }
    for(k = 0; k < 30; k++){
        pthread_join(threads[k], NULL);
    }

    for(k = 0; k < 30; k++){
        if(result_group[k].result_tab.size() != 0){
        tmp_result.table_info.insert(tmp_result.table_info.end(), result_group[k].result_tab.begin(), result_group[k].result_tab.end());
        result_group[k].result_tab.clear();
        }
    }
    }*/
    /*
    for(k = 1; k < pre_table[left_id].table_size; k++){
        for(m = 1; m < pre_table[right_id].table_size; m++){
            if(pre_table[left_id].table_info[k][i] == pre_table[right_id].table_info[m][j]){
                left_value = pre_table[left_id].table_info[k];
                right_value = pre_table[right_id].table_info[m];

                left_value.insert(left_value.end(), right_value.begin(), right_value.end());
                tmp_result.table_info.push_back(left_value);
            }
        }
    }
    */
    tmp_result.table_size = tmp_result.table_info.size();
    tmp_result.col_size = tmp_result.table_info[0].size();

    pre_table[0] = tmp_result;

    return;    
}

void build_hash(int table_ide, int col){
    int64_t hashing;
    int i;

    hash_tables.clear();
    hash_tables.resize( pre_table[table_ide].table_size * 2);

    for(i = 1; i < pre_table[table_ide].table_size; i++){
        hash_tab* tmp_hash = (hash_tab*)calloc(1,sizeof(hash_tab));        
        tmp_hash->row = i;
        tmp_hash->hash_value = pre_table[table_ide].table_info[i][col];

        hashing = tmp_hash->hash_value % (pre_table[table_ide].table_size * 2);
        tmp_hash->next = hash_tables[hashing].next;
        hash_tables[hashing].next = tmp_hash; 
    }
}

vector< vector<int64_t> > get_table(int table_id){
    vector< vector<int64_t> > table;
    pagenum_t num_col;
    buf_struct* header;
    buf_struct* page;
    off_t po;
    int i,j;
    vector<int64_t> category;
    vector<int64_t> content;

    header = buf_read_page(table_id, 0);
    num_col = header->columns_num;
    po = header->rpo;

    for(i = 1; i <= num_col; i++){
        category.push_back(16*(table_id-1) + i);
    }
    table.push_back(category);

    header->is_pinned--;

    page = buf_read_page(table_id, po / PAGESIZE);
    while(!page->is_leaf){
        po = page->ompo;
        page->is_pinned--;
        page = buf_read_page(table_id, po / PAGESIZE);
    }//page is leaf page

    //bring recordss(key,value[15])
    while(page->ompo != 0){
        for(i = 0; i < page->num_keys; i++){
            content.push_back(page->records[i].key);
            for(j = 0; j < num_col-1; j++){
                content.push_back(page->records[i].value[j]);
            }
            table.push_back(content);
            content.clear();
        }
        po = page->ompo;
        page->is_pinned--;
        page = buf_read_page(table_id, po / PAGESIZE);
    }
    for(i = 0; i < page->num_keys; i++){
        content.push_back(page->records[i].key);
        for(j = 0; j < num_col-1; j++){
            content.push_back(page->records[i].value[j]);
        }
        table.push_back(content);
        content.clear();
    }
    page->is_pinned--;

    return table;
}

vector<PredicationInfo> parse(string query){
    vector<PredicationInfo> ops;
    istringstream iss(query);

    while(1){
        int t1, c1, t2, c2;
        char equal, point, empersand;
        iss >> t1 >> point >> c1;
        assert(point == '.');
        iss >> equal;
        assert(equal == '=');
        iss >> t2 >> point >> c2;
        assert(point == '.');
        PredicationInfo op;
        op.left.table_id = t1;
        op.left.col = c1;
        op.right.table_id = t2;
        op.right.col = c2;
        ops.push_back(op);
        if(!(iss >> empersand) || empersand != '&') break;
        empersand = '\0';
    }

    return ops;
}

//Print bpt

void enqueue( off_t po ) {
    node * c;
    node * new_node = (node *)calloc(1, sizeof(node));
    new_node->po = po;
    new_node->next = NULL;

    if (queue == NULL) {
        queue = new_node;
        queue->next = NULL;
    }
    else {
        c = queue;
        while(c->next != NULL) {
            c = c->next;
        }
        c->next = new_node;
        new_node->next = NULL;
    }
}

node * dequeue( void ) {
    node * n = queue;
    queue = queue->next;
    n->next = NULL;
    return n;
}

void print_tree(int table_id) {
    node * n = NULL;
    buf_struct* tmp;
    buf_struct* tmp2;
    buf_struct* header;
    int i = 0;

    header = buf_read_page(table_id, 0);

    if (header->rpo == 0) {
        printf("Empty tree.\n");
        header->is_pinned--;
        return;
    }

    queue = NULL;
    enqueue(header->rpo);
    header->is_pinned--;
    while( queue != NULL ) {
        n = dequeue();
        tmp = buf_read_page(table_id, n->po / PAGESIZE);
        if(tmp->is_leaf){
            if(tmp->ppo != 0){
                printf("\n");
            }
            while(tmp->ompo != 0){
                for(i = 0 ; i < tmp->num_keys; i++){
                printf("%ld ",tmp->records[i].key);           
                }
            tmp->is_pinned--;    
            n = dequeue();
            tmp = buf_read_page(table_id, n->po / PAGESIZE);            
            printf("| ");  
            }
            for(i = 0 ; i < tmp->num_keys; i++){
             printf("%ld " ,tmp->records[i].key);
            }
            printf("|\n");
            tmp->is_pinned--;
        }
        else{
            if(tmp->ppo != 0){
            tmp2 = buf_read_page(table_id, tmp->ppo / PAGESIZE);
                if(tmp2->ompo == n->po) printf("\n");
                tmp2->is_pinned--;
            }

            for(i = 0 ; i < tmp->num_keys; i++){
                printf("%ld ",tmp->entrys[i].key);
            }
            if(!tmp->is_leaf){
                enqueue(tmp->ompo);
                for(i = 0; i < tmp->num_keys; i++){
                    enqueue(tmp->entrys[i].po);
                }  
            }
        printf("| ");
        tmp->is_pinned--;
        }
    }
}

void print_hash(){
    int i;
    hash_struct* tmp;

    for(i = 0; i<manager.size; i++){
        tmp = &manager.hash_table[i];
        if(tmp->table_id == 0){
            printf("%d : NULL\n", i);
        }
        else{
            while(tmp->next != NULL){
                printf("%d : %d %ld\n",i, tmp->table_id, tmp->pagenum);
                tmp = tmp->next;
            }
            printf("%d : %d %ld\n",i, tmp->table_id, tmp->pagenum);
        }
    }
}

void print_pin(){
    int i;
    buf_struct* tmp;

    for(i = 0; i<manager.size; i++){
        tmp = &manager.buffer_pool[i];
        if(tmp->table_id == 0){
            printf("NULL\n");
        }
        else{
            printf("page : %ld, pin : %d\n",tmp->pagenum, tmp->is_pinned);
        }
    }
}