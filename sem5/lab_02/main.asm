.386p

; ������� ���ਯ��
descr struc
    limit    dw 0
    base_l   dw 0
    base_m   db 0
    attr_1   db 0
    attr_2   db 0
    base_h   db 0
descr ends

; ������� ���ਯ�� �᪫�祭��
idescr struc
    offs_l  dw 0
    sel     dw 0
    cntr    db 0
    attr    db 0
    offs_h  dw 0
idescr ends

stack segment para stack 'STACK'
    stack_begin db 200h dup(?)
    stack_size = $ - stack_begin
stack ends

; screen segment at 0B800h
;     screen_size = 4000
; screen ends

data segment para public use16 'DATA'
    data_begin label byte

    gdt_table_begin label byte
        gdt_null        descr <0,0,0,0,0,0>
        gdt_data        descr <data_size - 1,0,0,92h,0,0>
        gdt_data4gb     descr <0FFFFh,0,0,92h,0CFh,0>
        gdt_stack       descr <stack_size - 1,0,0,92h,40h,0>
        gdt_code16      descr <code16_size - 1,0,0,98h,0,0>
        gdt_code32      descr <code32_size - 1,0,0,98h,40h,0>
        gdt_screen      descr <3999,8000h,0Bh,92h,0,0>
    gdt_size = $ - gdt_table_begin

    ; ᥫ����� ᥣ���⮢
    sel_data    = 8
    sel_data4gb = 16
    sel_stack   = 24
    sel_code16  = 32
    sel_code32  = 40
    sel_screen  = 48

    idt_table_begin label byte
        idescr_0_12     idescr 13 dup (<0,sel_code32,0,8Fh,0>)
        idescr_13       idescr         <0,sel_code32,0,8Fh,0>
        idescr_14_31    idescr 18 dup (<0,sel_code32,0,8Fh,0>)
        idescr_32       idescr         <0,sel_code32,0,8Eh,0>
        idescr_33       idescr         <0,sel_code32,0,8Eh,0>
    idt_size = $ - idt_table_begin

    ; ࠧ��� + ���� ⠡���� ��������� ���ਯ�஢
    gdt_pdescr label fword
    gdt_pdescr_size     dw gdt_size - 1
    gdt_pdescr_addr     dd ?
    
    ; ࠧ��� + ���� ⠡���� ���ਯ�஢ �᪫�祭��
    idt_pdescr label fword
    idt_pdescr_size     dw idt_size - 1
    idt_pdescr_addr     dd ?

    ; ࠧ��� � ���� ⠡���� ����஢ ���뢠��� ॠ�쭮�� ०���
    ivt_pdescr label fword
    ivt_pdescr_size     dw 3FFh
    ivt_pdescr_addr     dd 0

    ; ��᪨ ����஫��஢ ���뢠���
    mask_pic0   db ?
    mask_pic1   db ?

    ; ⥪�⮢� ᮮ�饭�� ��� �뢮�� �� �࠭
    msg_real    db 27, '[1;31m� ॠ�쭮� ०���!', 27, '[0m$'
    msg_prot    db '� ���饭��� ०���!', 0
    msg_mem     db '����㯭� �����: ', 0
    msg_byte    db ' ����.', 0
    msg_timer   db '����稪 ⨪��: ', 0

    ; ��������� � ��ਡ��� ����� ��� �뢮�� ⥪�� � ���. ०���
    cursor_pos  dw 80 * 2
    cursor_attr db 0Fh

    ; ����稪 ⨪��
    tick_num    dw 0
    secs_num    dw 0

    ; 䫠� ������ escape
    esc_pressed db 0
    shift_flag  db 0

    ; scan-����
    asciimap    db  0,  0, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 61,  0,  0
                db 81, 87, 69, 82, 84, 89, 85, 73, 79, 80, 91, 93,  0,  0, 65, 83
                db 68, 70, 71, 72, 74, 75, 76, 59, 39, 96,  0, 92, 90, 88, 67, 86
                db 66, 78, 77, 44, 46, 47,  0,  0,  0, 32

    data_size = $ - data_begin
data ends

code16 segment para public use16 'CODE'
        assume cs:code16, ds:data, ss:stack
    code16_begin label byte

    start:
        mov ax, data
        mov ds, ax

        mov ax, stack
        mov ss, ax

        mov esp, stack_size

        ; ���⪠ �࠭�
        ; mov ah, 00h
        ; mov al, 03h
        ; int 10h

        ; =======================
        ;  ���樠������ ���ᮢ ᥣ���⮢ � GDT
        ; =======================

        init_gdt_descr macro segm, desc
            xor eax, eax
            mov ax, segm
            shl eax, 4
            mov desc.base_l, ax
            shr eax, 16
            mov desc.base_m, al
            mov desc.base_h, ah            
        endm

        init_gdt_descr data,    gdt_data
        init_gdt_descr code16,  gdt_code16
        init_gdt_descr code32,  gdt_code32
        init_gdt_descr stack,   gdt_stack

        xor eax, eax
        mov ax, data
        shl eax, 4
        add eax, offset gdt_table_begin
        mov gdt_pdescr_addr, eax
        lgdt gdt_pdescr

        ; =======================
        ; ���樠������ ���ᮢ ��ࠡ��稪�� �᪫�祭�� � IDT
        ; =======================

        init_int_descr macro procname, idesc
            mov eax, offset procname
            mov idesc.offs_l, ax
            shr eax, 16
            mov idesc.offs_h, ax
        endm

        init_int_descr inth_0,          idescr_0_12
        init_int_descr inth_gen_prot,   idescr_13
        init_int_descr inth_0,          idescr_14_31
        init_int_descr inth_timer,      idescr_32
        init_int_descr inth_keyb,       idescr_33

        xor eax, eax
        mov ax, data
        shl eax, 4
        add eax, offset idt_table_begin
        mov idt_pdescr_addr, eax

        ; ���࠭���� ��᮪ ����஫��஢ ���뢠���

        in  al, 21h
        mov mask_pic0, al
        in  al, 0A1h
        mov mask_pic1, al

        ; ��९ணࠬ��஢���� ����饣� ����஫���

        mov al, 11h
        out 20h, al
        mov al, 32
        out 21h, al
        mov al, 4
        out 21h, al
        mov al, 1
        out 21h, al

        ; ��⠭���� ����� ��᮪ ��� ����஫��஢

        mov al, 0FCh
        out 21h, al
        mov al, 0FFh
        out 0A1h, al

        ; ����㧪� ॣ���� ⠡���� ���ਯ�஢ �᪫�祭��

        lidt fword ptr idt_pdescr

        ; ����⨥ ����� �20

        in  al, 92h
        or  al, 2
        out 92h, al

        cli

        ; ���室 � ���饭�� ०��

        mov eax, cr0
        or al, 1
        mov cr0, eax

        ; ���室 � 32-���� ᥣ���� ����
        db 66h, 0EAh
        dd offset prot_start
        dw sel_code32

    prot_end:

        ; ���室 � ॠ��� ०��

        mov eax, cr0
        and al, 0FEh
        mov cr0, eax

        ; ���������� ᥣ������ ॣ���஢

        db 0EAh
        dw offset real_jmp
        dw code16
    real_jmp:

        mov ax, data
        mov ds, ax

        mov ax, stack
        mov ss, ax

        ; ��९ணࠬ��஢���� ����஫��஢ ���뢠���
        ; �� ࠡ��� � ॠ�쭮� ०���

        mov al, 11h
        out 20h, al
        mov al, 8
        out 21h, al
        mov al, 4
        out 21h, al
        mov al, 1
        out 21h, al

        ; ����⠭������� ��᮪ ���뢠���

        mov al, mask_pic0
        out 21h, al
        mov al, mask_pic1
        out 0A1h, al

        ; ����㧪� ⠡���� ����஢ ���뢠��� ॠ�쭮�� ०���

        lidt ivt_pdescr

        ; �����⨥ ����� �20

        in  al, 70h 
        and al, 7Fh
        out 70h, al

        sti

        mov ah, 09h
        mov dx, offset msg_real
        int 21h

        mov ax, 4C00h
        int 21h

    code16_size = $ - code16_begin
code16 ends

code32 segment para public use32 'CODE'
    code32_begin label byte

    inth_0 proc
        iretd
    inth_0 endp

    inth_gen_prot proc
        pop eax
        iretd
    inth_gen_prot endp

    inth_timer proc uses eax edi
            cli

            push cursor_pos
            mov cursor_pos, 80 * 2 * 3

            mov esi, offset msg_timer
            call print_msg

            xor eax, eax
            mov ax, tick_num
            inc ax
            mov tick_num, ax

            call print_eax
            pop cursor_pos

            sti

            mov al, 20h
            out 20h, al

            iretd
    inth_timer endp

    ; scan-code in al, shift flag in dl
    ; returns: ah=1 - char is printable
    ;          al - char code
    key_dispatch proc uses ebx
            mov ah, 0
            mov bx, ax
            mov ah, ds:asciimap[bx]
            cmp ah, 0
            je not_printable
            xchg ah, al
            mov ah, 1
            cmp dl, 1
            je no_lower
            cmp al, 'A'
            jb no_lower
            cmp al, 'Z'
            ja no_lower
            add al, 'a' - 'A'
        no_lower:
        not_printable:
            ret
    key_dispatch endp

    inth_keyb proc uses eax ebx ecx edx ecx
            in al, 60h

            .if al == 1 ; ESC scan-code
                mov esc_pressed, 1
            .elseif al == 42 ; LSHIFT pressed
                mov ds:shift_flag, 1
            .elseif al == 0AAh ; LSHIFT released
                mov ds:shift_flag, 0
            .elseif al == 14
                call print_bs
            .elseif al <= 80h
                mov dl, ds:shift_flag
                call key_dispatch
                .if ah != 0
                    call print_chr
                .endif
            .endif

        allow_handle_keyboard:
            in  al, 61h
            push ax
            or  al, 80h
            out 61h, al
            ; and al, 7Fh
            pop ax
            out 61h, al

            mov al, 20h 
            out 20h, al

            iretd
    inth_keyb endp

    clear_screen proc uses cx bx
        mov cx, 80 * 25
        xor bx, bx
        cycle:
            mov word ptr es:[bx], 0F20h
            add bx, 2
            loop cycle
        ret
    clear_screen endp

    ; ���⠥� ᨬ��� � ॣ���� al
    print_chr proc uses ax bx
            cmp al, 0
            je no_char
            mov bx, cursor_pos
            mov ah, cursor_attr
            mov word ptr es:[bx], ax
            add cursor_pos, 2
        no_char:
            ret
    print_chr endp

    print_bs proc uses ax bx
        mov al, 32
        mov bx, cursor_pos
        mov ah, cursor_attr
        mov word ptr es:[bx], ax
        sub cursor_pos, 2
        ret
    print_bs endp

    print_newline proc uses ax
            mov ax, cursor_pos
        div_loop:
            cmp ax, 0
            jl end_loop
            sub ax, 80 * 2
            jmp div_loop
        end_loop:
            neg ax
            add cursor_pos, ax
            ret
    print_newline endp

    ; ���⠥� ᮮ�饭�� �� ds:esi
    print_msg proc uses ax esi
        str_loop:
            mov al, ds:[esi]
            call print_chr
            inc esi
            cmp al, 0
            jne str_loop
        ret
    print_msg endp

    ; ���⠥� �᫮ � eax
    print_eax proc uses eax ebx edx esi
        mov esi, 10
        mov ebx, ebp
        sub ebp, 4
        mov byte ptr ss:[ebp], 0
        digit_loop:
            xor edx, edx
            div esi     ; edx:eax - number
                        ; eax - ��⭮�
                        ; edx - ���⮪
            add dl, '0'
            dec ebp
            mov ss:[ebp], dl
            cmp eax, 0
            jne digit_loop
        rev_loop:
            mov al, ss:[ebp]
            call print_chr
            inc ebp
            cmp al, 0
            jne rev_loop
        mov ebp, ebx
        ret
    print_eax endp

    calc_mem proc uses ebx ecx
        mov ah, 0AEh
        mov ebx, 1000001h
        calc_loop:
            mov eax, gs:[ebx]
            mov gs:[ebx], ecx
            cmp gs:[ebx], ecx
            jne end_loop
            mov gs:[ebx], eax
            add ebx, 4
            jmp calc_loop
        end_loop:
        mov gs:[ebx], al
        sub ebx, 100001h
        mov eax, ebx
        ret
    calc_mem endp

    prot_start:

        ; ����㧪� ⥭���� ॣ���஢

        mov ax, sel_data
        mov ds, ax

        mov ax, sel_stack
        mov ss, ax

        mov ax, sel_screen
        mov es, ax

        mov ax, sel_data4gb
        mov gs, ax

        sti

        call clear_screen

        mov cursor_attr, 00000010b
        mov esi, offset msg_prot
        call print_msg
        call print_newline

        mov esi, offset msg_mem
        call print_msg
        call calc_mem
        call print_eax
        mov esi, offset msg_byte
        call print_msg
        call print_newline
        call print_newline

        wait_loop:
            mov al, esc_pressed
            cmp al, 1
            jne wait_loop

        cli

        ; ���室 ���⭮ � 16-���� ᥣ���� ����

        db 0EAh
        dd offset prot_end
        dw sel_code16

    code32_size = $ - code32_begin
code32 ends

end start
