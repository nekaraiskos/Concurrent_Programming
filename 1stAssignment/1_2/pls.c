#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

    if(argc < 2) {
        return 0;
    }

    int num = atoi(argv[1]);
    int i;
    for(i = 1; i <= num; i++){
        printf("%d ", i);
    }
    printf("\n");

    return 0;
}