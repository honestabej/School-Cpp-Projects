#ifndef HEAP_H
#define HEAP_H

struct Element{
    int key;
};

struct Heap{
    public:
        int capacity;
        int size;
        struct Element *H;
        bool initialized;
        bool empty;
};

void MaxHeapify(Element A[], int n, int i);
Heap Initialize(int n);
void BuildHeap(Heap *heap, Element A[], int n);
void Insert(Heap *heap, int flag, int k);
int DeleteMax(Heap *heap, int flag);
void IncreaseKey(Heap heap, int flag, int index, int value);
void printHeap(Heap heap);
void fileIn(Heap *heap, int n);

#endif