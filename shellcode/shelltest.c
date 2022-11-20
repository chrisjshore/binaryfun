#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

unsigned char code[] = "\x31\xc0\x50\x68\x2f\x63\x61\x74\x68\x2f\x62\x69\x6e\x89\xe3\x50\x68\x2e\x74\x78\x74\x68\x66\x6c\x61\x67\x89\xe1\x50\x51\x53\x89\xe1\x31\xc0\x83\xc0\x0b\xcd\x80";

int main(){
    //(*(void(*)()) code)();

    if (fork() == 0) {
        void* region = mmap(NULL, sizeof(code), PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

        if (region == MAP_FAILED){
            perror("mmap");
            return 1;
        }

        memcpy(region, code, sizeof(code));
        ((int(*)())region)();

        munmap(region, sizeof(code));
    }
    else {
        printf("running shellcode\n");
    }
    return 0;
}
