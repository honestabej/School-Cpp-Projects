#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include "main.h"
using namespace std;

int main(){
    // variables for the parser...
    char c;
    int n, k, f, i, v, deleted;
    struct Heap heap;
    bool flagCheck = false;

    while(1) {
        c = nextCommand(&f, &i, &v);
        switch (c) {

            // S: stops the program
            case 'S': 
                printf("COMMAND: %c\n", c);
                exit(0);

            // C n: creates empty heap with capacity n
            case 'C':
                n = f;
                printf("COMMAND: %c %d\n", c, n);
                heap = Initialize(n);
                break;

            // R: Reads in array A[] from HEAPinput.txt and builds maxheap based on A
            case 'R':
                printf("COMMAND: %c\n", c);
                if (heap.initialized != true){
                    cout << "Error: heap not initialized" << endl;
                    break;
                } else {
                    fileIn(&heap, n);
                    break;
                }

            // W: Writes current heap information to the screen
            case 'W':
                printf("COMMAND: %c\n", c);
                if (heap.initialized != true){
                    cout << "Error: heap not initialized" << endl;
                    break;
                } else {
                    printHeap(heap);
                    break;
                }

            // I f k: Inserts element k into the heap and sets the flag as f
            case 'I':
                flagCheck = false;
                k = i;
                printf("COMMAND: %c %d %d\n", c, f, k);
                if (f == 1 || f == 2) {
                    flagCheck = true;
                }
                if (heap.initialized != true){
                    cout << "Error: heap not initialized" << endl;
                    break;
                } else if (flagCheck != true) {
                    cout << "Error: invalid flag value" << endl;
                    break;
                } else {
                    Insert(&heap, f, k);
                    break;
                }

            // D f: Deletes the max element from the heap, prints it on the screen, and sets flag to f
            case 'D':
                flagCheck = false;
                printf("COMMAND: %c %d\n", c, f);
                if (f == 1 || f == 2) {
                    flagCheck = true;
                }
                if (heap.initialized != true){
                    cout << "Error: heap not initialized" << endl;
                    break;
                } else if(heap.size < 1){
                    cout << "Error: heap empty" << endl;
                } else if (flagCheck != true){
                    cout << "Error: invalid flag value" << endl;
                    break;
                } else {
                    deleted = DeleteMax(&heap, f);
                    printf("Deleted max = %d\n", deleted);
                    break;
                }

            // K f i v: Increases key of element i to v and sets falg to f
            case 'K': 
                flagCheck = false;
                printf("COMMAND: %c %d %d %d\n", c, f, i, v);
                if (f == 1 || f == 2) {
                    flagCheck = true;
                }
                if (heap.initialized != true){
                    cout << "Error: heap not initialized" << endl;
                    break;
                } else if (flagCheck != true) {
                    cout << "Error: invalid flag value" << endl;
                    break;
                } else {
                    IncreaseKey(heap, f, i, v);
                    break;
                }
                
            default: 
                break;
        }
    }
    exit(0);
    cout << "Stopped" << endl;
}

