#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

int main(){
    unsigned char code[] = "\x55\x48\x89\xe5\x89\x7d\xfc\x89\x75\xf8\x8b\x55\xfc\x8b\x45\xf8\x1\xd0\x5d\xc3";
    int* adder = mmap(NULL, sizeof(code), PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (adder == MAP_FAILED){
        perror("mmap");
        return 1;
    }

    //printf("size of code: %d\n", sizeof(code));
    memcpy(adder, code, sizeof(code));

    int result = ((int(*)())adder)(60,9);
    printf("Result: %d\n", result);

    munmap(adder, sizeof(code));
    //printf("munmap: %d\n", mun);

    return 0;
}
