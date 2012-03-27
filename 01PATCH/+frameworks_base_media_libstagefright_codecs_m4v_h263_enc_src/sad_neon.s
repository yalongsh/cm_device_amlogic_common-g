

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@        Function:   SAD_Macroblock
@        Date:       09/07/2000
@        Purpose:    Compute SAD 16x16 between blk and ref.
@        To do:      Uniform subsampling will be inserted later!
@                    Hypothesis Testing Fast Matching to be used later!
@        Changes:
@    11/7/00:     implemented MMX
@    1/24/01:     implemented SSE
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    .section .text
@    .global	SAD_Macroblock_C
@    .extern simd_sad_mb [CODE]
@SAD_Macroblock_C:
@    stmdb      sp!, {lr}
    
@    @@@@@@@@@@@@@@@@@@@@prepare parameters input@@@@@@@@@@@@@@@
@    mvn        r3,  #0
@    lsr        r3,  #16
@    and        r3,  r3, r2                           @Int lx = dmin_lx & 0xFFFF;
@    lsr        r2,  r2, #16                          @Int dmin = (ULong)dmin_lx >> 16;
    
@    bl         simd_sad_mb
    
@    ldmia      sp!, {pc}
@    @ENDP  @ |SAD_Macroblock_C|
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@        Function:   SAD_MB_HTFM_Collect and SAD_MB_HTFM
@        Date:       3/2/1
@        Purpose:    Compute the SAD on a 16x16 block using
@                    uniform subsampling and hypothesis testing fast matching
@                    for early dropout. SAD_MB_HP_HTFM_Collect is to collect
@                    the statistics to compute the thresholds to be used in
@                    SAD_MB_HP_HTFM.
@        Input/Output:
@        Changes:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .section .text
    .global	SAD_MB_HTFM_Collect
SAD_MB_HTFM_Collect:
    stmdb      sp!, {r4 - r12, lr}
    @@@@@@@@@@@@@@@@@@@allocate local variable here@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    sub        fp, sp, #64                           @fp: r11, move to low end
    sub        sp, sp, #64                           @int saddata[16]
    
    @@@@@@@@input parameters: r0: ref; r1:blk; r2:dmin_lx; r3:extra_info@@@@@@@
    mvn        r4,  #0
    lsl        r4,  r4, #16
    lsr        r4,  r4, #14                          @generate mask: 0x3fffc
    and        r4,  r4, r2, lsl #2                   @r4: lx4
    
    @@@@@@@@@@@@@r3: int *abs_dif_mad_avg = &(htfm_stat->abs_dif_mad_avg);@@@@@
    add        r6,  r3, #4                           @r6: UInt *countbreak = &(htfm_stat->countbreak);
    add        r5,  r3, #72                          @r5: Int *offsetRef = htfm_stat->offsetRef;
    
    mov        r7,  #0                               @r7: i
    VMOV.I64   d6,  #0                               @d6: sad
SAD_MB_HTFM_Collect_loop:
    lsl        r9,  r7, #2
    ldr        r8,  [r5, r9]                         @r8: offsetRef[i]
    add        r9,  r0, r8                           @r9: p1 = ref + offsetRef[i];
    
    VLD1.8     d0,  [r1]                             @d0: cur_word
    VLD4.8     {d1, d2, d3, d4}, [r9]                @d1: xxxxp1[12]p1[8]p1[4]p1[0]
    add        r9,  r9, r4                           @p1 += lx4;
    VLD4.8     {d2, d3, d4, d5}, [r9]                @d2: xxxxp1[12]p1[8]p1[4]p1[0]
    VZIP.32    d1,  d2                               @d1:p1[12]p1[8]p1[4]p1[0] - p1[12]p1[8]p1[4]p1[0]
    
    VABD.U8     d2, d0, d1
    VPADDL.U8   d2, d2
    VPADDL.U16  d2, d2                              @d2: sad
    VPADD.U32   d6, d6, d2
    
    add        r9,  r9, r4                           @p1 += lx4;
    add        r1,  r1, #8                           @blk += 4;blk += 4;
    
    VLD1.8     d0,  [r1]                             @d0: cur_word
    VLD4.8     {d1, d2, d3, d4}, [r9]                @d1: xxxxp1[12]p1[8]p1[4]p1[0]
    add        r9,  r9, r4                           @p1 += lx4;
    VLD4.8     {d2, d3, d4, d5}, [r9]                @d2: xxxxp1[12]p1[8]p1[4]p1[0]
    VZIP.32    d1,  d2                               @d1:p1[12]p1[8]p1[4]p1[0] - p1[12]p1[8]p1[4]p1[0]
    
    VABD.U8     d2, d0, d1
    VPADDL.U8   d2, d2
    VPADDL.U16  d2, d2                               @d2: sad
    VPADD.U32   d6, d6, d2
    VPADDL.U32  d6, d6
    VMOV.32     r8, d6[0]                            @r8: sad
    
    add        r9,  r9, r4                           @p1 += lx4;
    add        r1,  r1, #8                           @blk += 4;blk += 4;
    
    str        r8,  [fp, r7, lsl #2]                 @saddata[i] = sad;
    
    cmp        r7, #0
    ble        SAD_MB_HTFM_Collect_check_loop
    cmp        r8, r2, lsr #16                       @if ((ULong)sad > ((ULong)dmin_lx >> 16))
    bgt        SAD_MB_HTFM_Collect_store
SAD_MB_HTFM_Collect_check_loop:
    add        r7, r7, #1
    cmp        r7, #16
    blt        SAD_MB_HTFM_Collect_loop
    
SAD_MB_HTFM_Collect_store:
    ldmia      fp,  {r7, r10}                         @r7: saddata[0]; r10:saddata[1]
    add        r10, r10, #1                           @(saddata[1] + 1)
    
    subs       r7,  r7, r10, lsr #1                   @r7: difmad = saddata[0] - ((saddata[1] + 1) >> 1);
    bgt        SAD_MB_HTFM_Collect_POSITIVE_STORE
    mov        r10, #0
    sub        r7,  r10, r7
SAD_MB_HTFM_Collect_POSITIVE_STORE:
    ldr        r10, [r3]
    add        r7,  r7, r10
    str        r7,  [r3]                              @(*abs_dif_mad_avg) += ((difmad > 0) ? difmad : -difmad);
    ldr        r10, [r6]
    add        r10, r10, #1
    str        r10, [r6]                              @(*countbreak)++;
    mov        r0, r8
    
    add        sp, fp, #64                            @recover sp
    ldmia      sp!, {r4 - r12, pc}
    @ENDP  @ |SAD_MB_HTFM_Collect|
    
    
    .section .text
    .global	SAD_MB_HTFM
SAD_MB_HTFM:
    stmdb      sp!, {r4 - r12, lr}
    @@@@@@@@input parameters: r0: ref; r1:blk; r2:dmin_lx; r3:extra_info@@@@@@@
    mvn        r4,  #0
    lsl        r4,  r4, #16
    lsr        r4,  r4, #14                          @generate mask: 0x3fffc
    and        r4,  r4, r2, lsl #2                   @r4: lx4
    add        r5,  r3, #128                         @r5: Int *offsetRef = (Int*) extra_info + 32;
    lsr        r6,  r2, #20                          @r6: madstar = (ULong)dmin_lx >> 20;
    
    mov        r10, #0                               @r10: sadstar = 0;
    mov        r7,  #0                               @r7: i
    VMOV.I64   d6,  #0                               @d6: sad
SAD_MB_HTFM_LOOP:
    lsl        r11, r7, #2
    ldr        r8,  [r5, r11]                        @r8: offsetRef[i]
    add        r9,  r0, r8                           @r9: p1 = ref + offsetRef[i];
    
    VLD1.8     d0,  [r1]                             @d0: cur_word
    VLD4.8     {d1, d2, d3, d4}, [r9]                @d1: xxxxp1[12]p1[8]p1[4]p1[0]
    add        r9,  r9, r4                           @p1 += lx4;
    VLD4.8     {d2, d3, d4, d5}, [r9]                @d2: xxxxp1[12]p1[8]p1[4]p1[0]
    VZIP.32    d1,  d2                               @d1:p1[12]p1[8]p1[4]p1[0] - p1[12]p1[8]p1[4]p1[0]
    
    VABD.U8     d2, d0, d1
    VPADDL.U8   d2, d2
    VPADDL.U16  d2, d2                              @d2: sad
    VPADD.U32   d6, d6, d2
    
    add        r9,  r9, r4                           @p1 += lx4;
    add        r1,  r1, #8                           @blk += 4;blk += 4;
    
    VLD1.8     d0,  [r1]                             @d0: cur_word
    VLD4.8     {d1, d2, d3, d4}, [r9]                @d1: xxxxp1[12]p1[8]p1[4]p1[0]
    add        r9,  r9, r4                           @p1 += lx4;
    VLD4.8     {d2, d3, d4, d5}, [r9]                @d2: xxxxp1[12]p1[8]p1[4]p1[0]
    VZIP.32    d1,  d2                               @d1:p1[12]p1[8]p1[4]p1[0] - p1[12]p1[8]p1[4]p1[0]
    
    VABD.U8     d2, d0, d1
    VPADDL.U8   d2, d2
    VPADDL.U16  d2, d2                              @d2: sad
    VPADD.U32   d6, d6, d2
    VPADDL.U32  d6, d6
    VMOV.32     r11, d6[0]                          @r11: sad
    
    add        r9,  r9, r4                           @p1 += lx4;
    add        r1,  r1, #8                           @blk += 4;blk += 4;
    
    add        r10, r10, r6                          @sadstar += madstar;
    
    cmp        r11, r2, lsr #16
    bgt        SAD_MB_HTFM_RET1
    ldr        r12, [r3]
    add        r3,  r3, #1
    sub        r12, r10, r12                         @r12: (sadstar - *nrmlz_th++)
    cmp        r11, r12
    bgt        SAD_MB_HTFM_RET1
    
    add        r7,  #1
    cmp        r7,  #16
    blt        SAD_MB_HTFM_LOOP
    mov        r0, r11
    b          SAD_MB_HTFM_RET2

SAD_MB_HTFM_RET1:
    mov        r0, #1
    lsl        r0, #16

SAD_MB_HTFM_RET2:
    ldmia      sp!, {r4 - r12, pc}
    @ENDP  @ |SAD_MB_HTFM|

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@        Function:   SAD_Block
@        Date:       09/07/2000
@        Purpose:    Compute SAD 16x16 between blk and ref.
@        To do:      Uniform subsampling will be inserted later!
@                    Hypothesis Testing Fast Matching to be used later!
@        Changes:
@                    11/7/00:     implemented MMX
@                    1/24/01:     implemented SSE
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .section .text
    .global	SAD_Block_C

SAD_Block_C:
	stmdb      sp!, {r4 - r6, lr}
    @@@@@@@@@@@@input parameters: r0: ref(ii); r1:blk; r2:dmin; r3:lx@@@@@@@@@@
    mov         r4, #8                              @r4:i
    sub         r6, r3, #32                         @r6:width
    VMOV.I64    d3, #0                              @d3: sad; sum of absolute of tmp and tmp2
SAD_Block_C_Loop:
    VLD1.8      d0,  [r0]                           @d0:ii[7 - 0]
    VLD1.8      d1,  [r1]                           @d1:kk[1]kk[0]
    
    VABD.U8     d2, d0, d1
    VPADDL.U8   d2, d2
    VPADDL.U16  d2, d2                              @d2: sad
    VPADD.U32   d3, d3, d2
    VPADDL.U32  d3, d3
    
    add         r1, r1, r6                          @kk += (width >> 2);  note: int pointer adding, r6 needn't lsr #2
    add         r0, r0, r3                          @ii += lx;
    
    VMOV.32     r5,  d3[0]
    cmp         r5,  r2
    bgt         SAD_Block_C_Ret
    
    subs        r4,  r4, #1
    bne         SAD_Block_C_Loop
	
	@@@@@@@@@@@@@@@@@@@@@@@@return sad here@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
SAD_Block_C_Ret:
    mov        r0,  r5                              @pass return value to caller
    ldmia      sp!, {r4 - r6, pc}
	@ENDP  @ |SAD_Block_C|
    
    .end
    