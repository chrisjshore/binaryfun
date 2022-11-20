#include <stdio.h>
#include <string.h>

int main(){
    unsigned char code[] = "\x6c\x69\x62\x63\x2e\x73\x6f\x2e\x36\x00\x6c\x69\x62\x6d\x2e\x73\x6f\x2e\x36\x00";

    //printf("length: %u\n",strlen(code));
    int length = strlen(code);
    char str[length];

    strncpy(str, code+10, length);
    str[length+1] = 0x0;

    //printf("length: %u\n",strlen(str));
    printf("%s\n", str);
    return 0;
}
