/*
 *  bpt.c  
 */
#define Version "1.14"
#include "bpt.h"

int fd;
Hp *header;
node * queue = NULL;

int open_db(char *pathname){
	fd = open(pathname,O_RDWR | O_CREAT | O_EXCL | O_SYNC, 0777);// file already existed, return -1;
	header = (Hp*)calloc(1,PAGESIZE);
    page** free_page;
    int free_page_num = 100;
    int i;

    free_page = (page**)calloc(free_page_num,sizeof(page*));

    for(i = 0; i < free_page_num; i++){
        free_page[i]=(page*)calloc(1,PAGESIZE);
    }

	if(fd > 0){
		header->pages_num = 1;
        pwrite(fd,header,PAGESIZE,SEEK_SET);
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

		return 0;
	}//new file created

	fd = open(pathname,O_RDWR | O_SYNC,0777);
	if(fd > 0){
        pread(fd,header,PAGESIZE,SEEK_SET);
		return 0;
	}

	printf("file open error!\n");

	return -1;
}

// Allocate an on-disk page from the free page list
pagenum_t file_alloc_page(){
		
	page* new_page = (page*)calloc(1,PAGESIZE);
	pagenum_t pagenum;
	
	if(header->fpo==0){
		pwrite(fd,new_page,PAGESIZE,PAGESIZE * header->pages_num);
		header->pages_num++;
		pwrite(fd,header,PAGESIZE,SEEK_SET);
		free(new_page);
		return (header->pages_num)-1;
	}//free page not existed

	else{
		pread(fd,new_page,PAGESIZE,header->fpo);
		pagenum = header->fpo / PAGESIZE;
		header->fpo = new_page->ppo;
		pwrite(fd,header,PAGESIZE,SEEK_SET);
		free(new_page);
		return pagenum;
	}
}

// Free an on-disk page to the free page list
void file_free_page(pagenum_t pagenum){
	page* free_page = (page*)calloc(1,PAGESIZE);
	free_page->ppo = header->fpo;
    header->fpo = PAGESIZE * pagenum;

	pwrite(fd,free_page,PAGESIZE,PAGESIZE * pagenum);
	pwrite(fd,header,PAGESIZE,SEEK_SET);
	free(free_page);
}

// Read an on-disk page into the in-memory page structure(dest)
void file_read_page(pagenum_t pagenum, page* dest){
	int count;
    count = pread(fd,dest,PAGESIZE,PAGESIZE * pagenum);
    if(count != PAGESIZE){
        printf("file_read_page error!");
        exit(1);
    } 
}

// Write an in-memory page(src) to the on-disk page
void file_write_page(pagenum_t pagenum, const page* src){
	int count;
    count = pwrite(fd,src,PAGESIZE,PAGESIZE * pagenum);
    if(count != PAGESIZE){
        printf("file_write_page error!");
        exit(1);
    }
}

pagenum_t find_leaf (int64_t key) {

	int i = -1;
    off_t po = header->rpo;
    page* c = (page*)calloc(1,PAGESIZE);

    if (header->rpo == 0) { 
        printf("Empty tree.\n");
        return 0;
    }

    file_read_page((header->rpo) / PAGESIZE, c);

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

        file_read_page(po / PAGESIZE, c);
    }

    free(c);

    return (po / PAGESIZE);
}

char* find (int64_t key) {

	int i = 0;
	pagenum_t pagenum;

    if(header->rpo == 0) return NULL;

	pagenum = find_leaf(key);

    if(pagenum == 0){
        return NULL;
    }

    page* c = (page*)calloc(1,PAGESIZE);
    char* value = (char*)calloc(1,120);
    file_read_page(pagenum,c);

    for (i = 0; i < c->num_keys; i++)
        if (c->record[i].key == key) break;
    if (i == c->num_keys){
        free(c);
        return NULL;
    }
    else{
    	strcpy(value, c->record[i].value);
        free(c);
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

int get_left_index(page * parent, pagenum_t left_pagenum) {

    int left_index = 0;

    if(parent->ompo == (left_pagenum * PAGESIZE)) 
    	return -1;

    while (left_index < parent->num_keys - 1 && 
            parent->entry[left_index].po != (left_pagenum * PAGESIZE))
        left_index++;
    return left_index;
}

void insert_into_leaf( page * leaf, pagenum_t pagenum, int64_t key, char* value ) {

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
    file_write_page(pagenum,leaf);

    return ;
}

void insert_into_leaf_after_splitting(page* leaf, pagenum_t pagenum, int64_t key, char* value) {

    page * new_leaf;
    pagenum_t new_pagenum;
    int64_t* temp_keys;
    int64_t new_key;
    char** temp_value;
    int insertion_index, split, i, j, k;

    new_pagenum = file_alloc_page();
	new_leaf = (page*)calloc(1,PAGESIZE); 
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
    leaf->ompo = PAGESIZE * new_pagenum;

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

    file_write_page(pagenum, leaf);
    file_write_page(new_pagenum, new_leaf);

    insert_into_parent(pagenum, leaf, new_key, new_pagenum, new_leaf);

    return ;
}

void insert_into_page(pagenum_t parent_pagenum, page* parent, int left_index, int64_t key,
						pagenum_t new_pagenum, page* right) {
    int i;

    for (i = parent->num_keys - 1; i > left_index; i--) {
       	parent->entry[i + 1].po = parent->entry[i].po;
       	parent->entry[i + 1].key = parent->entry[i].key;
    }
    parent->entry[left_index + 1].po = (new_pagenum * PAGESIZE);
    parent->entry[left_index + 1].key = key;
    parent->num_keys++;

    file_write_page(parent_pagenum, parent);
 
    right->ppo = (parent_pagenum * PAGESIZE);

    file_write_page(new_pagenum,right);

    return ;
}

void insert_into_page_after_splitting(pagenum_t old_pagenum ,page * old_page, int left_index, 
        int64_t key, pagenum_t right_pagenum, page * right) {

    int i, j, split;
    int64_t k_prime;
    page * new_page, * child;
    pagenum_t new_pagenum;
    int64_t * temp_keys;
    off_t * temp_off;

    new_page = (page*)calloc(1,PAGESIZE);
    child = (page*)calloc(1,PAGESIZE);

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

    temp_off[left_index + 1] = (right_pagenum * PAGESIZE);
    temp_keys[left_index + 1] = key;

    split = cut(Internal_ORDER - 1);

    new_pagenum = file_alloc_page();

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

    file_read_page((new_page->ompo) / PAGESIZE, child);
    child->ppo = (new_pagenum * PAGESIZE);
    file_write_page((new_page->ompo) / PAGESIZE, child);
    for (i = 0; i < new_page->num_keys; i++) {
        file_read_page((new_page->entry[i].po) / PAGESIZE, child);
        child->ppo = (new_pagenum * PAGESIZE);
        file_write_page((new_page->entry[i].po) / PAGESIZE, child);
    }
    free(child);
    file_write_page(old_pagenum, old_page);
    file_write_page(new_pagenum, new_page);

    insert_into_parent(old_pagenum, old_page, k_prime, new_pagenum, new_page);

    return ;
}

void insert_into_parent(pagenum_t pagenum, page * left, int64_t key, pagenum_t new_pagenum, page * right) {

    int left_index;
    pagenum_t parent_pagenum;
    page * parent = (page *)calloc(1,PAGESIZE);

    parent_pagenum = (left->ppo) / PAGESIZE;

    if (left->ppo == 0){
       insert_into_new_root(pagenum, left, key, new_pagenum, right);
       free(parent);
       return ;
    }

    file_read_page(parent_pagenum, parent);

    left_index = get_left_index(parent, pagenum);
   
    if (parent->num_keys < Internal_ORDER - 1){
        insert_into_page( parent_pagenum, parent, left_index, key, new_pagenum, right);
        free(parent);
        return ;
    }

    else{
    	insert_into_page_after_splitting(parent_pagenum, parent, left_index, key, new_pagenum, right);
        free(parent);
        return ;    
    }
}

void insert_into_new_root(pagenum_t pagenum, page * left, int64_t key, pagenum_t new_pagenum, page * right) {

    page* root_page = (page*)calloc(1,PAGESIZE);
    pagenum_t root_pagenum;
    root_pagenum = file_alloc_page();

    root_page->entry[0].key = key;
    root_page->ompo = (PAGESIZE * pagenum);
    root_page->entry[0].po = (PAGESIZE * new_pagenum);

    left->ppo = (PAGESIZE * root_pagenum);
    right->ppo = (PAGESIZE * root_pagenum);
    root_page->ppo = 0;
    root_page->num_keys++;

    file_write_page(pagenum, left);
    file_write_page(new_pagenum, right);
    file_write_page(root_pagenum, root_page);

    header->rpo = (PAGESIZE * root_pagenum);
    pwrite(fd,header,PAGESIZE,SEEK_SET);

    free(root_page);

    return ;
}

void start_new_tree(int64_t key, char* value) {
	pagenum_t pagenum;
	page* new_page = (page*)calloc(1,PAGESIZE);

	pagenum = file_alloc_page();

	new_page->is_leaf = 1;
	new_page->record[0].key = key;
	strcpy(new_page->record[0].value, value);
	new_page->num_keys++;
    new_page->ppo = 0;
	file_write_page(pagenum, new_page);

	header->rpo = (PAGESIZE * pagenum);
	pwrite(fd, header, PAGESIZE, SEEK_SET);

    free(new_page);

	return ;
}

int insert( int64_t key, char* value ) {

    page * leaf = (page *)calloc(1,PAGESIZE);
    pagenum_t pagenum;

    if (strlen(value) > 120){
    	printf("too long, value length <= 120\n");
    	return -1;
    }
    if (find(key) != NULL)
        return -1;

    if (header->rpo == 0){
        start_new_tree(key, value);
    	return 0;
    }

    pagenum = find_leaf(key);

    file_read_page(pagenum, leaf);

    if (leaf->num_keys < Leaf_ORDER - 1) {
        insert_into_leaf(leaf, pagenum, key, value);
        free(leaf);
        return 0;
    }

    insert_into_leaf_after_splitting(leaf,pagenum, key, value);

    free(leaf);

    return 0;
}


// DELETION.

//• Find the matching record and delete it if found.
//• If success, return 0. Otherwise, return non-zero value.

int get_neighbor_index( pagenum_t pagenum, page* parent_page) {

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

page* remove_entry_from_page(pagenum_t pagenum, page* n, int64_t key) {

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
        file_write_page(pagenum, n);

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
        file_write_page(pagenum, n);

        return n;
    }
}

void adjust_root(pagenum_t pagenum, page* root) {

    if (root->num_keys > 0)
        return ;

    if (!root->is_leaf) {
        page * new_root = (page *)calloc(1, PAGESIZE);
        file_read_page(root->ompo / PAGESIZE, new_root);
        header->rpo = root->ompo;
        new_root->ppo = 0;
        file_free_page(pagenum);
        file_write_page(root->ompo / PAGESIZE, new_root);
        pwrite(fd, header, PAGESIZE, SEEK_SET);
        free(new_root);
    }

    else{
        header->rpo = 0;
        file_free_page(pagenum);
        pwrite(fd, header, PAGESIZE, SEEK_SET);
    }

    return ;
}

void coalesce_nodes(pagenum_t pagenum, page * n, off_t neighbor, int neighbor_index, int64_t k_prime) {

    int i, j, neighbor_insertion_index, n_end;
    page * tmp = (page *)calloc(1, PAGESIZE);
    pagenum_t tmp_pagenum;
    page * neighbor_page = (page *)calloc(1, PAGESIZE);
    page * parent = (page *)calloc(1, PAGESIZE);
    off_t parent_off;

    file_read_page(neighbor / PAGESIZE, neighbor_page);
    file_read_page(n->ppo / PAGESIZE, parent);

    parent_off = n->ppo;

    if (neighbor_index == -2) {
        tmp_pagenum = pagenum;
        pagenum = neighbor / PAGESIZE;
        neighbor = tmp_pagenum * PAGESIZE;
        file_read_page(neighbor / PAGESIZE, neighbor_page);
        file_read_page(pagenum, n);
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

        file_free_page(pagenum);
        for (i = neighbor_insertion_index; i < neighbor_page->num_keys ; i++) {
            file_read_page(neighbor_page->entry[i].po / PAGESIZE, tmp);
            tmp->ppo = neighbor;
            file_write_page(neighbor_page->entry[i].po / PAGESIZE, tmp);
        }  
        file_write_page(neighbor / PAGESIZE, neighbor_page);
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
        file_free_page(pagenum);
        file_write_page(neighbor / PAGESIZE, neighbor_page);
    }
    delete_entry(parent_off / PAGESIZE, parent, k_prime);
    free(tmp);
    free(neighbor_page);
    free(parent);
    return ;
}

void delete_entry( pagenum_t pagenum, page* n, int64_t key) {

    int min_keys;
    off_t neighbor;
    int neighbor_index;
    int k_prime_index;
    int64_t k_prime;

    n = remove_entry_from_page(pagenum, n, key);

    if (pagenum * PAGESIZE == header->rpo){
        adjust_root(pagenum, n);
        return ;
    }

    min_keys = 1;

    if (n->num_keys >= min_keys){
        return ;
    }

    page* parent = (page*)calloc(1, PAGESIZE);
    file_read_page(n->ppo / PAGESIZE, parent);

    neighbor_index = get_neighbor_index( pagenum, parent);
    k_prime_index = neighbor_index == -2 ? 0 : neighbor_index + 1;
    k_prime = parent->entry[k_prime_index].key;
    if(neighbor_index == -2){
        neighbor = parent->entry[0].po;
    }
    else{
        neighbor = neighbor_index == -1 ? parent->ompo : parent->entry[neighbor_index].po;
    }

    coalesce_nodes(pagenum, n, neighbor, neighbor_index, k_prime);

    free(parent);

    return;
}

int delete( int64_t key ) {

    pagenum_t key_leaf;
    char* value = (char *)calloc(1,120);

    value = find(key);
    key_leaf = find_leaf(key);

    if (value != NULL && key_leaf != 0) {

        page* cur_page =(page *)calloc(1, PAGESIZE);

        file_read_page(key_leaf, cur_page);
        delete_entry(key_leaf, cur_page, key);

        free(value);
        free(cur_page);
        
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

void print_tree( void ) {

    node * n = NULL;
    page * tmp = (page *)calloc(1, PAGESIZE);
    page * tmp2 = (page *)calloc(1, PAGESIZE);
    int i = 0;

    if (header->rpo == 0) {
        printf("Empty tree.\n");
        return;
    }

    queue = NULL;
    enqueue(header->rpo);
    while( queue != NULL ) {
        n = dequeue();
        file_read_page(n->po / PAGESIZE, tmp);
        if(tmp->is_leaf){
            if(tmp->ppo != 0){
                printf("\n");
            }
            while(tmp->ompo != 0){
                for(i = 0 ; i < tmp->num_keys; i++){
                printf("%ld ",tmp->record[i].key);           
                }
            n = dequeue();
            file_read_page(n->po / PAGESIZE, tmp);            
            printf("| ");  
            }
            for(i = 0 ; i < tmp->num_keys; i++){
             printf("%ld " ,tmp->record[i].key);
            }
            printf("|\n");
        }
        else{
            if(tmp->ppo != 0){
            file_read_page(tmp->ppo / PAGESIZE, tmp2);
                if(tmp2->ompo == n->po) printf("\n");
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
        }
    }
}
