#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(){
    //unsigned char code[] ="\x55\x48\x8b\xec\x48\x83\xec\x30\x48\x89\x5d\xd8\xbe\x14\x0\x0\x0\x48\x8b\x3d\x0\x0\x0\x0\xe8\x0\x0\x0\x0\xb9\x55\x0\x0\x0\x88\x8\xc6\x40\x1\x48\xba\x89\x0\x0\x0\x88\x50\x2\xc6\x40\x3\xe5\x88\x50\x4\xc6\x40\x5\x7d\xbb\xfc\x0\x0\x0\x88\x58\x6\x88\x50\x7\xc6\x40\x8\x75\xbe\xf8\x0\x0\x0\x40\x88\x70\x9\xbf\x8b\x0\x0\x0\x40\x88\x78\xa\x88\x48\xb\x88\x58\xc\x40\x88\x78\xd\xc6\x40\xe\x45\x40\x88\x70\xf\xc6\x40\x10\x1\xc6\x40\x11\xd0\xc6\x40\x12\x5d\xc6\x40\x13\xc3\x48\x89\xc1\xb8\x14\x0\x0\x0\x48\x89\x45\xe0\x48\x89\x4d\xe8\x45\x31\xc9\x41\xb8\xff\xff\xff\xff\xb9\x22\x0\x0\x0\xb2\x6\x48\x8b\x75\xe0\x31\xff\xe8\x0\x0\x0\x0\x48\x89\x45\xf0\x48\x8b\x55\xe0\x48\x8b\x75\xe8\x48\x89\xc7\xe8\x0\x0\x0\x0\xbe\x3c\x0\x0\x0\xbf\x9\x0\x0\x0\x48\xff\x55\xf0\x89\x45\xf8\x89\xc7\xe8\x0\x0\x0\x0\x48\x8b\x75\xe0\x48\x8b\x7d\xf0\xe8\x0\x0\x0\x0\x8b\x45\xf8\x48\x8b\x5d\xd8\xc9\xc3\x0";

    FILE* code = fopen("add.o", "rb");
    //FILE* code = fopen("main", "rb");
    struct stat st;
    stat("add.o", &st);
    //stat("main", &st);
    int size = st.st_size;
    unsigned char buffer[size];

    int* adder = mmap(NULL, size, PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (adder == MAP_FAILED){
        perror("mmap");
        return 1;
    }

    fread(&buffer, sizeof(unsigned char), size, code);
    fclose(code);

    //printf("size of code: %d\n", sizeof(buffer));
    //memcpy(adder, code, sizeof(code));
    memcpy(adder, buffer, sizeof(buffer));
    printf("after memcpy %p\n", adder);
    int result = ((int(*)())adder + 0xb0)();
    //int result = ((int(*)())adder + 0x4405a0)();
    printf("Result: %d\n", result);

    munmap(adder, sizeof(code));
    //printf("munmap: %d\n", mun);

    return 0;
}
