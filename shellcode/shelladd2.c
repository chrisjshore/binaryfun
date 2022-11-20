#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

int main(int argc, char** argv){

    if(argc < 2){
        printf("no file provided\n");
        return 1;
    }

    //FILE* code = fopen("adder.bin", "rb");
    char* binary = argv[1];
    FILE* code = fopen(binary, "rb");

    if(code == NULL){
        printf("unable to open file\n");
        return 1;
    }

    struct stat st;
    //stat("adder.bin", &st);
    stat(binary, &st);
    int size = st.st_size;
    unsigned char buffer[size];

    int* adder = mmap(NULL, size, PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (adder == MAP_FAILED){
        perror("mmap");
        return 1;
    }

    fread(&buffer, sizeof(unsigned char), size, code);
    fclose(code);

    memcpy(adder, buffer, size);

    int result = ((int(*)())adder)(60,9);
    printf("Result: %d\n", result);

    munmap(adder, sizeof(code));

    return 0;
}
