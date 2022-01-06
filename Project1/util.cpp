#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include "util.h"
//=============================================================================
int nextCommand(int *f, int *i, int *v){
    char c;
    while(1){
        scanf("%c", &c);

        // n = capacity of heap
        // f = flag
        // k = key
        // i = index
        // v = value

        // Space, indent, newline input
        if (c == ' ' || c == '\t' || c == '\n'){ 
            continue;
        } else if (c == 'S' || c == 's' || c == 'R' || c == 'r' || c == 'W' || c == 'w'){
            // S, R, W input
            if (c == 's'){
                c = 'S';
            }
            if (c == 'r'){
                c = 'R';
            }
            if (c == 'w'){
                c = 'W';
            }
            break;
        } else if (c == 'C' || c == 'c'){
            // C n input
            if (c == 'c'){
                c = 'C';
            }
            scanf("%d", f); // n value
            break;
        } else if (c == 'I' || c == 'i'){
            // I f k input
            if (c == 'i'){
                c = 'I';
            }
            scanf("%d", f);
            scanf("%d", i); // k value
            break;
        }else if (c == 'D' || c == 'd'){
            //D f input
            if (c == 'd'){
                c = 'D';
            }
            scanf("%d", f);
            break;
        } else if (c == 'K' || c == 'k') {
            // K f i v input
            if (c == 'k'){
                c = 'K';
            }
            scanf("%d", f); 
            scanf("%d", i); 
            scanf("%d", v);
            break;
        }else {
            printf("Invalid Command\n");
            break;
        }
    }
    return c;
}