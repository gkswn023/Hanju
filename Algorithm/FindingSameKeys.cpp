
//2014005032_¿Ã«—¡÷

#include <stdio.h>
#include <stdlib.h>

typedef struct HashTable {
	int key;
	struct HashTable* next;
}HashTable;

int main() {
	int num1, num2, hash_num, hashing, i;
	int list;
	HashTable* hash_table, *tmp_hash;
	int same_key = 0;

	scanf("%d %d", &num1, &num2);

	hash_table = (HashTable*)malloc(sizeof(HashTable) * num1);
	for (i = 0; i < num1; i++) {
		hash_table[i].next = NULL;
	}

	for (i = 0; i < num1; i++) {
		scanf("%d", &hash_num);
		hashing = hash_num % num1;
		tmp_hash = (HashTable*)malloc(sizeof(HashTable));
		tmp_hash->key = hash_num;
		tmp_hash->next = hash_table[hashing].next;
		hash_table[hashing].next = tmp_hash;
	}
	for (i = 0; i < num2; i++) {
		scanf("%d", &list);
		hashing = list % num1;
		tmp_hash = hash_table[hashing].next;
		while (tmp_hash != NULL) {
			if (tmp_hash->key == list) same_key++;
			tmp_hash = tmp_hash->next;
		}
	}

	printf("%d", same_key);

	return 0;
}