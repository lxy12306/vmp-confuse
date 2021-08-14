
        .model flat,c

        .const
        align 16
mask_16_internet_to_cpu db 1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14
        .code
        extern lxy_malloc:proc

;extern "C" unsigned char* m_lxy_memcmp32_noaligned(void* rhs, void* lhs, int size);

m_lxy_memcmp32_noaligned proc
    push ebp
    mov ebp,esp
    sub esp,10h
    push esi
    push edi

    mov eax,[ebp+10h]
    mov edi,[ebp+0ch]
    mov esi,[ebp+8h]

    push eax
    call lxy_malloc
    add esp,4

    mov [ebp-8h],eax
    mov ecx,[ebp+10h]
    add eax,[ebp+10h]
    cld
@@:
    repe cmpsb
    test ecx,ecx
    jz @exit
    lea edx,[ecx+1]
    neg edx
    mov byte ptr [eax+edx],0

    jmp @B

@exit:
    mov eax,[ebp-8h]
    pop esi
    pop edi
    leave
    ret
m_lxy_memcmp32_noaligned endp


;extern "C" unsigned char* m_lxy_memcmp32_aligned(void* rhs, void* lhs, int size);

m_lxy_memcmp32_aligned proc
    push ebp
    mov ebp,esp
    sub esp,10h
    push esi
    push edi

    mov eax,[ebp+10h]
    test eax,3fh
    jz m_lxy_memcmp32_aligned_duiqi1
    and eax,-40h
    add eax,40h
m_lxy_memcmp32_aligned_duiqi1:
    mov edi,[ebp+0ch]
    mov esi,[ebp+8h]
    mov [ebp+10h],eax

    push eax
    call lxy_malloc
    add esp,4

    mov [ebp-8h],eax

    mov ecx,[ebp+10h]
    prefetchT0 dword ptr [eax]
    prefetchT0 dword ptr [edi]
    prefetchT0 dword ptr [esi]

@@:
    movdqa xmm0,xmmword ptr [edi]
    movdqa xmm1,xmmword ptr [esi]

    movdqa xmm2,xmmword ptr [edi+10h]
    movdqa xmm3,xmmword ptr [esi+10h]

    movdqa xmm4,xmmword ptr [edi+20h]
    movdqa xmm5,xmmword ptr [esi+20h]

    movdqa xmm6,xmmword ptr [edi+30h]
    movdqa xmm7,xmmword ptr [esi+30h]

    pcmpeqb xmm0,xmm1
    pcmpeqb xmm2,xmm3
    pcmpeqb xmm4,xmm5
    pcmpeqb xmm6,xmm7
    add esi,40h
    add edi,40h

    movdqa xmmword ptr [eax], xmm0
    movdqa xmmword ptr [eax+10h], xmm2
    movdqa xmmword ptr [eax+20h],xmm4
    movdqa xmmword ptr [eax+30h],xmm6

    add eax,40h
    sub ecx,40h
    jnz @B

    mov eax,[ebp-08h]

    pop esi
    pop edi
    leave
    ret
m_lxy_memcmp32_aligned endp

;extern "C" static uint16_t m_lxy_checksum32(void* source, size_t size);
m_lxy_internet_checksum32 proc
    push ebp
    mov ebp,esp
    sub esp,20h
    mov [ebp-4h],esi
    mov [ebp-8h],edi
    mov [ebp-0ch],ebx
    
    mov eax,[ebp+8h]
    mov esi,[ebp+8h] ; 目标缓冲区
    mov ecx,[ebp+0ch]; 长度
    
    xor edi,edi
    test eax,0fh
    je @_aligned_16;表示内存地址16字节对齐
;计算开始的未对齐的内存区域的数据
    xor edx,edx
    add eax,10h
    neg edx
    shl edx,4   ;edx=...f0h
    and eax,edx

    sub eax,esi     ;eax为非16字节对齐的字节数

    cmp eax,ecx     ;与给的长度相比谁大
    cmova eax,ecx   ;防止越界计算
    sub ecx,eax     ;减去非对齐数量
    
    xor ebx,ebx
    shr eax,1
@@:
    mov bx,[esi]
    xchg bl,bh
    add esi,2
    add edi,ebx     ;计算大端序单字补码和
    dec eax
    jnz @B

@_aligned_16:
    pxor  xmm0,xmm0
    movdqa xmm7,xmmword ptr [mask_16_internet_to_cpu]
@@:
    cmp ecx,10h
    jl @_xmm_to_16
    movdqa xmm1,xmmword ptr [esi]
    sub ecx,10h
    pshufb xmm1,xmm7
    add esi,10h
    movhlps xmm2,xmm1
    pmovzxwd xmm1,xmm1
    pmovzxwd xmm2,xmm2
    paddd xmm0,xmm1
    paddd xmm0,xmm2
    jmp @B

;计算最终结果
@_xmm_to_16:
    phaddd xmm0,xmm0
    xor ebx,ebx
    phaddd xmm0,xmm0
    movd eax,xmm0

@_enddata:
    test ecx,ecx
    jz @end
    mov bx,[esi]
    add esi,2
    sub ecx,2
    xchg bl,bh
    add edi,ebx    
    jmp @_enddata

@end:
    add eax,edi
    mov esi,[ebp-4h]
    mov edi,[ebp-8h]
    mov ebx,[ebp-0ch]
    leave 
    ret
m_lxy_internet_checksum32 endp
	end


