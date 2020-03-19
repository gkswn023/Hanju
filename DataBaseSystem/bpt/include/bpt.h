#ifndef __BPT_H__
#define __BPT_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#ifdef WINDOWS
#define bool char
#define false 0
#define true 1
#endif

#define Internal_ORDER 249
#define Leaf_ORDER 32
#define PAGESIZE 4096
// Minimum order is necessarily 3.  We set the maximum
// order arbitrarily.  You may change the maximum order.
#define MIN_ORDER 3
#define MAX_ORDER 20

typedef uint64_t pagenum_t;

typedef struct record{
int64_t key;
char value[120];
}record;

typedef struct Internal_entry{
int64_t key;
off_t po;//page offset
}entry;

typedef struct Header_page{
off_t fpo;//free page offset
off_t rpo;//root page offset
pagenum_t pages_num;//number of pages
char reserved[4072];
}Hp;

typedef struct page_t{
off_t ppo;
int is_leaf;
int num_keys;
char reserved[104];
off_t ompo;
union {
entry entry[248];
record record[31];
};
}page;

typedef struct node{
off_t po;
struct node* next;
}node;

int open_db(char *pathname);
pagenum_t file_alloc_page();
void file_free_page(pagenum_t pagenum);
void file_read_page(pagenum_t pagenum, page* dest);
void file_write_page(pagenum_t pagenum, const page* src);

pagenum_t find_leaf (int64_t key);
char* find (int64_t key);
int cut( int length );

// Insertion.

int get_left_index(page * parent, pagenum_t left_pagenum);
void insert_into_leaf( page * leaf, pagenum_t pagenum, int64_t key, char* value );
void insert_into_leaf_after_splitting(page* leaf, pagenum_t pagenum, int64_t key, char* value);
void insert_into_page(pagenum_t parent_pagenum, page* parent, int left_index, int64_t key,
                        pagenum_t new_pagenum, page* right);
void insert_into_page_after_splitting(pagenum_t old_pagenum ,page * old_page, int left_index, 
        int64_t key, pagenum_t right_pagenum, page * right);
void insert_into_parent(pagenum_t pagenum, page * left, int64_t key, pagenum_t new_pagenum, page * right);
void insert_into_new_root(pagenum_t pagenum, page * left, int64_t key, pagenum_t new_pagenum, page * right);
void start_new_tree(int64_t key, char* value);
int insert( int64_t key, char* value );

// Deletion.

int get_neighbor_index( pagenum_t pagenum, page* parent_page);
page* remove_entry_from_page(pagenum_t pagenum, page* n, int64_t key);
void adjust_root(pagenum_t pagenum, page* root);
void coalesce_nodes(pagenum_t pagenum, page * n, off_t neighbor, int neighbor_index, int64_t k_prime);
void delete_entry( pagenum_t pagenum, page* n, int64_t key);
int delete( int64_t key );

//Print_bpt

void enqueue( off_t po );
node * dequeue( void );
void print_tree( void );

#endif /* __BPT_H__*/
