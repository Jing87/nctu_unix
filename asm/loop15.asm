
mov rbx, 0x600000
mov rcx, 0x600010
l1:
mov al, [rbx]
cmp al, 'a'
jl judge
mov [rcx], al
inc rbx
inc rcx
cmp rbx, 0x600010
jl l1

judge:
cmp al, 0x00
jnz tolower
mov [rcx], al
inc rbx
inc rcx
cmp rbx, 0x600010
jl l1

tolower:
add al, 0x20
mov [rcx], al
inc rbx
inc rcx
cmp rbx, 0x600010
jl l1