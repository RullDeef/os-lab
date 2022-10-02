        keybh proc
F000:E986    push ax
F000:E989    in al, [60]
F000:E98A    mov ah, 4Fh
F000:E98C    stc
F000:E98D    int 15h
F000:E98F    jnc E995h
F000:E991    db FEh
F000:E992    cmp [bx+si], dl
F000:E994    add dl, bh
F000:E996    mov al, 20h
F000:E998    out [20h], al
F000:E99A    pop ax
F000:E99B    iret
        keybh endp
