# Achievements

This file has all the info I gathered from reverse-engineering Achievements.

## Helpful LLDB Commands

Step out of a GlossHook header on 32bit.
`step -c 14`

Step through an `asm` C block. Use `disassemble` to see where the pointer is.
`stepi [-c count]`

Place a breakpoint at a library offset. Causes inconsistent behavior on 32bit.
`breakpoint set --shlib libswordigo.so --address 0x289f06`

Place a breakpoint at a Symbol offset. Crashes 32bit.
`b -a `(void())_ZN5Caver19AchievementsManagerC1Ev`+0x1a6c`

Show disassembly, in case IDE is showing Arm/Arm64 instead of Thumb instructions
`disassemble --arch thumbv7`

## Helpful ARM docs

64bit
https://developer.arm.com/documentation/100069/0609/A64-General-Instructions/A64-instructions-in-alphabetical-order?lang=en
https://developer.arm.com/documentation/111182/2025-09_ASL1/Base-Instructions

32bit
https://developer.arm.com/documentation/ddi0597/2025-09/Base-Instructions?lang=en

https://onlinetoolz.net/unsigned-signed#base=16
https://ret.futo.org/arm32/?thumb
https://armconverter.com/

## 64bit

### Assembly

```asm
	mov w0, #0x30
	bl #0xffffffffffe7df70		; operator.new
	mov x26, x0					; x26 = pointer to achievement
	madd x25, x27, x20, x22		; x25 = ( x27 * x20 ) + x22; All three must be pre-set.
	mov x0, x26					; 
	mov x1, x25					; x25 is the pointer to the achievement that was placed on the stack before, shifted by the mul/add.
	bl #0xffffffffffe7cda0		; Achievement Init
	str x26, [sp, #0x68]		; Place the pointer to ach into stack+68
	mov x0, x24
	mov x1, x26
	bl #0xffffffffffe812f0		; shared_count_init(count, achievement)
	add x0, sp, #0x68			; x0 = achievement*
	mov x1, x26
	mov x2, x26
	bl #0xffffffffffe8b2e0		; enable_shared_from_this
	ldp x1, x8, [x19, #8]
	cmp x1, x8
	b.eq #0x7c					; lbl1   v
	ldr x8, [sp, #0x68]
	str x8, [x1]
	ldr x8, [sp, #0x70]
	str x8, [x1, #8]
	cbz x8, #0x68				; lbl2 (skip 3 inst)
	ldr x9, [x8, #8]
	add x9, x9, #1
	str x9, [x8, #8]
lbl1:
	add x8, x1, #0x10
	str x8, [x19, #8]
	ldp x23, x26, [sp, #0x68]
	cbnz x26, #0x90				; lbl4
	b #0x9c						; lbl3
lbl2:
	add x2, sp, #0x68
	mov x0, x19
	bl #0xffffffffffe7e600		; 
	ldp x23, x26, [sp, #0x68]
	cbz x26, #0x9c
lbl3:
	ldr x8, [x26, #8]
	add x8, x8, #1
	str x8, [x26, #8]
lbl4:
	add x0, sp, #0x50
	mov x1, x25
	bl #0x1f2eb8
lbl5:
	ldr x0, [sp, #0x28]
	add x1, sp, #0x50
	bl #0xffffffffffe80310
	cbz x26, #0xc4
	ldr x8, [x26, #8]
	add x8, x8, #1
	str x8, [x26, #8]
lbl6:
	ldr x25, [x0, #8]
	stp x23, x26, [x0]
	cbz x25, #0x110
	ldr x8, [x25, #8]
	subs x8, x8, #1
	str x8, [x25, #8]
	b.ne #0x110
	ldr x8, [x25]
	ldr x8, [x8, #0x10]
	mov x0, x25
	blr x8
	ldr x8, [x25, #0x10]
	subs x8, x8, #1
	str x8, [x25, #0x10]
	b.ne #0x110
	ldr x8, [x25]
	ldr x8, [x8, #0x18]
	mov x0, x25
	blr x8
lbl7:
	ldr x8, [sp, #0x50]
	sub x0, x8, #0x18
	cmp x0, x21
	b.ne #0xfffffffffffffe54
lbl8:
	cbz x26, #0x164
	ldr x8, [x26, #8]
	subs x8, x8, #1
	str x8, [x26, #8]
	b.ne #0x164
	ldr x8, [x26]
	ldr x8, [x8, #0x10]
lbl9:
	mov x0, x26
	blr x8
	ldr x8, [x26, #0x10]
	subs x8, x8, #1
	str x8, [x26, #0x10]
	b.ne #0x164
	ldr x8, [x26]
	ldr x8, [x8, #0x18]
	mov x0, x26
	blr x8
lbl10:
	ldr x8, [sp, #0x68]
	mov x9, x8
	ldr x10, [x9, #0x20]!
	ldur x10, [x10, #-0x18]
	cmp x10, #0
	csel x1, x8, x9, eq
lbl11:
	add x0, sp, #0x50
	bl #0x1f2eb8
lbl12:
	ldr x0, [sp, #0x30]
	add x1, sp, #0x50
	bl #0xffffffffffe7d310
	mov x25, x0
	ldr x8, [sp, #0x50]
	sub x0, x8, #0x18
	cmp x0, x21
	b.ne #0xfffffffffffffe78
lbl13:
	ldr x8, [sp, #0x20]
	cmp x8, x25
	b.ne #0x218
	ldr x8, [sp, #0x68]
	mov x9, x8
	ldr x10, [x9, #0x20]!
	ldur x10, [x10, #-0x18]
	cmp x10, #0
	csel x1, x8, x9, eq
lbl14:
	add x0, sp, #0x50
	bl #0x1f2eb8
	ldp x8, x9, [x19, #0x50]
	cmp x8, x9
	b.eq #0x1f8
lbl15:
	add x1, sp, #0x50
	mov x0, x8
	bl #0x1f2eb8
	ldr x8, [x19, #0x50]
	add x8, x8, #8
	str x8, [x19, #0x50]
	b #0x208
lbl16:
	ldr x0, [sp, #0x10]
	add x2, sp, #0x50
	mov x1, x8
	bl #0xffffffffffe80d30
lbl17:
	ldr x8, [sp, #0x50]
	sub x0, x8, #0x18
	cmp x0, x21
	b.ne #0xffffffffffffffbc
lbl18:
	ldr x8, [sp, #0x68]
	mov x9, x8
	ldr x10, [x9, #0x20]!
	ldur x10, [x10, #-0x18]
	cmp x10, #0
	csel x1, x8, x9, eq
lbl19:
	add x0, sp, #0x740
	bl #0x1f2eb8
lbl20:
	add x0, sp, #0x38
	add x1, sp, #0x740
	bl #0x1f2eb8
	ldp x9, x8, [sp, #0x68]
	stp x9, x8, [sp, #0x40]
	cbz x8, #0x25c
	ldr x9, [x8, #8]
	add x9, x9, #1
	str x9, [x8, #8]
lbl21:
	add x0, sp, #0x50
	add x1, sp, #0x38
	bl #0x1f2eb8
	ldp x9, x8, [sp, #0x40]
	stp x9, x8, [sp, #0x58]
	cbz x8, #0x280
	ldr x9, [x8, #8]
	add x9, x9, #1
	str x9, [x8, #8]
	ldr x0, [sp, #0x30]
	add x1, sp, #0x50
	bl #0xffffffffffe81130
	ldr x25, [sp, #0x60]
	cbz x25, #0x2d4
	ldr x8, [x25, #8]
	subs x8, x8, #1
	str x8, [x25, #8]
	b.ne #0x2d4
	ldr x8, [x25]
	ldr x8, [x8, #0x10]
lbl22:
	mov x0, x25
	blr x8
	ldr x8, [x25, #0x10]
	subs x8, x8, #1
	str x8, [x25, #0x10]
	b.ne #0x2d4
	ldr x8, [x25]
	ldr x8, [x8, #0x18]
	mov x0, x25
	blr x8
lbl23:
	ldr x8, [sp, #0x50]
	sub x0, x8, #0x18
	cmp x0, x21
	b.ne #0xfffffffffffffe9c
lbl24:
	ldr x25, [sp, #0x48]
	cbz x25, #0x32c
	ldr x8, [x25, #8]
	subs x8, x8, #1
	str x8, [x25, #8]
	b.ne #0x32c
	ldr x8, [x25]
	ldr x8, [x8, #0x10]
lbl25:
	mov x0, x25
	blr x8
	ldr x8, [x25, #0x10]
	subs x8, x8, #1
	str x8, [x25, #0x10]
	b.ne #0x32c
	ldr x8, [x25]
	ldr x8, [x8, #0x18]
	mov x0, x25
	blr x8
lbl26:
	ldr x8, [sp, #0x38]
	sub x0, x8, #0x18
	cmp x0, x21
	b.ne #0xfffffffffffffec0
lbl27:
	ldr x8, [sp, #0x740]
	sub x0, x8, #0x18
	cmp x0, x21
	b.ne #0xfffffffffffffee4
lbl28:
	ldr x25, [sp, #0x70]
	cbz x25, #0x394
	ldr x8, [x25, #8]
	subs x8, x8, #1
	str x8, [x25, #8]
	b.ne #0x394
	ldr x8, [x25]
	ldr x8, [x8, #0x10]
lbl29:
	mov x0, x25
	blr x8
	ldr x8, [x25, #0x10]
	subs x8, x8, #1
	str x8, [x25, #0x10]
	b.ne #0x394
	ldr x8, [x25]
	ldr x8, [x8, #0x18]
	mov x0, x25
	blr x8
```

### Stack/Registers

long @ 68 written first
long @ 70 read first
long @ 50 read first
long @ 28 read first
long @ 30 read first
long @ 20 read first
long @ 10 read first
long @ 740 read first
long @ 38 read first
long @ 40 written first
long @ 48 written first (stp)
long @ 58 written first
long @ 60 written first (stp)
long @ 48 read first

w0 - Written first

x0 - Written first
x1 - Written first
x2 - Written first
x3
x4
x5
x6
x7
x8 - Written First
x9 - Written First
x10 - Written First
x11
x12
x13
x14
x15
x16
x17
x18
x19 - Read first, manager 'this' pointer
x20 - Read first, 0x30, size of achievement instance on stack
x21 - Read first, empty_cpp_string minus 24 bytes...
x22 - Read first, sp + 0x768, Address of first achievement on stack?
x23 - Written first
x24 - Read first, sp + 0x68 + 0x8, Shared Count location?
x25 - Written first
x26 - Written first
x27 - Read first, always 0 because only doing 1 achievement.
x29

A - manager+48
B - manager+48
C - manager+18
D - manager+60
name - Direct pointer to Name string.
F - (written first???)
G - (written first stp???)
H - Direct pointer to ID string.
I - Written first? Holds 3...
J - Written first, Holds NULL
achievement - Written first, junk.
L - Pointer to Pointer to... written first hopefully...

More can be found in `ach64.c`

## 32bit

### Assembly

```asm
	; Kinda preamble thing?
	add r0, sp, #0x48
	add r5, sp, #0x3c
	add.w r11, r0, #4
	add.w r8, sp, #0x3ac
	add.w r9, sp, #0x30
	movs r4, #0
	add.w r10, sp, #0x3c8
	str.w r10, [sp, #0x2c]

	; I think this is where the actual jump will land.
	movs r0, #0x18			; sizeof achievement.
	blx #0xfff1d228			; operator.new
	mov r6, r0
	ldr r1, [sp, #0x2c]
	mov r0, r6
	blx #0xfff30a80
	str r6, [sp, #0x48]
	mov r0, r11
	mov r1, r6
	blx #0xfff30a88
	str r4, [sp, #0x24]
	add r4, sp, #0x48
	mov r1, r6
	mov r2, r6
	mov r0, r4
	blx #0xfff1d340
	ldr r0, [sp, #0x28]
	mov r1, r4
	blx #0xfff30a98
	mov r10, r9
	mov r9, r8
	ldrd r6, r8, [sp, #0x48]
	cmp.w r8, #0
	ittt ne
	ldrne.w r0, [r8, #4]
	addne r0, #1
	strne.w r0, [r8, #4]
	ldr r1, [sp, #0x2c]
	mov r4, r5
	mov r0, r4
	bl #0xf148a
	ldr r0, [sp, #0x20]
	mov r1, r4
	blx #0xfff30aa0
	cmp.w r8, #0
	ittt ne
	ldrne.w r1, [r8, #4]
	addne r1, #1
	strne.w r1, [r8, #4]
	str r6, [r0]
	ldr r1, [r0, #4]
	str.w r8, [r0, #4]
	cbz r1, #0x7a
	mov r0, r1
	blx #0xfff1d354
	mov r0, r5
	bl #0xef872
	cmp.w r8, #0
	beq #0x8c
	mov r0, r8
	blx #0xfff1d358
	ldr r0, [sp, #0x48]
	mov r1, r0
	ldr r2, [r1, #0x10]!
	ldr r2, [r2, #-0xc]
	cmp r2, #0
	it eq
	moveq r1, r0
	mov r0, r5
	ldr r4, [sp, #0x1c]
	bl #0xf148a
	mov r8, r9
	mov r9, r10
	mov r0, r4
	mov r1, r5
	blx #0xfff30aac
	mov r6, r0
	mov r0, r5
	bl #0xef872
	ldr r0, [sp, #0x18]
	cmp r0, r6
	bne #0xe6
	ldr r0, [sp, #0x48]
	mov r1, r0
	ldr r2, [r1, #0x10]!
	ldr r2, [r2, #-0xc]
	cmp r2, #0
	it eq
	moveq r1, r0
	mov r0, r5
	bl #0xf148a
	ldr r0, [sp, #0x14]
	mov r1, r5
	blx #0xfff20ce8
	mov r0, r5
	bl #0xef872
	ldr r0, [sp, #0x48]
	mov r1, r0
	ldr r2, [r1, #0x10]!
	ldr r2, [r2, #-0xc]
	cmp r2, #0
	it eq
	moveq r1, r0
	mov r0, r8
	bl #0xf148a
	mov r0, r9
	mov r1, r8
	bl #0xf148a
	ldrd r1, r0, [sp, #0x48]
	cmp r0, #0
	strd r1, r0, [sp, #0x34]
	beq #0x118
	ldr r1, [r0, #4]
	adds r1, #1
	str r1, [r0, #4]
	mov r0, r5
	mov r1, r9
	bl #0xf148a
	ldrd r1, r0, [sp, #0x34]
	cmp r0, #0
	strd r1, r0, [sp, #0x40]
	beq #0x132
	ldr r1, [r0, #4]
	adds r1, #1
	str r1, [r0, #4]
	add r0, sp, #0x3c0
	mov r1, r4
	mov r2, r5
	blx #0xfff30abc
	ldrd r1, r2, [sp, #0x3c0]
	mov r0, r4
	mov r3, r5
	blx #0xfff30ac8
	ldr r0, [sp, #0x44]
	cbz r0, #0x150
	blx #0xfff1d358
	mov r0, r5
	bl #0xef872
	ldr r0, [sp, #0x38]
	cbz r0, #0x15e
	blx #0xfff1d354
	mov r0, r9
	bl #0xef872
	mov r0, r8
	bl #0xef872
	ldr r0, [sp, #0x4c]
	ldr r4, [sp, #0x24]
	cbz r0, #0x174
	blx #0xfff1d358
	ldr r0, [sp, #0x2c]
```

### Stack/Registers

r0 - Written first
r1 - Written first
r2 - Written first
r3 - Written first
r4 - Read first (Always 0x0)
r5 - Read first (sp + 0x3c)
r6 - Written first
r7 -
r8 - Read first (sp + 0x3ac)
r9 (sb) - Read first (sp + 0x30)
r10 (sl) - Written first
r11 (fp) - Read first (sp + 0x48 + 0x4)
r12 -

Stack:
2c - Read First (sp + 0x3c8, using r10) Input achievement pointer
48 - Written first (Work Achievement pointer)
24 - Written first (Shared count?)
28 - Read first (manager)
4c - Read first [ldrd 0x48 + 4] Completely unknown. Does not crash when initialized to 0x0.
20 - Read first (manager + 12)
1c - Read first (manager + 48)
18 - Read first (manager + 52)
14 - Read first (manager + 36)
34 - Written first
38 - Written first (34 + 4)
40 - Written first
44 - Written first (40 + 4)
3c0 - Appears to be read first, but is actually written first in a function call.
