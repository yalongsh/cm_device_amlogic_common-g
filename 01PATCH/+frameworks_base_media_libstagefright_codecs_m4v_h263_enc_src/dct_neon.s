
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 @ Copyright (C) 2011 Amlogic, Inc.
 @
 @ Licensed under the Apache License, Version 2.0 (the "License");
 @ you may not use this file except in compliance with the License.
 @ You may obtain a copy of the License at
 @
 @      http://www.apache.org/licenses/LICENSE-2.0
 @
 @ Unless required by applicable law or agreed to in writing, software
 @ distributed under the License is distributed on an "AS IS" BASIS,
 @ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 @ express or implied.
 @ See the License for the specific language governing permissions
 @ and limitations under the License.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    Function:   BlockDCT_AANwSub
@    Date:       4/7/2011
@    Input:      rec
@    Output:     out[64] ==> next block
@    Purpose:    Input directly from rec frame.
@    Modified:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .section .text
    .global	BlockDCT_AANwSub
BlockDCT_AANwSub:
    stmdb     sp!, {r4 - r12, lr}
    
    mov       r4,  #1
    mov       r6,  r4, lsl #9                         @round
    add       r4,  r0, #128                           @r4: dst = out + 64;
    
    ldrsh     r5,  [r4]                               @r5: ColTh
    add       r0,  r0, #256
    
    VMOV.S32  d31[0],  r6
    VMOV.I32  d10,  #0xFF
    VSHL.U32  d11,  d10, #1                           @d11: generate mask
    
fdct_nextrow_Loop5:
    VLD1.32   d0,  [r1]                               @d0:tmp;   cur[1] cur[0]
    VLD1.32   d9,  [r2]                               @d9:tmp2;  pred[1] pred[0]
    
    VAND      d1,  d9, d10                            @d1: k4 k0; k4 = tmp2 & 0xFF; k0 = tmp2 & 0xFF;
    VSHL.U32  Q6,  Q0, #1                             @d12: (tmp<<1)(tmp<<1); d13:(k4<<1)(k0<<1)
    
    VAND      d3,  d12, d11                           @d3: k5 k1; k5 = mask & (tmp << 1);k1 = mask & (tmp << 1);
    VSUB.S32  d2,  d3, d13                            @d2: k4 k0; k4 = k5 - (k4 << 1); k0 = k1 - (k0 << 1);
    
    VSHR.S32  d3,  d9, #8
    VAND      d3,  d3, d10                            @d3: k5 = (tmp2 >> 8) & 0xFF; k1 = (tmp2 >> 8) & 0xFF;
    VSHR.S32  d1,  d0, #7
    VAND      d1,  d1, d11                            @d1: k6 = mask & (tmp >> 7); k2 = mask & (tmp >> 7);
    VSHL.S32  d3,  d3, #1
    VSUB.S32  d3,  d1, d3                             @d3: k5 k1
    
    VSHR.U32  d4,  d9, #16
    VAND      d4,  d4, d10                            @d4: k6 = (tmp2 >> 16) & 0xFF; k2 = (tmp2 >> 16) & 0xFF;
    VSHR.S32  d1,  d0, #15
    VAND      d1,  d1, d11                            @d1: k7 = mask & (tmp >> 15);k3 = mask & (tmp >> 15);
    VSHL.S32  d4,  d4, #1
    VSUB.S32  d4,  d1, d4                             @d4: k6 = k7 - (k6 << 1); k2 = k3 - (k2 << 1);
    
    VSHR.U32  d5,  d9, #24
    VAND      d5,  d5, d10                            @d5: k7 = (tmp2 >> 24) & 0xFF; k3 = (tmp2 >> 24) & 0xFF;
    VSHR.S32  d1,  d0, #23
    VAND      d1,  d1, d11                            @d1: tmp = mask & (tmp >> 23);k4 = mask & (tmp >> 23);
    VSHL.S32  d5,  d5, #1
    VSUB.S32  d5,  d1, d5                             @d5: k7 = tmp - (k7 << 1); k3 = k4 - (k3 << 1);
    
    add       r1,  r1, r3
    add       r2,  r2, #16
    
    VREV64.32 d4,  d4                                 @d4:  k2 k6
    VREV64.32 d5,  d5                                 @d5:  k3 k7
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@  fdct1   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VADD.S32  d6,  d2, d5                             @k3 = k3+k4; k0 = k0+k7
    VADD.S32  d7,  d3, d4                             @k2 = k2+k5; k1 = k1+k6
    
    VREV64.32 d4,  d4                                 @swap back: d4:  k6 k2
    VREV64.32 d5,  d5                                 @@@@@@@@@@  d5:  k7 k3
    
    VUZP.32   Q1,  Q2                                 @q2: k7,k6,k5,k4
    VZIP.32   d6,  d7                                 @d6: k1, k0; d7: k2,k3
    VREV64.32 d6,  d6                                 @d6: k0, k1; d7: k2,k3
    
    VSHL.U32  Q2,  Q2, #1
    
    VSUB.S32  d5, d6, d5
    VSUB.S32  d4, d7, d4                             @d5: k7,k6; d4: k5,k4
    
    @@@@@@@@@@@@@@@@@@ k3 k2 calculation@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VREV64.32 d7,  d7                                @d7: k3,k2
    VADD.S32  d8,  d7, d6                            @d8: k0k1; k0=k0+k3,k1=k1+k2
    VSHL.U32  d7,  d7, #1
    VSUB.S32  d7,  d8, d7
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VPADDL.S32 d0,  d8
    VMOV.32    r6,  d0[0]
    strh       r6,  [r4]                              @actually, k0 is sum of k0 - k7
    VSHL.S32   d8,  d8, #1
    VSUB.S32   d0,  d0, d8
    VMOV.32    r6,  d0[0]
    strh       r6,  [r4, #8]
    
    VPADD.S32  d0,  d4, d5                            @d0:k6k4   k6=k6+k7,k4=k4+k5
    VMOV.32    r7,  d4[1]                             @r7: k5
    VMOV.32    r8,  d5[0]                             @r8: k6
    add        r7,  r7, r8                            @r7: k5=k5+k6
    VMOV.32    r8,  r9, d7                            @r8: k2; r9: k3
    add        r6,  r8, r9                            @r6: k2
    
    VMOV.S32   r12,  d31[0]                           @r12:round
    mov        r10, #724
    mla        r11, r7, r10, r12                      @r11:k5
    mla        r7,  r6, r10, r12                      @r7: k2
    lsr        r7,  r7, #10
    lsr        r11, r11, #10
    add        r7,  r7, r9
    strh       r7,  [r4, #4]
    
    lsl        r9,  r9, #1
    sub        r9,  r9, r7
    lsl        r9,  r9, #1
    strh       r9,  [r4, #12]
    
    VMOV.32    r6,  d0[0]                             @r6: k4
    VMOV.32    r8,  d0[1]                             @r8: k6
    VMOV.32    r9,  d5[1]                             @r9: k7
    
    sub        r7, r6, r8                             @r7:k0
    mov        r10, #392
    mla        r12, r7, r10, r12                      @r12:k1
    mov        r10, #552
    add        r10, r10, #2
    mla        r7, r6, r10, r12                       @r7:k0
    mov        r10, #1328
    add        r10, r10, #10
    mla        r12, r8, r10, r12
    lsr        r6, r7, #10                            @r6:  k4
    lsr        r8, r12, #10                           @r8:  k6
    
    add        r11, r11, r9                           @r9: k7, r11:k5
    lsl        r9, r9, #1
    sub        r9, r9, r11
    add        r6, r9, r6
    lsl        r9, r9, #1
    sub        r9, r9, r6
    add        r11, r11, r8
    lsl        r6,  r6, #1
    sub        r8,  r11, r8, lsl #1
    lsl        r8,  r8, #2
    strh       r6,  [r4, #10]
    strh       r11, [r4, #2]
    strh       r9, [r4, #6]
    strh       r8, [r4, #14]
    
    add        r4, r4, #16
    cmp        r4, r0
    blt        fdct_nextrow_Loop5
    
    sub        r0, r0, #128
    add        r4, r0, #16

vertical_block_loop5:
    ldrsh      r6, [r0]
    ldrsh      r7, [r0, #16]
    ldrsh      r8, [r0, #32]
    ldrsh      r9, [r0, #48]
    VMOV       d0, r6, r9                             @r6: k0; r9: k3; d0: k3,k0
    VMOV       d1, r8, r7                             @r8: k2; r7: k1; d1: k1,k2
    
    ldrsh      r6,[r0, #64]
    ldrsh      r7,[r0, #80]
    ldrsh      r8,[r0, #96]
    ldrsh      r9,[r0, #112]
    VMOV       d2, r9, r6                             @r6: k4; r9: k7; d2: k4,k7
    VMOV       d3, r7, r8                             @r8: k6; r7: k5; d3: k6,k5
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @                        we get q0: k1k2k3k0; q1: k6k5k4k7                            @ 
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VSHR.S32  q2,  q0, #31                            @q2: (k1>>31)(k2>>31)(k3>>31)(k0>>31)
    VSHR.S32  q3,  q1, #31                            @q3: (k6>>31)(k5>>31)(k4>>31)(k7>>31)
    
    VEOR      q4,  q0, q2                             @q4: (k1^(k1>>31))(k2^(k2>>31))(k3^(k3>>31))(k0^(k0>>31))
    VEOR      q5,  q1, q3                             @q5: (k6^(k6>>31))(k5^(k5>>31))(k4^(k4>>31))(k7^(k7>>31))
    
    mov       r10, #0
    VMOV.32   d4[0], r10                              @q2: ((k1>>31))((k2>>31))((k3>>31))(0)
    
    VSUB.S32  q6,  q4, q2
    VSUB.S32  q7,  q5, q3
    
    
    VADD.S32  q4,  q6, q7
    VPADD.S32 d4, d8, d9
    VPADDL.S32 d5, d4
    
    VMOV.32   r10, d5[0]
    
    cmp       r10, r5
    bge       go_on5
    mov       r10, #0x7f
    lsl       r10, r10, #8
    orr       r10, r10, #0xff
    strh      r10, [r0]
    add       r0,  r0, #2
    b         continue_vertical_block_loop5
    
go_on5:
    VADD.S32  q2,  q1, q0                             @q2: k1k2k3k0
    VSHL.S32  q3,  q1, #1
    VSUB.S32  q0,  q2, q3                             @q0: k6k5k4k7
    
    VPADD.S32 d2, d4, d5                              @d2: k1k0
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@k2 k3@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VSHR.U64  d4, d4, #32                             @d4: 0 k3
    VZIP.32   d4, d5                                  @d4: k2k3
    VSHL.S32  d4, d4, #1
    VSUB.S32  d4, d2, d4
    
    VPADDL.S32 d3, d2
    VMOV.32   r10, d3[0]
    strh      r10, [r0]
    
    VSHR.U64  d2,  d2, #32
    VSHL.S32  d2,  d2, #1
    VSUB.S32  d2,  d3, d2
    VMOV.32   r10, d2[0]
    strh      r10, [r0, #64]
    
    VPADD.S32 d2, d4, d1                              @d2: k5k2
    VMOV.S32  d5, #181
    VSHL.U32  d5, d5, #2                              @d5: 724
    VDUP.32   d3, d31[0]                              @d3: round round
    VMLA.S32  d3, d5, d2                              @d3: k5k2
    VSHR.S32  d3, d3, #10
    VADD.S32  d5, d3, d4                              @d5: xxk2
    VMOV.32   r10, d5[0]
    strh      r10, [r0, #32]
    
    VSHL.S32  d4, d4, #1
    VSUB.S32  d4, d4, d5
    VSHL.S32  d4, d4, #1
    VMOV.32   r10, d4[0]
    strh      r10, [r0, #96]
    
    VMOV      r6, r7, d0                              @r6:k7; r7:k4
    VMOV      r8, r9, d1                              @r8:old k5; r9:k6
    add       r7, r8, r7                              @r7: k4=k4+k5
    VMOV.32   r8, d3[1]                               @r8: new k5
    add       r9, r6, r9                              @r9: k6=k6+k7
    sub       r12, r7, r9                             @r12:k0
    
    VMOV.S32   r11,  d31[0]
    mov        r10, #392
    mla        r11, r12, r10, r11                     @r11:k1
    mov        r10, #552
    add        r10, r10, #2
    mla        r12, r7, r10, r11                      @r12:k0
    mov        r10, #1328
    add        r10, r10, #10
    mla        r11, r9, r10, r11                      @r11:k1
    lsr        r7,  r12, #10                          @r7:k4
    lsr        r12, r11, #10                          @r12:k6
    
    add        r8, r8, r6                             @r8:k5=k5+k7
    lsl        r6, r6, #1
    sub        r6, r6, r8                             @r6: k7
    add        r7, r7, r6
    lsl        r6, r6, #1
    sub        r6, r6, r7                             @r6:k7
    add        r8, r8, r12                            @r8:k5
    lsl        r7, r7, #1
    sub        r12,r8, r12, lsl #1
    lsl        r12, r12, #2
    strh       r8, [r0, #16]
    strh       r6,  [r0, #48]
    strh       r12, [r0, #112]
    strh       r7,  [r0, #80]
    
    add        r0,  #2
    
continue_vertical_block_loop5:
    cmp        r0,  r4
    blt        vertical_block_loop5
    
    ldmia     sp!, {r4 - r12, pc}
    @ENDP  @ |BlockDCT_AANwSub|
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    Function:   Block4x4DCT_AANwSub
@    Date:       4/7/2011
@    Input:      prev
@    Output:     out[64] ==> next block
@    Purpose:    Input directly from prev frame. output 2x2 DCT
@    Modified:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .section .text
    .global	Block4x4DCT_AANwSub
Block4x4DCT_AANwSub:
    stmdb     sp!, {r4 - r12, lr}
    
    mov       r4,  #1
    mov       r6,  r4, lsl #9                         @round
    add       r4,  r0, #128                           @r4: dst = out + 64;
    
    ldrsh     r5,  [r4]                               @r5: ColTh = *dst;
    add       r0,  r0, #256
    
    VMOV.S32  d31[0],  r6
    VMOV.I32  d10,  #0xFF
    VSHL.U32  d11,  d10, #1                           @d11: generate mask
    
fdct_nextrow_Loop4:
    VLD1.32   d0,  [r1]                               @d0:tmp;   cur[1] cur[0]
    VLD1.32   d9,  [r2]                               @d9:tmp2;  pred[1] pred[0]
    
    VAND      d1,  d9, d10                            @d1: k4 k0; k4 = tmp2 & 0xFF; k0 = tmp2 & 0xFF;
    VSHL.U32  Q6,  Q0, #1                             @d12: (tmp<<1)(tmp<<1); d13:(k4<<1)(k0<<1)
    
    VAND      d3,  d12, d11                           @d3: k5 k1; k5 = mask & (tmp << 1);k1 = mask & (tmp << 1);
    VSUB.S32  d2,  d3, d13                            @d2: k4 k0; k4 = k5 - (k4 << 1); k0 = k1 - (k0 << 1);
    
    VSHR.S32  d3,  d9, #8
    VAND      d3,  d3, d10                            @d3: k5 = (tmp2 >> 8) & 0xFF; k1 = (tmp2 >> 8) & 0xFF;
    VSHR.S32  d1,  d0, #7
    VAND      d1,  d1, d11                            @d1: k6 = mask & (tmp >> 7); k2 = mask & (tmp >> 7);
    VSHL.S32  d3,  d3, #1
    VSUB.S32  d3,  d1, d3                             @d3: k5 k1
    
    VSHR.U32  d4,  d9, #16
    VAND      d4,  d4, d10                            @d4: k6 = (tmp2 >> 16) & 0xFF; k2 = (tmp2 >> 16) & 0xFF;
    VSHR.S32  d1,  d0, #15
    VAND      d1,  d1, d11                            @d1: k7 = mask & (tmp >> 15);k3 = mask & (tmp >> 15);
    VSHL.S32  d4,  d4, #1
    VSUB.S32  d4,  d1, d4                             @d4: k6 = k7 - (k6 << 1); k2 = k3 - (k2 << 1);
    
    VSHR.U32  d5,  d9, #24
    VAND      d5,  d5, d10                            @d5: k7 = (tmp2 >> 24) & 0xFF; k3 = (tmp2 >> 24) & 0xFF;
    VSHR.S32  d1,  d0, #23
    VAND      d1,  d1, d11                            @d1: tmp = mask & (tmp >> 23);k4 = mask & (tmp >> 23);
    VSHL.S32  d5,  d5, #1
    VSUB.S32  d5,  d1, d5                             @d5: k7 = tmp - (k7 << 1); k3 = k4 - (k3 << 1);
    
    add       r1,  r1, r3
    add       r2,  r2, #16
    
    VREV64.32 d4,  d4                                 @d4:  k2 k6
    VREV64.32 d5,  d5                                 @d5:  k3 k7
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@  fdct1   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VADD.S32  d6,  d2, d5                             @k3 = k3+k4; k0 = k0+k7
    VADD.S32  d7,  d3, d4                             @k2 = k2+k5; k1 = k1+k6
    
    VREV64.32 d4,  d4                                 @swap back: d4:  k6 k2
    VREV64.32 d5,  d5                                 @@@@@@@@@@  d5:  k7 k3
    
    VUZP.32   Q1,  Q2                                 @q2: k7,k6,k5,k4
    VZIP.32   d6,  d7                                 @d6: k1, k0; d7: k2,k3
    VREV64.32 d6,  d6                                 @d6: k0, k1; d7: k2,k3
    
    VSHL.U32  Q2,  Q2, #1
    
    VSUB.S32  d5, d6, d5
    VSUB.S32  d4, d7, d4                             @d5: k7,k6; d4: k5,k4
    
    @@@@@@@@@@@@@@@@@@ k3 k2 calculation@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VREV64.32 d7,  d7                                @d7: k3,k2
    VADD.S32  d8,  d7, d6                            @d8: k0k1; k0=k0+k3,k1=k1+k2
    VSHL.U32  d7,  d7, #1
    VSUB.S32  d7,  d8, d7
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VPADDL.S32  d0,  d8
    VMOV.32    r6,  d0[0]
    strh       r6,  [r4]                              @actually, k0 is sum of k0 - k7
    
    VPADD.S32  d0,  d4, d5                            @d0:k6k4   k6=k6+k7,k4=k4+k5
    VMOV.32    r7,  d4[1]                             @r7: k5
    VMOV.32    r8,  d5[0]                             @r8: k6
    add        r7,  r7, r8                            @r7: k5=k5+k6
    VMOV.32    r8,  r9, d7                            @r8: k2; r9: k3
    add        r6,  r8, r9                            @r6: k2
    
    VMOV.S32   r12,  d31[0]                           @r12:round
    mov        r10, #724
    mla        r11, r7, r10, r12                      @r11:k5
    mla        r7,  r6, r10, r12                      @r7: k2
    lsr        r7,  r7, #10
    lsr        r11, r11, #10
    add        r7,  r7, r9
    strh       r7,  [r4, #4]
    
    VMOV.32    r6,  d0[0]                             @r6: k4
    VMOV.32    r8,  d0[1]                             @r8: k6
    VMOV.32    r9,  d5[1]                             @r9: k7
    
    sub        r7, r6, r8                             @r7:k0
    mov        r10, #392
    mla        r12, r7, r10, r12                      @r12:k1
    mov        r10, #552
    add        r10, r10, #2
    mla        r7, r6, r10, r12                       @r7:k0
    mov        r10, #1328
    add        r10, r10, #10
    mla        r12, r8, r10, r12
    lsr        r6, r7, #10                            @r6:  k4
    lsr        r8, r12, #10                           @r8:  k6
    
    add        r11, r11, r9
    lsl        r9, r9, #1
    sub        r9, r9, r11
    sub        r9, r9, r6
    add        r11, r11, r8
    strh       r11, [r4, #2]
    strh       r9, [r4, #6]
    
    add        r4, r4, #16
    cmp        r4, r0
    blt        fdct_nextrow_Loop4
    
    sub        r0, r0, #128
    add        r4, r0, #8

vertical_block_loop4:
    ldrsh      r6, [r0]
    ldrsh      r7, [r0, #16]
    ldrsh      r8, [r0, #32]
    ldrsh      r9, [r0, #48]
    VMOV       d0, r6, r9                             @r6: k0; r9: k3; d0: k3,k0
    VMOV       d1, r8, r7                             @r8: k2; r7: k1; d1: k1,k2
    
    ldrsh      r6,[r0, #64]
    ldrsh      r7,[r0, #80]
    ldrsh      r8,[r0, #96]
    ldrsh      r9,[r0, #112]
    VMOV       d2, r9, r6                             @r6: k4; r9: k7; d2: k4,k7
    VMOV       d3, r7, r8                             @r8: k6; r7: k5; d3: k6,k5
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @                        we get q0: k1k2k3k0; q1: k6k5k4k7                            @ 
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VSHR.S32  q2,  q0, #31                            @q2: (k1>>31)(k2>>31)(k3>>31)(k0>>31)
    VSHR.S32  q3,  q1, #31                            @q3: (k6>>31)(k5>>31)(k4>>31)(k7>>31)
    
    VEOR      q4,  q0, q2                             @q4: (k1^(k1>>31))(k2^(k2>>31))(k3^(k3>>31))(k0^(k0>>31))
    VEOR      q5,  q1, q3                             @q5: (k6^(k6>>31))(k5^(k5>>31))(k4^(k4>>31))(k7^(k7>>31))
    
    mov       r10, #0
    VMOV.32   d4[0], r10                              @q2: ((k1>>31))((k2>>31))((k3>>31))(0)
    
    VSUB.S32  q6,  q4, q2
    VSUB.S32  q7,  q5, q3
    
    
    VADD.S32  q4,  q6, q7
    VPADD.S32 d4, d8, d9
    VPADDL.S32 d5, d4
    
    VMOV.32   r10, d5[0]
    
    cmp       r10, r5
    bge       go_on4
    mov       r10, #0x7f
    lsl       r10, r10, #8
    orr       r10, r10, #0xff
    strh      r10, [r0]
    add       r0,  r0, #2
    b         continue_vertical_block_loop4
    
go_on4:
    VADD.S32  q2,  q1, q0                             @q2: k1k2k3k0
    VSHL.S32  q3,  q1, #1
    VSUB.S32  q0,  q2, q3                             @q0: k6k5k4k7
    
    VPADD.S32 d2, d4, d5                              @d2: k1k0
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@k2 k3@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VSHR.U64  d4, d4, #32                             @d4: 0 k3
    VZIP.32   d4, d5                                  @d4: k2k3
    VSHL.S32  d4, d4, #1
    VSUB.S32  d4, d2, d4
    
    VPADDL.S32 d3, d2
    VMOV.32   r10, d3[0]
    strh      r10, [r0]
    
    VPADD.S32 d2, d4, d1                              @d2: k5k2
    VMOV.S32  d5, #181
    VSHL.U32  d5, d5, #2                              @d5: 724
    VDUP.32   d3, d31[0]                              @d3: round round
    VMLA.S32  d3, d5, d2                              @d3: k5k2
    VSHR.S32  d3, d3, #10
    VADD.S32  d4, d3, d4                              @d4: xxk2
    VMOV.32   r10, d4[0]
    strh      r10, [r0, #32]
    
    VMOV      r6, r7, d0                              @r6:k7; r7:k4
    VMOV      r8, r9, d1                              @r8:old k5; r9:k6
    add       r7, r8, r7                              @r7: k4=k4+k5
    VMOV.32   r8, d3[1]                               @r8: new k5
    add       r9, r6, r9                              @r9: k6=k6+k7
    sub       r12, r7, r9                             @r12:k0
    
    VMOV.S32   r11,  d31[0]
    mov        r10, #392
    mla        r11, r12, r10, r11                     @r11:k1
    mov        r10, #552
    add        r10, r10, #2
    mla        r12, r7, r10, r11                      @r12:k0
    mov        r10, #1328
    add        r10, r10, #10
    mla        r11, r9, r10, r11                      @r11:k1
    lsr        r7,  r12, #10                          @r7:k4
    lsr        r12, r11, #10                          @r12:k6
    
    add        r8, r8, r6                             @r8:k5=k5+k7
    lsl        r6, r6, #1
    sub        r6, r6, r8
    sub        r6, r6, r7                             @r6:k7
    add        r8, r8, r12                            @r8:k5
    
    strh       r8, [r0, #16]
    strh       r6,  [r0, #48]
    add        r0,  #2
    
continue_vertical_block_loop4:
    cmp        r0,  r4
    blt        vertical_block_loop4
    
    ldmia     sp!, {r4 - r12, pc}
    @ENDP  @ |Block4x4DCT_AANwSub|
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    Function:   Block2x2DCT_AANwSub
@    Date:       4/7/2011
@    Input:      prev
@    Output:     out[64] ==> next block
@    Purpose:    Input directly from prev frame. output 2x2 DCT
@    Modified:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .section .text
    .global	Block2x2DCT_AANwSub
    
Block2x2DCT_AANwSub:
    stmdb     sp!, {r4 - r12, lr}
    
    mov       r4,  #1
    mov       r12,  r4, lsl #9                        @r12: round
    add       r4,  r0, #128                           @r4: dst = out + 64;
    
    ldrsh     r5,  [r4]                               @r5: ColTh
    add       r0,  r0, #256
    
    VMOV.I32  d10,  #0xFF
    VSHL.U32  d11,  d10, #1                           @d11: generate mask
    
fdct_nextrow_Loop3:
    VLD1.32   d0,  [r1]                               @d0:tmp;   cur[1] cur[0]
    VLD1.32   d9,  [r2]                               @d9:tmp2;  pred[1] pred[0]
    
    VAND      d1,  d9, d10                            @d1: k4 k0; k4 = tmp2 & 0xFF; k0 = tmp2 & 0xFF;
    VSHL.U32  Q6,  Q0, #1                             @d12: (tmp<<1)(tmp<<1); d13:(k4<<1)(k0<<1)
    
    VAND      d3,  d12, d11                           @d3: k5 k1; k5 = mask & (tmp << 1);k1 = mask & (tmp << 1);
    VSUB.S32  d2,  d3, d13                            @d2: k4 k0; k4 = k5 - (k4 << 1); k0 = k1 - (k0 << 1);
    
    VSHR.S32  d3,  d9, #8
    VAND      d3,  d3, d10                            @d3: k5 = (tmp2 >> 8) & 0xFF; k1 = (tmp2 >> 8) & 0xFF;
    VSHR.S32  d1,  d0, #7
    VAND      d1,  d1, d11                            @d1: k6 = mask & (tmp >> 7); k2 = mask & (tmp >> 7);
    VSHL.S32  d3,  d3, #1
    VSUB.S32  d3,  d1, d3                             @d3: k5 k1
    
    VSHR.U32  d4,  d9, #16
    VAND      d4,  d4, d10                            @d4: k6 = (tmp2 >> 16) & 0xFF; k2 = (tmp2 >> 16) & 0xFF;
    VSHR.S32  d1,  d0, #15
    VAND      d1,  d1, d11                            @d1: k7 = mask & (tmp >> 15);k3 = mask & (tmp >> 15);
    VSHL.S32  d4,  d4, #1
    VSUB.S32  d4,  d1, d4                             @d4: k6 = k7 - (k6 << 1); k2 = k3 - (k2 << 1);
    
    VSHR.U32  d5,  d9, #24
    VAND      d5,  d5, d10                            @d5: k7 = (tmp2 >> 24) & 0xFF; k3 = (tmp2 >> 24) & 0xFF;
    VSHR.S32  d1,  d0, #23
    VAND      d1,  d1, d11                            @d1: tmp = mask & (tmp >> 23);k4 = mask & (tmp >> 23);
    VSHL.S32  d5,  d5, #1
    VSUB.S32  d5,  d1, d5                             @d5: k7 = tmp - (k7 << 1); k3 = k4 - (k3 << 1);
    
    add       r1,  r1, r3
    add       r2,  r2, #16
    
    VREV64.32 d4,  d4                                 @d4:  k2 k6
    VREV64.32 d5,  d5                                 @d5:  k3 k7
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@  fdct1   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VADD.S32  d6,  d2, d5                             @k3 = k3+k4; k0 = k0+k7
    VADD.S32  d7,  d3, d4                             @k2 = k2+k5; k1 = k1+k6
    
    VADD.S32   d8,  d7, d6
    VPADDL.S32  d0,  d8
    VMOV.32    r6,  d0[0]
    strh       r6,  [r4]                              @actually, k0 is sum of k0 - k7
    
    VREV64.32 d4,  d4                                 @swap back: d4:  k6 k2
    VREV64.32 d5,  d5                                 @@@@@@@@@@  d5:  k7 k3
    
    VUZP.32   Q1,  Q2                                 @q2: k7,k6,k5,k4
    VZIP.32   d6,  d7                                 @d6: k1, k0; d7: k2,k3
    VREV64.32 d6,  d6                                 @d6: k0, k1; d7: k2,k3
    
    VSHL.U32  Q2,  Q2, #1                             @d5: k7,k6; d4: k5,k4
    
    VSUB.S32  d5, d6, d5
    VSUB.S32  d4, d7, d4
    
    @@@@@@@@@@@@unused k3 k2 calculation@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @VREV64.32 d7,  d7                                 @d7: k3,k2
    @VADD.S32  d6,  d7, d6
    @VSHL.U32  d7,  d7, #1
    @VSUB.S32  d7,  d6, d7
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VMOV.32    r6,  d4[0]                               @r6: k4
    VMOV.32    r7,  d4[1]                               @r7: k5
    VMOV.32    r8,  d5[0]                               @r8: k6
    VMOV.32    r9,  d5[1]                               @r9: k7
    
    add        r6,  r6, r7
    add        r7,  r7, r8
    add        r8,  r8, r9
    
    mov        r10, #724
    mla        r11, r7, r10, r12
    lsr        r11, r11, #10                            @r11: k5
    
    mov        r10, #392
    mla        r7, r6, r10, r12
    mov        r10, #944
    add        r10, r10, #2
    mla        r7, r8, r10, r7
    lsr        r7, r7, #10                              @r7:  k6
    
    add        r11, r11, r9
    add        r11, r11, r7
    strh       r11, [r4, #2]
    
    add        r4, r4, #16
    cmp        r4, r0
    blt        fdct_nextrow_Loop3
    
    sub        r0, r0, #128
    add        r4, r0, #4

vertical_block_loop3:
    ldrsh       r6, [r0]
    ldrsh       r7, [r0, #16]
    ldrsh       r8, [r0, #32]
    ldrsh       r9, [r0, #48]
    VMOV       d0, r6, r9                             @r6: k0; r9: k3; d0: k3,k0
    VMOV       d1, r8, r7                             @r8: k2; r7: k1; d1: k1,k2
    
    ldrsh       r6,[r0, #64]
    ldrsh       r7,[r0, #80]
    ldrsh       r8,[r0, #96]
    ldrsh       r9,[r0, #112]
    VMOV       d2, r9, r6                             @r6: k4; r9: k7; d2: k4,k7
    VMOV       d3, r7, r8                             @r8: k6; r7: k5; d3: k6,k5
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @                        we get q0: k1k2k3k0; q1: k6k5k4k7                            @ 
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VSHR.S32  q2,  q0, #31                            @q2: (k1>>31)(k2>>31)(k3>>31)(k0>>31)
    VSHR.S32  q3,  q1, #31                            @q3: (k6>>31)(k5>>31)(k4>>31)(k7>>31)
    
    VEOR      q4,  q0, q2                             @q4: (k1^(k1>>31))(k2^(k2>>31))(k3^(k3>>31))(k0^(k0>>31))
    VEOR      q5,  q1, q3                             @q5: (k6^(k6>>31))(k5^(k5>>31))(k4^(k4>>31))(k7^(k7>>31))
    
    mov       r10, #0
    VMOV.32   d4[0], r10                              @q2: ((k1>>31))((k2>>31))((k3>>31))(0)
    
    VSUB.S32  q6,  q4, q2
    VSUB.S32  q7,  q5, q3
    
    
    VADD.S32  q4,  q6, q7
    VPADD.S32 d4, d8, d9
    VPADDL.S32 d5, d4
    
    VMOV.32   r10, d5[0]
    
    cmp       r10, r5
    bge       go_on3
    mov       r10, #0x7f
    lsl       r10, r10, #8
    orr       r10, r10, #0xff
    strh      r10, [r0]
    add       r0,  r0, #2
    b         continue_vertical_block_loop3
    
go_on3:
    VADD.S32  q2,  q1, q0                             @q2: k1k2k3k0
    VSHL.S32  q3,  q1, #1
    VSUB.S32  q0,  q2, q3                             @q0: k6k5k4k7
    
    VPADD.S32  d2, d4, d5                             @d2: k1k0
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@unused@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @VSHR.U64  d6, d4, #32                            @d6: 0 k3
    @VSLI.32   d6, d5, #1                             @d6: (k2<<1)(k3<<1)
    @VSUB.S32  d3, d2, d6                             @d3: k2k3
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VPADDL.S32 d3, d2
    VMOV.32   r10, d3[0]
    strh      r10, [r0]
    
    VMOV      r6, r7, d0                              @r6:k7; r7:k4
    VMOV      r8, r9, d1                              @r8:k5; r9:k6
    
    add       r7, r7, r8
    add       r8, r8, r9
    add       r9, r9, r6                              @r6:k7; r7:k4; r8:k5; r9: k6
    
    mov        r10, #724
    mla        r11, r10, r8, r12
    lsr        r11, r11, #10                          @r11:k5   released r8
    
    mov        r10, #392
    mla        r8, r7, r10, r12
    mov        r10, #944
    add        r10, r10, #2
    mla        r7, r9, r10, r8
    lsr        r7, r7, #10                            @r7:k6
    
    add        r11, r11, r6
    add        r11, r11, r7
    
    strh       r11, [r0, #16]
    add        r0,  #2
    
continue_vertical_block_loop3:
    cmp        r0,  r4
    blt        vertical_block_loop3
  
    ldmia     sp!, {r4 - r12, pc}
    @ENDP  @ |Block2x2DCT_AANwSub|

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    Function:   BlockDCT_AANIntra
@    Date:       4/7/2011
@    Input:      rec
@    Output:     out[64] ==> next block
@    Purpose:    Input directly from rec frame.
@    Modified:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.section .text
    .global	BlockDCT_AANIntra
BlockDCT_AANIntra:
    stmdb     sp!, {r4 - r12, lr}
    
    mov       r4,  #1
    mov       r2,  r4, lsl #9                         @r2: round    unused para
    add       r4,  r0, #128                           @r4: dst = out + 64;
    
    ldrsh     r5,  [r4]                               @r5: ColTh
    add       r0,  r0, #256
    
    VMOV.S32  d31[0],  r2
    VMOV.I32  d1,  #0xFF
    VSHL.U32  d1,  d1, #1                             @d1: generate mask
    
fdct_nextrow_Loop2:
    VLD1.32   d0,  [r1]                               @d0: curInt[1] curInt[0]
    
    VSHL.U32  d2,  d0, #1
    VAND      d2,  d2, d1                             @d2:  k4 k0
    VSHR.U32  d3,  d0, #7
    VAND      d3,  d3, d1                             @d3:  k5 k1
    VSHR.U32  d4,  d0, #15
    VAND      d4,  d4, d1                             @d4:  k6 k2
    VSHR.U32  d5,  d0, #23
    VAND      d5,  d5, d1                             @d5:  k7 k3
    
    add       r1,  r1, r3
    
    VREV64.32 d4,  d4                                 @d4:  k2 k6
    VREV64.32 d5,  d5                                 @d5:  k3 k7
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@  fdct1   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VADD.S32  d6,  d2, d5                             @k3 = k3+k4; k0 = k0+k7
    VADD.S32  d7,  d3, d4                             @k2 = k2+k5; k1 = k1+k6
    
    VREV64.32 d4,  d4                                 @swap back: d4:  k6 k2
    VREV64.32 d5,  d5                                 @@@@@@@@@@  d5:  k7 k3
    
    VUZP.32   Q1,  Q2                                 @q2: k7,k6,k5,k4
    VZIP.32   d6,  d7                                 @d6: k1, k0; d7: k2,k3
    VREV64.32 d6,  d6                                 @d6: k0, k1; d7: k2,k3
    
    VSHL.U32  Q2,  Q2, #1
    
    VSUB.S32  d5, d6, d5
    VSUB.S32  d4, d7, d4                             @d5: k7,k6; d4: k5,k4
    
    @@@@@@@@@@@@@@@@@@ k3 k2 calculation@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VREV64.32 d7,  d7                                @d7: k3,k2
    VADD.S32  d8,  d7, d6                            @d8: k0k1; k0=k0+k3,k1=k1+k2
    VSHL.U32  d7,  d7, #1
    VSUB.S32  d7,  d8, d7
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VPADDL.S32 d0,  d8
    VMOV.32    r6,  d0[0]
    strh       r6,  [r4]                              @actually, k0 is sum of k0 - k7
    VSHL.S32   d8,  d8, #1
    VSUB.S32   d0,  d0, d8
    VMOV.32    r6,  d0[0]
    strh       r6,  [r4, #8]
    
    VPADD.S32  d0,  d4, d5                            @d0:k6k4   k6=k6+k7,k4=k4+k5
    VMOV.32    r7,  d4[1]                             @r7: k5
    VMOV.32    r8,  d5[0]                             @r8: k6
    add        r7,  r7, r8                            @r7: k5=k5+k6
    VMOV.32    r8,  r9, d7                            @r8: k2; r9: k3
    add        r6,  r8, r9                            @r6: k2
    
    mov        r10, #724
    mla        r11, r7, r10, r2                       @r11:k5
    mla        r7,  r6, r10, r2                       @r7: k2
    lsr        r7,  r7, #10
    lsr        r11, r11, #10
    add        r7,  r7, r9
    strh       r7,  [r4, #4]
    
    lsl        r9,  r9, #1
    sub        r9,  r9, r7
    lsl        r9,  r9, #1
    strh       r9,  [r4, #12]
    
    VMOV.32    r6,  d0[0]                             @r6: k4
    VMOV.32    r8,  d0[1]                             @r8: k6
    VMOV.32    r9,  d5[1]                             @r9: k7
    
    sub        r7, r6, r8                             @r7:k0
    mov        r10, #392
    mla        r12, r7, r10, r2                       @r12:k1
    mov        r10, #552
    add        r10, r10, #2
    mla        r7, r6, r10, r12                       @r7:k0
    mov        r10, #1328
    add        r10, r10, #10
    mla        r12, r8, r10, r12
    lsr        r6, r7, #10                            @r6:  k4
    lsr        r8, r12, #10                           @r8:  k6
    
    add        r11, r11, r9                           @r9: k7, r11:k5
    lsl        r9, r9, #1
    sub        r9, r9, r11
    add        r6, r9, r6
    lsl        r9, r9, #1
    sub        r9, r9, r6
    add        r11, r11, r8
    lsl        r6,  r6, #1
    sub        r8,  r11, r8, lsl #1
    lsl        r8,  r8, #2
    strh       r6,  [r4, #10]
    strh       r11, [r4, #2]
    strh       r9, [r4, #6]
    strh       r8, [r4, #14]
    
    add        r4, r4, #16
    cmp        r4, r0
    blt        fdct_nextrow_Loop2
    
    sub        r0, r0, #128
    add        r4, r0, #16

vertical_block_loop2:
    ldrsh      r6, [r0]
    ldrsh      r7, [r0, #16]
    ldrsh      r8, [r0, #32]
    ldrsh      r9, [r0, #48]
    VMOV       d0, r6, r9                             @r6: k0; r9: k3; d0: k3,k0
    VMOV       d1, r8, r7                             @r8: k2; r7: k1; d1: k1,k2
    
    ldrsh      r6,[r0, #64]
    ldrsh      r7,[r0, #80]
    ldrsh      r8,[r0, #96]
    ldrsh      r9,[r0, #112]
    VMOV       d2, r9, r6                             @r6: k4; r9: k7; d2: k4,k7
    VMOV       d3, r7, r8                             @r8: k6; r7: k5; d3: k6,k5
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @                        we get q0: k1k2k3k0; q1: k6k5k4k7                            @ 
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VSHR.S32  q2,  q0, #31                            @q2: (k1>>31)(k2>>31)(k3>>31)(k0>>31)
    VSHR.S32  q3,  q1, #31                            @q3: (k6>>31)(k5>>31)(k4>>31)(k7>>31)
    
    VEOR      q4,  q0, q2                             @q4: (k1^(k1>>31))(k2^(k2>>31))(k3^(k3>>31))(k0^(k0>>31))
    VEOR      q5,  q1, q3                             @q5: (k6^(k6>>31))(k5^(k5>>31))(k4^(k4>>31))(k7^(k7>>31))
    
    mov       r10, #0
    VMOV.32   d4[0], r10                              @q2: ((k1>>31))((k2>>31))((k3>>31))(0)
    
    VSUB.S32  q6,  q4, q2
    VSUB.S32  q7,  q5, q3
    
    
    VADD.S32  q4,  q6, q7
    VPADD.S32 d4, d8, d9
    VPADDL.S32 d5, d4
    
    VMOV.32   r10, d5[0]
    
    cmp       r10, r5
    bge       go_on2
    mov       r10, #0x7f
    lsl       r10, r10, #8
    orr       r10, r10, #0xff
    strh      r10, [r0]
    add       r0,  r0, #2
    b         continue_vertical_block_loop2
    
go_on2:
    VADD.S32  q2,  q1, q0                             @q2: k1k2k3k0
    VSHL.S32  q3,  q1, #1
    VSUB.S32  q0,  q2, q3                             @q0: k6k5k4k7
    
    VPADD.S32 d2, d4, d5                              @d2: k1k0
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@k2 k3@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VSHR.U64  d4, d4, #32                             @d4: 0 k3
    VZIP.32   d4, d5                                  @d4: k2k3
    VSHL.S32  d4, d4, #1
    VSUB.S32  d4, d2, d4
    
    VPADDL.S32 d3, d2
    VMOV.32   r10, d3[0]
    strh      r10, [r0]
    
    VSHR.U64  d2,  d2, #32
    VSHL.S32  d2,  d2, #1
    VSUB.S32  d2,  d3, d2
    VMOV.32   r10, d2[0]
    strh      r10, [r0, #64]
    
    VPADD.S32 d2, d4, d1                              @d2: k5k2
    VMOV.S32  d5, #181
    VSHL.U32  d5, d5, #2                              @d5: 724
    VDUP.32   d3, d31[0]                              @d3: round round
    VMLA.S32  d3, d5, d2                              @d3: k5k2
    VSHR.S32  d3, d3, #10
    VADD.S32  d5, d3, d4                              @d5: xxk2
    VMOV.32   r10, d5[0]
    strh      r10, [r0, #32]
    
    VSHL.S32  d4, d4, #1
    VSUB.S32  d4, d4, d5
    VSHL.S32  d4, d4, #1
    VMOV.32   r10, d4[0]
    strh      r10, [r0, #96]
    
    VMOV      r6, r7, d0                              @r6:k7; r7:k4
    VMOV      r8, r9, d1                              @r8:old k5; r9:k6
    add       r7, r8, r7                              @r7: k4=k4+k5
    VMOV.32   r8, d3[1]                               @r8: new k5
    add       r9, r6, r9                              @r9: k6=k6+k7
    sub       r12, r7, r9                             @r12:k0
    
    mov        r10, #392
    mla        r11, r12, r10, r2                      @r11:k1
    mov        r10, #552
    add        r10, r10, #2
    mla        r12, r7, r10, r11                      @r12:k0
    mov        r10, #1328
    add        r10, r10, #10
    mla        r11, r9, r10, r11                      @r11:k1
    lsr        r7,  r12, #10                          @r7:k4
    lsr        r12, r11, #10                          @r12:k6
    
    add        r8, r8, r6                             @r8:k5=k5+k7
    lsl        r6, r6, #1
    sub        r6, r6, r8                             @r6: k7
    add        r7, r7, r6
    lsl        r6, r6, #1
    sub        r6, r6, r7                             @r6:k7
    add        r8, r8, r12                            @r8:k5
    lsl        r7, r7, #1
    sub        r12,r8, r12, lsl #1
    lsl        r12, r12, #2
    strh       r8, [r0, #16]
    strh       r6,  [r0, #48]
    strh       r12, [r0, #112]
    strh       r7,  [r0, #80]
    
    add        r0,  #2
    
continue_vertical_block_loop2:
    cmp        r0,  r4
    blt        vertical_block_loop2
    
    ldmia     sp!, {r4 - r12, pc}
    @ENDP  @ |Block4x4DCT_AANIntra|
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    Function:   Block4x4DCT_AANIntra
@    Date:       4/6/2011
@    Input:      prev
@    Output:     out[64] ==> next block
@    Purpose:    Input directly from prev frame. output 2x2 DCT
@    Modified:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .section .text
    .global	Block4x4DCT_AANIntra
Block4x4DCT_AANIntra:
    stmdb     sp!, {r4 - r12, lr}
    
    mov       r4,  #1
    mov       r2,  r4, lsl #9                         @r2: round    unused para
    add       r4,  r0, #128                           @r4: dst = out + 64;
    
    ldrsh     r5,  [r4]                               @r5: ColTh
    add       r0,  r0, #256
    
    VMOV.S32  d31[0],  r2
    VMOV.I32  d1,  #0xFF
    VSHL.U32  d1,  d1, #1                             @d1: generate mask
    
fdct_nextrow_Loop1:
    VLD1.32   d0,  [r1]                               @d0: curInt[1] curInt[0]
    
    VSHL.U32  d2,  d0, #1
    VAND      d2,  d2, d1                             @d2:  k4 k0
    VSHR.U32  d3,  d0, #7
    VAND      d3,  d3, d1                             @d3:  k5 k1
    VSHR.U32  d4,  d0, #15
    VAND      d4,  d4, d1                             @d4:  k6 k2
    VSHR.U32  d5,  d0, #23
    VAND      d5,  d5, d1                             @d5:  k7 k3
    
    add       r1,  r1, r3
    
    VREV64.32 d4,  d4                                 @d4:  k2 k6
    VREV64.32 d5,  d5                                 @d5:  k3 k7
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@  fdct1   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VADD.S32  d6,  d2, d5                             @k3 = k3+k4; k0 = k0+k7
    VADD.S32  d7,  d3, d4                             @k2 = k2+k5; k1 = k1+k6
    
    VREV64.32 d4,  d4                                 @swap back: d4:  k6 k2
    VREV64.32 d5,  d5                                 @@@@@@@@@@  d5:  k7 k3
    
    VUZP.32   Q1,  Q2                                 @q2: k7,k6,k5,k4
    VZIP.32   d6,  d7                                 @d6: k1, k0; d7: k2,k3
    VREV64.32 d6,  d6                                 @d6: k0, k1; d7: k2,k3
    
    VSHL.U32  Q2,  Q2, #1
    
    VSUB.S32  d5, d6, d5
    VSUB.S32  d4, d7, d4                             @d5: k7,k6; d4: k5,k4
    
    @@@@@@@@@@@@@@@@@@ k3 k2 calculation@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VREV64.32 d7,  d7                                @d7: k3,k2
    VADD.S32  d8,  d7, d6                            @d8: k0k1; k0=k0+k3,k1=k1+k2
    VSHL.U32  d7,  d7, #1
    VSUB.S32  d7,  d8, d7
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VPADDL.S32  d0,  d8
    VMOV.32    r6,  d0[0]
    strh       r6,  [r4]                              @actually, k0 is sum of k0 - k7
    
    VPADD.S32  d0,  d4, d5                            @d0:k6k4   k6=k6+k7,k4=k4+k5
    VMOV.32    r7,  d4[1]                             @r7: k5
    VMOV.32    r8,  d5[0]                             @r8: k6
    add        r7,  r7, r8                            @r7: k5=k5+k6
    VMOV.32    r8,  r9, d7                            @r8: k2; r9: k3
    add        r6,  r8, r9                            @r6: k2
    
    mov        r10, #724
    mla        r11, r7, r10, r2                       @r11:k5
    mla        r7,  r6, r10, r2                       @r7: k2
    lsr        r7,  r7, #10
    lsr        r11, r11, #10
    add        r7,  r7, r9
    strh       r7,  [r4, #4]
    
    VMOV.32    r6,  d0[0]                             @r6: k4
    VMOV.32    r8,  d0[1]                             @r8: k6
    VMOV.32    r9,  d5[1]                             @r9: k7
    
    sub        r7, r6, r8                             @r7:k0
    mov        r10, #392
    mla        r12, r7, r10, r2                       @r12:k1
    mov        r10, #552
    add        r10, r10, #2
    mla        r7, r6, r10, r12                       @r7:k0
    mov        r10, #1328
    add        r10, r10, #10
    mla        r12, r8, r10, r12
    lsr        r6, r7, #10                            @r6:  k4
    lsr        r8, r12, #10                           @r8:  k6
    
    add        r11, r11, r9
    lsl        r9, r9, #1
    sub        r9, r9, r11
    sub        r9, r9, r6
    add        r11, r11, r8
    strh       r11, [r4, #2]
    strh       r9, [r4, #6]
    
    add        r4, r4, #16
    cmp        r4, r0
    blt        fdct_nextrow_Loop1
    
    sub        r0, r0, #128
    add        r4, r0, #8

vertical_block_loop1:
    ldrsh      r6, [r0]
    ldrsh      r7, [r0, #16]
    ldrsh      r8, [r0, #32]
    ldrsh      r9, [r0, #48]
    VMOV       d0, r6, r9                             @r6: k0; r9: k3; d0: k3,k0
    VMOV       d1, r8, r7                             @r8: k2; r7: k1; d1: k1,k2
    
    ldrsh      r6,[r0, #64]
    ldrsh      r7,[r0, #80]
    ldrsh      r8,[r0, #96]
    ldrsh      r9,[r0, #112]
    VMOV       d2, r9, r6                             @r6: k4; r9: k7; d2: k4,k7
    VMOV       d3, r7, r8                             @r8: k6; r7: k5; d3: k6,k5
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @                        we get q0: k1k2k3k0; q1: k6k5k4k7                            @ 
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VSHR.S32  q2,  q0, #31                            @q2: (k1>>31)(k2>>31)(k3>>31)(k0>>31)
    VSHR.S32  q3,  q1, #31                            @q3: (k6>>31)(k5>>31)(k4>>31)(k7>>31)
    
    VEOR      q4,  q0, q2                             @q4: (k1^(k1>>31))(k2^(k2>>31))(k3^(k3>>31))(k0^(k0>>31))
    VEOR      q5,  q1, q3                             @q5: (k6^(k6>>31))(k5^(k5>>31))(k4^(k4>>31))(k7^(k7>>31))
    
    mov       r10, #0
    VMOV.32   d4[0], r10                              @q2: ((k1>>31))((k2>>31))((k3>>31))(0)
    
    VSUB.S32  q6,  q4, q2
    VSUB.S32  q7,  q5, q3
    
    
    VADD.S32  q4,  q6, q7
    VPADD.S32 d4, d8, d9
    VPADDL.S32 d5, d4
    
    VMOV.32   r10, d5[0]
    
    cmp       r10, r5
    bge       go_on1
    mov       r10, #0x7f
    lsl       r10, r10, #8
    orr       r10, r10, #0xff
    strh      r10, [r0]
    add       r0,  r0, #2
    b         continue_vertical_block_loop1
    
go_on1:
    VADD.S32  q2,  q1, q0                             @q2: k1k2k3k0
    VSHL.S32  q3,  q1, #1
    VSUB.S32  q0,  q2, q3                             @q0: k6k5k4k7
    
    VPADD.S32 d2, d4, d5                              @d2: k1k0
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@k2 k3@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VSHR.U64  d4, d4, #32                             @d4: 0 k3
    VZIP.32   d4, d5                                  @d4: k2k3
    VSHL.S32  d4, d4, #1
    VSUB.S32  d4, d2, d4
    
    VPADDL.S32 d3, d2
    VMOV.32   r10, d3[0]
    strh      r10, [r0]
    
    VPADD.S32 d2, d4, d1                              @d2: k5k2
    VMOV.S32  d5, #181
    VSHL.U32  d5, d5, #2                              @d5: 724
    VDUP.32   d3, d31[0]                              @d3: round round
    VMLA.S32  d3, d5, d2                              @d3: k5k2
    VSHR.S32  d3, d3, #10
    VADD.S32  d4, d3, d4                              @d4: xxk2
    VMOV.32   r10, d4[0]
    strh      r10, [r0, #32]
    
    VMOV      r6, r7, d0                              @r6:k7; r7:k4
    VMOV      r8, r9, d1                              @r8:old k5; r9:k6
    add       r7, r8, r7                              @r7: k4=k4+k5
    VMOV.32   r8, d3[1]                               @r8: new k5
    add       r9, r6, r9                              @r9: k6=k6+k7
    sub       r12, r7, r9                             @r12:k0
    
    mov        r10, #392
    mla        r11, r12, r10, r2                      @r11:k1
    mov        r10, #552
    add        r10, r10, #2
    mla        r12, r7, r10, r11                      @r12:k0
    mov        r10, #1328
    add        r10, r10, #10
    mla        r11, r9, r10, r11                      @r11:k1
    lsr        r7,  r12, #10                          @r7:k4
    lsr        r12, r11, #10                          @r12:k6
    
    add        r8, r8, r6                             @r8:k5=k5+k7
    lsl        r6, r6, #1
    sub        r6, r6, r8
    sub        r6, r6, r7                             @r6:k7
    add        r8, r8, r12                            @r8:k5
    
    strh       r8, [r0, #16]
    strh       r6,  [r0, #48]
    add        r0,  #2
    
continue_vertical_block_loop1:
    cmp        r0,  r4
    blt        vertical_block_loop1
    
    ldmia     sp!, {r4 - r12, pc}
    @ENDP  @ |Block4x4DCT_AANIntra|
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    Function:   Block2x2DCT_AANIntra
@    Date:       3/23/2011
@    Input:      prev
@    Output:     out[64] ==> next block
@    Purpose:    Input directly from prev frame. output 2x2 DCT
@    Modified:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .section .text
    .global	Block2x2DCT_AANIntra
    
Block2x2DCT_AANIntra:
    stmdb     sp!, {r4 - r12, lr}
    
    mov       r4,  #1
    mov       r2,  r4, lsl #9                         @r2: round    unused para
    add       r4,  r0, #128                           @r4: dst = out + 64;
    
    ldrsh     r5,  [r4]                               @r5: ColTh
    add       r0,  r0, #256
    
    VMOV.I32  d1,  #0xFF
    VSHL.U32  d1,  d1, #1                             @d1: generate mask
    
fdct_nextrow_Loop:
    VLD1.32   d0,  [r1]                               @d0: curInt[1] curInt[0]
    
    VSHL.U32  d2,  d0, #1
    VAND      d2,  d2, d1                             @d2:  k4 k0
    VSHR.U32  d3,  d0, #7
    VAND      d3,  d3, d1                             @d3:  k5 k1
    VSHR.U32  d4,  d0, #15
    VAND      d4,  d4, d1                             @d4:  k6 k2
    VSHR.U32  d5,  d0, #23
    VAND      d5,  d5, d1                             @d5:  k7 k3
    
    add       r1,  r1, r3
    
    VREV64.32 d4,  d4                                 @d4:  k2 k6
    VREV64.32 d5,  d5                                 @d5:  k3 k7
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@  fdct1   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    VADD.S32  d6,  d2, d5                             @k3 = k3+k4; k0 = k0+k7
    VADD.S32  d7,  d3, d4                             @k2 = k2+k5; k1 = k1+k6
    
    VADD.S32   d8,  d7, d6
    VPADDL.S32  d0,  d8
    VMOV.32    r6,  d0[0]
    strh       r6,  [r4]                              @actually, k0 is sum of k0 - k7
    
    VREV64.32 d4,  d4                                 @swap back: d4:  k6 k2
    VREV64.32 d5,  d5                                 @@@@@@@@@@  d5:  k7 k3
    
    VUZP.32   Q1,  Q2                                 @q2: k7,k6,k5,k4
    VZIP.32   d6,  d7                                 @d6: k1, k0; d7: k2,k3
    VREV64.32 d6,  d6                                 @d6: k0, k1; d7: k2,k3
    
    VSHL.U32  Q2,  Q2, #1                             @d5: k7,k6; d4: k5,k4
    
    VSUB.S32  d5, d6, d5
    VSUB.S32  d4, d7, d4
    
    @@@@@@@@@@@@unused k3 k2 calculation@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @VREV64.32 d7,  d7                                 @d7: k3,k2
    @VADD.S32  d6,  d7, d6
    @VSHL.U32  d7,  d7, #1
    @VSUB.S32  d7,  d6, d7
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VMOV.32    r6,  d4[0]                               @r6: k4
    VMOV.32    r7,  d4[1]                               @r7: k5
    VMOV.32    r8,  d5[0]                               @r8: k6
    VMOV.32    r9,  d5[1]                               @r9: k7
    
    add        r6,  r6, r7
    add        r7,  r7, r8
    add        r8,  r8, r9
    
    mov        r10, #724
    mla        r11, r7, r10, r2
    lsr        r11, r11, #10                            @r11: k5
    
    mov        r10, #392
    mla        r7, r6, r10, r2
    mov        r10, #944
    add        r10, r10, #2
    mla        r7, r8, r10, r7
    lsr        r7, r7, #10                              @r7:  k6
    
    add        r11, r11, r9
    add        r11, r11, r7
    strh       r11, [r4, #2]
    
    add        r4, r4, #16
    cmp        r4, r0
    blt        fdct_nextrow_Loop
    
    sub        r0, r0, #128
    add        r4, r0, #4

vertical_block_loop:
    ldrsh       r6, [r0]
    ldrsh       r7, [r0, #16]
    ldrsh       r8, [r0, #32]
    ldrsh       r9, [r0, #48]
    VMOV       d0, r6, r9                             @r6: k0; r9: k3; d0: k3,k0
    VMOV       d1, r8, r7                             @r8: k2; r7: k1; d1: k1,k2
    
    ldrsh       r6,[r0, #64]
    ldrsh       r7,[r0, #80]
    ldrsh       r8,[r0, #96]
    ldrsh       r9,[r0, #112]
    VMOV       d2, r9, r6                             @r6: k4; r9: k7; d2: k4,k7
    VMOV       d3, r7, r8                             @r8: k6; r7: k5; d3: k6,k5
    
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @                        we get q0: k1k2k3k0; q1: k6k5k4k7                            @ 
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VSHR.S32  q2,  q0, #31                            @q2: (k1>>31)(k2>>31)(k3>>31)(k0>>31)
    VSHR.S32  q3,  q1, #31                            @q3: (k6>>31)(k5>>31)(k4>>31)(k7>>31)
    
    VEOR      q4,  q0, q2                             @q4: (k1^(k1>>31))(k2^(k2>>31))(k3^(k3>>31))(k0^(k0>>31))
    VEOR      q5,  q1, q3                             @q5: (k6^(k6>>31))(k5^(k5>>31))(k4^(k4>>31))(k7^(k7>>31))
    
    mov       r10, #0
    VMOV.32   d4[0], r10                              @q2: ((k1>>31))((k2>>31))((k3>>31))(0)
    
    VSUB.S32  q6,  q4, q2
    VSUB.S32  q7,  q5, q3
    
    
    VADD.S32  q4,  q6, q7
    VPADD.S32 d4, d8, d9
    VPADDL.S32 d5, d4
    
    VMOV.32   r10, d5[0]
    
    cmp       r10, r5
    bge       go_on
    mov       r10, #0x7f
    lsl       r10, r10, #8
    orr       r10, r10, #0xff
    strh      r10, [r0]
    add       r0,  r0, #2
    b         continue_vertical_block_loop
    
go_on:
    VADD.S32  q2,  q1, q0                             @q2: k1k2k3k0
    VSHL.S32  q3,  q1, #1
    VSUB.S32  q0,  q2, q3                             @q0: k6k5k4k7
    
    VPADD.S32  d2, d4, d5                             @d2: k1k0
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@unused@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @VSHR.U64  d6, d4, #32                            @d6: 0 k3
    @VSLI.32   d6, d5, #1                             @d6: (k2<<1)(k3<<1)
    @VSUB.S32  d3, d2, d6                             @d3: k2k3
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    VPADDL.S32 d3, d2
    VMOV.32   r10, d3[0]
    strh      r10, [r0]
    
    VMOV      r6, r7, d0                              @r6:k7; r7:k4
    VMOV      r8, r9, d1                              @r8:k5; r9:k6
    
    add       r7, r7, r8
    add       r8, r8, r9
    add       r9, r9, r6                              @r6:k7; r7:k4; r8:k5; r9: k6
    
    mov        r10, #724
    mla        r11, r10, r8, r2
    lsr        r11, r11, #10                          @r11:k5   released r8
    
    mov        r10, #392
    mla        r8, r7, r10, r2
    mov        r10, #944
    add        r10, r10, #2
    mla        r7, r9, r10, r8
    lsr        r7, r7, #10                            @r7:k6
    
    add        r11, r11, r6
    add        r11, r11, r7
    
    strh       r11, [r0, #16]
    add        r0,  #2
    
continue_vertical_block_loop:
    cmp        r0,  r4
    blt        vertical_block_loop
  
    ldmia     sp!, {r4 - r12, pc}
    @ENDP  @ |Block2x2DCT_AANIntra|

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    Function:   Block1x1DCTwSub
@    Date:       3/23/2011
@    Input:      block
@    Output:     y
@    Purpose:    Compute DC value only
@    Modified:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .section .text
    .global	Block1x1DCTwSub

Block1x1DCTwSub:
	stmdb      sp!, {r4 - r5, lr}

    sub        r4, r3, #8                               @r4: offset2
    add        r5, r2, #128                             @r5: end
    
    VMOV.I64   d4, #0
Block1x1DCTwSubLoop:
    VLD1.8     {d0},  [r1]!                             @d0: load 8 8bits data of cur
    VLD1.8     {d1},  [r2]!                             @d1: load 8 8bits data of pred
    
    VPADDL.U8  d0, d0
    VPADDL.U16 d0, d0
    VPADDL.U8  d1, d1
    VPADDL.U16 d1, d1                                   @must pair add first to avoid overflow
    VSUB.S32   d2, d0, d1
    VADD.S32   d4, d4, d2
    
    add        r1, r1, r4
    add        r2, r2, #8
    cmp        r2, r5
    blt        Block1x1DCTwSubLoop
    
    VPADDL.S32 d2, d4
    @@@@@@@@@@be carefule signed extension@@@@@@@@@@@@@@
    VSHL.U64   d2, d2, #48
    VSHR.U64   d2, d2, #48
    VSHR.S16   d2, d2, #3
	VMOV.I64   d3, #0
	VST1.16    {d2, d3}, [r0]
	
    ldmia      sp!, {r4 - r5, pc}
	@ENDP  @ |Block1x1DCTwSub|
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    Function:   Block1x1DCTIntra
@    Date:       3/23/2011
@    Input:      prev
@    Output:     out
@    Purpose:    Compute DC value only
@    Modified:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@rewrite function, not efficent@@@@@@@@@@@@@@@@@@@@@@@@@
@    .section .text
@    .global    Block1x1DCTIntra
	
@Block1x1DCTIntra:
@	stmdb      sp!, {r4, lr}
	
@	add        r4, r1, r3, lsl #3                          @r4:   end = cur + (width << 3);
	
@	VMOV.I32   d0, #0                                      @d0:   temp
@DCT_LOOP:
@    VLD1.32    {d1}, [r1]                                  @load cur 2 32bit word
@    VSRA.U32   d0, d1, #24
@    VMOV.I32   d3, #0x00FF0000
@    VAND       d3, d3, d1
@    VSRA.U32   d0, d3, #16
@    VMOV.I32   d3, #0x0000FF00
@    VAND       d3, d3, d1
@    VSRA.U32   d0, d3, #8
@    VMOV.I32   d3, #0x000000FF
@    VAND       d3, d3, d1
@    VADD.U32   d0, d3, d0
    
@    add        r1, r1, r3
@    cmp        r1, r4
@    blt        DCT_LOOP
@    
@    VPADDL.U32 d0, d0
@    VSHR.U64   d0, d0, #3
@    VMOV.I64   d1, #0
@    VST1.16    {d0, d1}, [r0]
    
@    ldmia     sp!, {r4, pc}
	
    .section .text
    .global    Block1x1DCTIntra
    
Block1x1DCTIntra:
    stmdb      sp!, {r4, lr}
    
    add        r4, r1, r3, lsl #3                          @r4:   end = cur + (width << 3);
    
    VMOV.I32   d0, #0                                      @d0:   temp
DCT_LOOP:
    VLD1.32    {d1}, [r1]                                  @load cur 2 32bit word
    VPADDL.U8  d1, d1
    VPADDL.U16 d1, d1
    VADD.U32   d0, d0, d1
    
    add        r1, r1, r3
    cmp        r1, r4
    blt        DCT_LOOP
    
    VPADDL.U32 d0, d0
    VSHR.U64   d0, d0, #3
    VMOV.I64   d1, #0
    VST1.16    {d0, d1}, [r0]
    
    ldmia     sp!, {r4, pc}
    @ENDP  @ |Block1x1DCTIntra|

    .end
	