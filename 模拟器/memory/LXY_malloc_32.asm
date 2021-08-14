.model flat,c
.const 
    align 16
KUOSANMASK byte 16 dup(0h)
.code

;extern "C" void lxy_memset(void* source, size_t size);

lxy_memset32 proc
    push ebp
    mov ebp,esp
    push edi

    mov ecx,10h
    mov edx,[ebp+08h]
    mov edi,[ebp+08h]
    mov eax,[ebp+0ch]
    and edx,0fh
    test edx,edx ;判断是否16字节对齐
    jz memset32_is_aligned
    sub ecx,edx
    cmp ecx,[ebp+010h]
    cmovg ecx,[ebp+010h] ;判断不对齐数据大小和长度 谁大谁小
    sub [ebp+10h],ecx   ;减去不对齐的长度
    cld
    rep stosb

memset32_is_aligned:
    movd xmm0,eax
    mov ecx,[ebp+10h]
    pshufb xmm0,xmmword ptr [KUOSANMASK]
@@:
    cmp ecx,0fh
    jl memset32_noaligned_last
    movdqa xmmword ptr [edi],xmm0
    sub ecx,10h
    add edi,10h
    jmp @B
 
memset32_noaligned_last: ;将剩余的非对齐內存进行初始化
    rep stosb

    pop edi
    leave
    ret
lxy_memset32 endp


;extern "C" void lxy_memcpy_aligned(void* lhs, size_t size_l, void* rhs, size_t size_r);
lxy_memcpy_aligned32 proc
    push ebp
    mov ebp,esp

;初始化参数
    mov ecx,[ebp+0ch]
    push edi
    push esi
    cmp ecx,[ebp+014h]
    mov edi,[ebp+08h]
    mov esi,[ebp+010h]
    cmovg ecx,[ebp+014h] ;ecx为目标长度和源长度中较小的一个

;判断长度是否大于40h
@@:
    cmp ecx,40h
    jl lxy_memcpy_aligned_deel10h
    movdqa xmm0,xmmword ptr [esi]
    movdqa xmm1,xmmword ptr [esi+10h]
    movdqa xmm2,xmmword ptr [esi+20h]
    movdqa xmm3,xmmword ptr [esi+30h]
    sub ecx,40h
    add esi,40h
    movdqa xmmword ptr [edi],xmm0
    movdqa xmmword ptr [edi+10h],xmm1
    movdqa xmmword ptr [edi+20h],xmm2
    movdqa xmmword ptr [edi+30h],xmm3
    add edi,40h
    jmp @B

;处理小于40h的 
lxy_memcpy_aligned_deel10h:
    cmp ecx,10h
    jl lxy_memcpy_aligned_deel_litlle10h
    movdqa xmm5,xmmword ptr [esi]
    sub ecx,10h
    add esi,10h
    movdqa xmmword ptr [edi],xmm5
    add edi,10h
    cmp ecx,10h
    jl lxy_memcpy_aligned_deel_litlle10h
    movdqa xmm6,xmmword ptr [esi]
    sub ecx,10h
    add esi,10h
    movdqa xmmword ptr [edi],xmm6
    add edi,10h
    cmp ecx,10h
    jl lxy_memcpy_aligned_deel_litlle10h
    movdqa xmm7,xmmword ptr [esi]
    sub ecx,10h
    add esi,10h
    movdqa xmmword ptr [edi],xmm7
    add edi,10h
;处理最后的
lxy_memcpy_aligned_deel_litlle10h:
    cld 
    rep movsb

    pop esi
    pop edi
    leave 
    ret
lxy_memcpy_aligned32 endp

;extern "C" void lxy_memcpy_noaligned(void* lhs, size_t size_l, void* rhs, size_t size_r);
lxy_memcpy_noaligned32 proc
    push ebp
    mov ebp,esp
;初始化参数
    mov ecx,[ebp+0ch]
    push edi
    push esi
    cmp ecx,[ebp+014h]
    mov edi,[ebp+08h]
    mov eax,[ebp+08h]
    mov esi,[ebp+010h]
    cmovg ecx,[ebp+014h] ;ecx为目标长度和源长度中较小的一个

    mov edx,10h
    and eax,0fh
    sub edx,eax
    cmp ecx,edx
    jg lxy_memcpy_noaligned_deel_sourese_noaligned ;要复制的长度小于对齐的长度
    rep movsb
    jmp lxy_memcpy_noaligned_end
lxy_memcpy_noaligned_deel_sourese_noaligned:
    xchg ecx,edx
    sub edx,ecx
    rep movsb
    mov ecx,edx

lxy_memcpy_noaligned_deel_sourese_aligned:
    cmp ecx,10h
    jl lxy_memcpy_noaligned_deel_sourese_noaligned_last
    movups xmm0,xmmword ptr[esi]
    sub ecx,10h
    add esi,10h
    movdqa xmmword ptr[edi],xmm0
    add edi,10h
    cmp ecx,10h
    jl lxy_memcpy_noaligned_deel_sourese_noaligned_last
    movups xmm1,xmmword ptr[esi]
    sub ecx,10h
    add esi,10h
    movdqa xmmword ptr[edi],xmm1
    add edi,10h
    jmp lxy_memcpy_noaligned_deel_sourese_aligned

lxy_memcpy_noaligned_deel_sourese_noaligned_last:
    rep movsb
lxy_memcpy_noaligned_end:
    pop esi
    pop edi
    leave
    ret

lxy_memcpy_noaligned32 endp
end