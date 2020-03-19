//2014005032_¿Ã«—¡÷
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int global_sum = 0;

typedef struct node {
	char* name;
	int key;
	node* left;
	node* right;
}node;

typedef struct Heap {
	int size;
	node** list;
}Heap;

void Min_Heapify(Heap* heap, int num);
void Insert(Heap* heap, node* new_node);
node* Extract_Min(Heap* heap);
node* Huffman(Heap* heap, int num);
node* Make_node(char* name, int key, node* left, node* right);
void print_order(node* huffman, int length);

int main() {
	int num, sum_value;
	int k = 1, bit = 0;
	int j = 1;
	char** name;
	int* key;
	node* new_node;
	node* huffman_node;
	Heap* heap1;

	scanf("%d", &num);

	name = (char**)malloc(sizeof(char*)*num);
	for (int i = 0; i < num; i++) {
		name[i] = (char*)malloc(sizeof(char) * 4);
	}
	key = (int*)malloc(sizeof(int)*num);
	heap1 = (Heap*)malloc(sizeof(Heap));
	heap1->size = 0;
	heap1->list = (node**)malloc(sizeof(node*)*(num + 1));

	for (int i = 0; i < num; i++) {
		scanf("%s %d", name[i], &key[i]);
	}

	scanf("%d", &sum_value);

	for (int i = 0; i < num; i++) {
		new_node = Make_node(name[i], key[i], NULL, NULL);
		Insert(heap1, new_node);
	}

	huffman_node = Huffman(heap1, heap1->size);

	print_order(huffman_node, 0);
	while (1) {
		if (num == 1) {
			bit = 1;
			break;
		}
		if (2 * k <= num && num < 4 * k) {
			bit = j + 1;
			break;
		}
		k = k * 2;
		j++;
	}
	printf("%d\n", bit*sum_value);
	printf("%d", global_sum);
	return 0;
}
void Min_Heapify(Heap* heap, int num) {
	int child;
	node* tmp;

	for (int i = num; i * 2 <= heap->size; i = child) {
		child = 2 * i;
		if (child != heap->size && heap->list[child]->key > heap->list[child + 1]->key)
			child++;
		if (heap->list[i]->key > heap->list[child]->key) {
			tmp = heap->list[i];
			heap->list[i] = heap->list[child];
			heap->list[child] = tmp;
		}
	}
}
void Insert(Heap* heap, node* new_node) {
	node* tmp;
	int parent;
	heap->size++;
	heap->list[heap->size] = new_node;
	for (int i = heap->size; i > 1; i = parent) {
		parent = i / 2;
		if (heap->list[i]->key < heap->list[parent]->key) {
			tmp = heap->list[i];
			heap->list[i] = heap->list[parent];
			heap->list[parent] = tmp;
		}
		else break;
	}
}
node* Extract_Min(Heap* heap) {
	node* delete_node;
	delete_node = (node*)malloc(sizeof(node));
	delete_node = heap->list[1];
	heap->list[1] = heap->list[heap->size];
	heap->size--;
	Min_Heapify(heap, 1);

	return delete_node;
}
node* Huffman(Heap* heap, int num) {
	int i;
	node* node1, *node2;
	node* tmp;
	node* complete_node;
	for (i = 0; i < num - 1; i++) {
		node1 = Extract_Min(heap);
		node2 = Extract_Min(heap);
		tmp = Make_node(NULL, node1->key + node2->key, node1, node2);
		Insert(heap, tmp);
	}
	complete_node = Extract_Min(heap);

	return complete_node;

}
node* Make_node(char* name, int key, node* left, node* right) {
	node* new_node = (node*)malloc(sizeof(node));
	new_node->name = name;
	new_node->key = key;
	new_node->left = left;
	new_node->right = right;
	return new_node;
}
void print_order(node* huffman, int length) {
	if (huffman->left == NULL && huffman->right == NULL) {
		global_sum += length * huffman->key;
	}
	else {
		length++;
		print_order(huffman->left, length);
		print_order(huffman->right, length);
	}
}