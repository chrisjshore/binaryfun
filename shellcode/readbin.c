#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char** argv){

    if (argc < 2){
        printf("no file provided\n");
        return 1;
    }

    struct stat size;
    stat(argv[1], &size);

    FILE *handle;
    unsigned char buffer[size.st_size];

    handle = fopen(argv[1], "rb");
    fread(buffer, sizeof(buffer), 1, handle);

    for(int i = 0; i < sizeof(buffer); i++)
        printf("\\x%x", buffer[i]);

    printf("\n");
    return 0;
}
