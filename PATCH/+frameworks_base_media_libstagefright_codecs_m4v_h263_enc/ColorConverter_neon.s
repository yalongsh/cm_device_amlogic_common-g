@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@inline static void ConvertYUV420SemiPlanarToYUV420Planar(
@        uint8_t *inyuv, uint8_t* outyuv,
@        int32_t width, int32_t height)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .section .text
    .global  ConvertYUV420SemiPlanarToYUV420Planar
    
ConvertYUV420SemiPlanarToYUV420Planar:
    stmdb      sp!, {r4 - r8, lr}
    
    mul        r7, r2, r3
    @@@@@@@@@@Y copying@@@@@@@@@@@@@@
    mov        r4, r7
    mov        r5, r1                  @r5: outy
    mov        r6, r0
1:
    VLD1.8     {d0, d1}, [r6]
    VST1.8     {d0, d1}, [r5]
    add        r5, r5, #16
    add        r6, r6, #16
    subs       r4, r4, #16
    bgt        1b
    
    @@@@@@@@@@U & V copying@@@@@@@@@@@
    add        r6, r0, r7              @r6: *inyuv_4 = (uint32_t *) (inyuv + outYsize);
    add        r8, r1, r7              @r8: uint16_t *outcb = (uint16_t *) (outyuv + outYsize);
    add        r7, r8, r7, lsr #2
    
    mov        r4, r3, lsr #1          @r4: i = height >> 1;
3:
    mov        r5, r2, lsr #4          @r5: j = width >> 4;
2:
    VLD1.8     {d0, d1}, [r6]
    VUZP.8     d0, d1
    VST1.16    {d0},     [r7]
    VST1.16    {d1},     [r8]
    add        r7,  r7,  #8
    add        r8,  r8,  #8
    add        r6,  r6,  #16
    subs       r5,  r5,  #1
    bgt        2b
    
    subs       r4,  r4,  #1
    bgt        3b
    
    ldmia     sp!, {r4 - r8, pc}
    @ENDP  @ |ConvertYUV420SemiPlanarToYUV420Planar|

    .end
    