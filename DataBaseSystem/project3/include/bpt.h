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

typedef struct page_t{
	union{
		struct{
			off_t fpo;//free page offset
			off_t rpo;//root page offset
			pagenum_t pages_num;//number of pages
			char reserved_h[4072];
		};
		struct{
			off_t ppo;
			int is_leaf;
			int num_keys;
			char reserved_p[104];
			off_t ompo;
			union {
				entry entry[248];
				record record[31];
			};
		};
	};
}page;

typedef struct Buffer_Structure{
union{
	struct{
		off_t fpo;//free page offset
		off_t rpo;//root page offset
		pagenum_t pages_num;//number of pages
		char reserved_h[4072];
	};
	struct{
		off_t ppo;
		int is_leaf;
		int num_keys;
		char reserved_p[104];
		off_t ompo;
		union {
			entry entry[248];
			record record[31];
		};
	};
};
int table_id;
pagenum_t pagenum;
int is_dirty;
int is_pinned;
struct Buffer_Structure* next;
struct Buffer_Structure* prev;
}buf_struct;

typedef struct Buffer_Manager{
int size;
int cur_size;
int table_id;
int table_info[11];
struct Buffer_Structure* first_page;
struct Buffer_Structure* last_page;
struct Buffer_Structure* buffer_pool;
struct Hash_Structure* hash_table; 
}buf_manager;

typedef struct Hash_Structure{
int table_id;
pagenum_t pagenum;
int buf_loc;
struct Hash_Structure* next;
}hash_struct;

typedef struct node{
off_t po;
struct node* next;
}node;

int init_db(int num_buf);
int open_table(char *pathname);
buf_struct* buf_alloc_page(int table_id);
void buf_free_page(int table_id, pagenum_t pagenum);
buf_struct* buf_read_page(int table_id, pagenum_t pagenum);
int find_location(int table_id, pagenum_t pagenum);
void buf_write_page(const buf_struct* src);
void insert_LRU_List(buf_struct* new);

pagenum_t find_leaf (int table_id, int64_t key);
char* find (int table_id, int64_t key);
int cut( int length );

// Insertion.

int get_left_index(buf_struct* parent, pagenum_t left_pagenum);
void insert_into_leaf( buf_struct* leaf, int64_t key, char* value );
void insert_into_leaf_after_splitting(buf_struct* leaf, int64_t key, char* value);
void insert_into_page(buf_struct* parent, int left_index, int64_t key, buf_struct* right);
void insert_into_page_after_splitting(buf_struct* old_page, int left_index, int64_t key, buf_struct* right);
void insert_into_parent(buf_struct* left, int64_t key, buf_struct* right);
void insert_into_new_root(buf_struct* left, int64_t key, buf_struct* right);
void start_new_tree(int table_id, int64_t key, char* value);
int insert(int table_id, int64_t key, char* value );

// Deletion.

int get_neighbor_index( pagenum_t pagenum, buf_struct* parent_page);
buf_struct* remove_entry_from_page(buf_struct* n, int64_t key);
void adjust_root(buf_struct* header, buf_struct* root);
void coalesce_nodes(buf_struct* n, buf_struct* neighbor_page, buf_struct* parent,
                     int neighbor_index, int64_t k_prime);
void delete_entry(buf_struct* n, int64_t key);
int delete(int table_id, int64_t key);
//close

int close_table(int table_id);
int shutdown_db(void);
//Print_bpt

void enqueue( off_t po );
node * dequeue( void );
void print_tree( int table_id );
void print_hash( void );
void print_pin( void );

#endif /* __BPT_H__*/
