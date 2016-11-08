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

#ifndef COMMON_SIMDKERNELS_H
#define COMMON_SIMDKERNELS_H

#include <stdint.h>
void TEMPLATE(block_avg_simd)(SAMPLE *p,SAMPLE *r0, SAMPLE *r1, int sp, int s0, int s1, int width, int height);
int TEMPLATE(sad_calc_simd_unaligned)(SAMPLE *a, SAMPLE *b, int astride, int bstride, int width, int height);
void TEMPLATE(get_inter_prediction_luma_simd)(int width, int height, int xoff, int yoff, SAMPLE *restrict qp, int qstride, const SAMPLE *restrict ip, int istride, int bipred, int bitdepth);
void TEMPLATE(get_inter_prediction_chroma_simd)(int width, int height, int xoff, int yoff, SAMPLE *restrict qp, int qstride, const SAMPLE *restrict ip, int istride, int bitdepth);
void transform_simd(const int16_t *block, int16_t *coeff, int size, int fast, int bitdepth);
void inverse_transform_simd(const int16_t *coeff, int16_t *block, int size, int bitdepth);
void TEMPLATE(clpf_block4)(const SAMPLE *src, SAMPLE *dst, int stride, int x0, int y0, int width, int height, unsigned int strength);
void TEMPLATE(clpf_block8)(const SAMPLE *src, SAMPLE *dst, int stride, int x0, int y0, int width, int height, unsigned int strength);
void TEMPLATE(scale_frame_down2x2_simd)(yuv_frame_t* sin, yuv_frame_t* sout);

SIMD_INLINE void TEMPLATE(clpf_block_simd)(const SAMPLE *src, SAMPLE *dst, int stride, int x0, int y0, int sizex, int sizey, int width, int height, unsigned int strength) {
  if (sizex == 4 && sizey == 4) // chroma 420
    TEMPLATE(clpf_block4)(src, dst, stride, x0, y0, width, height, strength);
  else if (sizex == 4) { // chroma 422
    TEMPLATE(clpf_block4)(src, dst, stride, x0, y0, width, height, strength);
    TEMPLATE(clpf_block4)(src + 4*stride, dst + 4*stride, stride, x0, y0, width, height, strength);
  } else // luma 444 or luma 420
    TEMPLATE(clpf_block8)(src, dst, stride, x0, y0, width, height, strength);
}

#endif
