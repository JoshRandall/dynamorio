/* **********************************************************
 * Copyright (c) 2019 Google, Inc.  All rights reserved.
 * **********************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Google, Inc. nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL GOOGLE, INC. OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef ASM_CODE_ONLY /* C code */
#    include "tools.h"

#    ifndef __AVX512F__
#        error "Build error, should only be added with AVX-512 support."
#    endif
#    ifdef X64
#        define NUM_SIMD_REGS 32
#    else
#        define NUM_SIMD_REGS 8
#    endif
#    define NUM_OPMASK_REGS 8

#    define VERBOSE 0

NOINLINE void
marker();
/* asm routines */
void
init_zmm(byte *buf);
void
get_zmm(byte *buf);
void
init_opmask(byte *buf);
void
get_opmask(byte *buf);

void
run_avx512()
{
    byte zmm_buf[NUM_SIMD_REGS * 64];
    byte zmm_ref[NUM_SIMD_REGS * 64];
    byte opmask_buf[NUM_OPMASK_REGS * 2];
    byte opmask_ref[NUM_OPMASK_REGS * 2];
    memset(zmm_buf, 0xde, sizeof(zmm_buf));
    memset(zmm_ref, 0xab, sizeof(zmm_ref));
    /* Even though DynamoRIO is capable of handling AVX512BW wide 64-bit mask registers,
     * we're simplifying the test here and are checking only AVX512F wide 16-bit mask
     * registers. This also lets us run the test in 32-bit mode.
     */
    memset(opmask_buf, 0xde, sizeof(opmask_buf));
    memset(opmask_ref, 0xab, sizeof(opmask_ref));

    init_zmm(zmm_ref);
    init_opmask(opmask_ref);

    marker();

    get_zmm(zmm_buf);
    get_opmask(opmask_buf);

#    if VERBOSE
    int zmm_off = 0;
    for (int i = 0; i < NUM_SIMD_REGS; ++i, zmm_off += 64) {
        print("got zmm[%d]:\n", i);
        for (int b = zmm_off; b < zmm_off + 64; ++b)
            print("%x", zmm_buf[b]);
        print("\nref zmm[%d]:\n", i);
        for (int b = zmm_off; b < zmm_off + 64; ++b)
            print("%x", zmm_ref[b]);
        print("\n");
    }
    int opmask_off = 0;
    for (int i = 0; i < NUM_OPMASK_REGS; ++i, opmask_off += 2) {
        print("got k[%i]:\n", i);
        for (int b = opmask_off; b < opmask_off + 2; ++b)
            print("%x", opmask_buf[b]);
        print("\nref k[%d]:\n", i);
        for (int b = opmask_off; b < opmask_off + 2; ++b)
            print("%x", opmask_ref[b]);
        print("\n");
    }
#    endif

    if (memcmp(zmm_buf, zmm_ref, sizeof(zmm_buf)))
        print("ERROR: wrong zmm value\n");
    if (memcmp(opmask_buf, opmask_ref, sizeof(opmask_buf)))
        print("ERROR: wrong mask value\n");

    print("Ok\n");
}

int
main()
{
    run_avx512();
    return 0;
}

#else /* asm code *************************************************************/
#    include "asm_defines.asm"
/* clang-format off */
START_FILE

#ifdef X64
# define FRAME_PADDING 0
#else
# define FRAME_PADDING 0
#endif

#define FUNCNAME marker
        DECLARE_FUNC_SEH(FUNCNAME)
GLOBAL_LABEL(FUNCNAME:)
        sub      REG_XSP, FRAME_PADDING
        END_PROLOG

        mov      REG_XAX, 0x12345678
        mov      REG_XAX, 0x12345678

        add      REG_XSP, FRAME_PADDING
        ret
        END_FUNC(FUNCNAME)
#undef FUNCNAME

#define FUNCNAME init_zmm
        DECLARE_FUNC_SEH(FUNCNAME)
GLOBAL_LABEL(FUNCNAME:)
        mov      REG_XCX, ARG1
        PUSH_CALLEE_SAVED_REGS()
        sub      REG_XSP, FRAME_PADDING
        END_PROLOG

        vmovups  zmm0,  [REG_XCX]
        vmovups  zmm1,  [REG_XCX + 0x40]
        vmovups  zmm2,  [REG_XCX + 0x80]
        vmovups  zmm3,  [REG_XCX + 0xc0]
        vmovups  zmm4,  [REG_XCX + 0x100]
        vmovups  zmm5,  [REG_XCX + 0x140]
        vmovups  zmm6,  [REG_XCX + 0x180]
        vmovups  zmm7,  [REG_XCX + 0x1c0]
#ifdef X64
        vmovups  zmm8,  [REG_XCX + 0x200]
        vmovups  zmm9,  [REG_XCX + 0x240]
        vmovups  zmm10, [REG_XCX + 0x280]
        vmovups  zmm11, [REG_XCX + 0x2c0]
        vmovups  zmm12, [REG_XCX + 0x300]
        vmovups  zmm13, [REG_XCX + 0x340]
        vmovups  zmm14, [REG_XCX + 0x380]
        vmovups  zmm15, [REG_XCX + 0x3c0]
        vmovups  zmm16, [REG_XCX + 0x400]
        vmovups  zmm17, [REG_XCX + 0x440]
        vmovups  zmm18, [REG_XCX + 0x480]
        vmovups  zmm19, [REG_XCX + 0x4c0]
        vmovups  zmm20, [REG_XCX + 0x500]
        vmovups  zmm21, [REG_XCX + 0x540]
        vmovups  zmm22, [REG_XCX + 0x580]
        vmovups  zmm23, [REG_XCX + 0x5c0]
        vmovups  zmm24, [REG_XCX + 0x600]
        vmovups  zmm25, [REG_XCX + 0x640]
        vmovups  zmm26, [REG_XCX + 0x680]
        vmovups  zmm27, [REG_XCX + 0x6c0]
        vmovups  zmm28, [REG_XCX + 0x700]
        vmovups  zmm29, [REG_XCX + 0x740]
        vmovups  zmm30, [REG_XCX + 0x780]
        vmovups  zmm31, [REG_XCX + 0x7c0]
#endif

        add      REG_XSP, FRAME_PADDING
        POP_CALLEE_SAVED_REGS()
        ret
        END_FUNC(FUNCNAME)
#undef FUNCNAME

#define FUNCNAME get_zmm
        DECLARE_FUNC_SEH(FUNCNAME)
GLOBAL_LABEL(FUNCNAME:)
        mov      REG_XCX, ARG1
        PUSH_CALLEE_SAVED_REGS()
        sub      REG_XSP, FRAME_PADDING
        END_PROLOG

       vmovups  [REG_XCX],         zmm0
       vmovups  [REG_XCX + 0x40],  zmm1
       vmovups  [REG_XCX + 0x80],  zmm2
       vmovups  [REG_XCX + 0xc0],  zmm3
       vmovups  [REG_XCX + 0x100], zmm4
       vmovups  [REG_XCX + 0x140], zmm5
       vmovups  [REG_XCX + 0x180], zmm6
       vmovups  [REG_XCX + 0x1c0], zmm7
#ifdef X64
       vmovups  [REG_XCX + 0x200], zmm8
       vmovups  [REG_XCX + 0x240], zmm9
       vmovups  [REG_XCX + 0x280], zmm10
       vmovups  [REG_XCX + 0x2c0], zmm11
       vmovups  [REG_XCX + 0x300], zmm12
       vmovups  [REG_XCX + 0x340], zmm13
       vmovups  [REG_XCX + 0x380], zmm14
       vmovups  [REG_XCX + 0x3c0], zmm15
       vmovups  [REG_XCX + 0x400], zmm16
       vmovups  [REG_XCX + 0x440], zmm17
       vmovups  [REG_XCX + 0x480], zmm18
       vmovups  [REG_XCX + 0x4c0], zmm19
       vmovups  [REG_XCX + 0x500], zmm20
       vmovups  [REG_XCX + 0x540], zmm12
       vmovups  [REG_XCX + 0x580], zmm22
       vmovups  [REG_XCX + 0x5c0], zmm23
       vmovups  [REG_XCX + 0x600], zmm24
       vmovups  [REG_XCX + 0x640], zmm25
       vmovups  [REG_XCX + 0x680], zmm26
       vmovups  [REG_XCX + 0x6c0], zmm27
       vmovups  [REG_XCX + 0x700], zmm28
       vmovups  [REG_XCX + 0x740], zmm29
       vmovups  [REG_XCX + 0x780], zmm30
       vmovups  [REG_XCX + 0x7c0], zmm31
#endif

        add      REG_XSP, FRAME_PADDING
        POP_CALLEE_SAVED_REGS()
        ret
        END_FUNC(FUNCNAME)
#undef FUNCNAME

#define FUNCNAME init_opmask
        DECLARE_FUNC_SEH(FUNCNAME)
GLOBAL_LABEL(FUNCNAME:)
        mov      REG_XCX, ARG1
        PUSH_CALLEE_SAVED_REGS()
        sub      REG_XSP, FRAME_PADDING
        END_PROLOG

        mov      ax, WORD [REG_XCX]
        kmovw    k0, eax
        mov      ax, WORD [REG_XCX + 0x2]
        kmovw    k1, eax
        mov      ax, WORD [REG_XCX + 0x4]
        kmovw    k2, eax
        mov      ax, WORD [REG_XCX + 0x6]
        kmovw    k3, eax
        mov      ax, WORD [REG_XCX + 0x8]
        kmovw    k4, eax
        mov      ax, WORD [REG_XCX + 0xa]
        kmovw    k5, eax
        mov      ax, WORD [REG_XCX + 0xc]
        kmovw    k6, eax
        mov      ax, WORD [REG_XCX + 0xe]
        kmovw    k7, eax

        add      REG_XSP, FRAME_PADDING
        POP_CALLEE_SAVED_REGS()
        ret
        END_FUNC(FUNCNAME)
#undef FUNCNAME

#define FUNCNAME get_opmask
        DECLARE_FUNC_SEH(FUNCNAME)
GLOBAL_LABEL(FUNCNAME:)
        mov      REG_XCX, ARG1
        PUSH_CALLEE_SAVED_REGS()
        sub      REG_XSP, FRAME_PADDING
        END_PROLOG

        kmovw    eax, k0
        mov      WORD [REG_XCX], ax
        kmovw    eax, k1
        mov      WORD [REG_XCX + 0x2], ax
        kmovw    eax, k2
        mov      WORD [REG_XCX + 0x4], ax
        kmovw    eax, k3
        mov      WORD [REG_XCX + 0x6], ax
        kmovw    eax, k4
        mov      WORD [REG_XCX + 0x8], ax
        kmovw    eax, k5
        mov      WORD [REG_XCX + 0xa], ax
        kmovw    eax, k6
        mov      WORD [REG_XCX + 0xc], ax
        kmovw    eax, k7
        mov      WORD [REG_XCX + 0xe], ax

        add      REG_XSP, FRAME_PADDING
        POP_CALLEE_SAVED_REGS()
        ret
        END_FUNC(FUNCNAME)
#undef FUNCNAME

END_FILE
/* clang-format on */
#endif