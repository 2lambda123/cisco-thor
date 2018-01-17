/*
Copyright (c) 2015, Cisco Systems
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#if !defined(_GLOBAL_H_)
#define _GLOBAL_H_

#include <stdlib.h>
#include <stdio.h>

#if VS_2015  //TODO: Find a pre-defined macro
#define restrict __restrict
#define inline __inline
#endif

static inline void fatalerror(char error_text[])
{
    fprintf(stderr,"Run-time error...\n");
    fprintf(stderr,"%s\n",error_text);
    fprintf(stderr,"...now exiting to system...\n");
    abort();
}

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#define MAX_SB_SIZE 128          //Maximum block size
#define MIN_BLOCK_SIZE 8         //Minimum block size
#define NUM_BLOCK_SIZES 5        //Number of distinct block sizes (=log2(MAX_SB_SIZE/MIN_BLOCK_SIZE)+1)
#define MIN_PB_SIZE 4            //Minimum pu block size
#define MAX_QUANT_SIZE 16        //Maximum quantization block size
#define MAX_BUFFER_SIZE 4000000  //Maximum compressed buffer size per frame
#define MAX_TR_SIZE 128          //Maximum transform size
#define TR_SIZE_RANGE (NUM_BLOCK_SIZES+1)
#define PADDING_Y (MAX_SB_SIZE+32) //One-sided padding range for luma
#define MAX_UINT32 1<<31         //Used e.g. to initialize search for minimum cost
#define EARLY_SKIP_BLOCK_SIZE 32 //maximum block size for early skip check
#define MAX_REF_FRAMES 33        //Maximum number of reference frames
#define MAX_SKIP_FRAMES 8        //Maximum number of frames to skip/interpolate
#define MAX_REORDER_BUFFER 32    //Maximum number of frames to store for reordering
#define ME_CANDIDATES 6          //Number of ME candidates
#define MAX_QP 51                //Maximum QP value

#define DYADIC_CODING 1          // Support hierarchical B frames

/* RATE CONTROL PARAMETERS */
#define MAX_STEP_SIZE 228        //2^((MAX_QP-4)/6)
#define INTRA_FIXED_QP 32


/* Experimental */
#define BIPRED_PART 0

#define NEW_DEBLOCK_TEST 1       //New test for deblocking a block edge
#define NEW_MV_TEST 1            //New MV test for deblocking a line
#define NEW_DEBLOCK_FILTER 1     //New deblocking filter
#define LIMITED_SKIP 1           //Limit number of skip candidates
#define MODIFIED_DEBLOCK_TEST 1  //New test for whether or not to apply deblock filter
#define CDEF 1                   //Constrained Directional Enhancement Filter

#if LIMITED_SKIP
#define MAX_NUM_SKIP 2           //Maximum number of skip candidates
#else
#define MAX_NUM_SKIP 4           //Maximum number of skip candidates
#endif
#define MAX_NUM_MERGE 2          //Maximum number of merge candidates

#define COEFF_TYPE_INTER_Y 0     // Coefficient types for modifying coding
#define COEFF_TYPE_INTER_C 1     // Coefficient types for modifying coding
#define COEFF_TYPE_INTRA_Y 2     // Coefficient types for modifying coding
#define COEFF_TYPE_INTRA_C 3     // Coefficient types for modifying coding

#define qmtx_t uint16_t
#define INV_WEIGHT_SHIFT 6      // Bit accuracy of inverse weights
#define WEIGHT_SHIFT 6          // Bit accuracy of forward weights
#define NUM_QM_LEVELS 12

#define TEMP_INTERP_USE_CHROMA 0 // 444 not supported!

#if CDEF
#define CDEF_PRI_STRENGTHS 16
#define CDEF_SEC_STRENGTHS 4
#define CDEF_BLOCKSIZE 64
#define CDEF_BLOCKSIZE_LOG2 6
#define CDEF_NBLOCKS (CDEF_BLOCKSIZE / 8)
#define CDEF_SB_SHIFT (MAX_SB_SIZE_LOG2 - CDEF_BLOCKSIZE_LOG2)
#define CDEF_VBORDER 3
#define CDEF_HBORDER 8
#define CDEF_VERY_LARGE 30000
#define CDEF_INBUF_SIZE (CDEF_BSTRIDE * (CDEF_BLOCKSIZE + 2 * CDEF_VBORDER))
#define CDEF_MAX_STRENGTHS 16
#define CDEF_STRENGTH_BITS 6
#define CDEF_FULL 0  // 1 = 7x7 filter, 0 = 5x5 filter
#endif

/* Testing and analysis*/
#define STAT 0                   //Extended statistics printout in decoder

#include "types.h"

static inline unsigned int saturate(int n, int bitdepth) {
  return min((1<<bitdepth)-1, max(0, n));
}

// Used with int64_t in improve_uv_prediction()
#define clip(n, low, high) min(high, max(n, low))

#endif
