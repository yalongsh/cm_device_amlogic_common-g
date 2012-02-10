    @W1:        .word     2841
    @W2:        .word     2676
    @W3:        .word     2408
    @W5:        .word     1609
    @W6:        .word     1108
    @W7:        .word     565
    
    
    .section .text
    .global	idct_col0
idct_col0:
    stmdb      sp!, {lr}
    ldmia      sp!, {pc}
    @ENDP  @ |idct_col0|

    .section .text
    .global	idct_col1
idct_col1:
    stmdb      sp!, {r1, lr}
    ldrsh      r1,  [r0]
    lsl        r1,  r1,  #3
    strh       r1,  [r0]
    strh       r1,  [r0, #16]
    strh       r1,  [r0, #32]
    strh       r1,  [r0, #48]
    strh       r1,  [r0, #64]
    strh       r1,  [r0, #80]
    strh       r1,  [r0, #96]
    strh       r1,  [r0, #112]
    ldmia      sp!, {r1, pc}
    @ENDP  @ |idct_col1|

    .section .text
    .global	idct_col2
idct_col2:
    stmdb      sp!, {r1 - r3, lr}
    ldrsh      r1,  [r0]
    ldrsh      r2,  [r0, #16]
    
    lsl        r1,  #11
    add        r1,  r1,  #128
    
    VDUP.S32   Q1,  r1                            @Q1: x0 x0 x0 x0
    ldr        r1,  =565
    mul        r3,  r2, r1
    VMOV.S32   d0[0], r3                          @r3: x7
    ldr        r1,  =2841
    mul        r1,  r2, r1
    VMOV.S32   d0[1], r1                          @r1: x1;  d0: x1 x3
    
    sub        r2,  r1, r3
    VMOV.S32   d4[0], r2                          @x1-x7
    add        r2,  r1, r3
    VMOV.S32   d4[1], r2                          @x1+x7
    
    VMOV.S32   d1,  #128
    VMOV.S32   d5,  #181
    VMLA.S32   d1,  d4, d5                        @d1: x7 x5
    VSHR.S32   d1,  d1, #8
    
    VADD.S32   Q2,  Q1, Q0
    VSUB.S32   Q1,  Q1, Q0
    VSHR.S32   Q0,  Q2, #8
    VSHR.S32   Q1,  Q1, #8
    
    VMOV       r1,  r2, d0
    strh       r1,  [r0, #48]
    strh       r2,  [r0]
    VMOV       r1,  r2, d1
    strh       r1,  [r0, #32]
    strh       r2,  [r0, #16]
    VMOV       r1,  r2, d2
    strh       r1,  [r0, #64]
    strh       r2,  [r0, #112]
    VMOV       r1,  r2, d3
    strh       r1,  [r0, #80]
    strh       r2,  [r0, #96]
    
    ldmia      sp!, {r1 - r3, pc}
    @ENDP  @ |idct_col2|
    
    .section .text
    .global	idct_col3
idct_col3:
    stmdb      sp!, {r1 - r4, lr}
    ldrsh      r1,  [r0]
    ldrsh      r2,  [r0, #16]
    ldrsh      r3,  [r0, #32]
    
    lsl        r1,  #11
    add        r1,  r1,  #128                     @r1: x0; r2:x1; r3:x2
    
    
    VDUP.S32   d0,  r3                            @d0: x2 x2
    VDUP.S32   d1,  r2                            @d1: x1 x1
    
    ldr        r4,  =1108
    VMOV.S32   d2[0], r4
    ldr        r4,  =2676
    VMOV.S32   d2[1], r4
    ldr        r4,  =2841
    VMOV.S32   d3[0], r4
    ldr        r4,  =565
    VMOV.S32   d3[1], r4                          @Q1: W7W1W2W6
    VMUL.S32   Q0,  Q1, Q0                        @Q0: x7x1x2x6
    
    VDUP.S32   d2,  r1                            @d2: x0 x4
    VSUB.S32   d3,  d2, d0                        @d3: x2 x6
    VADD.S32   d2,  d2, d0                        @d2: x0 x4
    VZIP.S32   d2,  d3                            @d2: x6 x4; d3: x2 x0
    
    VREV64.S32 d0,  d1                            @d0: x1 x7
    VPADDL.S32 d4,  d1                            @d4: x1+x7
    VSUB.S32   d5,  d1, d0                        @d5: x1-x7
    VZIP.S32   d4,  d5                            @d4: (x1-x7)(x1+x7)
    
    VMOV.S32   d0,  #128
    VMOV.S32   d5,  #181
    VMLA.S32   d0,  d4, d5                        @d0: x5 x7 ;d1: x3 x1
    VSHR.S32   d0,  d0, #8
    
    VADD.S32   Q2,  Q1, Q0
    VSUB.S32   Q1,  Q1, Q0
    VSHR.S32   Q0,  Q2, #8
    VSHR.S32   Q1,  Q1, #8
    
    VMOV       r1,  r2, d0
    strh       r1,  [r0, #16]
    strh       r2,  [r0, #32]
    VMOV       r1,  r2, d1
    strh       r1,  [r0]
    strh       r2,  [r0, #48]
    VMOV       r1,  r2, d2
    strh       r1,  [r0, #96]
    strh       r2,  [r0, #80]
    VMOV       r1,  r2, d3
    strh       r1,  [r0, #112]
    strh       r2,  [r0, #64]
    
    ldmia      sp!, {r1 - r4, pc}
    @ENDP  @ |idct_col3|
    
    .section .text
    .global	idct_col4
idct_col4:
    stmdb      sp!, {r1 - r5, lr}
    ldrsh      r1,  [r0]                          @r1: x0
    ldrsh      r2,  [r0, #16]                     @r2: x1
    ldrsh      r3,  [r0, #32]                     @r3: x2
    ldrsh      r5,  [r0, #48]                     @r5: x3
    
    lsl        r1,  #11
    add        r1,  r1,  #128                     @r1: x0; r2:x1; r3:x2
    
    
    VDUP.S32   d0,  r3                            @d0: x2 x2
    VDUP.S32   d1,  r2                            @d1: x1 x1
    
    ldr        r4,  =1108
    VMOV.S32   d2[0], r4
    ldr        r4,  =2676
    VMOV.S32   d2[1], r4
    ldr        r4,  =2841
    VMOV.S32   d3[0], r4
    ldr        r4,  =565
    VMOV.S32   d3[1], r4                          @Q1: W7W1W2W6
    VMUL.S32   Q0,  Q1, Q0                        @Q0: x7x1x2x6
    
    VDUP.S32   d2,  r1                            @d2: x0 x4
    VSUB.S32   d3,  d2, d0                        @d3: x2 x6
    VADD.S32   d2,  d2, d0                        @d2: x0 x4
    VZIP.S32   d2,  d3                            @d2: x6 x4; d3: x2 x0
    
    VDUP.S32   d0,  r5                            @d0: x3 x3
    ldr        r4,  =2408
    VMOV.S32   d4[0], r4
    ldr        r4,  =-1609
    VMOV.S32   d4[1], r4                          @d4: -W5 W3
    VMUL.S32   d0,  d4, d0                        @d0: x3 x5
    
    VSUB.S32   d4,  d1, d0                        @d4: x8 x5
    VADD.S32   d1,  d1, d0                        @d1: x3 x1
    
    VREV64.S32 d0,  d4                            @d0: x5 x8
    VPADDL.S32 d5,  d4                            @d5: x8+x5
    VSUB.S32   d4,  d4, d0                        @d4: x5-x8
    VZIP.S32   d5,  d4                            @d5: (x5-x8)(x8+x5)
    
    VMOV.S32   d0,  #128
    VMOV.S32   d4,  #181
    VMLA.S32   d0,  d4, d5                        @d0: x5 x7 ;d1: x3 x1
    VSHR.S32   d0,  d0, #8
    
    VADD.S32   Q2,  Q1, Q0
    VSUB.S32   Q1,  Q1, Q0
    VSHR.S32   Q0,  Q2, #8
    VSHR.S32   Q1,  Q1, #8
    
    VMOV       r1,  r2, d0
    strh       r1,  [r0, #16]
    strh       r2,  [r0, #32]
    VMOV       r1,  r2, d1
    strh       r1,  [r0]
    strh       r2,  [r0, #48]
    VMOV       r1,  r2, d2
    strh       r1,  [r0, #96]
    strh       r2,  [r0, #80]
    VMOV       r1,  r2, d3
    strh       r1,  [r0, #112]
    strh       r2,  [r0, #64]
    
    ldmia      sp!, {r1 - r5, pc}
    @ENDP  @ |idct_col4|
    
    .section .text
    .global	idct_col0x40
idct_col0x40:
    stmdb      sp!, {r1 - r3, lr}
    ldrsh      r2,  [r0, #16]
    
    VMOV.S32   Q1,  #128                          @Q1: x0 x0 x0 x0
    ldr        r1,  =565
    mul        r3,  r2, r1
    VMOV.S32   d0[0], r3                          @r3: x7
    ldr        r1,  =2841
    mul        r1,  r2, r1
    VMOV.S32   d0[1], r1                          @r1: x1;  d0: x1 x3
    
    sub        r2,  r1, r3
    VMOV.S32   d4[0], r2                          @x1-x7
    add        r2,  r1, r3
    VMOV.S32   d4[1], r2                          @x1+x7
    
    VMOV.S32   d1,  #128
    VMOV.S32   d5,  #181
    VMLA.S32   d1,  d4, d5                        @d1: x7 x5
    VSHR.S32   d1,  d1, #8
    
    VADD.S32   Q2,  Q1, Q0
    VSUB.S32   Q1,  Q1, Q0
    VSHR.S32   Q0,  Q2, #8
    VSHR.S32   Q1,  Q1, #8
    
    VMOV       r1,  r2, d0
    strh       r1,  [r0, #48]
    strh       r2,  [r0]
    VMOV       r1,  r2, d1
    strh       r1,  [r0, #32]
    strh       r2,  [r0, #16]
    VMOV       r1,  r2, d2
    strh       r1,  [r0, #64]
    strh       r2,  [r0, #112]
    VMOV       r1,  r2, d3
    strh       r1,  [r0, #80]
    strh       r2,  [r0, #96]
    
    ldmia      sp!, {r1 - r3, pc}
    @ENDP  @ |idct_col0x40|
    
    .section .text
    .global	idct_col0x20
idct_col0x20:
    stmdb      sp!, {r1 - r2, lr}
    ldrsh      r1,  [r0, #32]
    
    VMOV.S32   d2,  #128
    VDUP.S32   d1,  r1
    ldr        r1,  =1108
    VMOV.S32   d0[0], r1
    ldr        r1,  =2676
    VMOV.S32   d0[1], r1                          @d0: W2 W6
    VMUL.S32   d0,  d0, d1                        @d0: x2 x6
    
    VADD.S32   d1,  d2, d0                        @d1: x0 x4
    VSUB.S32   d0,  d2, d0                        @d0: x2 x6
    VSHR.S32   Q0,  Q0, #8
    
    VMOV       r1,  r2, d0
    strh       r1,  [r0, #32]
    strh       r1,  [r0, #80]
    strh       r2,  [r0, #48]
    strh       r2,  [r0, #64]
    VMOV       r1,  r2, d1
    strh       r1,  [r0, #96]
    strh       r1,  [r0, #16]
    strh       r2,  [r0, #112]
    strh       r2,  [r0]
    
    ldmia      sp!, {r1 - r2, pc}
    @ENDP  @ |idct_col0x20|
    
    .section .text
    .global	idct_col0x10
idct_col0x10:
    stmdb      sp!, {r1 - r3, lr}
    ldrsh      r2,  [r0, #48]
    
    VMOV.S32   Q1,  #128                          @Q1: x0 x0 x0 x0
    ldr        r1,  =-2408
    mul        r3,  r2, r1
    VMOV.S32   d0[1], r3                          @r3: -x1
    ldr        r1,  =1609
    mul        r1,  r2, r1
    VMOV.S32   d0[0], r1                          @r1: x3;  d0: -x1 x3
    
    sub        r2,  r3, r1
    VMOV.S32   d4[0], r2                          @-x1-x3
    add        r2,  r1, r3
    VMOV.S32   d4[1], r2                          @x3-x1
    
    VMOV.S32   d1,  #128
    VMOV.S32   d5,  #181
    VMLA.S32   d1,  d4, d5                        @d1: x7 x5
    VSHR.S32   d1,  d1, #8
    
    VADD.S32   Q2,  Q1, Q0                        @Q2: (128+x7)(128+x5)(128-x1)(128+x3)
    VSUB.S32   Q1,  Q1, Q0                        @Q1: (128-x7)(128-x5)(128+x1)(128-x3)
    VSHR.S32   Q0,  Q2, #8
    VSHR.S32   Q1,  Q1, #8
    
    VMOV       r1,  r2, d0
    strh       r1,  [r0, #64]
    strh       r2,  [r0, #112]
    VMOV       r1,  r2, d1
    strh       r1,  [r0, #32]
    strh       r2,  [r0, #16]
    VMOV       r1,  r2, d2
    strh       r1,  [r0, #48]
    strh       r2,  [r0]
    VMOV       r1,  r2, d3
    strh       r1,  [r0, #80]
    strh       r2,  [r0, #96]
    
    ldmia      sp!, {r1 - r3, pc}
    @ENDP  @ |idct_col0x10|
    
    .section .text
    .global	idct_col
idct_col:
    stmdb      sp!, {r1 - r4, lr}
    ldrsh      r1,  [r0, #16]
    ldrsh      r2,  [r0, #112]
    VMOV.S32   d0, r1, r2                                                      @d0: x5 x4
    ldrsh      r1,  [r0, #80]
    ldrsh      r2,  [r0, #48]
    VMOV.S32   d1, r1, r2                                                      @d1: x7 x6
    
    ldrsh      r1,  [r0, #96]
    ldrsh      r2,  [r0, #32]
    VMOV.S32   d2, r1, r2                                                      @d2: x3 x2
    VMOV.S32   d3, d2                                                          @d3: x3 x2
    
    ldr        r3,  =2408
    ldr        r4,  =565
    VMOV.S32   d4, r4, r3                                                      @d4: W3W7
    ldr        r4,  =1108
    VDUP.S32   d5, r4                                                          @d5: W6W6
    
    ldr        r4,  =2841-565
    ldr        r3,  =-2841-565
    VMOV.S32   d6, r4, r3
    ldr        r4,  =1609-2408
    ldr        r3,  =-1609-2408
    VMOV.S32   d7, r4, r3                                                      @Q3:(-(W3+W5))(-(W3-W5))(-(W1+W7))(W1-W7)
    
    VPADD.S32  d8, d0, d1
    VPADD.S32  d9, d2, d3                                                      @Q4: (x3+x2)(x3+x2)(x6+x7)(x4+x5)
    VMUL.S32   Q2, Q4, Q2                                                      @Q2: x1 x1 x8h x8l
    VDUP.S32   d8, d4[0]
    VDUP.S32   d9, d4[1]
    VMLA.S32   Q4, Q0, Q3                                                      @Q4: x7x6x5x4
    
    ldr        r3,  =-2676-1108
    ldr        r4,  =2676-1108
    VMOV.S32   d3, r3, r4                                                     @d3: (W2-W6)(-(W2+W6))
    VMLA.S32   d5, d2, d3                                                     @d5: x3 x2
    
    
    ldrsh      r1,  [r0]
    ldrsh      r2,  [r0, #64]
    lsl        r1,  r1, #11
    add        r1,  r1, #128
    lsl        r2,  r2, #11
    add        r3,  r1, r2
    sub        r4,  r1, r2
    VMOV.S32   d2, r4, r3                                                      @d2: x8 x0
    
    VADD.S32   d0, d8, d9
    VSUB.S32   d1, d8, d9                                                      @Q0: x5x4x6x1
    
    VSUB.S32   d3, d2, d5
    VADD.S32   d2, d2, d5                                                      @Q1: x8x0x7x3
    
    VPADDL.S32 d4, d1
    VREV64.S32 d5, d1
    VSUB.S32   d5, d1, d5
    VZIP.S32   d4, d5
    
    VMOV.S32   d5, #181
    VMOV.S32   d1, #128
    VMLA.S32   d1, d5, d4                                                      @Q0: x4 x2 x6 x1
    VSHR.S32   d1, d1, #8
    VZIP.S32   d1, d0                                                          @Q0: x1 x2 x6 x4
    
    VADD.S32   d4, d1, d2
    VADD.S32   d5, d0, d3                                                      @Q2: (x8+x6)(x0+x4)(x7+x1)(x3+x2)
    VSHR.S32   Q2, Q2, #8
    VMOV       r1, r2, d4
    strh       r1, [r0, #16]
    strh       r2, [r0]
    VMOV       r1, r2, d5
    strh       r1, [r0, #32]
    strh       r2, [r0, #48]
    
    VSUB.S32   d4, d2, d1
    VSUB.S32   d5, d3, d0                                                      @Q2: (x8-x6)(x0-x4)(x7-x1)(x3-x2)
    VSHR.S32   Q2, Q2, #8
    VMOV       r1, r2, d4
    strh       r1, [r0, #96]
    strh       r2, [r0, #112]
    VMOV       r1, r2, d5
    strh       r1, [r0, #80]
    strh       r2, [r0, #64]
    
    ldmia      sp!, {r1 - r4, pc}
    @ENDP  @ |idct_col|
    
    .section .text
    .global	idct_row0Inter
idct_row0Inter:
    stmdb      sp!, {lr}
    ldmia      sp!, {pc}
    @ENDP  @ |idct_row0Inter|
    
    .section .text
    .global	idct_row1Inter
idct_row1Inter:
    stmdb      sp!, {r3 - r5, lr}
    mov        r5,  #0
    mov        r3,  #8                                                         @r3: i
idct_row1Inter_loop:
    ldrsh      r4,  [r0]                                                       @r4: *blk
    add        r4,  r4, #32
    asr        r4,  r4, #6                                                     @tmp = (*(blk += 8) + 32) >> 6;
    VDUP.16    Q1,  r4                                                         @Q1: (tmp...)
    strh       r5,  [r0], #16                                                  @*blk = 0;   post index change
    
    VLD1.8     d0,  [r1]
    VADDW.U8   Q1,  Q1,  d0
    
    VMOV.S16   Q2,  #0xff
    VMOV.I8    Q4,  #0xff
    VSHR.S16   Q3,  Q1,  #15
    VEOR       Q3,  Q3,  Q4
    VAND       Q3,  Q3,  Q2
    VCGT.U16   Q2,  Q1,  Q2
    
    VUZP.8     d2,  d3
    VUZP.8     d4,  d5
    VUZP.8     d6,  d7
    VBIT.8     d2,  d6,  d4
    
    VST1.32    {d2}, [r1], r2
    @add        r1,  r1, r2
    @add        r0,  r0, #16
    subs       r3,  #1
    bgt        idct_row1Inter_loop
    
    ldmia      sp!, {r3 - r5, pc}
    @ENDP  @ |idct_row1Inter|
    
    
    
    .section .text
    .global	idct_row2Inter
idct_row2Inter:
    stmdb      sp!, {r3 - r6, lr}
    mov        r6,  #2816
    add        r6,  r6, #25
    VMOV.S32   d4[0], r6
    mov        r6,  #512
    add        r6,  r6, #53
    VMOV.S32   d4[1], r6                                                       @d4: W7W1
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r6,  #0
    mov        r3,  #8                                                         @r3: i
idct_row2Inter_loop:
    ldrsh      r4,  [r0]
    ldrsh      r5,  [r0, #2]                                                   @r5: x4 = blk[9];
    lsl        r4,  r4, #8
    add        r4,  r4, #8192                                                  @r4: x0 = ((*(blk += 8)) << 8) + 8192;
    str        r6,  [r0], #16
    
    VDUP.S32   Q0,  r4                                                         @Q0: x0 x0 x0 x0
    VDUP.S32   d2,  r5                                                         @d2: x4 x4
    
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d4
    VSHR.S32   d2,  d3,  #3                                                    @d2:x5 = (W7 * x4 + 4) >> 3;x4 = (W1 * x4 + 4) >> 3;
    
    VPADDL.S32 d3,  d2                                                         @(x4 + x5)
    VREV64.S32 d5,  d2                                                         @d5: x4 x5
    VSUB.S32   d5,  d2, d5
    VZIP.S32   d3,  d5                                                         @d3:(x4 - x5)(x4 + x5)
    
    VMOV.S32   d5,  #181
    VMOV.S32   d6,  #128
    VMLA.S32   d6,  d3, d5
    VSHR.S32   d3,  d6, #8                                                     @d3: x1 x2; Q1:x1 x2 x5 x4
    
    VADD.S32   Q3,  Q0,  Q1
    VSHR.S32   Q3,  Q3,  #14                                                   @Q3: x1 x2 x5 x4
    VSUB.S32   Q0,  Q0,  Q1
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: -x1 -x2 -x5 -x4
    
    VREV64.S32 d0,  d0                                                         @d0: -x4 -x5 
    VZIP.S32   d6,  d0                                                         @d6: -x5 x4; d0: -x4 x5
    VREV64.S32 d1,  d1                                                         @d1: -x2 -x1
    VZIP.S32   d7,  d1                                                         @d7: -x1 x2; d1: -x2 x1
    
    VLD1.8     d2,  [r1]
    
    VAND.S32   d3,  d2, d10
    VADD.S32   d6,  d6, d3                                                     @low: -x5 x4
    VCGT.U32   d3,  d6, d10
    VSHR.S32   d8,  d6, #31
    VEOR       d8,  d8, d9
    VBIT.32    d6,  d8, d3
    
    VSHR.S32   d3,  d2, #8
    VAND.S32   d3,  d3, d10                                                    @ -x1 x2
    VADD.S32   d7,  d7, d3
    VCGT.U32   d3,  d7, d10
    VSHR.S32   d8,  d7, #31
    VEOR       d8,  d8, d9
    VBIT.32    d7,  d8, d3
    
    VSHR.S32   d3,  d2, #16
    VAND.S32   d3,  d3, d10                                                    @-x2 x1
    VADD.S32   d1,  d1, d3
    VCGT.U32   d3,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d3
    
    VSHR.S32   d3,  d2, #24
    VAND.S32   d3,  d3, d10                                                    @-x4 x5
    VADD.S32   d0,  d0, d3
    VCGT.U32   d3,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d3
    
    VZIP.I8    d6,  d7                                                         @d6:...x2x4; d7:...-x1 -x5 
    VZIP.I8    d1,  d0                                                         @d1:...x5x1; d0:...-x4 -x2 
    VZIP.I16   d6,  d1                                                         @d6:...x5x1x2x4
    VZIP.I16   d7,  d0                                                         @d7:...-x4 -x2 -x1 -x5
    VZIP.I32   d6,  d7
    
    VST1.32    d6,  [r1], r2
    
    @add        r0,  r0, #16
    @add        r1,  r1, r2
    subs       r3,  r3, #1
    bgt        idct_row2Inter_loop
    
    ldmia      sp!, {r3 - r6, pc}
    @ENDP  @ |idct_row2Inter|
    
    
    
    .section .text
    .global	idct_row3Inter
idct_row3Inter:
    stmdb      sp!, {r3 - r7, lr}
    mov        r7,  #2816
    add        r7,  r7, #25
    VMOV.S32   d4[1], r7
    mov        r7,  #512
    add        r7,  r7, #53
    VMOV.S32   d4[0], r7                                                       @d4: W1W7
    
    mov        r7,  #1088
    add        r7,  r7, #20
    VMOV.S32   d5[0], r7
    mov        r7,  #2624
    add        r7,  r7, #52
    VMOV.S32   d5[1], r7                                                       @d5: W2W6
    
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r7,  #0
    mov        r3,  #8                                                         @r3: i
idct_row3Inter_loop:
    ldrsh      r4,  [r0]
    ldrsh      r5,  [r0, #2]                                                   @r5: x1 = blk[9];
    ldrsh      r6,  [r0, #4]                                                   @r6: x2 = blk[10];
    
    lsl        r4,  r4, #8
    add        r4,  r4, #8192                                                  @r4: x0 = ((*(blk += 8)) << 8) + 8192;
    strh       r7,  [r0, #2]
    strh       r7,  [r0, #4]
    strh       r7,  [r0], #16
    
    VDUP.S32   d0,  r5                                                         @d0: x1 x1
    VDUP.S32   d1,  r6                                                         @d1: x2 x2
    
    VMOV.S32   Q1,  #4
    VMLA.S32   Q1,  Q0,  Q2
    VSHR.S32   Q0,  Q1,  #3                                                    @Q0: x2 x6 x1 x7
    
    VDUP.S32   d2,  r4                                                         @d2: x0 x0
    VSUB.S32   d7,  d2, d1                                                     @d7: x2x6   (x0-x2)(x4-x6)
    VADD.S32   d6,  d2, d1                                                     @d6: x0x4   (x0+x2)(x4+x6)
    
    VPADDL.S32 d1,  d0                                                         @d0: x1 x3; d1: ..(x7 + x1)
    VREV64.S32 d3,  d0                                                         @d3: x7 x1
    VSUB.S32   d3,  d3, d0
    VZIP.S32   d3,  d1                                                         @d3:(x1 + x7)(x1 - x7)
    
    VMOV.S32   d2,  #181
    VMOV.S32   d1,  #128
    VMLA.S32   d1,  d2, d3
    VSHR.S32   d1,  d1, #8                                                     @d1: x7 x5; Q1:x7 x5 x1 x3
    VZIP.S32   d1,  d0                                                         @d0: x1 x7; d1: x3 x5
    
    VADD.S32   Q1,  Q3,  Q0
    VSHR.S32   Q1,  Q1,  #14                                                   @Q1: (x2+x3)(x6+x5)(x0+x1)(x4+x7)
    VSUB.S32   Q0,  Q3,  Q0
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: (x2-x3)(x6-x5)(x0-x1)(x4-x7)
    
    VZIP.S32   d3,  d0                                                         @d3: (x4-x7)(x6+x5); d0: (x0-x1)(x2+x3)
    VZIP.S32   d2,  d1                                                         @d2: (x6-x5)(x4+x7); d1: (x2-x3)(x0+x1)
    
    VLD1.8     d6,  [r1]
    
    VAND.S32   d7,  d6, d10
    VADD.S32   d1,  d7, d1                                                     @d1: low: (x2-x3)(x0+x1)
    VCGT.U32   d7,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d7
    
    VSHR.S32   d7,  d6, #8
    VAND.S32   d7,  d7, d10                                                    @d2: (x6-x5)(x4+x7)
    VADD.S32   d2,  d7, d2
    VCGT.U32   d7,  d2, d10
    VSHR.S32   d8,  d2, #31
    VEOR       d8,  d8, d9
    VBIT.32    d2,  d8, d7
    
    VSHR.S32   d7,  d6, #16
    VAND.S32   d7,  d7, d10                                                    @d3: (x4-x7)(x6+x5)
    VADD.S32   d3,  d3, d7
    VCGT.U32   d7,  d3, d10
    VSHR.S32   d8,  d3, #31
    VEOR       d8,  d8, d9
    VBIT.32    d3,  d8, d7
    
    VSHR.S32   d7,  d6, #24
    VAND.S32   d7,  d7, d10                                                    @d0: (x0-x1)(x2+x3)
    VADD.S32   d0,  d0, d7
    VCGT.U32   d7,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d7
    
    VZIP.I8    d1,  d2                                                         @d1: (x4+x7)(x0+x1); d2: (x6-x5)(x2-x3)
    VZIP.I8    d3,  d0                                                         @d3:(x2+x3)(x6+x5); d0:(x0-x1)(x4-x7)
    VZIP.I16   d1,  d3                                                         @d1:(x2+x3)(x6+x5)(x4+x7)(x0+x1)
    VZIP.I16   d2,  d0                                                         @d2:(x0-x1)(x4-x7)(x6-x5)(x2-x3)
    VZIP.I32   d1,  d2
    
    VST1.32    d1,  [r1], r2
    
    @add        r0,  r0, #16
    @add        r1,  r1, r2
    subs       r3,  r3, #1
    bgt        idct_row3Inter_loop
    
    ldmia      sp!, {r3 - r7, pc}
    @ENDP  @ |idct_row3Inter|
    
    .section .text
    .global	idct_row4Inter
idct_row4Inter:
    stmdb      sp!, {r3 - r7, lr}
    mov        r7,  #2816
    add        r7,  r7, #25
    VMOV.S32   d4[1], r7
    mov        r7,  #512
    add        r7,  r7, #53
    VMOV.S32   d4[0], r7                                                       @d4: W1W7
    
    mov        r7,  #1088
    add        r7,  r7, #20
    VMOV.S32   d5[0], r7
    mov        r7,  #2624
    add        r7,  r7, #52
    VMOV.S32   d5[1], r7                                                       @d5: W2W6
    
    mov        r7,  #2400
    add        r7,  r7, #8
    VMOV.S32   d12[1], r7
    mov        r7,  #7
    sub        r7,  r7, #1616
    VMOV.S32   d12[0], r7                                                      @d12: W3(-W5)
    
    VMOV.I16   d11,  #0
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r3,  #8                                                         @r3: i
idct_row4Inter_loop:
    ldrsh      r4,  [r0]
    ldrsh      r5,  [r0, #2]                                                   @r5: x1 = blk[9];
    ldrsh      r6,  [r0, #4]                                                   @r6: x2 = blk[10];
    ldrsh      r7,  [r0, #6]                                                   @r7: x3
    
    lsl        r4,  r4, #8
    add        r4,  r4, #8192                                                  @r4: x0 = ((*(blk += 8)) << 8) + 8192;
    
    VST1.16    d11,  [r0]
    
    VDUP.S32   d0,  r5                                                         @d0: x1 x1
    VDUP.S32   d1,  r6                                                         @d1: x2 x2
    
    VMOV.S32   Q1,  #4
    VMLA.S32   Q1,  Q0,  Q2
    VSHR.S32   Q0,  Q1,  #3                                                    @Q0: x2 x6 x1 x7
    
    VDUP.S32   d2,  r4                                                         @d2: x0 x0
    VSUB.S32   d7,  d2, d1                                                     @d7: x2x6   (x0-x2)(x4-x6)
    VADD.S32   d6,  d2, d1                                                     @d6: x0x4   (x0+x2)(x4+x6)
    
    VDUP.S32   d2,  r7                                                         @d2: x3x3
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d12
    VSHR.S32   d2,  d3,  #3                                                    @d2: x5x3
    VSUB.S32   d1,  d0,  d2                                                    @d1: x5x8
    VADD.S32   d0,  d0,  d2                                                    @d0: x1x3
    
    
    VPADDL.S32 d2,  d1                                                         @d2: ..(x5 + x8)
    VREV64.S32 d3,  d1                                                         @d3: x8 x5
    VSUB.S32   d3,  d3, d1
    VZIP.S32   d3,  d2                                                         @d3:(x5 + x8)(x5 - x8)
    
    VMOV.S32   d2,  #181
    VMOV.S32   d1,  #128
    VMLA.S32   d1,  d2, d3
    VSHR.S32   d1,  d1, #8                                                     @d1: x7 x5; Q1:x7 x5 x1 x3
    VZIP.S32   d1,  d0                                                         @d0: x1 x7; d1: x3 x5
    
    VADD.S32   Q1,  Q3,  Q0
    VSHR.S32   Q1,  Q1,  #14                                                   @Q1: (x2+x3)(x6+x5)(x0+x1)(x4+x7)
    VSUB.S32   Q0,  Q3,  Q0
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: (x2-x3)(x6-x5)(x0-x1)(x4-x7)
    
    VZIP.S32   d3,  d0                                                         @d3: (x4-x7)(x6+x5); d0: (x0-x1)(x2+x3)
    VZIP.S32   d2,  d1                                                         @d2: (x6-x5)(x4+x7); d1: (x2-x3)(x0+x1)
    
    VLD1.8     d6,  [r1]
    
    VAND.S32   d7,  d6, d10
    VADD.S32   d1,  d7, d1                                                     @d1: low: (x2-x3)(x0+x1)
    VCGT.U32   d7,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d7
    
    VSHR.S32   d7,  d6, #8
    VAND.S32   d7,  d7, d10                                                    @d2: (x6-x5)(x4+x7)
    VADD.S32   d2,  d7, d2
    VCGT.U32   d7,  d2, d10
    VSHR.S32   d8,  d2, #31
    VEOR       d8,  d8, d9
    VBIT.32    d2,  d8, d7
    
    VSHR.S32   d7,  d6, #16
    VAND.S32   d7,  d7, d10                                                    @d3: (x4-x7)(x6+x5)
    VADD.S32   d3,  d3, d7
    VCGT.U32   d7,  d3, d10
    VSHR.S32   d8,  d3, #31
    VEOR       d8,  d8, d9
    VBIT.32    d3,  d8, d7
    
    VSHR.S32   d7,  d6, #24
    VAND.S32   d7,  d7, d10                                                    @d0: (x0-x1)(x2+x3)
    VADD.S32   d0,  d0, d7
    VCGT.U32   d7,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d7
    
    VZIP.I8    d1,  d2                                                         @d1: (x4+x7)(x0+x1); d2: (x6-x5)(x2-x3)
    VZIP.I8    d3,  d0                                                         @d3:(x2+x3)(x6+x5); d0:(x0-x1)(x4-x7)
    VZIP.I16   d1,  d3                                                         @d1:(x2+x3)(x6+x5)(x4+x7)(x0+x1)
    VZIP.I16   d2,  d0                                                         @d2:(x0-x1)(x4-x7)(x6-x5)(x2-x3)
    VZIP.I32   d1,  d2
    
    VST1.32    d1,  [r1], r2
    
    add        r0,  r0, #16
    @add        r1,  r1, r2
    subs       r3,  r3, #1
    bgt        idct_row4Inter_loop
    
    ldmia      sp!, {r3 - r7, pc}
    @ENDP  @ |idct_row4Inter|
    
    .section .text
    .global	idct_row0x40Inter
idct_row0x40Inter:
    stmdb      sp!, {r3 - r5, lr}
    ldr        r4,  =565
    ldr        r5,  =2841
    VMOV.S32   d4, r5, r4                                                      @d4: W7W1
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    VMOV.S32   Q6,  #8192                                                      @Q0: x0 x0 x0 x0
    mov        r5,  #0
    mov        r3,  #8                                                         @r3: i
idct_row0x40Inter_loop:
    ldrsh      r4,  [r0, #2]!                                                  @r4: x4 = blk[9];
    VDUP.S32   d2,  r4                                                         @d2: x4 x4
    strh       r5,  [r0], #14
    
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d4
    VSHR.S32   d2,  d3,  #3                                                    @d2:x5 = (W7 * x4 + 4) >> 3;x4 = (W1 * x4 + 4) >> 3;
    
    VPADDL.S32 d3,  d2                                                         @(x4 + x5)
    VREV64.S32 d5,  d2                                                         @d5: x4 x5
    VSUB.S32   d5,  d2, d5
    VZIP.S32   d3,  d5                                                         @d3:(x4 - x5)(x4 + x5)
    
    VMOV.S32   d5,  #181
    VMOV.S32   d6,  #128
    VMLA.S32   d6,  d3, d5
    VSHR.S32   d3,  d6, #8                                                     @d3: x1 x2; Q1:x1 x2 x5 x4
    
    VADD.S32   Q3,  Q6,  Q1
    VSHR.S32   Q3,  Q3,  #14                                                   @Q3: x1 x2 x5 x4
    VSUB.S32   Q1,  Q6,  Q1
    VSHR.S32   Q0,  Q1,  #14                                                   @Q0: -x1 -x2 -x5 -x4
    
    VREV64.S32 d0,  d0                                                         @d0: -x4 -x5 
    VZIP.S32   d6,  d0                                                         @d6: -x5 x4; d0: -x4 x5
    VREV64.S32 d1,  d1                                                         @d1: -x2 -x1
    VZIP.S32   d7,  d1                                                         @d7: -x1 x2; d1: -x2 x1
    
    VLD1.8     d2,  [r1]
    
    VAND.S32   d3,  d2, d10
    VADD.S32   d6,  d6, d3                                                     @low: -x5 x4
    VCGT.U32   d3,  d6, d10
    VSHR.S32   d8,  d6, #31
    VEOR       d8,  d8, d9
    VBIT.32    d6,  d8, d3
    
    VSHR.S32   d3,  d2, #8
    VAND.S32   d3,  d3, d10                                                    @ -x1 x2
    VADD.S32   d7,  d7, d3
    VCGT.U32   d3,  d7, d10
    VSHR.S32   d8,  d7, #31
    VEOR       d8,  d8, d9
    VBIT.32    d7,  d8, d3
    
    VSHR.S32   d3,  d2, #16
    VAND.S32   d3,  d3, d10                                                    @-x2 x1
    VADD.S32   d1,  d1, d3
    VCGT.U32   d3,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d3
    
    VSHR.S32   d3,  d2, #24
    VAND.S32   d3,  d3, d10                                                    @-x4 x5
    VADD.S32   d0,  d0, d3
    VCGT.U32   d3,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d3
    
    VZIP.I8    d6,  d7                                                         @d6:...x2x4; d7:...-x1 -x5 
    VZIP.I8    d1,  d0                                                         @d1:...x5x1; d0:...-x4 -x2 
    VZIP.I16   d6,  d1                                                         @d6:...x5x1x2x4
    VZIP.I16   d7,  d0                                                         @d7:...-x4 -x2 -x1 -x5
    VZIP.I32   d6,  d7
    
    VST1.32    d6,  [r1], r2
    
    subs       r3,  r3, #1
    bgt        idct_row0x40Inter_loop
    
    ldmia      sp!, {r3 - r5, pc}
    @ENDP  @ |idct_row0x40Inter|
    
    .section .text
    .global	idct_row0x20Inter
idct_row0x20Inter:
    stmdb      sp!, {r3 - r5, lr}
    mov        r4,  #1088
    add        r4,  r4, #20
    VMOV.S32   d4[1], r4
    mov        r4,  #2624
    add        r4,  r4, #52
    VMOV.S32   d4[0], r4                                                       @d4: W6W2
    
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    VMOV.S32   d8,   #8192                                                     @d8: 8192
    
    mov        r4,  #0
    mov        r3,  #8                                                         @r3: i
idct_row0x20Inter_loop:
    ldrsh      r5,  [r0, #4]!                                                  @r5: x2 = blk[2];
    strh       r4,  [r0], #12                                                  @blk[2] = 0;
    
    VDUP.S32   d2,  r5                                                         @d2: x2 x2
    
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d4
    VSHR.S32   d2,  d3,  #3                                                    @d2:x6 = (W6 * x2 + 4) >> 3;x2 = (W2 * x2 + 4) >> 3;
    
    VADD.S32   d0,  d2,  d8                                                    @d0: x4 x0
    VSUB.S32   d1,  d8,  d2                                                    @d1: x6 x2
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: x6 x2 x4 x0
    VZIP.S32   d0,  d1                                                         @d0: x2 x0; d1: x6 x4
    
    VLD1.8     d2,  [r1]
    
    VAND.S32   d6,  d2, d10
    VADD.S32   d6,  d0, d6                                                     @low:d6: x2 x0
    VCGT.U32   d3,  d6, d10
    VSHR.S32   d5,  d6, #31
    VEOR       d5,  d5, d9
    VBIT.32    d6,  d5, d3
    
    VSHR.S32   d7,  d2, #8
    VAND.S32   d7,  d7, d10
    VADD.S32   d7,  d7, d1                                                     @d7: x6 x4
    VCGT.U32   d3,  d7, d10
    VSHR.S32   d5,  d7, #31
    VEOR       d5,  d5, d9
    VBIT.32    d7,  d5, d3
    
    VREV64.S32 d1,  d1                                                         @d1:x4 x6
    VREV64.S32 d0,  d0                                                         @d0:x0 x2
    
    VSHR.S32   d5,  d2, #16
    VAND.S32   d5,  d5, d10
    VADD.S32   d1,  d1, d5                                                     @d1:x4 x6
    VCGT.U32   d3,  d1, d10
    VSHR.S32   d5,  d1, #31
    VEOR       d5,  d5, d9
    VBIT.32    d1,  d5, d3
    
    VSHR.S32   d5,  d2, #24
    VAND.S32   d5,  d5, d10                                                    @d0:x0 x2
    VADD.S32   d0,  d0, d5
    VCGT.U32   d3,  d0, d10
    VSHR.S32   d5,  d0, #31
    VEOR       d5,  d5, d9
    VBIT.32    d0,  d5, d3
    
    VZIP.I8    d6,  d7                                                         @d6:......x4x0;d7:......x6x2
    VZIP.I8    d1,  d0                                                         @d1:......x2x6;d0:......x0x4
    VZIP.I16   d6,  d1                                                         @d6:....x2x6x4x0
    VZIP.I16   d7,  d0                                                         @d7:....x0x4x6x2
    VZIP.I32   d6,  d7
    
    VST1.32    d6,  [r1], r2
    
    subs       r3,  r3, #1
    bgt        idct_row0x20Inter_loop
    
    ldmia      sp!, {r3 - r5, pc}
    @ENDP  @ |idct_row0x20Inter|
    
    .section .text
    .global	idct_row0x10Inter
idct_row0x10Inter:
    stmdb      sp!, {r3 - r5, lr}
    mov        r4,  #2400
    add        r4,  r4, #8
    VMOV.S32   d4[0], r4
    mov        r4,  #7
    sub        r4,  r4, #1616
    VMOV.S32   d4[1], r4                                                       @d4: (-W5)W3
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r4,  #-181
    VMOV.S32   d11,  #181
    VMOV.S32   d11[0], r4
    
    VMOV.S32   Q6,  #8192                                                      @Q0: x0 x0 x0 x0
    mov        r5,  #0
    mov        r3,  #8                                                         @r3: i
idct_row0x10Inter_loop:
    ldrsh      r4,  [r0, #6]!                                                  @r4: x3 = blk[3];
    VDUP.S32   d2,  r4                                                         @d2: x3 x3
    strh       r5,  [r0], #10
    
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d4
    VSHR.S32   d2,  d3,  #3                                                    @d2:x3 = (-W5 * x3 + 4) >> 3;x1 = (W3 * x3 + 4) >> 3;
    
    VPADDL.S32 d3,  d2                                                         @d3:(x3 + x1)
    VREV64.S32 d5,  d2                                                         @d5: x1 x3
    VSUB.S32   d5,  d5, d2                                                     @d5:(x3 - x1)
    VZIP.S32   d3,  d5                                                         @d3:(x3 - x1)(x3 + x1)
    
    VMOV.S32   d6,  #128
    VMLA.S32   d6,  d3, d11
    VSHR.S32   d3,  d6, #8                                                     @d3: x5 x7; Q1:x5 x7 x3 x1
    
    VADD.S32   Q3,  Q6,  Q1
    VSHR.S32   Q3,  Q3,  #14                                                   @Q3: x5 x7 x3 x1
    VSUB.S32   Q1,  Q6,  Q1
    VSHR.S32   Q0,  Q1,  #14                                                   @Q0: -x5 -x7 -x3 -x1
    
    VREV64.S32 d0,  d0                                                         @d0: -x1 -x3 
    VZIP.S32   d6,  d0                                                         @d6: -x3 x1; d0: -x1 x3
    VREV64.S32 d1,  d1                                                         @d1: -x7 -x5
    VZIP.S32   d7,  d1                                                         @d7: -x5 x7; d1: -x7 x5
    
    VLD1.8     d2,  [r1]
    
    VAND.S32   d3,  d2, d10
    VADD.S32   d6,  d6, d3                                                     @low: -x3 x1
    VCGT.U32   d3,  d6, d10
    VSHR.S32   d8,  d6, #31
    VEOR       d8,  d8, d9
    VBIT.32    d6,  d8, d3
    
    VSHR.S32   d3,  d2, #8
    VAND.S32   d3,  d3, d10                                                    @ -x5 x7
    VADD.S32   d7,  d7, d3
    VCGT.U32   d3,  d7, d10
    VSHR.S32   d8,  d7, #31
    VEOR       d8,  d8, d9
    VBIT.32    d7,  d8, d3
    
    VSHR.S32   d3,  d2, #16
    VAND.S32   d3,  d3, d10                                                    @-x7 x5
    VADD.S32   d1,  d1, d3
    VCGT.U32   d3,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d3
    
    VSHR.S32   d3,  d2, #24
    VAND.S32   d3,  d3, d10                                                    @-x1 x3
    VADD.S32   d0,  d0, d3
    VCGT.U32   d3,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d3
    
    VZIP.I8    d6,  d7                                                         @d6:...x7x1; d7:...-x5 -x3 
    VZIP.I8    d1,  d0                                                         @d1:...x3x5; d0:...-x1 -x7 
    VZIP.I16   d6,  d1                                                         @d6:...x3x5x7x1
    VZIP.I16   d7,  d0                                                         @d7:...-x1 -x7 -x5 -x3
    VZIP.I32   d6,  d7
    
    VST1.32    d6,  [r1], r2
    
    @add        r0,  r0, #16
    @add        r1,  r1, r2
    subs       r3,  r3, #1
    bgt        idct_row0x10Inter_loop
    
    ldmia      sp!, {r3 - r5, pc}
    @ENDP  @ |idct_row0x10Inter|
    
    
    .section .text
    .global	idct_rowInter
idct_rowInter:
    stmdb      sp!, {r3 - r4, lr}
    ldr        r4,  =2408
    VMOV.S32   d4[1], r4
    ldr        r4,  =565
    VMOV.S32   d4[0], r4                                                       @d4: W3W7
    ldr        r4,  =1108
    VDUP.S32   d5, r4                                                          @d5: W6W6
    
    ldr        r4,  =2841-565
    ldr        r3,  =-2841-565
    orr        r4,  r4, r3, lsl #16
    VMOV.S32   d12[0], r4
    ldr        r4,  =1609-2408
    lsl        r4,  r4, #16
    lsr        r4,  r4, #16
    ldr        r3,  =-1609-2408
    orr        r4,  r4, r3, lsl #16
    VMOV.S32   d12[1], r4                                                      @d12:(-(W3+W5))(-(W3-W5))(-(W1+W7))(W1-W7)
    
    ldr        r4,  =-2676-1108
    VMOV.S32   d13[0], r4
    ldr        r4,  =2676-1108
    VMOV.S32   d13[1], r4                                                      @d13: (W2-W6)(-(W2+W6))
    
    VMOV.S32   d10,  #0xff
    VMOV.I8    d11,  #0xff
    
    VMOV.I16   Q7,  #0
    mov        r4,  #16
    mov        r3,  #8                                                         @r3: i
idct_rowInter_loop:
    VLD1.16    {d0, d1},   [r0]                                                @d0: x7x3x4(x0);d1: x5x2x6(x1)
    VST1.16    {d14, d15}, [r0], r4
    
    
    VADDL.S16  Q1,  d0,  d1                                                    @Q1: .(x3+x2).((x0)+(x1))
    VSUBL.S16  Q4,  d0,  d1                                                    @Q4: ...((x0)-(x1))
    VZIP.S32   d8,  d2                                                         @d8:(x0)+(x1))((x0)-(x1))
    VSHL.S32   d2,  d8,  #8
    VMOV.S32   d8,  #8192
    VADD.S32   d2,  d2,  d8                                                    @d2: x8 x0
    
    VREV64.S32 d0,  d0                                                         @d0: x4(x0)x7x3
    VADDL.S16  Q4,  d0,  d1                                                    @Q4: (x4+x5).(x7+x6).
    VZIP.S32   d9,  d8                                                         @d8: (x7+x6)(x4+x5)
    VDUP.S32   d9,  d3[0]                                                      @d9: (x3+x2)(x3+x2)
    
    VMOV.S32   Q3,  #4
    VMLA.S32   Q3,  Q4,  Q2                                                    @Q3: x1x1x8x8
    
    VZIP.S16   d0,  d1                                                         @d0: x6x7(x1)x3; d1:x5x4x2(x0)
    VREV32.S16 d0,  d0                                                         @d0: x7x6x3(x1)
    VZIP.S32   d1,  d0                                                         @d1: x3(x1)x2(x0); d0: x7x6x5x4
    VSHR.S32   d1,  d1,  #16                                                   @d1: x3x2
    VMLA.S32   d7,  d1, d13
    VSHR.S32   d7,  d7, #3                                                     @d7:x3x2
    
    VMULL.S16  Q4,  d0, d12                                                    @Q4: x7x6x5x4
    VDUP.S32   d1,  d6[0]                                                      @d8:x5x4 + d6[0];d9:x7x6 + d6[1]
    VADD.S32   d8,  d8, d1
    VDUP.S32   d1,  d6[1]
    VADD.S32   d9,  d9, d1
    VSHR.S32   Q4,  Q4, #3                                                     @Q4:x7x6x5x4
    
    VADD.S32   d0,  d8, d9                                                     @d0: x6x1
    VSUB.S32   d1,  d8, d9                                                     @d1: x5x4
    
    VADD.S32   d3,  d2, d7                                                     @d3: x7x3
    VSUB.S32   d2,  d2, d7                                                     @d2: x8x0
    
    VPADDL.S32 d6,  d1                                                         @d6: ..(x5+x4)
    VREV64.S32 d7,  d1                                                         @d7: x4x5
    VSUB.S32   d1,  d1, d7
    VZIP.S32   d6,  d1                                                         @d6:(x4 - x5)(x4 + x5)
    
    VMOV.S32   d7,  #181
    VMOV.S32   d1,  #128
    VMLA.S32   d1,  d6, d7
    VSHR.S32   d1,  d1, #8                                                     @d1: x4 x2 Q0: x4x2x6x1;  Q1:x7x3x8x0
    VZIP.S32   d1,  d0                                                         @d0: x6x4; d1: x1x2;      Q0:x1x2x6x4
    
    VADD.S32   Q3,  Q1,  Q0
    VSUB.S32   Q0,  Q1,  Q0
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: (x7-x1)(x3-x2)(x8-x6)(x0-x4)
    VSHR.S32   Q1,  Q3,  #14                                                   @Q1: (x7+x1)(x3+x2)(x8+x6)(x0+x4)
    
    
    VZIP.S32   d3,  d0                                                         @d3: (x0-x4)(x3+x2); d0: (x8-x6)((x7+x1)
    VZIP.S32   d2,  d1                                                         @d2: (x3-x2)(x0+x4); d1: (x7-x1)(x8+x6)
    
    VLD1.8     d6,  [r1]
    
    VAND.S32   d7,  d6, d10
    VADD.S32   d0,  d7, d0                                                     @d0: low: (x8-x6)(x7+x1)
    VCGT.U32   d7,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d11
    VBIT.32    d0,  d8, d7
    
    VSHR.S32   d7,  d6, #8
    VAND.S32   d7,  d7, d10                                                    @d3: (x0-x4)(x3+x2)
    VADD.S32   d3,  d7, d3
    VCGT.U32   d7,  d3, d10
    VSHR.S32   d8,  d3, #31
    VEOR       d8,  d8, d11
    VBIT.32    d3,  d8, d7
    
    VSHR.S32   d7,  d6, #16
    VAND.S32   d7,  d7, d10                                                    @d2: (x3-x2)(x0+x4)
    VADD.S32   d2,  d2, d7
    VCGT.U32   d7,  d2, d10
    VSHR.S32   d8,  d2, #31
    VEOR       d8,  d8, d11
    VBIT.32    d2,  d8, d7
    
    VSHR.S32   d7,  d6, #24
    VAND.S32   d7,  d7, d10                                                    @d1: (x7-x1)(x8+x6)
    VADD.S32   d1,  d1, d7
    VCGT.U32   d7,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d11
    VBIT.32    d1,  d8, d7
    
    VZIP.I8    d0,  d3                                                         @d0:(x3+x2)(x7+x1); d3:(x0-x4)(x8-x6)
    VZIP.I8    d2,  d1                                                         @d2:(x8+x6)(x0+x4); d1:(x7-x1)(x3-x2)
    VZIP.I16   d0,  d2                                                         @d0:(x8+x6)(x0+x4)(x3+x2)(x7+x1)
    VZIP.I16   d3,  d1                                                         @d3:(x7-x1)(x3-x2)(x0-x4)(x8-x6)
    VZIP.I32   d0,  d3
    
    VST1.32    d0,  [r1], r2
    
    @add        r1,  r1, r2
    @add        r0,  r0, #16
    subs       r3,  r3, #1
    bgt        idct_rowInter_loop
    
    ldmia      sp!, {r3 - r4, pc}
    @ENDP  @ |idct_rowInter|
    
    
    
    .ltorg
    
    
    
            
    .section .text
    .global	idct_row0Intra
idct_row0Intra:
    stmdb      sp!, {lr}
    ldmia      sp!, {pc}
    @ENDP  @ |idct_row0Intra|
    
    .section .text
    .global	idct_row1Intra
idct_row1Intra:
    stmdb      sp!, {r3 - r5, lr}
    mov        r3,  #8
idct_row1Intra_loop:
    ldrsh      r4,  [r0]
    add        r4,  r4, #32
    asr        r4,  r4, #6
    mov        r5,  #0
    strh       r5,  [r0], #16
    
    cmp        r4,  #0xff
    bls        idct_row1Intra_goon
    mvn        r4,  r4, asr #31
    and        r4,  r4, #0xff
idct_row1Intra_goon:
    VDUP.U8     d0,  r4
    VST1.8     {d0},  [r1], r2
    
    @add        r0,  r0, #16
    @add        r1,  r1, r2
    subs       r3,  #1
    bgt        idct_row1Intra_loop
    ldmia      sp!, {r3 - r5, pc}
    @ENDP  @ |idct_row1Intra|
    
    .section .text
    .global	idct_row2Intra
idct_row2Intra:
    stmdb      sp!, {r3 - r6, lr}
    mov        r6,  #2816
    add        r6,  r6, #25
    VMOV.S32   d4[0], r6
    mov        r6,  #512
    add        r6,  r6, #53
    VMOV.S32   d4[1], r6                                                       @d4: W7W1
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r6,  #0
    mov        r3,  #8                                                         @r3: i
idct_row2Intra_loop:
    ldrsh      r4,  [r0]
    ldrsh      r5,  [r0, #2]                                                   @r5: x4 = blk[9];
    lsl        r4,  r4, #8
    add        r4,  r4, #8192                                                  @r4: x0 = ((*(blk += 8)) << 8) + 8192;
    str        r6,  [r0], #16
    
    VDUP.S32   Q0,  r4                                                         @Q0: x0 x0 x0 x0
    VDUP.S32   d2,  r5                                                         @d2: x4 x4
    
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d4
    VSHR.S32   d2,  d3,  #3                                                    @d2:x5 = (W7 * x4 + 4) >> 3;x4 = (W1 * x4 + 4) >> 3;
    
    VPADDL.S32 d3,  d2                                                         @(x4 + x5)
    VREV64.S32 d5,  d2                                                         @d5: x4 x5
    VSUB.S32   d5,  d2, d5
    VZIP.S32   d3,  d5                                                         @d3:(x4 - x5)(x4 + x5)
    
    VMOV.S32   d5,  #181
    VMOV.S32   d6,  #128
    VMLA.S32   d6,  d3, d5
    VSHR.S32   d3,  d6, #8                                                     @d3: x1 x2; Q1:x1 x2 x5 x4
    
    VADD.S32   Q3,  Q0,  Q1
    VSHR.S32   Q3,  Q3,  #14                                                   @Q3: x1 x2 x5 x4
    VSUB.S32   Q0,  Q0,  Q1
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: -x1 -x2 -x5 -x4
    
    VREV64.S32 d0,  d0                                                         @d0: -x4 -x5 
    VZIP.S32   d6,  d0                                                         @d6: -x5 x4; d0: -x4 x5
    VREV64.S32 d1,  d1                                                         @d1: -x2 -x1
    VZIP.S32   d7,  d1                                                         @d7: -x1 x2; d1: -x2 x1
    
    VCGT.U32   d3,  d6, d10                                                    @low: -x5 x4
    VSHR.S32   d8,  d6, #31
    VEOR       d8,  d8, d9
    VBIT.32    d6,  d8, d3
    
    VCGT.U32   d3,  d7, d10                                                    @ -x1 x2
    VSHR.S32   d8,  d7, #31
    VEOR       d8,  d8, d9
    VBIT.32    d7,  d8, d3
    
    VCGT.U32   d3,  d1, d10                                                    @-x2 x1
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d3
    
    VCGT.U32   d3,  d0, d10                                                    @-x4 x5
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d3
    
    VZIP.I8    d6,  d7                                                         @d6:...x2x4; d7:...-x1 -x5 
    VZIP.I8    d1,  d0                                                         @d1:...x5x1; d0:...-x4 -x2 
    VZIP.I16   d6,  d1                                                         @d6:...x5x1x2x4
    VZIP.I16   d7,  d0                                                         @d7:...-x4 -x2 -x1 -x5
    VZIP.I32   d6,  d7
    
    VST1.32    d6,  [r1], r2
    
    subs       r3,  r3, #1
    bgt        idct_row2Intra_loop
    
    ldmia      sp!, {r3 - r6, pc}
    @ENDP  @ |idct_row2Intra|
    
    .section .text
    .global	idct_row3Intra
idct_row3Intra:
    stmdb      sp!, {r3 - r7, lr}
    mov        r7,  #2816
    add        r7,  r7, #25
    VMOV.S32   d4[1], r7
    mov        r7,  #512
    add        r7,  r7, #53
    VMOV.S32   d4[0], r7                                                       @d4: W1W7
    
    mov        r7,  #1088
    add        r7,  r7, #20
    VMOV.S32   d5[0], r7
    mov        r7,  #2624
    add        r7,  r7, #52
    VMOV.S32   d5[1], r7                                                       @d5: W2W6
    
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r7,  #0
    mov        r3,  #8                                                         @r3: i
idct_row3Intra_loop:
    ldrsh      r4,  [r0]
    ldrsh      r5,  [r0, #2]                                                   @r5: x1 = blk[9];
    ldrsh      r6,  [r0, #4]                                                   @r6: x2 = blk[10];
    
    lsl        r4,  r4, #8
    add        r4,  r4, #8192                                                  @r4: x0 = ((*(blk += 8)) << 8) + 8192;
    strh       r7,  [r0, #2]
    strh       r7,  [r0, #4]
    strh       r7,  [r0], #16
    
    VDUP.S32   d0,  r5                                                         @d0: x1 x1
    VDUP.S32   d1,  r6                                                         @d1: x2 x2
    
    VMOV.S32   Q1,  #4
    VMLA.S32   Q1,  Q0,  Q2
    VSHR.S32   Q0,  Q1,  #3                                                    @Q0: x2 x6 x1 x7
    
    VDUP.S32   d2,  r4                                                         @d2: x0 x0
    VSUB.S32   d7,  d2, d1                                                     @d7: x2x6   (x0-x2)(x4-x6)
    VADD.S32   d6,  d2, d1                                                     @d6: x0x4   (x0+x2)(x4+x6)
    
    VPADDL.S32 d1,  d0                                                         @d0: x1 x3; d1: ..(x7 + x1)
    VREV64.S32 d3,  d0                                                         @d3: x7 x1
    VSUB.S32   d3,  d3, d0
    VZIP.S32   d3,  d1                                                         @d3:(x1 + x7)(x1 - x7)
    
    VMOV.S32   d2,  #181
    VMOV.S32   d1,  #128
    VMLA.S32   d1,  d2, d3
    VSHR.S32   d1,  d1, #8                                                     @d1: x7 x5; Q1:x7 x5 x1 x3
    VZIP.S32   d1,  d0                                                         @d0: x1 x7; d1: x3 x5
    
    VADD.S32   Q1,  Q3,  Q0
    VSHR.S32   Q1,  Q1,  #14                                                   @Q1: (x2+x3)(x6+x5)(x0+x1)(x4+x7)
    VSUB.S32   Q0,  Q3,  Q0
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: (x2-x3)(x6-x5)(x0-x1)(x4-x7)
    
    VZIP.S32   d3,  d0                                                         @d3: (x4-x7)(x6+x5); d0: (x0-x1)(x2+x3)
    VZIP.S32   d2,  d1                                                         @d2: (x6-x5)(x4+x7); d1: (x2-x3)(x0+x1)
    
    VCGT.U32   d7,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d7                                                     @d1: low: (x2-x3)(x0+x1)
    
    VCGT.U32   d7,  d2, d10
    VSHR.S32   d8,  d2, #31
    VEOR       d8,  d8, d9
    VBIT.32    d2,  d8, d7                                                     @d2: (x6-x5)(x4+x7)
    
    VCGT.U32   d7,  d3, d10
    VSHR.S32   d8,  d3, #31
    VEOR       d8,  d8, d9
    VBIT.32    d3,  d8, d7                                                     @d3: (x4-x7)(x6+x5)
    
    VCGT.U32   d7,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d7                                                     @d0: (x0-x1)(x2+x3)
    
    VZIP.I8    d1,  d2                                                         @d1: (x4+x7)(x0+x1); d2: (x6-x5)(x2-x3)
    VZIP.I8    d3,  d0                                                         @d3:(x2+x3)(x6+x5); d0:(x0-x1)(x4-x7)
    VZIP.I16   d1,  d3                                                         @d1:(x2+x3)(x6+x5)(x4+x7)(x0+x1)
    VZIP.I16   d2,  d0                                                         @d2:(x0-x1)(x4-x7)(x6-x5)(x2-x3)
    VZIP.I32   d1,  d2
    
    VST1.32    d1,  [r1], r2
    
    subs       r3,  r3, #1
    bgt        idct_row3Intra_loop
    
    ldmia      sp!, {r3 - r7, pc}
    @ENDP  @ |idct_row3Intra|
    
    .section .text
    .global	idct_row4Intra
idct_row4Intra:
    stmdb      sp!, {r3 - r7, lr}
    mov        r7,  #2816
    add        r7,  r7, #25
    VMOV.S32   d4[1], r7
    mov        r7,  #512
    add        r7,  r7, #53
    VMOV.S32   d4[0], r7                                                       @d4: W1W7
    
    mov        r7,  #1088
    add        r7,  r7, #20
    VMOV.S32   d5[0], r7
    mov        r7,  #2624
    add        r7,  r7, #52
    VMOV.S32   d5[1], r7                                                       @d5: W2W6
    
    mov        r7,  #2400
    add        r7,  r7, #8
    VMOV.S32   d12[1], r7
    mov        r7,  #7
    sub        r7,  r7, #1616
    VMOV.S32   d12[0], r7                                                      @d12: W3(-W5)
    
    VMOV.I16   d11,  #0
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r3,  #8                                                         @r3: i
idct_row4Intra_loop:
    ldrsh      r4,  [r0]
    ldrsh      r5,  [r0, #2]                                                   @r5: x1 = blk[9];
    ldrsh      r6,  [r0, #4]                                                   @r6: x2 = blk[10];
    ldrsh      r7,  [r0, #6]                                                   @r7: x3
    
    lsl        r4,  r4, #8
    add        r4,  r4, #8192                                                  @r4: x0 = ((*(blk += 8)) << 8) + 8192;
    
    VST1.16    d11,  [r0]
    
    VDUP.S32   d0,  r5                                                         @d0: x1 x1
    VDUP.S32   d1,  r6                                                         @d1: x2 x2
    
    VMOV.S32   Q1,  #4
    VMLA.S32   Q1,  Q0,  Q2
    VSHR.S32   Q0,  Q1,  #3                                                    @Q0: x2 x6 x1 x7
    
    VDUP.S32   d2,  r4                                                         @d2: x0 x0
    VSUB.S32   d7,  d2, d1                                                     @d7: x2x6   (x0-x2)(x4-x6)
    VADD.S32   d6,  d2, d1                                                     @d6: x0x4   (x0+x2)(x4+x6)
    
    VDUP.S32   d2,  r7                                                         @d2: x3x3
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d12
    VSHR.S32   d2,  d3,  #3                                                    @d2: x5x3
    VSUB.S32   d1,  d0,  d2                                                    @d1: x5x8
    VADD.S32   d0,  d0,  d2                                                    @d0: x1x3
    
    
    VPADDL.S32 d2,  d1                                                         @d2: ..(x5 + x8)
    VREV64.S32 d3,  d1                                                         @d3: x8 x5
    VSUB.S32   d3,  d3, d1
    VZIP.S32   d3,  d2                                                         @d3:(x5 + x8)(x5 - x8)
    
    VMOV.S32   d2,  #181
    VMOV.S32   d1,  #128
    VMLA.S32   d1,  d2, d3
    VSHR.S32   d1,  d1, #8                                                     @d1: x7 x5; Q1:x7 x5 x1 x3
    VZIP.S32   d1,  d0                                                         @d0: x1 x7; d1: x3 x5
    
    VADD.S32   Q1,  Q3,  Q0
    VSHR.S32   Q1,  Q1,  #14                                                   @Q1: (x2+x3)(x6+x5)(x0+x1)(x4+x7)
    VSUB.S32   Q0,  Q3,  Q0
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: (x2-x3)(x6-x5)(x0-x1)(x4-x7)
    
    VZIP.S32   d3,  d0                                                         @d3: (x4-x7)(x6+x5); d0: (x0-x1)(x2+x3)
    VZIP.S32   d2,  d1                                                         @d2: (x6-x5)(x4+x7); d1: (x2-x3)(x0+x1)
    
    VCGT.U32   d7,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d7                                                     @d1: low: (x2-x3)(x0+x1)
    
    VCGT.U32   d7,  d2, d10
    VSHR.S32   d8,  d2, #31
    VEOR       d8,  d8, d9
    VBIT.32    d2,  d8, d7                                                    @d2: (x6-x5)(x4+x7)
    
    VCGT.U32   d7,  d3, d10
    VSHR.S32   d8,  d3, #31
    VEOR       d8,  d8, d9
    VBIT.32    d3,  d8, d7                                                    @d3: (x4-x7)(x6+x5)
    
    VCGT.U32   d7,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d7                                                    @d0: (x0-x1)(x2+x3)
    
    VZIP.I8    d1,  d2                                                         @d1: (x4+x7)(x0+x1); d2: (x6-x5)(x2-x3)
    VZIP.I8    d3,  d0                                                         @d3:(x2+x3)(x6+x5); d0:(x0-x1)(x4-x7)
    VZIP.I16   d1,  d3                                                         @d1:(x2+x3)(x6+x5)(x4+x7)(x0+x1)
    VZIP.I16   d2,  d0                                                         @d2:(x0-x1)(x4-x7)(x6-x5)(x2-x3)
    VZIP.I32   d1,  d2
    
    VST1.32    d1,  [r1], r2
    
    add        r0,  r0, #16
    subs       r3,  r3, #1
    bgt        idct_row4Intra_loop
    
    ldmia      sp!, {r3 - r7, pc}
    @ENDP  @ |idct_row4Intra|
    
    
    .section .text
    .global	idct_row0x40Intra
idct_row0x40Intra:
    stmdb      sp!, {r3 - r5, lr}
    ldr        r4,  =565
    ldr        r5,  =2841
    VMOV.S32   d0, r5, r4                                                      @d0: W7W1
    
    VMOV.S32   Q4,  #0xff
    VMOV.I8    Q5,  #0xff
    
    VMOV.S32   Q1,  #8192                                                      @Q1: x0 x0 x0 x0
    mov        r5,  #0
    mov        r3,  #8                                                         @r3: i
idct_row0x40Intra_loop:
    ldrsh      r4,  [r0, #2]!                                                  @r4: x4 = blk[9];
    VDUP.S32   d1,  r4                                                         @d1: x4 x4
    strh       r5,  [r0], #14
    
    VMOV.S32   d4,  #4
    VMLA.S32   d4,  d0,  d1
    VSHR.S32   d4,  d4,  #3                                                    @d4:x5 = (W7 * x4 + 4) >> 3;x4 = (W1 * x4 + 4) >> 3;
    
    VPADDL.S32 d5,  d4                                                         @d5: (x4 + x5)
    VREV64.S32 d1,  d4                                                         @d1: x4 x5
    VSUB.S32   d1,  d4, d1
    VZIP.S32   d5,  d1                                                         @d5:(x4 - x5)(x4 + x5)
    
    VMOV.S32   d1,  #181
    VMOV.S32   d6,  #128
    VMLA.S32   d6,  d1, d5
    VSHR.S32   d5,  d6, #8                                                     @d5: x1 x2; Q2:x1 x2 x5 x4
    
    VADD.S32   Q3,  Q2,  Q1
    VSHR.S32   Q3,  Q3,  #14                                                   @Q3: x1 x2 x5 x4
    VSUB.S32   Q2,  Q1,  Q2
    VSHR.S32   Q2,  Q2,  #14                                                   @Q2: -x1 -x2 -x5 -x4
    
    
    VCGT.U32   Q6,  Q3, Q4
    VSHR.S32   Q7,  Q3, #31
    VEOR       Q7,  Q7, Q5
    VBIT.32    Q3,  Q7, Q6
    
    VCGT.U32   Q6,  Q2, Q4
    VSHR.S32   Q7,  Q2, #31
    VEOR       Q7,  Q7, Q5
    VBIT.32    Q2,  Q7, Q6
    
    @@@@@@@@@@@@@@@@Q2: -x1 -x2 -x5 -x4; Q3: x1 x2 x5 x4@@@@@@@@@@@@@@@@@@@@@@@@
    VMOV.S32   Q6,  Q3
    VZIP.I8    d6,  d7                                                         @d6:...x2x4; d7:...x1x5
    VZIP.I8    d13, d12                                                        @d12:x5x1; d13:x4x2
    VZIP.I16   d6,  d12                                                        @d6: x5x1x2x4
    
    VMOV.S32   Q6,  Q2
    VZIP.I8    d4,  d5                                                         @d4:...-x2 -x4; d5:...-x1 -x5
    VZIP.I8    d13, d12                                                        @d12: -x5 -x1;d13:-x4 -x2
    VZIP.I16   d5,  d13                                                        @d6: -x4 -x2 -x1 -x5
    
    VZIP.I32   d6,  d5
    
    VST1.32    d6,  [r1], r2
    
    subs       r3,  r3, #1
    bgt        idct_row0x40Intra_loop
    
    ldmia      sp!, {r3 - r5, pc}
    @ENDP  @ |idct_row0x40Intra|
    
    .section .text
    .global	idct_row0x20Intra
idct_row0x20Intra:
    stmdb      sp!, {r3 - r5, lr}
    ldr        r4,  =1108
    VMOV.S32   d0[1], r4
    ldr        r4,  =2676
    VMOV.S32   d0[0], r4                                                       @d0: W6W2
    VMOV.S32   d1,   #8192                                                     @d1: 8192
    
    VMOV.S32   Q2,  #0xff
    VMOV.I8    Q3,  #0xff
    
    mov        r4,  #0
    mov        r3,  #8                                                         @r3: i
idct_row0x20Intra_loop:
    ldrsh      r5,  [r0, #4]!                                                  @r5: x2 = blk[2];
    strh       r4,  [r0], #12                                                  @blk[2] = 0;
    
    VDUP.S32   d2,  r5                                                         @d2: x2 x2
    
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d0
    VSHR.S32   d2,  d3,  #3                                                    @d2:x6 = (W6 * x2 + 4) >> 3;x2 = (W2 * x2 + 4) >> 3;
    
    VSUB.S32   d3,  d1,  d2                                                    @d3: x6 x2
    VADD.S32   d2,  d1,  d2                                                    @d2: x4 x0
    VSHR.S32   Q1,  Q1,  #14                                                   @Q1: x6 x2 x4 x0
    
    VCGT.U32   Q4,  Q1, Q2
    VSHR.S32   Q5,  Q1, #31
    VEOR       Q5,  Q5, Q3
    VBIT.32    Q1,  Q5, Q4
    
    VREV64.S32 d8,  d2                                                         @d8: x0 x4
    VREV64.S32 d9,  d3                                                         @d9: x2 x6
    
    VZIP.I8    d2,  d8                                                         @d2:......x4x0;d8:......x0x4
    VZIP.I8    d3,  d9                                                         @d3:......x6x2;d9:......x2x6
    VZIP.I16   d2,  d9                                                         @d2:....x2x6x4x0
    VZIP.I16   d3,  d8                                                         @d3:....x0x4x6x2
    VZIP.I32   d2,  d3
    
    VST1.32    d2,  [r1], r2
    
    subs       r3,  r3, #1
    bgt        idct_row0x20Intra_loop
    
    ldmia      sp!, {r3 - r5, pc}
    @ENDP  @ |idct_row0x20Intra|
    
    .section .text
    .global	idct_row0x10Intra
idct_row0x10Intra:
    stmdb      sp!, {r3 - r5, lr}
    ldr        r4,  =2408
    ldr        r5,  =1609
    VMOV.S32   d0,  r4, r5                                                     @d0: W5W3
    
    mov        r4,  #-181
    VMOV.S32   d1,  #181
    VMOV.S32   d1[1], r4                                                       @d1: -181 181
    
    VMOV.S32   Q4,  #0xff
    VMOV.I8    Q5,  #0xff
    
    VMOV.S32   Q1,  #8192                                                      @Q1: x0 x0 x0 x0
    mov        r5,  #0
    mov        r3,  #8                                                         @r3: i
idct_row0x10Intra_loop:
    ldrsh      r4,  [r0, #6]!                                                  @r4: x3 = blk[3];
    VDUP.S32   d4,  r4                                                         @d4: x3 x3
    strh       r5,  [r0], #10
    
    VMOV.S32   d5,  #4
    VMLA.S32   d5,  d0,  d4
    VSHR.S32   d4,  d5,  #3                                                    @d4:x3 = (W5 * x3 + 4) >> 3;x1 = (W3 * x3 + 4) >> 3;
    
    VPADDL.S32 d5,  d4                                                         @d5:(x3 + x1)
    VREV64.S32 d6,  d4                                                         @d6: x1 x3
    VSUB.S32   d6,  d6, d4                                                     @d6:(x3 - x1)
    VZIP.S32   d6,  d5                                                         @d6:(x3 + x1)(x3 - x1)
    
    VMOV.S32   d5,  #128
    VMLA.S32   d5,  d6, d1
    VSHR.S32   d5,  d5, #8                                                     @d5: x5 x7; Q2:x5 x7 x3 x1
    
    VADD.S32   Q3,  Q1,  Q2
    VSHR.S32   Q3,  Q3,  #14                                                   @Q3: x5 x7 x3 x1
    VSUB.S32   Q2,  Q1,  Q2
    VSHR.S32   Q2,  Q2,  #14                                                   @Q2: -x5 -x7 -x3 -x1
    
    @@@@@@@@@@@@@@@@Q2: -x5 -x7 -x3 -x1; Q3: x5 x7 x3 x1
    VCGT.U32   Q6,  Q2, Q4
    VSHR.S32   Q7,  Q2, #31
    VEOR       Q7,  Q7, Q5
    VBIT.32    Q2,  Q7, Q6                                                     @low: x3 x1
    
    VCGT.U32   Q6,  Q3, Q4
    VSHR.S32   Q7,  Q3, #31
    VEOR       Q7,  Q7, Q5
    VBIT.32    Q3,  Q7, Q6
    
    VREV64.S32 d7,  d7                                                         @d7: x7 x5
    VZIP.I32   d5,  d7                                                         @d5: x5 -x7; d7: x7 -x5
    VREV64.S32 d7,  d7                                                         @d7: -x5 x7
    
    VZIP.I8    d6,  d7                                                         @d6: x7x1; d7:-x5x3
    VZIP.I8    d5,  d4                                                         @d4: -x3 x5; d5:-x1 -x7
    VZIP.I16   d6,  d4
    VZIP.I16   d7,  d5
    VZIP.I32   d6,  d7
    
    VST1.32    d6,  [r1], r2
    
    subs       r3,  r3, #1
    bgt        idct_row0x10Intra_loop
    
    ldmia      sp!, {r3 - r5, pc}
    @ENDP  @ |idct_row0x10IIntra|
    
    
    .section .text
    .global	idct_rowIntra
idct_rowIntra:
    stmdb      sp!, {r3 - r4, lr}
    ldr        r4,  =2408
    VMOV.S32   d4[1], r4
    ldr        r4,  =565
    VMOV.S32   d4[0], r4                                                       @d4: W3W7
    ldr        r4,  =1108
    VDUP.S32   d5, r4                                                          @d5: W6W6
    
    ldr        r4,  =2841-565
    ldr        r3,  =-2841-565
    orr        r4,  r4, r3, lsl #16
    VMOV.S32   d12[0], r4
    ldr        r4,  =1609-2408
    lsl        r4,  r4, #16
    lsr        r4,  r4, #16
    ldr        r3,  =-1609-2408
    orr        r4,  r4, r3, lsl #16
    VMOV.S32   d12[1], r4                                                      @d12:(-(W3+W5))(-(W3-W5))(-(W1+W7))(W1-W7)
    
    ldr        r4,  =-2676-1108
    VMOV.S32   d13[0], r4
    ldr        r4,  =2676-1108
    VMOV.S32   d13[1], r4                                                      @d13: (W2-W6)(-(W2+W6))
    
    VMOV.S32   Q5,  #0xff
    VMOV.I8    Q8,  #0xff
    
    VMOV.I16   Q7,  #0
    mov        r4,  #16
    mov        r3,  #8                                                         @r3: i
idct_rowIntra_loop:
    VLD1.16    {d0, d1},   [r0]                                                @d0: x7x3x4(x0);d1: x5x2x6(x1)
    VST1.16    {d14, d15}, [r0], r4
    
    
    VADDL.S16  Q1,  d0,  d1                                                    @Q1: .(x3+x2).((x0)+(x1))
    VSUBL.S16  Q4,  d0,  d1                                                    @Q4: ...((x0)-(x1))
    VZIP.S32   d8,  d2                                                         @d8:(x0)+(x1))((x0)-(x1))
    VSHL.S32   d2,  d8,  #8
    VMOV.S32   d8,  #8192
    VADD.S32   d2,  d2,  d8                                                    @d2: x8 x0
    
    VREV64.S32 d0,  d0                                                         @d0: x4(x0)x7x3
    VADDL.S16  Q4,  d0,  d1                                                    @Q4: (x4+x5).(x7+x6).
    VZIP.S32   d9,  d8                                                         @d8: (x7+x6)(x4+x5)
    VDUP.S32   d9,  d3[0]                                                      @d9: (x3+x2)(x3+x2)
    
    VMOV.S32   Q3,  #4
    VMLA.S32   Q3,  Q4,  Q2                                                    @Q3: x1x1x8x8
    
    VZIP.S16   d0,  d1                                                         @d0: x6x7(x1)x3; d1:x5x4x2(x0)
    VREV32.S16 d0,  d0                                                         @d0: x7x6x3(x1)
    VZIP.S32   d1,  d0                                                         @d1: x3(x1)x2(x0); d0: x7x6x5x4
    VSHR.S32   d1,  d1,  #16                                                   @d1: x3x2
    VMLA.S32   d7,  d1, d13
    VSHR.S32   d7,  d7, #3                                                     @d7:x3x2
    
    VMULL.S16  Q4,  d0, d12                                                    @Q4: x7x6x5x4
    VDUP.S32   d1,  d6[0]                                                      @d8:x5x4 + d6[0];d9:x7x6 + d6[1]
    VADD.S32   d8,  d8, d1
    VDUP.S32   d1,  d6[1]
    VADD.S32   d9,  d9, d1
    VSHR.S32   Q4,  Q4, #3                                                     @Q4:x7x6x5x4
    
    VADD.S32   d0,  d8, d9                                                     @d0: x6x1
    VSUB.S32   d1,  d8, d9                                                     @d1: x5x4
    
    VADD.S32   d3,  d2, d7                                                     @d3: x7x3
    VSUB.S32   d2,  d2, d7                                                     @d2: x8x0
    
    VPADDL.S32 d6,  d1                                                         @d6: ..(x5+x4)
    VREV64.S32 d7,  d1                                                         @d7: x4x5
    VSUB.S32   d1,  d1, d7
    VZIP.S32   d6,  d1                                                         @d6:(x4 - x5)(x4 + x5)
    
    VMOV.S32   d7,  #181
    VMOV.S32   d1,  #128
    VMLA.S32   d1,  d6, d7
    VSHR.S32   d1,  d1, #8                                                     @d1: x4 x2 Q0: x4x2x6x1;  Q1:x7x3x8x0
    VZIP.S32   d1,  d0                                                         @d0: x6x4; d1: x1x2;      Q0:x1x2x6x4
    
    VADD.S32   Q3,  Q1,  Q0
    VSUB.S32   Q0,  Q1,  Q0
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: (x7-x1)(x3-x2)(x8-x6)(x0-x4)
    VSHR.S32   Q1,  Q3,  #14                                                   @Q1: (x7+x1)(x3+x2)(x8+x6)(x0+x4)
    
    VCGT.U32   Q3,  Q0, Q5
    VSHR.S32   Q4,  Q0, #31
    VEOR       Q4,  Q4, Q8
    VBIT.32    Q0,  Q4, Q3
    
    VCGT.U32   Q3,  Q1, Q5
    VSHR.S32   Q4,  Q1, #31
    VEOR       Q4,  Q4, Q8
    VBIT.32    Q1,  Q4, Q3
    
    VREV64.S32 d6,  d0                                                         @d6: (x0-x4)(x8-x6)
    VREV64.S32 d7,  d1                                                         @d7: (x3-x2)(x7-x1)
    VZIP.I8    d6,  d0                                                         @d6: ......(x0-x4)(x8-x6)
    VZIP.I8    d1,  d7                                                         @d1: ......(x7-x1)(x3-x2)
    VZIP.I16   d6,  d1                                                         @d6: ....(x7-x1)(x3-x2)(x0-x4)(x8-x6)
    
    VREV64.S32 d0,  d2                                                         @d0: (x0+x4)(x8+x6)
    VREV64.S32 d1,  d3                                                         @d1: (x3+x2)(x7+x1)
    VZIP.I8    d2,  d0                                                         @d2: ......(x8+x6)(x0+x4)
    VZIP.I8    d1,  d3                                                         @d1: ......(x3+x2)(x7+x1)
    VZIP.I16   d1,  d2                                                         @d1: ....(x8+x6)(x0+x4)(x3+x2)(x7+x1)
    
    VZIP.I32   d1,  d6
    
    VST1.32    d1,  [r1], r2
    
    subs       r3,  r3, #1
    bgt        idct_rowIntra_loop
    
    ldmia      sp!, {r3 - r4, pc}
    @ENDP  @ |idct_rowIntra|
    
    .section .text
    .global	idct_row0zmv
idct_row0zmv:
    stmdb      sp!, {lr}
    ldmia      sp!, {pc}
    @ENDP  @ |idct_row0zmv|
    
    .section .text
    .global	idct_row1zmv
idct_row1zmv:
    stmdb      sp!, {r4 - r7, lr}
    mov        r5,  #0
    mov        r6,  #8                                                         @r6: i
    mov        r7,  #16
idct_row1zmv_loop:
    ldrsh      r4,  [r0]                                                       @r4: *blk
    add        r4,  r4, #32
    asr        r4,  r4, #6                                                     @tmp = (*(blk += 8) + 32) >> 6;
    VDUP.16    Q1,  r4                                                         @Q1: (tmp...)
    strh       r5,  [r0], #16                                                  @*blk = 0;   post index change
    
    VLD1.8     d0,  [r2], r7
    VADDW.U8   Q1,  Q1,  d0
    
    VMOV.S16   Q2,  #0xff
    VMOV.I8    Q4,  #0xff
    VSHR.S16   Q3,  Q1,  #15
    VEOR       Q3,  Q3,  Q4
    VAND       Q3,  Q3,  Q2
    VCGT.U16   Q2,  Q1,  Q2
    
    VUZP.8     d2,  d3
    VUZP.8     d4,  d5
    VUZP.8     d6,  d7
    VBIT.8     d2,  d6,  d4
    
    VST1.32    {d2}, [r1], r3
    subs       r6,  #1
    bgt        idct_row1zmv_loop
    
    ldmia      sp!, {r4 - r7, pc}
    @ENDP  @ |idct_row1zmv|
    
    .section .text
    .global	idct_row2zmv
idct_row2zmv:
    stmdb      sp!, {r4 - r8, lr}
    mov        r6,  #2816
    add        r6,  r6, #25
    VMOV.S32   d4[0], r6
    mov        r6,  #512
    add        r6,  r6, #53
    VMOV.S32   d4[1], r6                                                       @d4: W7W1
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r6,  #0
    mov        r7,  #8                                                         @r7: i
    mov        r8,  #16
idct_row2zmv_loop:
    ldrsh      r4,  [r0]
    ldrsh      r5,  [r0, #2]                                                   @r5: x4 = blk[9];
    lsl        r4,  r4, #8
    add        r4,  r4, #8192                                                  @r4: x0 = ((*(blk += 8)) << 8) + 8192;
    str        r6,  [r0], #16
    
    VDUP.S32   Q0,  r4                                                         @Q0: x0 x0 x0 x0
    VDUP.S32   d2,  r5                                                         @d2: x4 x4
    
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d4
    VSHR.S32   d2,  d3,  #3                                                    @d2:x5 = (W7 * x4 + 4) >> 3;x4 = (W1 * x4 + 4) >> 3;
    
    VPADDL.S32 d3,  d2                                                         @(x4 + x5)
    VREV64.S32 d5,  d2                                                         @d5: x4 x5
    VSUB.S32   d5,  d2, d5
    VZIP.S32   d3,  d5                                                         @d3:(x4 - x5)(x4 + x5)
    
    VMOV.S32   d5,  #181
    VMOV.S32   d6,  #128
    VMLA.S32   d6,  d3, d5
    VSHR.S32   d3,  d6, #8                                                     @d3: x1 x2; Q1:x1 x2 x5 x4
    
    VADD.S32   Q3,  Q0,  Q1
    VSHR.S32   Q3,  Q3,  #14                                                   @Q3: x1 x2 x5 x4
    VSUB.S32   Q0,  Q0,  Q1
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: -x1 -x2 -x5 -x4
    
    VREV64.S32 d0,  d0                                                         @d0: -x4 -x5 
    VZIP.S32   d6,  d0                                                         @d6: -x5 x4; d0: -x4 x5
    VREV64.S32 d1,  d1                                                         @d1: -x2 -x1
    VZIP.S32   d7,  d1                                                         @d7: -x1 x2; d1: -x2 x1
    
    VLD1.8     d2,  [r2], r8
    
    VAND.S32   d3,  d2, d10
    VADD.S32   d6,  d6, d3                                                     @low: -x5 x4
    VCGT.U32   d3,  d6, d10
    VSHR.S32   d8,  d6, #31
    VEOR       d8,  d8, d9
    VBIT.32    d6,  d8, d3
    
    VSHR.S32   d3,  d2, #8
    VAND.S32   d3,  d3, d10                                                    @ -x1 x2
    VADD.S32   d7,  d7, d3
    VCGT.U32   d3,  d7, d10
    VSHR.S32   d8,  d7, #31
    VEOR       d8,  d8, d9
    VBIT.32    d7,  d8, d3
    
    VSHR.S32   d3,  d2, #16
    VAND.S32   d3,  d3, d10                                                    @-x2 x1
    VADD.S32   d1,  d1, d3
    VCGT.U32   d3,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d3
    
    VSHR.S32   d3,  d2, #24
    VAND.S32   d3,  d3, d10                                                    @-x4 x5
    VADD.S32   d0,  d0, d3
    VCGT.U32   d3,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d3
    
    VZIP.I8    d6,  d7                                                         @d6:...x2x4; d7:...-x1 -x5 
    VZIP.I8    d1,  d0                                                         @d1:...x5x1; d0:...-x4 -x2 
    VZIP.I16   d6,  d1                                                         @d6:...x5x1x2x4
    VZIP.I16   d7,  d0                                                         @d7:...-x4 -x2 -x1 -x5
    VZIP.I32   d6,  d7
    
    VST1.32    d6,  [r1], r3
    
    subs       r7,  r7, #1
    bgt        idct_row2zmv_loop
    
    ldmia      sp!, {r4 - r8, pc}
    @ENDP  @ |idct_row2zmv|
    
    .section .text
    .global	idct_row3zmv
idct_row3zmv:
    stmdb      sp!, {r4 - r9, lr}
    ldr        r7,  =2841
    VMOV.S32   d4[1], r7
    ldr        r7,  =565
    VMOV.S32   d4[0], r7                                                       @d4: W1W7
    
    ldr        r7,  =1108
    VMOV.S32   d5[0], r7
    ldr        r7,  =2676
    VMOV.S32   d5[1], r7                                                       @d5: W2W6
    
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r7,  #0
    mov        r8,  #8                                                         @r8: i
    mov        r9,  #16
idct_row3zmv_loop:
    ldrsh      r4,  [r0]
    ldrsh      r5,  [r0, #2]                                                   @r5: x1 = blk[9];
    ldrsh      r6,  [r0, #4]                                                   @r6: x2 = blk[10];
    
    lsl        r4,  r4, #8
    add        r4,  r4, #8192                                                  @r4: x0 = ((*(blk += 8)) << 8) + 8192;
    strh       r7,  [r0, #2]
    strh       r7,  [r0, #4]
    strh       r7,  [r0], #16
    
    VDUP.S32   d0,  r5                                                         @d0: x1 x1
    VDUP.S32   d1,  r6                                                         @d1: x2 x2
    
    VMOV.S32   Q1,  #4
    VMLA.S32   Q1,  Q0,  Q2
    VSHR.S32   Q0,  Q1,  #3                                                    @Q0: x2 x6 x1 x7
    
    VDUP.S32   d2,  r4                                                         @d2: x0 x0
    VSUB.S32   d7,  d2, d1                                                     @d7: x2x6   (x0-x2)(x4-x6)
    VADD.S32   d6,  d2, d1                                                     @d6: x0x4   (x0+x2)(x4+x6)
    
    VPADDL.S32 d1,  d0                                                         @d0: x1 x3; d1: ..(x7 + x1)
    VREV64.S32 d3,  d0                                                         @d3: x7 x1
    VSUB.S32   d3,  d3, d0
    VZIP.S32   d3,  d1                                                         @d3:(x1 + x7)(x1 - x7)
    
    VMOV.S32   d2,  #181
    VMOV.S32   d1,  #128
    VMLA.S32   d1,  d2, d3
    VSHR.S32   d1,  d1, #8                                                     @d1: x7 x5; Q1:x7 x5 x1 x3
    VZIP.S32   d1,  d0                                                         @d0: x1 x7; d1: x3 x5
    
    VADD.S32   Q1,  Q3,  Q0
    VSHR.S32   Q1,  Q1,  #14                                                   @Q1: (x2+x3)(x6+x5)(x0+x1)(x4+x7)
    VSUB.S32   Q0,  Q3,  Q0
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: (x2-x3)(x6-x5)(x0-x1)(x4-x7)
    
    VZIP.S32   d3,  d0                                                         @d3: (x4-x7)(x6+x5); d0: (x0-x1)(x2+x3)
    VZIP.S32   d2,  d1                                                         @d2: (x6-x5)(x4+x7); d1: (x2-x3)(x0+x1)
    
    VLD1.8     d6,  [r2], r9
    
    VAND.S32   d7,  d6, d10
    VADD.S32   d1,  d7, d1                                                     @d1: low: (x2-x3)(x0+x1)
    VCGT.U32   d7,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d7
    
    VSHR.S32   d7,  d6, #8
    VAND.S32   d7,  d7, d10                                                    @d2: (x6-x5)(x4+x7)
    VADD.S32   d2,  d7, d2
    VCGT.U32   d7,  d2, d10
    VSHR.S32   d8,  d2, #31
    VEOR       d8,  d8, d9
    VBIT.32    d2,  d8, d7
    
    VSHR.S32   d7,  d6, #16
    VAND.S32   d7,  d7, d10                                                    @d3: (x4-x7)(x6+x5)
    VADD.S32   d3,  d3, d7
    VCGT.U32   d7,  d3, d10
    VSHR.S32   d8,  d3, #31
    VEOR       d8,  d8, d9
    VBIT.32    d3,  d8, d7
    
    VSHR.S32   d7,  d6, #24
    VAND.S32   d7,  d7, d10                                                    @d0: (x0-x1)(x2+x3)
    VADD.S32   d0,  d0, d7
    VCGT.U32   d7,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d7
    
    VZIP.I8    d1,  d2                                                         @d1: (x4+x7)(x0+x1); d2: (x6-x5)(x2-x3)
    VZIP.I8    d3,  d0                                                         @d3:(x2+x3)(x6+x5); d0:(x0-x1)(x4-x7)
    VZIP.I16   d1,  d3                                                         @d1:(x2+x3)(x6+x5)(x4+x7)(x0+x1)
    VZIP.I16   d2,  d0                                                         @d2:(x0-x1)(x4-x7)(x6-x5)(x2-x3)
    VZIP.I32   d1,  d2
    
    VST1.32    d1,  [r1], r3
    
    subs       r8,  r8, #1
    bgt        idct_row3zmv_loop
    
    ldmia      sp!, {r4 - r9, pc}
    @ENDP  @ |idct_row3zmv|
    
    .section .text
    .global	idct_row4zmv
idct_row4zmv:
    stmdb      sp!, {r4 - r9, lr}
    mov        r7,  #2816
    add        r7,  r7, #25
    VMOV.S32   d4[1], r7
    mov        r7,  #512
    add        r7,  r7, #53
    VMOV.S32   d4[0], r7                                                       @d4: W1W7
    
    mov        r7,  #1088
    add        r7,  r7, #20
    VMOV.S32   d5[0], r7
    mov        r7,  #2624
    add        r7,  r7, #52
    VMOV.S32   d5[1], r7                                                       @d5: W2W6
    
    mov        r7,  #2400
    add        r7,  r7, #8
    VMOV.S32   d12[1], r7
    mov        r7,  #7
    sub        r7,  r7, #1616
    VMOV.S32   d12[0], r7                                                      @d12: W3(-W5)
    
    VMOV.I16   d11,  #0
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r8,  #8                                                         @r8: i
    mov        r9,  #16
idct_row4zmv_loop:
    ldrsh      r4,  [r0]
    ldrsh      r5,  [r0, #2]                                                   @r5: x1 = blk[9];
    ldrsh      r6,  [r0, #4]                                                   @r6: x2 = blk[10];
    ldrsh      r7,  [r0, #6]                                                   @r7: x3
    
    lsl        r4,  r4, #8
    add        r4,  r4, #8192                                                  @r4: x0 = ((*(blk += 8)) << 8) + 8192;
    
    VST1.16    d11,  [r0], r9
    
    VDUP.S32   d0,  r5                                                         @d0: x1 x1
    VDUP.S32   d1,  r6                                                         @d1: x2 x2
    
    VMOV.S32   Q1,  #4
    VMLA.S32   Q1,  Q0,  Q2
    VSHR.S32   Q0,  Q1,  #3                                                    @Q0: x2 x6 x1 x7
    
    VDUP.S32   d2,  r4                                                         @d2: x0 x0
    VSUB.S32   d7,  d2, d1                                                     @d7: x2x6   (x0-x2)(x4-x6)
    VADD.S32   d6,  d2, d1                                                     @d6: x0x4   (x0+x2)(x4+x6)
    
    VDUP.S32   d2,  r7                                                         @d2: x3x3
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d12
    VSHR.S32   d2,  d3,  #3                                                    @d2: x5x3
    VSUB.S32   d1,  d0,  d2                                                    @d1: x5x8
    VADD.S32   d0,  d0,  d2                                                    @d0: x1x3
    
    
    VPADDL.S32 d2,  d1                                                         @d2: ..(x5 + x8)
    VREV64.S32 d3,  d1                                                         @d3: x8 x5
    VSUB.S32   d3,  d3, d1
    VZIP.S32   d3,  d2                                                         @d3:(x5 + x8)(x5 - x8)
    
    VMOV.S32   d2,  #181
    VMOV.S32   d1,  #128
    VMLA.S32   d1,  d2, d3
    VSHR.S32   d1,  d1, #8                                                     @d1: x7 x5; Q1:x7 x5 x1 x3
    VZIP.S32   d1,  d0                                                         @d0: x1 x7; d1: x3 x5
    
    VADD.S32   Q1,  Q3,  Q0
    VSHR.S32   Q1,  Q1,  #14                                                   @Q1: (x2+x3)(x6+x5)(x0+x1)(x4+x7)
    VSUB.S32   Q0,  Q3,  Q0
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: (x2-x3)(x6-x5)(x0-x1)(x4-x7)
    
    VZIP.S32   d3,  d0                                                         @d3: (x4-x7)(x6+x5); d0: (x0-x1)(x2+x3)
    VZIP.S32   d2,  d1                                                         @d2: (x6-x5)(x4+x7); d1: (x2-x3)(x0+x1)
    
    VLD1.8     d6,  [r2], r9
    
    VAND.S32   d7,  d6, d10
    VADD.S32   d1,  d7, d1                                                     @d1: low: (x2-x3)(x0+x1)
    VCGT.U32   d7,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d7
    
    VSHR.S32   d7,  d6, #8
    VAND.S32   d7,  d7, d10                                                    @d2: (x6-x5)(x4+x7)
    VADD.S32   d2,  d7, d2
    VCGT.U32   d7,  d2, d10
    VSHR.S32   d8,  d2, #31
    VEOR       d8,  d8, d9
    VBIT.32    d2,  d8, d7
    
    VSHR.S32   d7,  d6, #16
    VAND.S32   d7,  d7, d10                                                    @d3: (x4-x7)(x6+x5)
    VADD.S32   d3,  d3, d7
    VCGT.U32   d7,  d3, d10
    VSHR.S32   d8,  d3, #31
    VEOR       d8,  d8, d9
    VBIT.32    d3,  d8, d7
    
    VSHR.S32   d7,  d6, #24
    VAND.S32   d7,  d7, d10                                                    @d0: (x0-x1)(x2+x3)
    VADD.S32   d0,  d0, d7
    VCGT.U32   d7,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d7
    
    VZIP.I8    d1,  d2                                                         @d1: (x4+x7)(x0+x1); d2: (x6-x5)(x2-x3)
    VZIP.I8    d3,  d0                                                         @d3:(x2+x3)(x6+x5); d0:(x0-x1)(x4-x7)
    VZIP.I16   d1,  d3                                                         @d1:(x2+x3)(x6+x5)(x4+x7)(x0+x1)
    VZIP.I16   d2,  d0                                                         @d2:(x0-x1)(x4-x7)(x6-x5)(x2-x3)
    VZIP.I32   d1,  d2
    
    VST1.32    d1,  [r1], r3
    
    subs       r8,  r8, #1
    bgt        idct_row4zmv_loop
    
    ldmia      sp!, {r4 - r9, pc}
    @ENDP  @ |idct_row4zmv|
    
    .section .text
    .global	idct_row0x40zmv
idct_row0x40zmv:
    stmdb      sp!, {r4 - r7, lr}
    ldr        r4,  =565
    ldr        r5,  =2841
    VMOV.S32   d4, r5, r4                                                      @d4: W7W1
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    VMOV.S32   Q6,  #8192                                                      @Q0: x0 x0 x0 x0
    mov        r5,  #0
    mov        r6,  #8                                                         @r6: i
    mov        r7,  #16
idct_row0x40zmv_loop:
    ldrsh      r4,  [r0, #2]!                                                  @r4: x4 = blk[9];
    VDUP.S32   d2,  r4                                                         @d2: x4 x4
    strh       r5,  [r0], #14
    
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d4
    VSHR.S32   d2,  d3,  #3                                                    @d2:x5 = (W7 * x4 + 4) >> 3;x4 = (W1 * x4 + 4) >> 3;
    
    VPADDL.S32 d3,  d2                                                         @(x4 + x5)
    VREV64.S32 d5,  d2                                                         @d5: x4 x5
    VSUB.S32   d5,  d2, d5
    VZIP.S32   d3,  d5                                                         @d3:(x4 - x5)(x4 + x5)
    
    VMOV.S32   d5,  #181
    VMOV.S32   d6,  #128
    VMLA.S32   d6,  d3, d5
    VSHR.S32   d3,  d6, #8                                                     @d3: x1 x2; Q1:x1 x2 x5 x4
    
    VADD.S32   Q3,  Q6,  Q1
    VSHR.S32   Q3,  Q3,  #14                                                   @Q3: x1 x2 x5 x4
    VSUB.S32   Q1,  Q6,  Q1
    VSHR.S32   Q0,  Q1,  #14                                                   @Q0: -x1 -x2 -x5 -x4
    
    VREV64.S32 d0,  d0                                                         @d0: -x4 -x5 
    VZIP.S32   d6,  d0                                                         @d6: -x5 x4; d0: -x4 x5
    VREV64.S32 d1,  d1                                                         @d1: -x2 -x1
    VZIP.S32   d7,  d1                                                         @d7: -x1 x2; d1: -x2 x1
    
    VLD1.8     d2,  [r2], r7
    
    VAND.S32   d3,  d2, d10
    VADD.S32   d6,  d6, d3                                                     @low: -x5 x4
    VCGT.U32   d3,  d6, d10
    VSHR.S32   d8,  d6, #31
    VEOR       d8,  d8, d9
    VBIT.32    d6,  d8, d3
    
    VSHR.S32   d3,  d2, #8
    VAND.S32   d3,  d3, d10                                                    @ -x1 x2
    VADD.S32   d7,  d7, d3
    VCGT.U32   d3,  d7, d10
    VSHR.S32   d8,  d7, #31
    VEOR       d8,  d8, d9
    VBIT.32    d7,  d8, d3
    
    VSHR.S32   d3,  d2, #16
    VAND.S32   d3,  d3, d10                                                    @-x2 x1
    VADD.S32   d1,  d1, d3
    VCGT.U32   d3,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d3
    
    VSHR.S32   d3,  d2, #24
    VAND.S32   d3,  d3, d10                                                    @-x4 x5
    VADD.S32   d0,  d0, d3
    VCGT.U32   d3,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d3
    
    VZIP.I8    d6,  d7                                                         @d6:...x2x4; d7:...-x1 -x5 
    VZIP.I8    d1,  d0                                                         @d1:...x5x1; d0:...-x4 -x2 
    VZIP.I16   d6,  d1                                                         @d6:...x5x1x2x4
    VZIP.I16   d7,  d0                                                         @d7:...-x4 -x2 -x1 -x5
    VZIP.I32   d6,  d7
    
    VST1.32    d6,  [r1], r3
    
    subs       r6,  r6, #1
    bgt        idct_row0x40zmv_loop
    
    ldmia      sp!, {r4 - r7, pc}
    @ENDP  @ |idct_row0x40zmv|
    
    .section .text
    .global	idct_row0x20zmv
idct_row0x20zmv:
    stmdb      sp!, {r4 - r7, lr}
    mov        r4,  #1088
    add        r4,  r4, #20
    VMOV.S32   d4[1], r4
    mov        r4,  #2624
    add        r4,  r4, #52
    VMOV.S32   d4[0], r4                                                       @d4: W6W2
    
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    VMOV.S32   d8,   #8192                                                     @d8: 8192
    
    mov        r4,  #0
    mov        r6,  #8                                                         @r6: i
    mov        r7,  #16
idct_row0x20zmv_loop:
    ldrsh      r5,  [r0, #4]!                                                  @r5: x2 = blk[2];
    strh       r4,  [r0], #12                                                  @blk[2] = 0;
    
    VDUP.S32   d2,  r5                                                         @d2: x2 x2
    
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d4
    VSHR.S32   d2,  d3,  #3                                                    @d2:x6 = (W6 * x2 + 4) >> 3;x2 = (W2 * x2 + 4) >> 3;
    
    VADD.S32   d0,  d2,  d8                                                    @d0: x4 x0
    VSUB.S32   d1,  d8,  d2                                                    @d1: x6 x2
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: x6 x2 x4 x0
    VZIP.S32   d0,  d1                                                         @d0: x2 x0; d1: x6 x4
    
    VLD1.8     d2,  [r2], r7
    
    VAND.S32   d6,  d2, d10
    VADD.S32   d6,  d0, d6                                                     @low:d6: x2 x0
    VCGT.U32   d3,  d6, d10
    VSHR.S32   d5,  d6, #31
    VEOR       d5,  d5, d9
    VBIT.32    d6,  d5, d3
    
    VSHR.S32   d7,  d2, #8
    VAND.S32   d7,  d7, d10
    VADD.S32   d7,  d7, d1                                                     @d7: x6 x4
    VCGT.U32   d3,  d7, d10
    VSHR.S32   d5,  d7, #31
    VEOR       d5,  d5, d9
    VBIT.32    d7,  d5, d3
    
    VREV64.S32 d1,  d1                                                         @d1:x4 x6
    VREV64.S32 d0,  d0                                                         @d0:x0 x2
    
    VSHR.S32   d5,  d2, #16
    VAND.S32   d5,  d5, d10
    VADD.S32   d1,  d1, d5                                                     @d1:x4 x6
    VCGT.U32   d3,  d1, d10
    VSHR.S32   d5,  d1, #31
    VEOR       d5,  d5, d9
    VBIT.32    d1,  d5, d3
    
    VSHR.S32   d5,  d2, #24
    VAND.S32   d5,  d5, d10                                                    @d0:x0 x2
    VADD.S32   d0,  d0, d5
    VCGT.U32   d3,  d0, d10
    VSHR.S32   d5,  d0, #31
    VEOR       d5,  d5, d9
    VBIT.32    d0,  d5, d3
    
    VZIP.I8    d6,  d7                                                         @d6:......x4x0;d7:......x6x2
    VZIP.I8    d1,  d0                                                         @d1:......x2x6;d0:......x0x4
    VZIP.I16   d6,  d1                                                         @d6:....x2x6x4x0
    VZIP.I16   d7,  d0                                                         @d7:....x0x4x6x2
    VZIP.I32   d6,  d7
    
    VST1.32    d6,  [r1], r3
    
    subs       r6,  r6, #1
    bgt        idct_row0x20zmv_loop
    
    ldmia      sp!, {r4 - r7, pc}
    @ENDP  @ |idct_row0x20zmv|
    
    .section .text
    .global	idct_row0x10zmv
idct_row0x10zmv:
    stmdb      sp!, {r4 - r7, lr}
    mov        r4,  #2400
    add        r4,  r4, #8
    VMOV.S32   d4[0], r4
    mov        r4,  #7
    sub        r4,  r4, #1616
    VMOV.S32   d4[1], r4                                                       @d4: (-W5)W3
    VMOV.S32   d10,  #0xff
    VMOV.I8    d9,   #0xff
    
    mov        r4,  #-181
    VMOV.S32   d11,  #181
    VMOV.S32   d11[0], r4
    
    VMOV.S32   Q6,  #8192                                                      @Q0: x0 x0 x0 x0
    mov        r5,  #0
    mov        r6,  #8                                                         @r6: i
    mov        r7,  #16
idct_row0x10zmv_loop:
    ldrsh      r4,  [r0, #6]!                                                  @r4: x3 = blk[3];
    VDUP.S32   d2,  r4                                                         @d2: x3 x3
    strh       r5,  [r0], #10
    
    VMOV.S32   d3,  #4
    VMLA.S32   d3,  d2,  d4
    VSHR.S32   d2,  d3,  #3                                                    @d2:x3 = (-W5 * x3 + 4) >> 3;x1 = (W3 * x3 + 4) >> 3;
    
    VPADDL.S32 d3,  d2                                                         @d3:(x3 + x1)
    VREV64.S32 d5,  d2                                                         @d5: x1 x3
    VSUB.S32   d5,  d5, d2                                                     @d5:(x3 - x1)
    VZIP.S32   d3,  d5                                                         @d3:(x3 - x1)(x3 + x1)
    
    VMOV.S32   d6,  #128
    VMLA.S32   d6,  d3, d11
    VSHR.S32   d3,  d6, #8                                                     @d3: x5 x7; Q1:x5 x7 x3 x1
    
    VADD.S32   Q3,  Q6,  Q1
    VSHR.S32   Q3,  Q3,  #14                                                   @Q3: x5 x7 x3 x1
    VSUB.S32   Q1,  Q6,  Q1
    VSHR.S32   Q0,  Q1,  #14                                                   @Q0: -x5 -x7 -x3 -x1
    
    VREV64.S32 d0,  d0                                                         @d0: -x1 -x3 
    VZIP.S32   d6,  d0                                                         @d6: -x3 x1; d0: -x1 x3
    VREV64.S32 d1,  d1                                                         @d1: -x7 -x5
    VZIP.S32   d7,  d1                                                         @d7: -x5 x7; d1: -x7 x5
    
    VLD1.8     d2,  [r2], r7
    
    VAND.S32   d3,  d2, d10
    VADD.S32   d6,  d6, d3                                                     @low: -x3 x1
    VCGT.U32   d3,  d6, d10
    VSHR.S32   d8,  d6, #31
    VEOR       d8,  d8, d9
    VBIT.32    d6,  d8, d3
    
    VSHR.S32   d3,  d2, #8
    VAND.S32   d3,  d3, d10                                                    @ -x5 x7
    VADD.S32   d7,  d7, d3
    VCGT.U32   d3,  d7, d10
    VSHR.S32   d8,  d7, #31
    VEOR       d8,  d8, d9
    VBIT.32    d7,  d8, d3
    
    VSHR.S32   d3,  d2, #16
    VAND.S32   d3,  d3, d10                                                    @-x7 x5
    VADD.S32   d1,  d1, d3
    VCGT.U32   d3,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d9
    VBIT.32    d1,  d8, d3
    
    VSHR.S32   d3,  d2, #24
    VAND.S32   d3,  d3, d10                                                    @-x1 x3
    VADD.S32   d0,  d0, d3
    VCGT.U32   d3,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d9
    VBIT.32    d0,  d8, d3
    
    VZIP.I8    d6,  d7                                                         @d6:...x7x1; d7:...-x5 -x3 
    VZIP.I8    d1,  d0                                                         @d1:...x3x5; d0:...-x1 -x7 
    VZIP.I16   d6,  d1                                                         @d6:...x3x5x7x1
    VZIP.I16   d7,  d0                                                         @d7:...-x1 -x7 -x5 -x3
    VZIP.I32   d6,  d7
    
    VST1.32    d6,  [r1], r3
    
    subs       r6,  r6, #1
    bgt        idct_row0x10zmv_loop
    
    ldmia      sp!, {r4 - r7, pc}
    @ENDP  @ |idct_row0x10zmv|
    
    .section .text
    .global	idct_rowzmv
idct_rowzmv:
    stmdb      sp!, {r4 - r5, lr}
    ldr        r5,  =2408
    ldr        r4,  =565
    VMOV.S32   d4,  r4,  r5                                                    @d4: W3W7
    ldr        r4,  =1108
    VDUP.S32   d5, r4                                                          @d5: W6W6
    
    ldr        r4,  =2841-565
    ldr        r5,  =-2841-565
    orr        r4,  r4, r5, lsl #16
    VMOV.S32   d12[0], r4
    ldr        r4,  =1609-2408
    lsl        r4,  r4, #16
    lsr        r4,  r4, #16
    ldr        r5,  =-1609-2408
    orr        r4,  r4, r5, lsl #16
    VMOV.S32   d12[1], r4                                                      @d12:(-(W3+W5))(-(W3-W5))(-(W1+W7))(W1-W7)
    
    ldr        r5,  =-2676-1108
    ldr        r4,  =2676-1108
    VMOV.S32   d13, r5, r4                                                     @d13: (W2-W6)(-(W2+W6))
    
    VMOV.S32   d10,  #0xff
    VMOV.I8    d11,  #0xff
    
    VMOV.I16   Q7,  #0
    mov        r4,  #16
    mov        r5,  #8                                                         @r5: i
idct_rowzmv_loop:
    VLD1.16    {d0, d1},   [r0]                                                @d0: x7x3x4(x0);d1: x5x2x6(x1)
    VST1.16    {d14, d15}, [r0], r4
    
    
    VADDL.S16  Q1,  d0,  d1                                                    @Q1: .(x3+x2).((x0)+(x1))
    VSUBL.S16  Q4,  d0,  d1                                                    @Q4: ...((x0)-(x1))
    VZIP.S32   d8,  d2                                                         @d8:(x0)+(x1))((x0)-(x1))
    VSHL.S32   d2,  d8,  #8
    VMOV.S32   d8,  #8192
    VADD.S32   d2,  d2,  d8                                                    @d2: x8 x0
    
    VREV64.S32 d0,  d0                                                         @d0: x4(x0)x7x3
    VADDL.S16  Q4,  d0,  d1                                                    @Q4: (x4+x5).(x7+x6).
    VZIP.S32   d9,  d8                                                         @d8: (x7+x6)(x4+x5)
    VDUP.S32   d9,  d3[0]                                                      @d9: (x3+x2)(x3+x2)
    
    VMOV.S32   Q3,  #4
    VMLA.S32   Q3,  Q4,  Q2                                                    @Q3: x1x1x8x8
    
    VZIP.S16   d0,  d1                                                         @d0: x6x7(x1)x3; d1:x5x4x2(x0)
    VREV32.S16 d0,  d0                                                         @d0: x7x6x3(x1)
    VZIP.S32   d1,  d0                                                         @d1: x3(x1)x2(x0); d0: x7x6x5x4
    VSHR.S32   d1,  d1,  #16                                                   @d1: x3x2
    VMLA.S32   d7,  d1, d13
    VSHR.S32   d7,  d7, #3                                                     @d7:x3x2
    
    VMULL.S16  Q4,  d0, d12                                                    @Q4: x7x6x5x4
    VDUP.S32   d1,  d6[0]                                                      @d8:x5x4 + d6[0];d9:x7x6 + d6[1]
    VADD.S32   d8,  d8, d1
    VDUP.S32   d1,  d6[1]
    VADD.S32   d9,  d9, d1
    VSHR.S32   Q4,  Q4, #3                                                     @Q4:x7x6x5x4
    
    VADD.S32   d0,  d8, d9                                                     @d0: x6x1
    VSUB.S32   d1,  d8, d9                                                     @d1: x5x4
    
    VADD.S32   d3,  d2, d7                                                     @d3: x7x3
    VSUB.S32   d2,  d2, d7                                                     @d2: x8x0
    
    VPADDL.S32 d6,  d1                                                         @d6: ..(x5+x4)
    VREV64.S32 d7,  d1                                                         @d7: x4x5
    VSUB.S32   d1,  d1, d7
    VZIP.S32   d6,  d1                                                         @d6:(x4 - x5)(x4 + x5)
    
    VMOV.S32   d7,  #181
    VMOV.S32   d1,  #128
    VMLA.S32   d1,  d6, d7
    VSHR.S32   d1,  d1, #8                                                     @d1: x4 x2 Q0: x4x2x6x1;  Q1:x7x3x8x0
    VZIP.S32   d1,  d0                                                         @d0: x6x4; d1: x1x2;      Q0:x1x2x6x4
    
    VADD.S32   Q3,  Q1,  Q0
    VSUB.S32   Q0,  Q1,  Q0
    VSHR.S32   Q0,  Q0,  #14                                                   @Q0: (x7-x1)(x3-x2)(x8-x6)(x0-x4)
    VSHR.S32   Q1,  Q3,  #14                                                   @Q1: (x7+x1)(x3+x2)(x8+x6)(x0+x4)
    
    
    VZIP.S32   d3,  d0                                                         @d3: (x0-x4)(x3+x2); d0: (x8-x6)((x7+x1)
    VZIP.S32   d2,  d1                                                         @d2: (x3-x2)(x0+x4); d1: (x7-x1)(x8+x6)
    
    VLD1.8     d6,  [r2], r4
    
    VAND.S32   d7,  d6, d10
    VADD.S32   d0,  d7, d0                                                     @d0: low: (x8-x6)(x7+x1)
    VCGT.U32   d7,  d0, d10
    VSHR.S32   d8,  d0, #31
    VEOR       d8,  d8, d11
    VBIT.32    d0,  d8, d7
    
    VSHR.S32   d7,  d6, #8
    VAND.S32   d7,  d7, d10                                                    @d3: (x0-x4)(x3+x2)
    VADD.S32   d3,  d7, d3
    VCGT.U32   d7,  d3, d10
    VSHR.S32   d8,  d3, #31
    VEOR       d8,  d8, d11
    VBIT.32    d3,  d8, d7
    
    VSHR.S32   d7,  d6, #16
    VAND.S32   d7,  d7, d10                                                    @d2: (x3-x2)(x0+x4)
    VADD.S32   d2,  d2, d7
    VCGT.U32   d7,  d2, d10
    VSHR.S32   d8,  d2, #31
    VEOR       d8,  d8, d11
    VBIT.32    d2,  d8, d7
    
    VSHR.S32   d7,  d6, #24
    VAND.S32   d7,  d7, d10                                                    @d1: (x7-x1)(x8+x6)
    VADD.S32   d1,  d1, d7
    VCGT.U32   d7,  d1, d10
    VSHR.S32   d8,  d1, #31
    VEOR       d8,  d8, d11
    VBIT.32    d1,  d8, d7
    
    VZIP.I8    d0,  d3                                                         @d0:(x3+x2)(x7+x1); d3:(x0-x4)(x8-x6)
    VZIP.I8    d2,  d1                                                         @d2:(x8+x6)(x0+x4); d1:(x7-x1)(x3-x2)
    VZIP.I16   d0,  d2                                                         @d0:(x8+x6)(x0+x4)(x3+x2)(x7+x1)
    VZIP.I16   d3,  d1                                                         @d3:(x7-x1)(x3-x2)(x0-x4)(x8-x6)
    VZIP.I32   d0,  d3
    
    VST1.32    d0,  [r1], r3
    
    subs       r5,  r5, #1
    bgt        idct_rowzmv_loop
    
    ldmia      sp!, {r4 - r5, pc}
    @ENDP  @ |idct_rowzmv|
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@  Function : BlockIDCTMotionComp
@  Date     : 5/11/2011
@  Purpose  : fast IDCT routine
@  In/out   :
@        Int* coeff_in   Dequantized coefficient
@        Int block_out   output IDCT coefficient
@        Int maxval      clip value
@  Modified :   7/31/01, add checking for all-zero and DC-only block.
@              do 8 columns at a time
@               8/2/01, do column first then row-IDCT.
@               8/2/01, remove clipping (included in motion comp).
@               8/7/01, combine with motion comp.
@               8/8/01, use AAN IDCT
@               9/4/05, use Chen's IDCT and 16 bit block
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .section .text
    .global	BlockIDCTMotionComp
    .extern   idct_col
    .extern   idctcolVCA
    .extern   idctrowVCAIntra
    .extern   idctrowVCAzmv
    .extern   idct_rowIntra
    .extern   idct_rowzmv
BlockIDCTMotionComp:
    stmdb      sp!, {r4 - r12, lr}
    @@@@@@@@data stack:(highend)lx_intra,pred,rec,r12-r4 lr(lowend)@@@@@@@@@
    mov        fp,  sp                               @set frame pointer reg
    
    ldr        r4,  [fp, #48]                        @r4: lx_intra
    asr        r5,  r4, #1                           @r5: Int lx = lx_intra >> 1;
    and        r4,  r4, #1                           @r4: Int intra = (lx_intra & 1);
    
    cmp        r3,  #0
    beq        BlockIDCTMotionComp_zero_block
    cmp        r2,  #0
    bne        BlockIDCTMotionComp_DC_block_test
BlockIDCTMotionComp_zero_block:
    cmp        r4,  #0
    beq        BlockIDCTMotionComp_copy_from_previous_frame
    ldr        r4,  [fp, #40]                        @r4:rec
    
    VMOV.U32   d0,  #0
    VST1.32    {d0}, [r4], r5                        @*((ULong*)rec) = *((ULong*)(rec + 4)) = 0; @rec += lx
    VST1.32    {d0}, [r4], r5
    VST1.32    {d0}, [r4], r5
    VST1.32    {d0}, [r4], r5
    VST1.32    {d0}, [r4], r5
    VST1.32    {d0}, [r4], r5
    VST1.32    {d0}, [r4], r5
    VST1.32    {d0}, [r4]
    b          BlockIDCTMotionComp_ret
    
BlockIDCTMotionComp_copy_from_previous_frame:
    ldr        r4,  [fp, #40]                        @r4:rec
    ldr        r6,  [fp, #44]                        @r6:pred
    mov        r7,  #16
    
    VLD1.32    d0,  [r6], r7
    VST1.32    d0,  [r4], r5
    
    VLD1.32    d0,  [r6], r7
    VST1.32    d0,  [r4], r5
    
    VLD1.32    d0,  [r6], r7
    VST1.32    d0,  [r4], r5
    
    VLD1.32    d0,  [r6], r7
    VST1.32    d0,  [r4], r5
    
    VLD1.32    d0,  [r6], r7
    VST1.32    d0,  [r4], r5
    
    VLD1.32    d0,  [r6], r7
    VST1.32    d0,  [r4], r5
    
    VLD1.32    d0,  [r6], r7
    VST1.32    d0,  [r4], r5
    
    VLD1.32    d0,  [r6]
    VST1.32    d0,  [r4]
    b          BlockIDCTMotionComp_ret
    
BlockIDCTMotionComp_DC_block_test:
    cmp        r3,  #1
    beq        BlockIDCTMotionComp_DC_block
    cmp        r2,  #0x80
    bne        BlockIDCTMotionComp_all
    ldrb       r6,  [r1]
    cmp        r6,  #0x80
    bne        BlockIDCTMotionComp_all
BlockIDCTMotionComp_DC_block:
    ldrsh      r6,  [r0]                             @r6: block[0]
    lsl        r6,  r6, #3
    add        r6,  r6, #32
    asr        r6,  r6, #6                           @r6: i
    mov        r7,  #0
    strh       r7,  [r0]
    
    cmp        r4,  #0
    beq        BlockIDCTMotionComp_DC_block_else
BlockIDCTMotionComp_DC_block_if:
    cmp        r6,  #0xff
    bls        BlockIDCTMotionComp_DC_block_todo
    mvn        r6,  r6, asr #31
    and        r6,  r6, #0xff
    
BlockIDCTMotionComp_DC_block_todo:
    VDUP.U8    d0,  r6
    
    ldr        r4,  [fp, #40]                        @r4:rec
    VST1.32    d0,  [r4], r5
    VST1.32    d0,  [r4], r5
    VST1.32    d0,  [r4], r5
    VST1.32    d0,  [r4], r5
    VST1.32    d0,  [r4], r5
    VST1.32    d0,  [r4], r5
    VST1.32    d0,  [r4], r5
    VST1.32    d0,  [r4]
    b          BlockIDCTMotionComp_ret
BlockIDCTMotionComp_DC_block_else:
    @@@@@@@@@@@@@@r5: lx; r6: i@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ldr        r4,  [fp, #40]                        @r4:rec
    ldr        r7,  [fp, #44]                        @r7:pred
    add        r8,  r4, r5, lsl #3                   @r8:endcol = rec + (lx << 3);
    
    VDUP.16    Q1,  r6                               @actually only low 16 bits
    VMOV.I16   Q2,  #0xff
    VMOV.I8    Q6,  #0xff
BlockIDCTMotionComp_DC_block_else_loop:
    VLD1.32    d0,  [r7]
    
    VADDW.U8   Q3,  Q1, d0
    VCGT.U16   Q4,  Q3, Q2
    VSHR.S16   Q5,  Q3, #15
    VEOR.S16   Q5,  Q5, Q6
    @VAND       Q5,  Q5, Q2
    VBIT.I16   Q3,  Q5, Q4
    
    VUZP.8     d6,  d7
    
    VST1.32    d6,  [r4]
    
    add        r4,  r4, r5
    add        r7,  r7, #16
    
    cmp        r4,  r8
    blt        BlockIDCTMotionComp_DC_block_else_loop
    b          BlockIDCTMotionComp_ret
    
BlockIDCTMotionComp_all:
    mov        r9,  r0                                  @r9: Short *ptr = block;
    mov        r6,  #0                                  @r6: i
BlockIDCTMotionComp_all_loop:
    ldrb       r7,  [r1, r6]                            @bmap = (Int)bitmapcol[i];
    cmp        r7,  #0
    beq        BlockIDCTMotionComp_all_loop_goon
    and        r8,  r7, #0xf
    cmp        r8,  #0
    bne        BlockIDCTMotionComp_all_loop_else
BlockIDCTMotionComp_all_loop_if:
    ldr        r10,  =idctcolVCA
    lsr        r8,  r7,  #4
    ldr        r10,  [r10, r8, lsl #2]
    stmdb      sp!, {r0}
    mov        r0,  r9
    blx        r10
    ldmia      sp!, {r0}
    b          BlockIDCTMotionComp_all_loop_goon
BlockIDCTMotionComp_all_loop_else:
    stmdb      sp!, {r0}
    mov        r0,  r9
    bl         idct_col
    ldmia      sp!, {r0}
    
BlockIDCTMotionComp_all_loop_goon:
    add        r9,  r9, #2
    add        r6,  r6, #1
    cmp        r6,  r3
    blt        BlockIDCTMotionComp_all_loop
    
    and        r6,  r2, #0xf
    cmp        r6,  #0
    bne        BlockIDCTMotionComp_all_else
BlockIDCTMotionComp_all_if:
    cmp        r4,  #0
    beq        BlockIDCTMotionComp_all_if_else
BlockIDCTMotionComp_all_if_if:
    ldr        r10,  =idctrowVCAIntra
    lsr        r2,   r2,  #4
    ldr        r10,  [r10, r2, lsl #2]
    stmdb      sp!, {r0 - r2}
    ldr        r1,  [fp, #40]                 @rec
    mov        r2,  r5
    blx        r10
    ldmia      sp!, {r0 - r2}
    b          BlockIDCTMotionComp_ret
BlockIDCTMotionComp_all_if_else:
    ldr        r10,  =idctrowVCAzmv
    lsr        r2,   r2,  #4
    ldr        r10,  [r10, r2, lsl #2]
    stmdb      sp!, {r0 - r3}
    ldr        r1,  [fp, #40]                 @rec
    ldr        r2,  [fp, #44]
    mov        r3,  r5
    blx        r10
    ldmia      sp!, {r0 - r3}
    b          BlockIDCTMotionComp_ret
BlockIDCTMotionComp_all_else:
    cmp        r4,  #0
    beq        BlockIDCTMotionComp_all_else_else
BlockIDCTMotionComp_all_else_if:
    stmdb      sp!, {r0 - r2}
    ldr        r1,  [fp, #40]                 @rec
    mov        r2,  r5
    bl         idct_rowIntra
    ldmia      sp!, {r0 - r2}
    b          BlockIDCTMotionComp_ret
BlockIDCTMotionComp_all_else_else:
    stmdb      sp!, {r0 - r3}
    ldr        r1,  [fp, #40]                 @rec
    ldr        r2,  [fp, #44]
    mov        r3,  r5
    bl         idct_rowzmv
    ldmia      sp!, {r0 - r3}
BlockIDCTMotionComp_ret:
    mov        sp,  fp
    ldmia      sp!, {r4 - r12, pc}
    @ENDP  @ |BlockIDCTMotionComp|
    
    .end
    
    