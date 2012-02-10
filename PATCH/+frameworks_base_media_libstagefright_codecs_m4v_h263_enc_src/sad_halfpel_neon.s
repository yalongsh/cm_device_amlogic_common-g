
    .section .text
    .global	HalfPel1_SAD_MB
HalfPel1_SAD_MB:
    stmdb      sp!, {r4 - r6, r11, lr}
    @@@@@@@@data stack:(highend)jh,ih,lr,r11,r6-r4(lowend)@@@@@@@@@
    mov        fp, sp                               @set frame pointer reg
    
    ldr        r5,  [fp, #20]                       @r5: ih
    cmp        r5,  #0
    beq        HalfPel1_SAD_MB_ELSE
HalfPel1_SAD_MB_IF:
    add        r4,  r0, #1                          @r4: p2 = ref + 1;
    b          HalfPel1_SAD_MB_GO_ON
HalfPel1_SAD_MB_ELSE:
    add        r4,  r0, r3                          @r4: p2 = ref + width;
    
HalfPel1_SAD_MB_GO_ON:
    VMOV.I64   d10, #0                              @d10:sad=0;
    mov        r5,  #16                             @r5:i
HalfPel1_SAD_MB_loop:
    VLD1.8     {d0, d1}, [r1]                       @Q0: kk(15 - 0)
    VLD1.8     {d2, d3}, [r0]                       @Q1: p1(15 - 0)
    VLD1.8     {d4, d5}, [r4]                       @Q2: p2(15 - 0)
    
    VADDL.U8   Q3,  d2, d4                          @low end
    VADDL.U8   Q4,  d3, d5                          @high end
    
    VMOV.U16   Q1,  #1
    VADD.U16   Q3,  Q3, Q1
    VADD.U16   Q4,  Q4, Q1
    
    VSHR.U16   Q3,  Q3, #1
    VSHR.U16   Q4,  Q4, #1
    VSUBW.U8   Q3,  Q3, d0
    VSUBW.U8   Q4,  Q4, d1
    VABS.S16   Q3,  Q3
    VABS.S16   Q4,  Q4
    
    VADD.U16   Q3,  Q3, Q4
    VADD.U16   d6,  d6, d7
    VPADDL.U16 d6,  d6
    VADD.U32   d10, d6, d10
    VPADDL.U32 d10, d10
    
    add        r0,  r0, r3                          @p1 += width;
    add        r4,  r4, r3                          @p3 += width;
    add        r1,  r1, #16
    
    VMOV.U32   r6,  d10[0]                          @r6: sad
    cmp        r6,  r2
    bgt        HalfPel1_SAD_MB_ret
    subs       r5,  r5, #1
    bgt        HalfPel1_SAD_MB_loop
HalfPel1_SAD_MB_ret:
    mov        r0,  r6
    ldmia      sp!, {r4 - r6, r11, pc}
    @ENDP  @ |HalfPel1_SAD_MB|




    .section .text
    .global	HalfPel1_SAD_Blk
HalfPel1_SAD_Blk:
    stmdb      sp!, {r4 - r6, r11, lr}
    @@@@@@@@data stack:(highend)jh,ih,lr,r11,r6-r4(lowend)@@@@@@@@@
    mov        fp, sp                               @set frame pointer reg
    
    ldr        r5,  [fp, #20]                       @r5: ih
    cmp        r5,  #0
    beq        HalfPel1_SAD_Blk_ELSE
HalfPel1_SAD_Blk_IF:
    add        r4,  r0, #1                          @r4: p2 = ref + 1;
    b          HalfPel1_SAD_Blk_GO_ON
HalfPel1_SAD_Blk_ELSE:
    add        r4,  r0, r3                          @r4: p2 = ref + width;

HalfPel1_SAD_Blk_GO_ON:
    VMOV.I64   d10, #0                              @d10:sad=0;
    mov        r5,  #8                              @r5:i
HalfPel1_SAD_Blk_loop:
    VLD1.8     {d0}, [r1]                           @d0: kk(7 - 0)
    VLD1.8     {d2}, [r0]                           @d2: p1(7 - 0)
    VLD1.8     {d3}, [r4]                           @d0: p2(7 - 0)
    
    VADDL.U8   Q1,  d2, d3                          @p1[j] + p2[j]
    VMOV.U16   Q2,  #1
    VADD.U16   Q1,  Q1, Q2
    VSHR.U16   Q1,  Q1, #1
    VSUBW.U8   Q1,  Q1, d0    
    VABS.S16   Q1,  Q1
    
    VADD.U16   d1,  d2, d3
    VPADDL.U16 d1,  d1
    VADD.U32   d10,  d1, d10
    VPADDL.U32 d10, d10
    
    add        r0,  r0, r3                          @p1 += width;
    add        r4,  r4, r3                          @p2 += width;
    add        r1,  r1, #16
    
    VMOV.U32   r6,  d10[0]                          @r6: sad
    cmp        r6,  r2
    bgt        HalfPel1_SAD_Blk_ret
    subs       r5,  r5, #1
    bgt        HalfPel1_SAD_Blk_loop
HalfPel1_SAD_Blk_ret:
    mov        r0,  r6
    mov        sp, fp
    ldmia      sp!, {r4 - r6, r11, pc}
    @ENDP  @ |HalfPel1_SAD_Blk|

    .section .text
    .global	HalfPel2_SAD_MB
HalfPel2_SAD_MB:
    stmdb      sp!, {r4 - r6, lr}
    add        r4,  r0, r3                          @r4: p3 = ref + width;
    
    VMOV.I64   d10, #0                              @d10:sad=0;
    mov        r5,  #16                             @r5:i
HalfPel2_SAD_MB_loop:
    VLD1.8     {d0, d1}, [r1]                       @Q0: kk(15 - 0)
    VLD1.8     {d2, d3, d4, d5}, [r0]               @Q1: p1(15 - 0)
    
    VMOV.U64   d7,  d4
    VMOV.U64   d6,  d3                              @Q3: p1(23 - 8)
    VSHR.U64   Q2, Q1, #8                           @Q2: 0 p1(15 - 9) 0 p1(7 - 1)
    VSLI.U64   Q2, Q3, #56                          @Q2: p1(16 - 1)
    
    VADDL.U8   Q3,  d2, d4                          @low end
    VADDL.U8   Q4,  d3, d5                          @high end
    
    VLD1.8     {d2, d3, d4, d5}, [r4]               @Q1: p3(15 - 0)
    VMOV.U64   d13,  d4
    VMOV.U64   d12,  d3                             @Q6: p3(23 - 8)
    VSHR.U64   Q2, Q1, #8                           @Q2: 0 p3(15 - 9) 0 p3(7 - 1)
    VSLI.U64   Q2, Q6, #56                          @Q2: p3(16 - 1)
    
    VADDL.U8   Q6,  d2, d4                          @low end
    VADDL.U8   Q7,  d3, d5                          @high end
    
    VADD.U16   Q3,  Q3, Q6
    VADD.U16   Q4,  Q4, Q7
    
    VMOV.U16   Q1,  #2
    VADD.U16   Q3,  Q3, Q1
    VADD.U16   Q4,  Q4, Q1
    VSHR.U16   Q3,  Q3, #2
    VSHR.U16   Q4,  Q4, #2
    VSUBW.U8   Q3,  Q3, d0
    VSUBW.U8   Q4,  Q4, d1
    VABS.S16   Q3,  Q3
    VABS.S16   Q4,  Q4
    
    VADD.U16   Q3,  Q3, Q4
    VADD.U16   d6,  d6, d7
    VPADDL.U16 d6,  d6
    VADD.U32   d10, d6, d10
    VPADDL.U32 d10, d10
    
    add        r0,  r0, r3                          @p1 += width;
    add        r4,  r4, r3                          @p3 += width;
    add        r1,  r1, #16
    
    VMOV.U32   r6,  d10[0]                          @r6: sad
    cmp        r6,  r2
    bgt        HalfPel2_SAD_MB_ret
    subs       r5,  r5, #1
    bgt        HalfPel2_SAD_MB_loop
HalfPel2_SAD_MB_ret:
    mov        r0,  r6
    ldmia      sp!, {r4 - r6, pc}
    @ENDP  @ |HalfPel2_SAD_MB|

    .section .text
    .global	HalfPel2_SAD_Blk
HalfPel2_SAD_Blk:
    stmdb      sp!, {r4 - r6, lr}
    add        r4,  r0, r3                          @r4: p3 = ref + width;
    
    VMOV.I64   d10, #0                              @d10:sad=0;
    mov        r5,  #8                              @r5:i
HalfPel2_SAD_Blk_loop:
    VLD1.8     {d0}, [r1]                           @d0: kk(7 - 0)
    VLD1.8     {d2, d3}, [r0]                       @d2: p1(7 - 0); d3: p1(15 - 8)
    VSHR.U64   Q2, Q1, #8                           @d4: 0 p1(7 - 1)
    VSLI.U64   d4, d3, #56                          @d4: p2(7 - 0)/p1(8 - 1)
    
    VLD1.8     {d6, d7}, [r4]                       @d6: p3(7 - 0); d7: p3(15 - 8)
    VSHR.U64   Q4, Q3, #8                           @d8: 0 p3(7 - 1)
    VSLI.U64   d8, d7, #56                          @d8: p4(7 - 0)/p4(8 - 1)
    
    VADDL.U8   Q1,  d2, d4
    VADDL.U8   Q2,  d6, d8                          @p1[j] + p2[j] + p3[j] + p4[j]
    VADD.U16   Q1,  Q1, Q2
    
    VMOV.U16   Q2,  #2
    VADD.U16   Q1,  Q1, Q2
    VSHR.U16   Q1,  Q1, #2
    VSUBW.U8   Q1,  Q1, d0    
    VABS.S16   Q1,  Q1
    
    VADD.U16   d1,  d2, d3
    VPADDL.U16 d1,  d1
    VADD.U32   d10,  d1, d10
    VPADDL.U32 d10, d10
    
    add        r0,  r0, r3                          @p1 += width;
    add        r4,  r4, r3                          @p3 += width;
    add        r1,  r1, #16
    
    VMOV.U32   r6,  d10[0]                          @r6: sad
    cmp        r6,  r2
    bgt        HalfPel2_SAD_Blk_ret
    subs       r5,  r5, #1
    bgt        HalfPel2_SAD_Blk_loop
HalfPel2_SAD_Blk_ret:
    mov        r0,  r6
    ldmia      sp!, {r4 - r6, pc}
    @ENDP  @ |HalfPel2_SAD_Blk|


    .section .text
    .global	SAD_MB_HalfPel_Cxhyh
SAD_MB_HalfPel_Cxhyh:
    stmdb      sp!, {r4 - r7, lr}
    mvn        r4,  #0
    lsl        r4,  #16
    and        r4,  r2, r4, lsr #16                 @r4:Int rx = dmin_rx & 0xFFFF;
    
    add        r7,  r0, r4                          @r7: p3 = ref + rx;
    
    VMOV.I64   d10, #0                              @d10:sad=0;
    mov        r5,  #16                             @r5:i
SAD_MB_HalfPel_Cxhyh_loop:
    VLD1.8     {d0, d1}, [r1]                       @Q0: kk(15 - 0)
    VLD1.8     {d2, d3, d4, d5}, [r0]               @Q1: p1(15 - 0)
    
    VMOV.U64   d7,  d4
    VMOV.U64   d6,  d3                              @Q3: p1(23 - 8)
    VSHR.U64   Q2, Q1, #8                           @Q2: 0 p1(15 - 9) 0 p1(7 - 1)
    VSLI.U64   Q2, Q3, #56                          @Q2: p1(16 - 1)
    
    VADDL.U8   Q3,  d2, d4                          @low end
    VADDL.U8   Q4,  d3, d5                          @high end
    
    VLD1.8     {d2, d3, d4, d5}, [r7]               @Q1: p3(15 - 0)
    VMOV.U64   d13,  d4
    VMOV.U64   d12,  d3                             @Q6: p3(23 - 8)
    VSHR.U64   Q2, Q1, #8                           @Q2: 0 p3(15 - 9) 0 p3(7 - 1)
    VSLI.U64   Q2, Q6, #56                          @Q2: p3(16 - 1)
    
    VADDL.U8   Q6,  d2, d4                          @low end
    VADDL.U8   Q7,  d3, d5                          @high end
    
    VADD.U16   Q3,  Q3, Q6
    VADD.U16   Q4,  Q4, Q7
    
    VMOV.U16   Q1,  #2
    VADD.U16   Q3,  Q3, Q1
    VADD.U16   Q4,  Q4, Q1
    VSHR.U16   Q3,  Q3, #2
    VSHR.U16   Q4,  Q4, #2
    VSUBW.U8   Q3,  Q3, d0
    VSUBW.U8   Q4,  Q4, d1
    VABS.S16   Q3,  Q3
    VABS.S16   Q4,  Q4
    
    VADD.U16   Q3,  Q3, Q4
    VADD.U16   d6,  d6, d7
    VPADDL.U16 d6,  d6
    VADD.U32   d10, d6, d10
    VPADDL.U32 d10, d10
    
    add        r0,  r0, r4                          @p1 += rx;
    add        r7,  r7, r4                          @p3 += rx;
    add        r1,  r1, #16
    
    VMOV.U32   r6,  d10[0]                          @r6: sad
    cmp        r6,  r2, lsr #16
    bgt        SAD_MB_HalfPel_Cxhyh_ret
    subs       r5,  r5, #1
    bgt        SAD_MB_HalfPel_Cxhyh_loop
SAD_MB_HalfPel_Cxhyh_ret:
    mov        r0,  r6
    ldmia      sp!, {r4 - r7, pc}
    @ENDP  @ |SAD_MB_HalfPel_Cxhyh|


    .section .text
    .global	SAD_MB_HalfPel_Cyh
SAD_MB_HalfPel_Cyh:
    stmdb      sp!, {r4 - r7, lr}
    mvn        r4,  #0
    lsl        r4,  #16
    and        r4,  r2, r4, lsr #16                 @r4:Int rx = dmin_rx & 0xFFFF;
    add        r7,  r0, r4                          @p2 = ref + rx;
    
    VMOV.I64   d10, #0                              @d10:sad=0;
    mov        r5,  #16                             @r5:i
SAD_MB_HalfPel_Cyh_loop:
    VLD1.8     {d0, d1}, [r1]                       @Q0: kk(15 - 0)
    VLD1.8     {d2, d3}, [r0]                       @Q1: p1(15 - 0)
    VLD1.8     {d4, d5}, [r7]                       @Q2: p2(15 - 0)
    
    VADDL.U8   Q3,  d2, d4                          @low end
    VADDL.U8   Q4,  d3, d5                          @high end
    
    VMOV.U16   Q1,  #1
    VADD.U16   Q3,  Q3, Q1
    VADD.U16   Q4,  Q4, Q1
    VSHR.U16   Q3,  Q3, #1
    VSHR.U16   Q4,  Q4, #1
    VSUBW.U8   Q3,  Q3, d0
    VSUBW.U8   Q4,  Q4, d1
    VABS.S16   Q3,  Q3
    VABS.S16   Q4,  Q4
    
    VADD.U16   Q3,  Q3, Q4
    VADD.U16   d6,  d6, d7
    VPADDL.U16 d6,  d6
    VADD.U32   d10, d6, d10
    VPADDL.U32 d10, d10
    
    add        r0,  r0, r4                          @p1 += rx;
    add        r7,  r7, r4                          @p2 += rx;
    add        r1,  r1, #16
    
    VMOV.U32   r6,  d10[0]                          @r6: sad
    cmp        r6,  r2, lsr #16
    bgt        SAD_MB_HalfPel_Cyh_ret
    subs       r5,  r5, #1
    bgt        SAD_MB_HalfPel_Cyh_loop
SAD_MB_HalfPel_Cyh_ret:
    mov        r0,  r6
    ldmia      sp!, {r4 - r7, pc}
    @ENDP  @ |SAD_MB_HalfPel_Cyh|





    .section .text
    .global	SAD_MB_HalfPel_Cxh
SAD_MB_HalfPel_Cxh:
    stmdb      sp!, {r4 - r6, lr}
    mvn        r4,  #0
    lsl        r4,  #16
    and        r4,  r2, r4, lsr #16                 @r4:Int rx = dmin_rx & 0xFFFF;
    
    VMOV.I64   d10, #0                              @d10:sad=0;
    mov        r5,  #16                             @r5:i
SAD_MB_HalfPel_Cxh_loop:
    VLD1.8     {d0, d1}, [r1]                       @Q0: kk(15 - 0)
    VLD1.8     {d2, d3, d4, d5}, [r0]               @Q1: p1(15 - 0)
    @@@@@@@@@@@@@@@@@align issue@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @VLD1.8     {d4, d5}, [r0, #1]                   @Q2: p1(16 - 1)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VMOV.U64   d7,  d4
    VMOV.U64   d6,  d3                             @Q3: p1(23 - 8)
    VSHR.U64   Q2, Q1, #8                          @Q2: 0 p1(15 - 9) 0 p1(7 - 1)
    VSLI.U64   Q2, Q3, #56                         @Q2: p1(16 - 1)
    
    VADDL.U8   Q3,  d2, d4                          @low end
    VADDL.U8   Q4,  d3, d5                          @high end
    
    VMOV.U16   Q1,  #1
    VADD.U16   Q3,  Q3, Q1
    VADD.U16   Q4,  Q4, Q1
    VSHR.U16   Q3,  Q3, #1
    VSHR.U16   Q4,  Q4, #1
    VSUBW.U8   Q3,  Q3, d0
    VSUBW.U8   Q4,  Q4, d1
    VABS.S16   Q3,  Q3
    VABS.S16   Q4,  Q4
    
    VADD.U16   Q3,  Q3, Q4
    VADD.U16   d6,  d6, d7
    VPADDL.U16 d6,  d6
    VADD.U32   d10, d6, d10
    VPADDL.U32 d10, d10
    
    add        r0,  r0, r4                          @p1 += rx;
    add        r1,  r1, #16
    
    VMOV.U32   r6,  d10[0]                          @r6: sad
    cmp        r6,  r2, lsr #16
    bgt        SAD_MB_HalfPel_Cxh_ret
    subs       r5,  r5, #1
    bgt        SAD_MB_HalfPel_Cxh_loop
SAD_MB_HalfPel_Cxh_ret:
    mov        r0,  r6
    ldmia      sp!, {r4 - r6, pc}
    @ENDP  @ |SAD_MB_HalfPel_Cxh|
    
    

    .section .text
    .global	SAD_MB_HP_HTFM_Collectxhyh
SAD_MB_HP_HTFM_Collectxhyh:
    stmdb      sp!, {r4 - r12, lr}
    @@@@@@@@@@@@@@@@@@@allocate local variable here@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    sub        fp, sp, #64                           @fp: r11, move to low end
    sub        sp, sp, #64                           @int saddata[16]
    
    mvn        r4,  #0
    lsl        r4,  r4, #16
    and        r4,  r2, r4, lsr #16                  @r4:Int rx=dmin_rx&0xFFFF;
    lsl        r5,  r4, #2                           @r5:Int refwx4 = rx << 2;
    
    @@@@@@@@@@@@@r3: int *abs_dif_mad_avg = &(htfm_stat->abs_dif_mad_avg);@@@@@
    add        r7,  r3, #4                           @r7: UInt *countbreak = &(htfm_stat->countbreak);
    add        r6,  r3, #72                          @r6: Int *offsetRef = htfm_stat->offsetRef;
    
    VMOV.I64   d8, #0                                @d5: Int sad = 0;
    mov        r8,  #0                               @r8: i
SAD_MB_HP_HTFM_Collectxhyh_loop:
    ldr        r9,  [r6, r8, lsl #2]                 @r9:offsetRef[i];
    add        r9,  r9, r0                           @r9:p1 = ref + offsetRef[i];
    add        r12, r9, r4                           @r12:p2 = p1 + rx;
    
    mov        r10, #2
SAD_MB_HP_HTFM_Collectxhyh_loop_inner:
    VLD1.8     {d0},  [r1]                           @d0:cur_word[1-0]
    
    VLD2.16    {d2, d3},  [r9]                       @d2:p1[13]p1[12]p1[9]p1[8]p1[5]p1[4]p1[1]p1[0]
    VLD2.16    {d3, d4},  [r12]                      @d3:p2[13]p2[12]p2[9]p2[8]p2[5]p2[4]p2[1]p2[0]
    
    VADDL.U8   Q2, d2, d3                            @Q2:tmp = p1[12] + p2[12]; tmp2 = p1[13] + p2[13]; ...
    VPADD.U16  d6, d4, d5                            @d4: tmp += tmp2;
    VMOV.U16   d5, #2
    VADD.U16   d6, d6, d5                            @tmp += 2;
    
    add        r9, r9, r5                            @p1 += refwx4;
    add        r12, r12, r5                          @p2 += refwx4;
    
    VLD2.16    {d2, d3},  [r9]                       @d2:p1[13]p1[12]p1[9]p1[8]p1[5]p1[4]p1[1]p1[0]
    VLD2.16    {d3, d4},  [r12]                      @d3:p2[13]p2[12]p2[9]p2[8]p2[5]p2[4]p2[1]p2[0]
    
    VADDL.U8   Q2, d2, d3                            @Q2:tmp = p1[12] + p2[12]; tmp2 = p1[13] + p2[13]; ...
    VPADD.U16  d7, d4, d5                            @d4: tmp += tmp2;
    VMOV.U16   d5, #2
    VADD.U16   d7, d7, d5                            @tmp += 2;
    
    
    VSHR.U16   Q3, Q3, #2
    VSUBW.U8   Q3, Q3, d0
    VABS.S16   Q3, Q3
    
    VPADD.U16   d2, d6, d7
    VPADDL.U16  d2, d2
    VADD.U32    d8, d2, d8
    VPADDL.U32  d8, d8
    
    add        r9, r9, r5                            @p1 += refwx4;
    add        r12, r12, r5                          @p2 += refwx4;
    add        r1, r1, #8
    
    subs       r10, r10, #1
    bgt        SAD_MB_HP_HTFM_Collectxhyh_loop_inner
    
    VMOV.U32   r10, d8[0]                            @r10:sad
    str        r10, [fp, r8, lsl #2]                 @saddata[i] = sad;
    
    cmp        r8, #0
    ble        SAD_MB_HP_HTFM_Collectxhyh_check_loop
    cmp        r10, r2, lsr #16                      @if ((ULong)sad > ((ULong)dmin_lx >> 16))
    bgt        SAD_MB_HP_HTFM_Collectxhyh_store
SAD_MB_HP_HTFM_Collectxhyh_check_loop:
    add        r8, r8, #1
    cmp        r8, #16
    blt        SAD_MB_HP_HTFM_Collectxhyh_loop
    
SAD_MB_HP_HTFM_Collectxhyh_store:
    ldmia      fp,  {r8, r9}                          @r8: saddata[0]; r9:saddata[1]
    add        r9, r9, #1                             @(saddata[1] + 1)
    
    subs       r8,  r8, r9, lsr #1                    @r8: difmad = saddata[0] - ((saddata[1] + 1) >> 1);
    bgt        SAD_MB_HP_HTFM_Collectxhyh_POSITIVE_STORE
    mov        r9,  #0
    sub        r8,  r9, r8
SAD_MB_HP_HTFM_Collectxhyh_POSITIVE_STORE:
    ldr        r9, [r3]
    add        r8,  r8, r9
    str        r8,  [r3]                              @(*abs_dif_mad_avg) += ((difmad > 0) ? difmad : -difmad);
    ldr        r9, [r7]
    add        r9, r9, #1
    str        r9, [r7]                               @(*countbreak)++;
    mov        r0, r10
    
    add        sp,  fp, #64                           @recover sp
    ldmia      sp!, {r4 - r12, pc}
    @ENDP  @ |SAD_MB_HP_HTFM_Collectxhyh|
    
    
    .section .text
    .global	SAD_MB_HP_HTFM_Collectyh
SAD_MB_HP_HTFM_Collectyh:
    stmdb      sp!, {r4 - r12, lr}
    @@@@@@@@@@@@@@@@@@@allocate local variable here@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    sub        fp, sp, #64                           @fp: r11, move to low end
    sub        sp, sp, #64                           @int saddata[16]
    
    mvn        r4,  #0
    lsl        r4,  r4, #16
    and        r4,  r2, r4, lsr #16                  @r4:Int rx=dmin_rx&0xFFFF;
    lsl        r5,  r4, #2                           @r5:Int refwx4 = rx << 2;
    
    @@@@@@@@@@@@@r3: int *abs_dif_mad_avg = &(htfm_stat->abs_dif_mad_avg);@@@@@
    add        r7,  r3, #4                           @r6: UInt *countbreak = &(htfm_stat->countbreak);
    add        r6,  r3, #72                          @r5: Int *offsetRef = htfm_stat->offsetRef;
    
    VMOV.I64   d8, #0                                @d5: Int sad = 0;
    mov        r8,  #0                               @r8: i
SAD_MB_HP_HTFM_Collectyh_loop:
    ldr        r9,  [r6, r8, lsl #2]                 @r9:offsetRef[i];
    add        r9,  r9, r0                           @r9:p1 = ref + offsetRef[i];
    add        r12, r9, r4                           @r12:p2 = p1 + rx;
    
    mov        r10, #2
SAD_MB_HP_HTFM_Collectyh_loop_inner:
    VLD1.8     {d0},  [r1]                           @d0:cur_word[1-0]
    VLD4.8     {d1 - d4},  [r12]                     @d1:p2[28]p1[24]p2[20]p2[16]p2[12]p2[8]p2[4]p2[0]
    VLD4.8     {d2 - d5},  [r9]                      @d2:p1[28]p1[24]p1[20]p1[16]p1[12]p1[8]p1[4]p1[0]
    VZIP.U8    d2, d1                                @d2:p2[12]p1[12]p2[8]p1[8]p2[4]p1[4]p2[0]p1[0]
    
    VPADDL.U8  d2, d2                                @d2:(p2[12]+p1[12])(p2[8]+p1[8])(p2[4]+p1[4])(p2[0]+p1[0])
    VMOV.U16   d3, #1
    VADD.U16   d2, d2, d3
    
    add        r9, r9, r5                            @p1 += refwx4;
    add        r12, r12, r5                          @p2 += refwx4;
    
    VLD4.8     {d3 - d6},  [r9]                      @d3:p1[28]p1[24]p1[20]p1[16]p1[12]p1[8]p1[4]p1[0]
    VLD4.8     {d4 - d7},  [r12]                     @d4:p2[28]p1[24]p2[20]p2[16]p2[12]p2[8]p1[4]p2[0]
    VZIP.U8    d3, d4                                @d3:p2[12]p2[8]p1[4]p2[0]p1[12]p1[8]p1[4]p1[0]
    
    VPADDL.U8  d3, d3                                @d3:(p2[12]+p1[12])(p2[8]+p1[8])(p2[4]+p1[4])(p2[0]+p1[0])
    VMOV.U16   d4, #1
    VADD.U16   d3, d3, d4
    
    VSHR.U16   Q1, Q1, #1
    VSUBW.U8   Q1, Q1, d0
    VABS.S16   Q1, Q1
    
    VPADD.U16   d2, d2, d3
    VPADDL.U16  d2, d2
    VADD.U32    d8, d2, d8
    VPADDL.U32  d8, d8
    
    add        r9, r9, r5                            @p1 += refwx4;
    add        r12, r12, r5                          @p2 += refwx4;
    add        r1, r1, #8
    
    subs       r10, r10, #1
    bgt        SAD_MB_HP_HTFM_Collectyh_loop_inner
    
    VMOV.U32   r10, d8[0]                            @r10:sad
    str        r10, [fp, r8, lsl #2]                 @saddata[i] = sad;
    
    cmp        r8, #0
    ble        SAD_MB_HP_HTFM_Collectyh_check_loop
    cmp        r10, r2, lsr #16                      @if ((ULong)sad > ((ULong)dmin_lx >> 16))
    bgt        SAD_MB_HP_HTFM_Collectyh_store
SAD_MB_HP_HTFM_Collectyh_check_loop:
    add        r8, r8, #1
    cmp        r8, #16
    blt        SAD_MB_HP_HTFM_Collectyh_loop
    
SAD_MB_HP_HTFM_Collectyh_store:
    ldmia      fp,  {r8, r9}                          @r8: saddata[0]; r9:saddata[1]
    add        r9, r9, #1                             @(saddata[1] + 1)
    
    subs       r8,  r8, r9, lsr #1                    @r8: difmad = saddata[0] - ((saddata[1] + 1) >> 1);
    bgt        SAD_MB_HP_HTFM_Collectyh_POSITIVE_STORE
    mov        r9,  #0
    sub        r8,  r9, r8
SAD_MB_HP_HTFM_Collectyh_POSITIVE_STORE:
    ldr        r9, [r3]
    add        r8,  r8, r9
    str        r8,  [r3]                              @(*abs_dif_mad_avg) += ((difmad > 0) ? difmad : -difmad);
    ldr        r9, [r7]
    add        r9, r9, #1
    str        r9, [r7]                              @(*countbreak)++;
    mov        r0, r10
    
    add        sp, fp, #64                            @recover sp
    ldmia      sp!, {r4 - r12, pc}
    @ENDP  @ |SAD_MB_HP_HTFM_Collectyh|
    
    
    
    .section .text
    .global	SAD_MB_HP_HTFM_Collectxh
SAD_MB_HP_HTFM_Collectxh:
    stmdb      sp!, {r4 - r12, lr}
    @@@@@@@@@@@@@@@@@@@allocate local variable here@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    sub        fp, sp, #64                           @fp: r11, move to low end
    sub        sp, sp, #64                           @int saddata[16]
    
    mvn        r4,  #0
    lsl        r4,  r4, #16
    and        r4,  r2, r4, lsr #16                  @r4:Int rx=dmin_rx&0xFFFF;
    lsl        r5,  r4, #2                           @r5:Int refwx4 = rx << 2;
    
    @@@@@@@@@@@@@r3: int *abs_dif_mad_avg = &(htfm_stat->abs_dif_mad_avg);@@@@@
    add        r7,  r3, #4                           @r6: UInt *countbreak = &(htfm_stat->countbreak);
    add        r6,  r3, #72                          @r5: Int *offsetRef = htfm_stat->offsetRef;
    
    VMOV.I64   d5, #0                                @d5: Int sad = 0;
    mov        r8,  #0                               @r8: i
SAD_MB_HP_HTFM_Collectxh_loop:
    ldr        r9,  [r6, r8, lsl #2]                 @r9:offsetRef[i];
    add        r9,  r9, r0                           @r9:p1 = ref + offsetRef[i];
    
    mov        r10, #2
SAD_MB_HP_HTFM_Collectxh_loop_inner:
    VLD1.8     {d0},  [r1]                           @d0:cur_word[1-0]
    VLD2.16    {d2, d3},  [r9]                       @d2:p1[13]p1[12]p1[9]p1[8]p1[5]p1[4]p1[1]p1[0]
    VPADDL.U8  d2, d2                                @d2:(p1[13]+p1[12])(p1[9]+p1[8])(p1[5]+p1[4])(p1[1]+p1[0])
    VMOV.U16   d3, #1
    VADD.U16   d2, d2, d3
    
    add        r9, r9, r5                            @p1 += refwx4;
    VLD2.16    {d3, d4},  [r9]                       @d3:p1[13]p1[12]p1[9]p1[8]p1[5]p1[4]p1[1]p1[0]
    VPADDL.U8  d3, d3                                @d3:(p1[13]+p1[12])(p1[9]+p1[8])(p1[5]+p1[4])(p1[1]+p1[0])
    VMOV.U16   d4, #1
    VADD.U16   d3, d3, d4
    
    VSHR.U16   Q1, Q1, #1
    VSUBW.U8   Q1, Q1, d0
    VABS.S16   Q1, Q1
    
    VPADD.U16   d2, d2, d3
    VPADDL.U16  d2, d2
    VADD.U32    d5, d2, d5
    VPADDL.U32  d5, d5
    
    add        r9, r9, r5                            @p1 += refwx4;
    add        r1, r1, #8
    
    subs       r10, r10, #1
    bgt        SAD_MB_HP_HTFM_Collectxh_loop_inner
    
    VMOV.U32   r12, d5[0]                            @r12:sad
    str        r12, [fp, r8, lsl #2]                 @saddata[i] = sad;
    
    cmp        r8, #0
    ble        SAD_MB_HP_HTFM_Collectxh_check_loop
    cmp        r12, r2, lsr #16                      @if ((ULong)sad > ((ULong)dmin_lx >> 16))
    bgt        SAD_MB_HP_HTFM_Collectxh_store
SAD_MB_HP_HTFM_Collectxh_check_loop:
    add        r8, r8, #1
    cmp        r8, #16
    blt        SAD_MB_HP_HTFM_Collectxh_loop
    
SAD_MB_HP_HTFM_Collectxh_store:
    ldmia      fp,  {r8, r10}                         @r8: saddata[0]; r10:saddata[1]
    add        r10, r10, #1                           @(saddata[1] + 1)
    
    subs       r8,  r8, r10, lsr #1                   @r8: difmad = saddata[0] - ((saddata[1] + 1) >> 1);
    bgt        SAD_MB_HP_HTFM_Collectxh_POSITIVE_STORE
    mov        r10, #0
    sub        r8,  r10, r8
SAD_MB_HP_HTFM_Collectxh_POSITIVE_STORE:
    ldr        r10, [r3]
    add        r8,  r8, r10
    str        r8,  [r3]                              @(*abs_dif_mad_avg) += ((difmad > 0) ? difmad : -difmad);
    ldr        r10, [r7]
    add        r10, r10, #1
    str        r10, [r7]                              @(*countbreak)++;
    mov        r0, r12
    
    add        sp, fp, #64                            @recover sp
    ldmia      sp!, {r4 - r12, pc}
    @ENDP  @ |SAD_MB_HP_HTFM_Collectxh|
    
    
    
    .section .text
    .global	SAD_MB_HP_HTFMxhyh
SAD_MB_HP_HTFMxhyh:
    stmdb      sp!, {r4 - r12, lr}
    mvn        r4,  #0
    lsl        r4,  r4, #16
    and        r4,  r2, r4, lsr #16                  @r4:Int rx=dmin_rx&0xFFFF;
    lsl        r5,  r4, #2                           @r5:Int refwx4 = rx << 2;
    
    add        r6,  r3, #128                         @r6:Int *offsetRef = nrmlz_th + 32;
    lsr        r7,  r2, #20                          @r7:madstar = (ULong)dmin_rx >> 20;
    
    VMOV.I64   d8, #0                                @d5: Int sad = 0;
    mov        r11, #0                               @r11:Int sadstar = 0
    mov        r8,  #0                               @r8: i
SAD_MB_HP_HTFMxhyh_loop:
    ldr        r9,  [r6, r8, lsl #2]                 @r9:offsetRef[i];
    add        r9,  r9, r0                           @r9:p1 = ref + offsetRef[i];
    add        r12, r9, r4                           @r12:p2 = p1 + rx;
    
    mov        r10, #2
SAD_MB_HP_HTFMxhyh_loop_inner:
    VLD1.8     {d0},  [r1]                           @d0:cur_word[1-0]
    
    VLD2.16    {d2, d3},  [r9]                       @d2:p1[13]p1[12]p1[9]p1[8]p1[5]p1[4]p1[1]p1[0]
    VLD2.16    {d3, d4},  [r12]                      @d3:p2[13]p2[12]p2[9]p2[8]p2[5]p2[4]p2[1]p2[0]
    
    VADDL.U8   Q2, d2, d3                            @Q2:tmp = p1[12] + p2[12]; tmp2 = p1[13] + p2[13]; ...
    VPADD.U16  d6, d4, d5                            @d4: tmp += tmp2;
    VMOV.U16   d5, #2
    VADD.U16   d6, d6, d5                            @tmp += 2;
    
    add        r9, r9, r5                            @p1 += refwx4;
    add        r12, r12, r5                          @p2 += refwx4;
    
    VLD2.16    {d2, d3},  [r9]                       @d2:p1[13]p1[12]p1[9]p1[8]p1[5]p1[4]p1[1]p1[0]
    VLD2.16    {d3, d4},  [r12]                      @d3:p2[13]p2[12]p2[9]p2[8]p2[5]p2[4]p2[1]p2[0]
    
    VADDL.U8   Q2, d2, d3                            @Q2:tmp = p1[12] + p2[12]; tmp2 = p1[13] + p2[13]; ...
    VPADD.U16  d7, d4, d5                            @d4: tmp += tmp2;
    VMOV.U16   d5, #2
    VADD.U16   d7, d7, d5                            @tmp += 2;
    
    
    VSHR.U16   Q3, Q3, #2
    VSUBW.U8   Q3, Q3, d0
    VABS.S16   Q3, Q3
    
    VPADD.U16   d2, d6, d7
    VPADDL.U16  d2, d2
    VADD.U32    d8, d2, d8
    VPADDL.U32  d8, d8
    
    add        r9, r9, r5                            @p1 += refwx4;
    add        r12, r12, r5                          @p2 += refwx4;
    add        r1, r1, #8
    
    subs       r10, r10, #1
    bgt        SAD_MB_HP_HTFMxhyh_loop_inner
    
    VMOV.U32   r12, d8[0]                            @r12:sad
    add        r11, r11, r7                          @sadstar += madstar;
    ldr        r10, [r3, r8, lsl #2]                 @r10:nrmlz_th[i]
    sub        r10, r11, r10                         @sadstar - nrmlz_th[i]
    
    cmp        r12, r10
    bgt        SAD_MB_HP_HTFMxhyh_break_ret
    cmp        r12, r2, lsr #16
    bgt        SAD_MB_HP_HTFMxhyh_break_ret
    
    add        r8,  r8, #1
    cmp        r8,  #16
    blt        SAD_MB_HP_HTFMxhyh_loop
    mov        r0,  r12
    b          SAD_MB_HP_HTFMxhyh_normal_ret
SAD_MB_HP_HTFMxhyh_break_ret:
    mov        r0, #1
    lsl        r0, r0, #16
SAD_MB_HP_HTFMxhyh_normal_ret:
    ldmia      sp!, {r4 - r12, pc}
    @ENDP  @ |SAD_MB_HP_HTFMxhyh|
    
    .section .text
    .global	SAD_MB_HP_HTFMyh
SAD_MB_HP_HTFMyh:
    stmdb      sp!, {r4 - r12, lr}
    mvn        r4,  #0
    lsl        r4,  r4, #16
    and        r4,  r2, r4, lsr #16                  @r4:Int rx=dmin_rx&0xFFFF;
    lsl        r5,  r4, #2                           @r5:Int refwx4 = rx << 2;
    
    add        r6,  r3, #128                         @r6:Int *offsetRef = nrmlz_th + 32;
    lsr        r7,  r2, #20                          @r7:madstar = (ULong)dmin_rx >> 20;
    
    VMOV.I64   d8, #0                                @d5: Int sad = 0;
    mov        r11, #0                               @r11:Int sadstar = 0
    mov        r8,  #0                               @r8: i
SAD_MB_HP_HTFMyh_loop:
    ldr        r9,  [r6, r8, lsl #2]                 @r9:offsetRef[i];
    add        r9,  r9, r0                           @r9:p1 = ref + offsetRef[i];
    add        r12, r9, r4                           @r12:p2 = p1 + rx;
    
    mov        r10, #2
SAD_MB_HP_HTFMyh_loop_inner:
    VLD1.8     {d0},  [r1]                           @d0:cur_word[1-0]
    VLD4.8     {d1 - d4},  [r12]                     @d1:p2[28]p1[24]p2[20]p2[16]p2[12]p2[8]p2[4]p2[0]
    VLD4.8     {d2 - d5},  [r9]                      @d2:p1[28]p1[24]p1[20]p1[16]p1[12]p1[8]p1[4]p1[0]
    VZIP.U8    d2, d1                                @d2:p2[12]p1[12]p2[8]p1[8]p2[4]p1[4]p2[0]p1[0]
    
    VPADDL.U8  d2, d2                                @d2:(p2[12]+p1[12])(p2[8]+p1[8])(p2[4]+p1[4])(p2[0]+p1[0])
    VMOV.U16   d3, #1
    VADD.U16   d2, d2, d3
    
    add        r9, r9, r5                            @p1 += refwx4;
    add        r12, r12, r5                          @p2 += refwx4;
    
    VLD4.8     {d3 - d6},  [r9]                      @d3:p1[28]p1[24]p1[20]p1[16]p1[12]p1[8]p1[4]p1[0]
    VLD4.8     {d4 - d7},  [r12]                     @d4:p2[28]p1[24]p2[20]p2[16]p2[12]p2[8]p1[4]p2[0]
    VZIP.U8    d3, d4                                @d3:p2[12]p2[8]p1[4]p2[0]p1[12]p1[8]p1[4]p1[0]
    
    VPADDL.U8  d3, d3                                @d3:(p2[12]+p1[12])(p2[8]+p1[8])(p2[4]+p1[4])(p2[0]+p1[0])
    VMOV.U16   d4, #1
    VADD.U16   d3, d3, d4
    
    VSHR.U16   Q1, Q1, #1
    VSUBW.U8   Q1, Q1, d0
    VABS.S16   Q1, Q1
    
    VPADD.U16   d2, d2, d3
    VPADDL.U16  d2, d2
    VADD.U32    d8, d2, d8
    VPADDL.U32  d8, d8
    
    add        r9, r9, r5                            @p1 += refwx4;
    add        r12, r12, r5                          @p2 += refwx4;
    add        r1, r1, #8
    
    subs       r10, r10, #1
    bgt        SAD_MB_HP_HTFMyh_loop_inner
    
    VMOV.U32   r12, d8[0]                            @r12:sad
    add        r11, r11, r7                          @sadstar += madstar;
    ldr        r10, [r3, r8, lsl #2]                 @r10:nrmlz_th[i]
    sub        r10, r11, r10                         @sadstar - nrmlz_th[i]
    
    cmp        r12, r10
    bgt        SAD_MB_HP_HTFMyh_break_ret
    cmp        r12, r2, lsr #16
    bgt        SAD_MB_HP_HTFMyh_break_ret
    
    add        r8,  r8, #1
    cmp        r8,  #16
    blt        SAD_MB_HP_HTFMyh_loop
    mov        r0,  r12
    b          SAD_MB_HP_HTFMyh_normal_ret
SAD_MB_HP_HTFMyh_break_ret:
    mov        r0, #1
    lsl        r0, r0, #16
SAD_MB_HP_HTFMyh_normal_ret:
    ldmia      sp!, {r4 - r12, pc}
    @ENDP  @ |SAD_MB_HP_HTFMyh|
    
    
    
    .section .text
    .global	SAD_MB_HP_HTFMxh
SAD_MB_HP_HTFMxh:
    stmdb      sp!, {r4 - r12, lr}
    mvn        r4,  #0
    lsl        r4,  r4, #16
    and        r4,  r2, r4, lsr #16                  @r4:Int rx=dmin_rx&0xFFFF;
    lsl        r5,  r4, #2                           @r5:Int refwx4 = rx << 2;
    
    add        r6,  r3, #128                         @r6:Int *offsetRef = nrmlz_th + 32;
    lsr        r7,  r2, #20                          @r7:madstar = (ULong)dmin_rx >> 20;
    
    VMOV.I64   d5, #0                                @d5: Int sad = 0;
    mov        r11, #0                               @r11:Int sadstar = 0
    mov        r8,  #0                               @r8: i
SAD_MB_HP_HTFMxh_loop:
    ldr        r9,  [r6, r8, lsl #2]                 @r9:offsetRef[i];
    add        r9,  r9, r0                           @r9:p1 = ref + offsetRef[i];
    
    mov        r10, #2
SAD_MB_HP_HTFMxh_loop_inner:
    VLD1.8     {d0},  [r1]                           @d0:cur_word[1-0]
    VLD2.16    {d2, d3},  [r9]                       @d2:p1[13]p1[12]p1[9]p1[8]p1[5]p1[4]p1[1]p1[0]
    VPADDL.U8  d2, d2                                @d2:(p1[13]+p1[12])(p1[9]+p1[8])(p1[5]+p1[4])(p1[1]+p1[0])
    VMOV.U16   d3, #1
    VADD.U16   d2, d2, d3
    
    add        r9, r9, r5                            @p1 += refwx4;
    VLD2.16    {d3, d4},  [r9]                       @d3:p1[13]p1[12]p1[9]p1[8]p1[5]p1[4]p1[1]p1[0]
    VPADDL.U8  d3, d3                                @d3:(p1[13]+p1[12])(p1[9]+p1[8])(p1[5]+p1[4])(p1[1]+p1[0])
    VMOV.U16   d4, #1
    VADD.U16   d3, d3, d4
    
    VSHR.U16   Q1, Q1, #1
    VSUBW.U8   Q1, Q1, d0
    VABS.S16   Q1, Q1
    
    VPADD.U16   d2, d2, d3
    VPADDL.U16  d2, d2
    VADD.U32    d5, d2, d5
    VPADDL.U32  d5, d5
    
    add        r9, r9, r5                            @p1 += refwx4;
    add        r1, r1, #8
    
    subs       r10, r10, #1
    bgt        SAD_MB_HP_HTFMxh_loop_inner
    
    VMOV.U32   r12, d5[0]                            @r12:sad
    add        r11, r11, r7                          @sadstar += madstar;
    ldr        r10, [r3, r8, lsl #2]                 @r10:nrmlz_th[i]
    sub        r10, r11, r10                         @sadstar - nrmlz_th[i]
    
    cmp        r12, r10
    bgt        SAD_MB_HP_HTFMxh_break_ret
    cmp        r12, r2, lsr #16
    bgt        SAD_MB_HP_HTFMxh_break_ret
    
    add        r8,  r8, #1
    cmp        r8,  #16
    blt        SAD_MB_HP_HTFMxh_loop
    mov        r0,  r12
    b          SAD_MB_HP_HTFMxh_normal_ret
SAD_MB_HP_HTFMxh_break_ret:
    mov        r0, #1
    lsl        r0, r0, #16
SAD_MB_HP_HTFMxh_normal_ret:
    ldmia      sp!, {r4 - r12, pc}
    @ENDP  @ |SAD_MB_HP_HTFMxh|
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@        Function:   SAD_Blk_HalfPel_C
@        Date:       04/30/2001
@        Purpose:    Compute SAD 16x16 between blk and ref in halfpel
@                    resolution,
@        Changes:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .section .text
    .global	SAD_Blk_HalfPel_C
SAD_Blk_HalfPel_C:
    stmdb       sp!, {r4 - r12, lr}
    @@@@@@@@data stack:(highend) extra_info,yh,xh,rx,lr,r12-r4(lowend)@@@@@@@@@
    mov         fp, sp                                   @set frame pointer reg
    
    VMOV.I64    d5, #0                                   @d5: Int sad = 0;
    ldr         r4, [fp, #40]                            @r4: rx
    ldr         r5, [fp, #44]                            @r5: xh
    cmp         r5, #0
    beq         SAD_Blk_HalfPel_C_ELSE
    ldr         r6, [fp, #48]                            @r6: yh
    cmp         r6, #0
    beq         SAD_Blk_HalfPel_C_ELSE
SAD_Blk_HalfPel_C_IF:
    add         r7, r0, r5                               @r7: p2 = ref + xh; r0:ref
    mla         r8, r6, r4, r0                           @r8: p3 = ref + yh * rx;
    add         r9, r8, r5                               @r9: p4 = ref + yh * rx + xh;
    
    mov         r10, #8
IF_I_LOOP:
    VLD1.8      d0, [r0]                                 @d0: p1[8]-p1[0]
    VLD1.8      d1, [r7]                                 @d1: p2[8]-p2[0]
    VLD1.8      d2, [r8]                                 @d2: p3[8]-p3[0]
    VLD1.8      d3, [r9]                                 @d3: p4[8]-p4[0]
    VLD1.8      d4, [r1]                                 @d4: kk[8]-kk[0]
    
    VADDL.U8    Q3, d0, d1
    VADDL.U8    Q4, d2, d3
    VADD.U16    Q3, Q3, Q4
    VMOV.U16    Q4, #2
    VADD.U16    Q3, Q3, Q4
    VSHR.U16    Q3, Q3, #2
    VSUBW.U8    Q3, Q3, d4
    VABS.S16    Q3, Q3
    VPADD.U16   d6, d6, d7
    VPADDL.U16  d6, d6
    VADD.U32    d5, d6, d5
    VPADDL.U32  d5, d5
    
    VMOV.U32    r5, d5[0]
    cmp         r5, r2                                   @if (sad > dmin)
    bgt         SAD_Blk_HalfPel_C_RET
    add         r0, r0, r4                               @p1 += rx;
    add         r7, r7, r4                               @p2 += rx;
    add         r8, r8, r4                               @p3 += rx;
    add         r9, r9, r4                               @p4 += rx;
    add         r1, r1, r3                               @kk += width;
    subs        r10,r10,#1
    bgt         IF_I_LOOP
    b           SAD_Blk_HalfPel_C_RET
SAD_Blk_HalfPel_C_ELSE:
    mla         r7, r6, r4, r5
    add         r7, r7, r0                               @r7:p2 = ref + xh + yh * rx;
    
    mov         r10, #8
ELSE_I_LOOP:
    VLD1.8      d0, [r0]                                 @d0: p1[8]-p1[0]
    VLD1.8      d1, [r7]                                 @d1: p2[8]-p2[0]
    VLD1.8      d4, [r1]                                 @d4: kk[8]-kk[0]
    
    VADDL.U8    Q1, d0, d1
    VMOV.U16    Q0, #1
    VADD.U16    Q0, Q0, Q1
    VSHR.U16    Q0, Q0, #1
    VSUBW.U8    Q0, Q0, d4
    VABS.S16    Q0, Q0
    
    VPADD.U16   d0, d0, d1
    VPADDL.U16  d0, d0
    VADD.U32    d5, d0, d5
    VPADDL.U32  d5, d5
    
    VMOV.U32    r5, d5[0]
    cmp         r5, r2                                   @if (sad > dmin)
    bgt         SAD_Blk_HalfPel_C_RET
    add         r0, r0, r4                               @p1 += rx;
    add         r7, r7, r4                               @p2 += rx;
    add         r1, r1, r3                               @kk += width;
    subs        r10,r10,#1
    bgt         ELSE_I_LOOP
SAD_Blk_HalfPel_C_RET:
    mov        r0,  r5
    mov        sp,  fp
    ldmia      sp!, {r4 - r12, pc}
    @ENDP  @ |SAD_MB_HTFM_Collect|
    
    .end
    