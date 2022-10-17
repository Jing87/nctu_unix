    mov dx, 9
    oloop:
        mov cx, 9
        mov ebx, 0x600000
        iloop:
            mov eax, [ebx]                 ; Because compare can't have both memory
            cmp eax, [ebx+4]
            jl common                      ; if al is less than [si+1] Skip the below two lines for swapping.
            xchg eax, [ebx+4]
            mov [ebx], eax                    ; Coz we can't use two memory locations in xchg directly.

            common:
                add ebx, 4
                loop iloop

        dec dx
        jnz oloop