//2014005032_¿Ã«—¡÷
#include<stdio.h>
#include<stdlib.h>

typedef struct HeapStruct Heap;

struct HeapStruct {
	int size;
	int length;
	int* list;
};

void Build_Heap(Heap* heap);
void Max_Heapify(Heap* heap, int num);

int main() {
	int num1, num2;
	int i, tmp;
	Heap* heap;

	heap = (Heap*)malloc(sizeof(HeapStruct));

	scanf("%d %d", &num1, &num2);

	heap->list = (int*)malloc(sizeof(int)*(num1 + 1));
	heap->length = num1;

	for (i = 1; i <= num1; i++) {
		scanf("%d", &heap->list[i]);
	}

	Build_Heap(heap);
	for (i = heap->length; i > heap->length - num2; i--) {
		printf("%d ", heap->list[1]);
		tmp = heap->list[i];
		heap->list[i] = heap->list[1];
		heap->list[1] = tmp;
		heap->size--;
		Max_Heapify(heap, 1);
	}
	printf("\n");
	for (i = 1; i <= heap->size; i++) {
		printf("%d ", heap->list[i]);
	}
	return 0;
}

void Build_Heap(Heap* heap) {
	heap->size = heap->length;
	for (int i = heap->length / 2; i > 0; i--) {
		Max_Heapify(heap, i);
	}
}
void Max_Heapify(Heap* heap, int num) {
	int child, tmp;

	for (int i = num; i * 2 <= heap->size; i = child) {
		child = 2 * i;
		if (child != heap->size && heap->list[child] < heap->list[child + 1])
			child++;
		if (heap->list[i] < heap->list[child]) {
			tmp = heap->list[i];
			heap->list[i] = heap->list[child];
			heap->list[child] = tmp;
		}
	}
}