#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <unistd.h>

void * myalloc(size_t size){
    
    void * ptr = sbrk(size);
    return ptr;
}


int main(int argc, char **argv){

   
    return 0;
}