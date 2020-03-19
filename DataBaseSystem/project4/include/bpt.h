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
#include <string.h>
#include <vector>
#include <assert.h>
#include <sstream>
#include <string>
#include <cstdio>
#include <iostream>
#include <time.h>

#ifdef WINDOWS
#define bool char
#define false 0
#define true 1
#endif

#define Internal_ORDER 249//4
#define Leaf_ORDER 32//4
#define PAGESIZE 4096
// Minimum order is necessarily 3.  We set the maximum
// order arbitrarily.  You may change the maximum order.
#define MIN_ORDER 3
#define MAX_ORDER 20

using namespace std;

typedef uint64_t pagenum_t;

typedef struct Record{
int64_t key;
int64_t value[15];
}Record;

typedef struct Internal_entry{
int64_t key;
off_t po;//page offset
}Entry;

typedef struct page_t{
	union{
		struct{
			off_t fpo;//free page offset
			off_t rpo;//root page offset
			pagenum_t pages_num;//number of pages
			pagenum_t columns_num;//number of columns
			char reserved_h[4064];
		};
		struct{
			off_t ppo;
			int is_leaf;
			int num_keys;
			char reserved_p[104];
			off_t ompo;
			union {
				Entry entrys[248];
				Record records[31];
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
		pagenum_t columns_num;//number of columns		
		char reserved_h[4064];
	};
	struct{
		off_t ppo;
		int is_leaf;
		int num_keys;
		char reserved_p[104];
		off_t ompo;
		union {
			Entry entrys[248];
			Record records[31];
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

typedef struct Operation{
int table_id;
int col;
}Op;

typedef struct PredicationInfo{
struct Operation left;
struct Operation right;
}PredicationInfo;

typedef struct Pre_table{
vector< vector<int64_t> > table_info;
int table_size;
int col_size;
}Pre_table;

typedef struct hash_tab{
	int row;
	int64_t hash_value;
	struct hash_tab* next;
}hash_tab;

typedef struct thread_struct{
int left_tab;
int left_col;
int right_tab;
int right_col;
int cur_num;
}thread_struct;

typedef struct thread_result{
vector< vector<int64_t> > result_tab;
}thread_result;

typedef struct node{
off_t po;
struct node* next;
}node;

int init_db(int num_buf);
int open_table(char *pathname, int num_column);
buf_struct* buf_alloc_page(int table_id);
void buf_free_page(int table_id, pagenum_t pagenum);
buf_struct* buf_read_page(int table_id, pagenum_t pagenum);
int find_location(int table_id, pagenum_t pagenum);
void buf_write_page(const buf_struct* src);
void insert_LRU_List(buf_struct* new_page);

pagenum_t find_leaf (int table_id, int64_t key);
int64_t* find (int table_id, int64_t key);
int cut( int length );

// Insertion.

int get_left_index(buf_struct* parent, pagenum_t left_pagenum);
void insert_into_leaf( buf_struct* leaf, int64_t key, int64_t* values, int ncolumn );
void insert_into_leaf_after_splitting(buf_struct* leaf, int64_t key, int64_t* values, int ncolumn);
void insert_into_page(buf_struct* parent, int left_index, int64_t key, buf_struct* right);
void insert_into_page_after_splitting(buf_struct* old_page, int left_index, int64_t key, buf_struct* right);
void insert_into_parent(buf_struct* left, int64_t key, buf_struct* right);
void insert_into_new_root(buf_struct* left, int64_t key, buf_struct* right);
void start_new_tree(int table_id, int64_t key, int64_t* values);
int insert(int table_id, int64_t key, int64_t* values );

// Deletion.

int get_neighbor_index( pagenum_t pagenum, buf_struct* parent_page);
buf_struct* remove_entry_from_page(buf_struct* n, int64_t key, int ncolumn);
void adjust_root(buf_struct* header, buf_struct* root);
void coalesce_nodes(buf_struct* n, buf_struct* neighbor_page, buf_struct* parent,
                     int neighbor_index, int64_t k_prime, int ncolumn);
void delete_entry(buf_struct* n, int64_t key);
int erase(int table_id, int64_t key);
//close

int close_table(int table_id);
int shutdown_db(void);

//Join


int64_t join(const char* query);
int is_used_table(vector<int> used_tables, int table_id);
vector< vector<int64_t> > get_table(int table_id);
vector<PredicationInfo> parse(string s);
int64_t result_sum(vector<int> used_tables);
void build_hash(int table_ide, int col);
void Run(int left_id, int left_col, int right_id, int right_col);
                                      
//Print_bpt

void enqueue( off_t po );
node * dequeue( void );
void print_tree( int table_id );
void print_hash( void );
void print_pin( void );


//test
void testopen(void);
void testinput(void);

#endif /* __BPT_H__*/
