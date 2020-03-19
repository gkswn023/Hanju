//2014005032_¿Ã«—¡÷
#include<stdio.h>
#include<stdlib.h>

#define MINUSNUM -100;
typedef struct Priority_Queue Heap;

struct Priority_Queue {
	int size;
	int* list;
};

void Max_Heapify(Heap* heap, int num);
void Insert(Heap* heap, int num);
int Extract_Max(Heap* heap);
void Modify_Element(Heap* heap, int x, int y);

int main() {
	int key = 777;
	int num1, num2;
	int size = 0;
	int* list;
	Heap* heap;

	heap = (Heap*)malloc(sizeof(Priority_Queue));
	heap->list = (int*)malloc(sizeof(int) * 100000);
	heap->size = 0;
	list = (int*)malloc(sizeof(int) * 100000);

	while (key != 0) {
		scanf("%d", &key);
		switch (key) {
		case 1:
			scanf("%d", &num1);
			Insert(heap, num1);
			break;
		case 2:
			list[size] = Extract_Max(heap);
			size++;
			break;
		case 3:
			scanf("%d %d", &num1, &num2);
			Modify_Element(heap, num1, num2);
			break;
		case 0:
			for (int i = 0; i < size; i++) {
				printf("%d ", list[i]);
			}
			printf("\n");
			for (int i = 1; i <= heap->size; i++) {
				printf("%d ", heap->list[i]);
			}
			break;
		}
	}
	return 0;
}

void Max_Heapify(Heap* heap, int num) {
	int child, tmp;

	for (int i = num; i * 2 <= heap->size; i = child) {
		child = 2 * i;
		if (child != heap->size && heap->list[child] <= heap->list[child + 1])
			child++;
		if (heap->list[i] < heap->list[child]) {
			tmp = heap->list[i];
			heap->list[i] = heap->list[child];
			heap->list[child] = tmp;
		}
	}
}

void Insert(Heap* heap, int num) {
	heap->size += 1;
	heap->list[heap->size] = MINUSNUM;
	Modify_Element(heap, heap->size, num);
}
int Extract_Max(Heap* heap) {
	int max;
	max = heap->list[1];
	heap->list[1] = heap->list[heap->size];
	heap->size -= 1;
	Max_Heapify(heap, 1);

	return max;
}
void Modify_Element(Heap* heap, int x, int y) {
	int parent, tmp;
	heap->list[x] = y;
	if (x == 1) Max_Heapify(heap, 1);
	else if (heap->list[x] > heap->list[x / 2]) {
		for (int i = x; i > 1; i = parent) {
			parent = i / 2;
			if (heap->list[i] > heap->list[parent]) {
				tmp = heap->list[i];
				heap->list[i] = heap->list[parent];
				heap->list[parent] = tmp;
			}
			else break;
		}
	}
	else Max_Heapify(heap, x);
}