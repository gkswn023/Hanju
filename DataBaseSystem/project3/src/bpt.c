/*
 *  bpt.c  
 */
#define Version "1.14"
#include "bpt.h"

node* queue = NULL;
buf_manager manager;

int init_db(int num_buf){
    int i;
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

int open_table(char *pathname){
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

    free_page = (page**)calloc(free_page_num,sizeof(page*));

    for(i = 0; i < free_page_num; i++){
        free_page[i]=(page*)calloc(1,PAGESIZE);
    }

	if(fd > 0){
        manager.table_info[manager.table_id++] = fd;
        //header_page setting
		header->pages_num = 1;
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
        manager.table_info[manager.table_id++] = fd;
        return manager.table_id-1;
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
void insert_LRU_List(buf_struct* new){
    if(manager.first_page == NULL && manager.last_page == NULL){
        manager.first_page = new;
        manager.last_page = new;
        return;
    }    
    else{
        if(new->next == NULL && new->prev == NULL){
            manager.first_page->next = new;
            new->prev = manager.first_page;
            manager.first_page = new;
            manager.first_page->next = NULL;
        }
        else if(new->table_id == manager.first_page->table_id && 
            new->pagenum == manager.first_page->pagenum){
            return;
        }
        else if(new->table_id == manager.last_page->table_id && 
            new->pagenum == manager.last_page->pagenum){
            manager.first_page->next = new;
            new->prev = manager.first_page;
            manager.first_page = new;
            manager.last_page = manager.last_page->next;
            manager.first_page->next = NULL;
            manager.last_page->prev = NULL;
        }
        else{
            manager.first_page->next = new;
            new->prev->next = new->next;
            new->next->prev = new->prev;
            new->prev = manager.first_page;
            manager.first_page = new;
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
            if (key >= c->entry[i+1].key) i++;
            else break;   
        }
        if(i == -1){
        	po = c->ompo;
        }
        else{
        	po = c->entry[i].po;
        }

        c->is_pinned--;
        c = buf_read_page(table_id, po / PAGESIZE);
    }

    c->is_pinned--;
    return (po / PAGESIZE);
}
//find the value of the key and return value
char* find (int table_id, int64_t key) {
	int i = 0;
	pagenum_t pagenum;

    if(manager.table_info[table_id] == -1){
        return NULL;
    }

    buf_struct* header;

    header = buf_read_page(table_id, 0);

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
    char* value = (char*)calloc(1,120);
    c = buf_read_page(table_id, pagenum);

    for (i = 0; i < c->num_keys; i++)
        if (c->record[i].key == key) break;
    if (i == c->num_keys){
        c->is_pinned--;
        return NULL;
    }
    else{
    	strcpy(value, c->record[i].value);
        c->is_pinned--;
        return value;
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
            parent->entry[left_index].po != (left_pagenum * PAGESIZE))
        left_index++;
    return left_index;
}

void insert_into_leaf( buf_struct* leaf, int64_t key, char* value ) {
    int i, insertion_point;

    insertion_point = 0;
    while (insertion_point < leaf->num_keys && leaf->record[insertion_point].key < key)
        insertion_point++;

    for (i = leaf->num_keys; i > insertion_point; i--) {
        leaf->record[i].key = leaf->record[i - 1].key;
        strcpy(leaf->record[i].value, leaf->record[i-1].value);   
    }
    leaf->record[insertion_point].key = key;
    strcpy(leaf->record[insertion_point].value, value);
    leaf->num_keys++;
    
    leaf->is_dirty = 1;
    leaf->is_pinned--;

    return ;
}

void insert_into_leaf_after_splitting(buf_struct* leaf, int64_t key, char* value) {

    buf_struct* new_leaf;
    int64_t* temp_keys;
    int64_t new_key;
    char** temp_value;
    int insertion_index, split, i, j, k;

    new_leaf = buf_alloc_page(leaf->table_id);
    new_leaf->is_leaf = 1;

    temp_keys = (int64_t*)calloc(1,Leaf_ORDER * sizeof(int64_t));
    if (temp_keys == NULL) {
        perror("Temporary keys array.");
        exit(EXIT_FAILURE);
    }

    temp_value = (char**)calloc(1, Leaf_ORDER * sizeof(char *));
    for(k=0;k<Leaf_ORDER;k++){
        temp_value[k]=(char*)calloc(1,120);
    }
    if (temp_value == NULL) {
        perror("Temporary pointers array.");
        exit(EXIT_FAILURE);
    }

    insertion_index = 0;
    while (insertion_index < Leaf_ORDER - 1 && leaf->record[insertion_index].key < key)
        insertion_index++;

    for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
        if (j == insertion_index) j++;
        temp_keys[j] = leaf->record[i].key;
        strcpy(temp_value[j],leaf->record[i].value);
    }

    temp_keys[insertion_index] = key;
    strcpy(temp_value[insertion_index], value);

    leaf->num_keys = 0;

    split = cut( Leaf_ORDER );

    for (i = 0; i < split; i++) {
        strcpy(leaf->record[i].value, temp_value[i]);
        leaf->record[i].key = temp_keys[i];
        leaf->num_keys++;
    }

    for (i = split, j = 0; i < Leaf_ORDER; i++, j++) {
        strcpy(new_leaf->record[j].value, temp_value[i]);
        new_leaf->record[j].key = temp_keys[i];
        new_leaf->num_keys++;
    }

    for(k = 0; k < Leaf_ORDER; k++){
        free(temp_value[k]);
    }
    free(temp_value);
    free(temp_keys);

    new_leaf->ompo = leaf->ompo;
    leaf->ompo = PAGESIZE * (new_leaf->pagenum);

    for (i = leaf->num_keys; i < Leaf_ORDER - 1; i++){
        leaf->record[i].key = 0;
        memset(leaf->record[i].value, 0, 120);
    }
    for (i = new_leaf->num_keys; i < Leaf_ORDER - 1; i++){
        new_leaf->record[i].key = 0;
        memset(new_leaf->record[i].value, 0, 120);
    }

    new_leaf->ppo = leaf->ppo;
    new_key = new_leaf->record[0].key;

    leaf->is_dirty = 1;
    new_leaf->is_dirty = 1;

    insert_into_parent(leaf, new_key, new_leaf);

    return ;
}

void insert_into_page(buf_struct* parent, int left_index, int64_t key, buf_struct* right) {
    int i;

    for (i = parent->num_keys - 1; i > left_index; i--) {
        parent->entry[i + 1].po = parent->entry[i].po;
        parent->entry[i + 1].key = parent->entry[i].key;
    }
    parent->entry[left_index + 1].po = (right->pagenum * PAGESIZE);
    parent->entry[left_index + 1].key = key;
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
        temp_off[j] = old_page->entry[i].po;
    }

    for (i = 0, j = 0; i < old_page->num_keys; i++, j++) {
        if (j == left_index + 1) j++;
        temp_keys[j] = old_page->entry[i].key;
    }

    temp_off[left_index + 1] = (right->pagenum * PAGESIZE);
    temp_keys[left_index + 1] = key;

    right->is_dirty = 1;
    right->is_pinned--;

    split = cut(Internal_ORDER - 1);

    new_page = buf_alloc_page(old_page->table_id);

    old_page->num_keys = 0;
    for (i = 0; i < split ; i++) {
        old_page->entry[i].po = temp_off[i];
        old_page->entry[i].key = temp_keys[i];
        old_page->num_keys++;
    }
    new_page->ompo = temp_off[i];
    k_prime = temp_keys[i];
    for (++i, j = 0; i < Internal_ORDER; i++, j++) {
        new_page->entry[j].po = temp_off[i];
        new_page->entry[j].key = temp_keys[i];
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
        child = buf_read_page(old_page->table_id, (new_page->entry[i].po) / PAGESIZE);
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

    root_page->entry[0].key = key;
    root_page->ompo = (PAGESIZE * left->pagenum);
    root_page->entry[0].po = (PAGESIZE * right->pagenum);

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
void start_new_tree(int table_id, int64_t key, char* value) {
    buf_struct* new_page;
    buf_struct* header;

	new_page = buf_alloc_page(table_id);

	new_page->is_leaf = 1;
	new_page->record[0].key = key;
	strcpy(new_page->record[0].value, value);
	new_page->num_keys++;
    new_page->ppo = 0;

    new_page->is_dirty = 1;
    new_page->is_pinned--;

    header = buf_read_page(table_id, 0);
	header->rpo = (PAGESIZE * new_page->pagenum);
    header->is_dirty = 1;
    header->is_pinned--;

	return;
}

int insert(int table_id, int64_t key, char* value ) {
    buf_struct* leaf;
    buf_struct* header;
    pagenum_t pagenum;

    if (strlen(value) > 120){
    	printf("too long, value length <= 120\n");
    	return -1;
    }
    if (find(table_id, key) != NULL)
        return -1;

    header = buf_read_page(table_id, 0);

    if (header->rpo == 0){
        header->is_pinned--;
        start_new_tree(table_id, key, value);
    	return 0;
    }
    header->is_pinned--;

    pagenum = find_leaf(table_id, key); //find the page where enter the key

    leaf = buf_read_page(table_id, pagenum);
    //just enter the key in the leaf page
    if (leaf->num_keys < Leaf_ORDER - 1) {
        insert_into_leaf(leaf, key, value);
        return 0;
    }
    //need to splitting to enter the key
    insert_into_leaf_after_splitting(leaf, key, value);

    return 0;
}


// DELETION.

//• Find the matching record and delete it if found.
//• If success, return 0. Otherwise, return non-zero value.

int get_neighbor_index( pagenum_t pagenum, buf_struct* parent_page) {

    int i;

    if(parent_page->ompo == PAGESIZE * pagenum){
        return -2;
    }
    for (i = 0; i < parent_page->num_keys; i++){
        if (parent_page->entry[i].po == PAGESIZE * pagenum){
            return i - 1;
        }
    }
    // Error state.
    printf("Search for nonexistent page_offset to page in parent.\n");
    exit(EXIT_FAILURE);
}

buf_struct* remove_entry_from_page(buf_struct* n, int64_t key) {

    int i;

    if(n->is_leaf){
        i = 0;

        while(n->record[i].key != key){
            i++;
        }
        for(++i; i < n->num_keys; i++){
            n->record[i - 1].key = n->record[i].key;
            strcpy(n->record[i - 1].value, n->record[i].value);   
        }

        n->num_keys--;

        for (i = n->num_keys; i < Leaf_ORDER - 1; i++){
        n->record[i].key = 0;
        memset(n->record[i].value, 0, 120);
        }
        
        n->is_dirty = 1;

        return n;
    }
    else{
        i = 0;

        while(n->entry[i].key != key){
            i++;
        }
        for(++i; i < n->num_keys; i++){
            n->entry[i - 1].key = n->entry[i].key; 
            n->entry[i - 1].po  = n->entry[i].po; 
        }

        n->num_keys--;

        for(i = n->num_keys; i < Internal_ORDER - 1; i++){
            n->entry[i].key = 0;
            n->entry[i].po  = 0;
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
                     int neighbor_index, int64_t k_prime) {

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

        neighbor_page->entry[neighbor_insertion_index].key = k_prime;    
        neighbor_page->num_keys++;
        n_end = n->num_keys;

        neighbor_page->entry[neighbor_insertion_index].po = n->ompo;

        for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            neighbor_page->entry[i].key = n->entry[j].key;
            neighbor_page->entry[i].po = n->entry[j].po;
            neighbor_page->num_keys++;
            n->num_keys--;
        }

        for (i = neighbor_insertion_index; i < neighbor_page->num_keys ; i++) {
            tmp = buf_read_page(n->table_id, neighbor_page->entry[i].po / PAGESIZE);
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
            neighbor_page->record[i].key = n->record[j].key;
            strcpy(neighbor_page->record[i].value, n->record[j].value);
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

    n = remove_entry_from_page(n, key);

    header = buf_read_page(n->table_id, 0);

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
    k_prime = parent->entry[k_prime_index].key;
    if(neighbor_index == -2){
        neighbor = parent->entry[0].po;
    }
    else{
        neighbor = neighbor_index == -1 ? parent->ompo : parent->entry[neighbor_index].po;
    }

    neighbor_page = buf_read_page(n->table_id, neighbor / PAGESIZE);

    coalesce_nodes(n, neighbor_page, parent, neighbor_index, k_prime);
}

int delete(int table_id, int64_t key) {

    pagenum_t key_leaf;
    char* value = (char *)calloc(1,120);

    value = find(table_id, key);
    key_leaf = find_leaf(table_id, key);
    if (value != NULL && key_leaf != 0) {

        buf_struct* cur_page;

        cur_page = buf_read_page(table_id, key_leaf);
        delete_entry(cur_page, key);

        free(value);
        
        return 0;
    }

    return -1;
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
                printf("%ld ",tmp->record[i].key);           
                }
            tmp->is_pinned--;    
            n = dequeue();
            tmp = buf_read_page(table_id, n->po / PAGESIZE);            
            printf("| ");  
            }
            for(i = 0 ; i < tmp->num_keys; i++){
             printf("%ld " ,tmp->record[i].key);
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
                printf("%ld ",tmp->entry[i].key);
            }
            if(!tmp->is_leaf){
                enqueue(tmp->ompo);
                for(i = 0; i < tmp->num_keys; i++){
                    enqueue(tmp->entry[i].po);
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