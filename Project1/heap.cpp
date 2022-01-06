#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include "heap.h"
using namespace std;


Heap Initialize(int n) {
    struct Heap heap;
    // Can't initilaize the heap as negative numbers
    if (n < 0){
        cout << "Error: invalid heap capacity" << endl;
        return heap;
    }
    heap.capacity = n;
    heap.size = 0;
    heap.H = (Element*)malloc(sizeof(Element)*heap.capacity); // Allocates memory for the heap
    heap.initialized = true; // Checks that array has been initialized
    return heap;
}

// Sorts the Element array A into a max heap
void MaxHeapify(Element A[], int n, int i){
    int largest = i; // Initialize largest as root
    int l = 2 * i + 1; // Get left child node 
    int r = 2 * i + 2; // Get right child node

    // Check if left child node is larger than parent node
    if (l < n && A[l].key > A[largest].key) {
        largest = l;
    }
    
    // Check if right child node is larger than parent node
    if (r < n && A[r].key > A[largest].key) {
        largest = r;
    }

    // If the parent node was not the largest element, swap it with the largest child node
    if (largest != i) {
        int temp = A[i].key;
        A[i].key = A[largest].key;
        A[largest].key = temp;
        // Recursively call MaxHeapify
        MaxHeapify(A, n, largest);
    }
}

// Prints the heap and its info
void printHeap(Heap heap){
    cout << "Capacity = " << heap.capacity << endl;
    cout << "Size = " << heap.size << endl;
    for (int i = 1; i <= heap.size; i++){
        cout << heap.H[i].key << endl;
    }
}

// Copies the elements of array A into the heap H after sorting it using MaxHeapify
void BuildHeap(Heap *heap, Element A[], int n) {
    // MaxHeapify the array A
    for (int i = (n / 2) - 1; i >= 0; i--) {
        MaxHeapify(A, n, i);
    }
    // Build the heap from the sorted array A, with base index 1
    for (int i = 1; i <= n; i++) {
        heap->H[i].key = A[i-1].key;
    }
}

// Inserts a new number into the heap and then sorts it
void Insert(Heap *heap, int flag, int k) {
    if (flag == 1){
        heap->size++;   // Make room for k
        // Increase the capacity if the heap is full
        if (heap->size >= heap->capacity){
            heap->capacity = pow(2,ceil(log2(heap->size)));
        }
        heap->H[heap->size].key = k; // Insert k at the end of the heap

        // Make the heap a MaxHeap again by copying heap into an array A of elements and using MaxHeapify on that array
        Element A[heap->size];
        for (int i = 0; i < heap->size; i++){
            A[i].key = heap->H[i+1].key;
        }
        BuildHeap(heap, A, heap->size);

    } else if (flag == 2){
        // Print out the heap before k is inserted
        cout << "Before insert operation:" << endl;
        printHeap(*heap);

        heap->size++;   // Make room for k
        // Increase the capacity if the heap is full
        if (heap->size >= heap->capacity){
            heap->capacity = pow(2,ceil(log2(heap->size)));
        }
        heap->H[heap->size].key = k; // Insert k at the end of the heap

        // Make the heap a MaxHeap again by copying heap into an array A of elements and using MaxHeapify on that array
        Element A[heap->size];
        for (int i = 0; i < heap->size; i++){
            A[i].key = heap->H[i+1].key;
        }
        BuildHeap(heap, A, heap->size);

        // Print out the heap after k has been inserted
        cout << "After insert operation:" << endl;
        printHeap(*heap);
    } else {
        // Print if the flag value is not 1 or 2
        cout << "Error: invalid flag value" << endl;
    }
}

// Deletes the max value in the heap and then adjusts it to maintain MaxHeap property
int DeleteMax(Heap *heap, int flag){
    // Dont delete element if heap is empty
    if (heap->size < 1){
        return 0;
    }
    if (flag == 1){
        // If heap only has one element in it just return that element and delete it
        if (heap->size == 1){
            heap->size--;
            return heap->H[1].key;
        } else{
            int max = heap->H[1].key; // Max element should be at head of heap
            // Move elements over to erase the first (max) element
            for (int i = 1; i < heap->size; i++){
                heap->H[i].key = heap->H[i+1].key;
            } 
            heap->size--; // Adjust heap size to account for deleted element

            // Make the heap a MaxHeap again by copying heap into an array A of elements and using MaxHeapify on that array
            Element A[heap->size];
            for (int i = 0; i < heap->size; i++){
                A[i].key = heap->H[i+1].key;
            }
            BuildHeap(heap, A, heap->size);
            return max;
        }
    } else if (flag == 2){
        // Print the heap before deleting the element
        cout << "Before delete max operation:" << endl;
        printHeap(*heap);

        // If heap has only one element then return that element and delete it
        if (heap->size == 1){
            heap->size--;
            return heap->H[1].key;
        } else{
            int max = heap->H[1].key; // Max element should be at head of heap
            // move elements over to erase the first (max) element
            for (int i = 1; i < heap->size; i++){
                heap->H[i].key = heap->H[i+1].key;
            }
            heap->size--; // Adjust heap size to account for deleted element

            // Make the heap a MaxHeap again by copying heap into an array A of elements and using MaxHeapify on that array
            Element A[heap->size];
            for (int i = 0; i < heap->size; i++){
                A[i].key = heap->H[i+1].key;
            }
            BuildHeap(heap, A, heap->size);

            cout << "After delete max operation:" << endl;
            printHeap(*heap);
            return max;
        }
    } else {
        // When flag is incorrect
        return 0;
    }
}

// Increases the value of one of the elements in the heap 
void IncreaseKey(Heap heap, int flag, int index, int value){
    // Testing for invalid index
    if (index == 0 || index > heap.size){
        cout << "Error: invalid index" << endl;
        return;
    }
    // Testing for invalid value
    if (value < heap.H[index].key){
        cout << "Error: new key is smaller than current key" << endl;
        return;
    }
    // find the key and record its index
    for (int i = 1; i <= heap.size; i++) {
        if (heap.H[i].key == value) {
            index = i;
            break;
        }
    }
    if (flag == 1){
        heap.H[index].key = value; // change the key at the index to value
    } else if (flag == 2){
        cout << "Before increase key operation:" << endl;
        printHeap(heap);
        heap.H[index].key = value; // change the key at the index to value
        // Make the heap a MaxHeap again by copying heap into an array A of elements and using MaxHeapify on that array
        Element A[heap.size];
        for (int i = 0; i < heap.size; i++){
            A[i].key = heap.H[i+1].key;
        }
        BuildHeap(&heap, A, heap.size);
        cout << "After increase key operation:" << endl;
        printHeap(heap);
    } else {
        // When flag is wrong value
        return;
    }

}

// Gets the input of the heap from a file called "HEAPinput.txt"
void fileIn(Heap *heap, int n) {
    string filein = "HEAPinput.txt";
    ifstream in;
    in.open(filein.c_str());
    if (!in.is_open()){
        cout << "Error: could not open file" << endl;
        exit(EXIT_FAILURE);
    }
    // Get input array size
    int sizeA;
    in >> sizeA;
    if (heap->capacity < sizeA){
        cout << "Error: array size exceeds heap capacity" << endl;
        return;
    }
    // If heap is not empty, remove the elements to make room for new inputs 
    if (heap->size > 0){
        int temp = heap->size;
        for (int i = 0; i < temp; i++){
            heap->size--;
        }
    }
    // Increase the heap size to allow elements to be inserted
    for (int i = 1; i <= n; i++) {
        heap->size++;
    }
    Element A[heap->size];
    // Insert the numbers from the file into the Element array A
    for (int i = 0; i < sizeA; i++){
        in >> A[i].key;
    }
    BuildHeap(heap, A, n);
    in.close();
}





