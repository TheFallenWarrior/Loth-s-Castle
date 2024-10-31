;
; Start-up code for Loth's Castle
;
; based on code by Groepaz/Hitmen and Ullrich von Bassewitz
;

        .include        "zeropage.inc"
        .include        "nes.inc"

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib, callmain
        .import         push0, _main, zerobss, copydata
        .import         ppubuf_flush

        ; Linker-generated symbols
        .import         __RAM_START__, __RAM_SIZE__
        .import         __SRAM_START__, __SRAM_SIZE__
        .import         __ROM0_START__, __ROM0_SIZE__
        .import         __STARTUP_LOAD__,__STARTUP_RUN__, __STARTUP_SIZE__
        .import         __CODE_LOAD__,__CODE_RUN__, __CODE_SIZE__
        .import         __RODATA_LOAD__,__RODATA_RUN__, __RODATA_SIZE__

; ------------------------------------------------------------------------
; Character data
; ------------------------------------------------------------------------

.segment "CHARS"

        .incbin         "tileset.chr"

; ------------------------------------------------------------------------
; 16-byte NES 2.0 header

.segment        "HEADER"

;    +--------+------+------------------------------------------+
;    | Offset | Size | Content(s)                               |
;    |   0    |  3   | 'NES'                                    |
;    |   3    |  1   | $1A                                      |
;    |   4    |  1   | 16K PRG-ROM page count                   |
;    |   5    |  1   | 8K CHR-ROM page count                    |
;    |   6    |  1   | Mapper, mirroring, battery saving        |
;    |   7    |  1   | Mapper, NES 2.0                          |
;    |   8    |  1   | Mapper, Submapper                        |
;    |   9    |  1   | PRG-ROM/CHR-ROM page count MSB           |
;    |   10   |  1   | PRG-RAM/PRG-NVRAM size                   |
;    |   11   |  1   | CHR-RAM/CHR-NVRAM size                   |
;    |   12   |  1   | CPU/PPU Timing                           |
;    |   13   |  1   | VS. System/Console type                  |
;    |   14   |  1   | Misc ROMs (for non-standard hardware)    |
;    |   15   |  1   | Input Device                             |
;    +--------+------+------------------------------------------+

        .byte   $4e,$45,$53,$1a ; "NES\x1a"
        .byte   2               ; 32 KiB PRG-ROM
        .byte   1               ; 8 KiB CHR-ROM
        .byte   1               ; NROM, Vertical mirroring
        .byte   8               ; NROM, NES 2.0
        .byte   0, 0            ; NROM, Submapper 0
        .byte   7               ; 8 KiB PRG-RAM
        .byte   0               ; 0 KiB CHR-RAM
        .byte   0               ; NTSC
        .byte   0, 0            ; Standard hardware
        .byte   1               ; Standard controllers

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment        "STARTUP"

start:

; Set up the CPU and System-IRQ.

        sei
        cld
        ldx     #0
        stx     VBLANK_FLAG

        stx     ringread
        stx     ringwrite
        stx     ringcount

        txs

        lda     #$20
@l:     sta     ringbuff,x
        sta     ringbuff+$0100,x
        sta     ringbuff+$0200,x
        inx
        bne     @l

; Clear the BSS data.

        jsr     zerobss

; Initialize the data.
        jsr     copydata

; Set up the stack.

        lda     #<(__SRAM_START__ + __SRAM_SIZE__)
        ldx     #>(__SRAM_START__ + __SRAM_SIZE__)
        sta     sp
        stx     sp+1            ; Set argument stack ptr

; Call the module constructors.

        jsr     initlib

; Push the command-line arguments; and, call main().

        jsr     callmain

; Call the module destructors. This is also the exit() entry.

_exit:  jsr     donelib         ; Run module destructors

; Reset the NES.

        jmp start

; ------------------------------------------------------------------------
; System V-Blank Interrupt
; Updates PPU Memory (buffered).
; Updates VBLANK_FLAG and tickcount.
; ------------------------------------------------------------------------

nmi:    pha
        tya
        pha
        txa
        pha

        lda     #1
        sta     VBLANK_FLAG

        inc     tickcount
        bne     @s
        inc     tickcount+1

@s:     jsr     ppubuf_flush

        ; Reset the video counter.
        lda     #$20
        sta     PPU_VRAM_ADDR2
        lda     #$00
        sta     PPU_VRAM_ADDR2

        ; Reset scrolling.
        sta     PPU_VRAM_ADDR1
        sta     PPU_VRAM_ADDR1

        pla
        tax
        pla
        tay
        pla

; Interrupt exit

irq:
        rti


; ------------------------------------------------------------------------
; Hardware vectors
; ------------------------------------------------------------------------

.segment "VECTORS"

        .word   nmi         ; $fffa vblank nmi
        .word   start       ; $fffc reset
        .word   irq         ; $fffe irq / brk