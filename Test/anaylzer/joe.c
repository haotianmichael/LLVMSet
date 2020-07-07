#include <stdio.h>

void my_function(int unknowvalue) {
    int schroedinger_integer;
    if(unknowvalue) 
        schroedinger_integer = 5;


    printf("hi");
    if(!unknowvalue) {
        printf("%d", schroedinger_integer); 
    }
}
