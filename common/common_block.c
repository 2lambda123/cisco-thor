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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#include "global.h"
#include "common_block.h"

int zigzag16[16] = {
    0, 1, 5, 6, 
    2, 4, 7, 12, 
    3, 8, 11, 13, 
    9, 10, 14, 15
};

int zigzag64[64] = {
     0,  1,  5,  6, 14, 15, 27, 28,
     2,  4,  7, 13, 16, 26, 29, 42,
     3,  8, 12, 17, 25, 30, 41, 43,
     9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63
};

int zigzag256[256] = {
    0,  1,  5,  6, 14, 15, 27, 28, 44, 45, 65, 66, 90, 91,119,120,
    2,  4,  7, 13, 16, 26, 29, 43, 46, 64, 67, 89, 92,118,121,150,
    3,  8, 12, 17, 25, 30, 42, 47, 63, 68, 88, 93,117,122,149,151,
    9, 11, 18, 24, 31, 41, 48, 62, 69, 87, 94,116,123,148,152,177,
   10, 19, 23, 32, 40, 49, 61, 70, 86, 95,115,124,147,153,176,178,
   20, 22, 33, 39, 50, 60, 71, 85, 96,114,125,146,154,175,179,200,
   21, 34, 38, 51, 59, 72, 84, 97,113,126,145,155,174,180,199,201,
   35, 37, 52, 58, 73, 83, 98,112,127,144,156,173,181,198,202,219,
   36, 53, 57, 74, 82, 99,111,128,143,157,172,182,197,203,218,220,
   54, 56, 75, 81,100,110,129,142,158,171,183,196,204,217,221,234,
   55, 76, 80,101,109,130,141,159,170,184,195,205,216,222,233,235,
   77, 79,102,108,131,140,160,169,185,194,206,215,223,232,236,245,
   78,103,107,132,139,161,168,186,193,207,214,224,231,237,244,246,
  104,106,133,138,162,167,187,192,208,213,225,230,238,243,247,252,
  105,134,137,163,166,188,191,209,212,226,229,239,242,248,251,253,
  135,136,164,165,189,190,210,211,227,228,240,241,249,250,254,255
};


int chroma_qp[52] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29,
        30, 31, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38,
        39, 40, 41, 42, 43, 44, 45
};

const uint16_t gquant_table[6] = {26214,23302,20560,18396,16384,14564};
const uint16_t gdequant_table[6] = {40,45,51,57,64,72};

int get_left_available(int ypos, int xpos, int bwidth, int bheight, int fwidth, int fheight, int sb_size) {
  int left_available = xpos > 0;
  return left_available;
}

int get_up_available(int ypos, int xpos, int bwidth, int bheight, int fwidth, int fheight, int sb_size) {
  int up_available = ypos > 0;
  return up_available;
}

int get_upright_available(int ypos, int xpos, int bwidth, int bheight, int fwidth, int fheight, int sb_size) {

  int upright_available;
  int size, size2;

  /* Test for frame boundaries */
  upright_available = (ypos > 0) && (xpos + bwidth < fwidth);

  /* Test for coding block boundaries */
  size = max(bwidth, bheight);
  for (size2 = size; size2 < sb_size; size2 *= 2) {
    if ((ypos % (size2 << 1)) == size2 && (xpos % size2) == (size2 - size)) upright_available = 0;
  }
  return upright_available;
}

int get_downleft_available(int ypos, int xpos, int bwidth, int bheight, int fwidth, int fheight, int sb_size) {

  int downleft_available;
  int size, size2;

  /* Test for frame boundaries */
  downleft_available = (xpos > 0) && (ypos + bheight < fheight);

  size = max(bwidth, bheight);
  /* Test for external super block boundaries */
  if ((ypos % sb_size) == (sb_size - size) && (xpos % sb_size) == 0) downleft_available = 0;

  /* Test for coding block boundaries */
  size = max(bwidth, bheight);
  for (size2 = 2 * size; size2 <= sb_size; size2 *= 2) {
    if ((ypos % size2) == (size2 - size) && (xpos % size2) > 0) downleft_available = 0;
  }

  return downleft_available;
}

void dequantize (int16_t *coeff, int16_t *rcoeff, int qp, int size, qmtx_t * wt_matrix)
{
  int tr_log2size = log2i(size);
  const int lshift = qp / 6;
  const int qsize = min(size,MAX_QUANT_SIZE);
  const int rshift = tr_log2size - 1 + (wt_matrix!=NULL ? INV_WEIGHT_SHIFT : 0);
  const int64_t scale = gdequant_table[qp % 6];
  const int64_t add = lshift < rshift ? (1<<(rshift-lshift-1)) : 0;

  if (lshift >= rshift) {
    for (int i = 0; i < qsize ; i++){
      for (int j = 0; j < qsize; j++){
        int c = coeff[i*qsize + j];
        if (wt_matrix)
          c = c*wt_matrix[i*qsize+j];
        rcoeff[i*size+j] = (int16_t)((c * scale) << (lshift-rshift));// needs clipping?
      }
    }
  } else {
    for (int i = 0; i < qsize ; i++){
      for (int j = 0; j < qsize; j++){
        int c = coeff[i*qsize+j];
        if (wt_matrix)
          c = c*wt_matrix[i*qsize+j];
        rcoeff[i*size+j] = (int16_t)((c * scale + add) >> (rshift - lshift));//needs clipping
      }
    }

  }
}

void reconstruct_block(int16_t *block, uint8_t *pblock, uint8_t *rec, int size, int pstride, int stride)
{ 
  int i,j;
  for(i=0;i<size;i++){    
    for (j=0;j<size;j++){
      rec[i*stride+j] = (uint8_t)clip255(block[i*size+j] + (int16_t)pblock[i*pstride+j]);
    }
  }
}

void find_block_contexts(int ypos, int xpos, int height, int width, int size, deblock_data_t *deblock_data, block_context_t *block_context, int enable){

  if (ypos >= MIN_BLOCK_SIZE && xpos >= MIN_BLOCK_SIZE && ypos + size < height && xpos + size < width && enable && size <= MAX_TR_SIZE) {
    int by = ypos/MIN_PB_SIZE;
    int bx = xpos/MIN_PB_SIZE;
    int bs = width/MIN_PB_SIZE;
    int bindex = by*bs+bx;
    block_context->split = (deblock_data[bindex-bs].size < size) + (deblock_data[bindex-1].size < size);
    int cbp1;
    cbp1 = (deblock_data[bindex-bs].cbp.y > 0) + (deblock_data[bindex-1].cbp.y > 0);
    block_context->cbp = cbp1;
    int cbp2 = (deblock_data[bindex-bs].cbp.y > 0 || deblock_data[bindex-bs].cbp.u > 0 || deblock_data[bindex-bs].cbp.v > 0) +
           (deblock_data[bindex-1].cbp.y > 0 || deblock_data[bindex-1].cbp.u > 0 || deblock_data[bindex-1].cbp.v > 0);
    block_context->index = 3*block_context->split + cbp2;
  }
  else{
    block_context->split = -1;
    block_context->cbp = -1;
    block_context->index = -1;
  }
}

int clpf_sample(int X, int A, int B, int C, int D, int E, int F, int b) {
  int delta =
    4*clip(A - X, -b, b) + clip(B - X, -b, b) + 3*clip(C - X, -b, b) +
    3*clip(D - X, -b, b) + clip(E - X, -b, b) + 4*clip(F - X, -b, b);
  return (8 + delta - (delta < 0)) >> 4;
}

void clpf_block(const uint8_t *src, uint8_t *dst, int stride, int x0, int y0, int sizex, int sizey, int width, int height, unsigned int strength) {
  for (int y = y0; y < y0+sizey; y++){
    for (int x = x0; x < x0+sizex; x++) {
      int X = src[y*stride + x];
      int A = src[max(0, y-1)*stride + x];
      int B = src[y*stride + max(0, x-2)];
      int C = src[y*stride + max(0, x-1)];
      int D = src[y*stride + min(width-1, x+1)];
      int E = src[y*stride + min(width-1, x+2)];
      int F = src[min(height-1, y+1)*stride + x];
      int delta;
      delta = clpf_sample(X, A, B, C, D, E, F, strength);
      dst[y*stride + x] = X + delta;
    }
  }
}

void improve_uv_prediction(uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *ry, int n, int cstride, int stride, int sub)
{
  int nc = n >> sub;
  int lognc = log2i(nc);

  // Compute squared residual
  int64_t squared_residual = 0;
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      squared_residual +=
        (ry[i*stride + j] - y[i*n + j]) *
        (ry[i*stride + j] - y[i*n + j]);

  // If the luma prediction is good, we change nothing
  if ((squared_residual >> (log2i(n) + log2i(n))) <= 64)
    return;

  // Compute linear fit between predicted chroma and predicted luma
  int32_t ysum = 0, usum = 0, vsum = 0, yysum = 0, yusum = 0, yvsum = 0, uusum = 0, vvsum = 0;
  for (int i = 0; i < nc; i++)
    for (int j = 0; j < nc; j++) {
      int us = u[i * (cstride >> sub) + j];
      int vs = v[i * (cstride >> sub) + j];
      int ys = sub ?
	(y[(i*2 + 0)*n + j*2 + 0] + y[(i*2 + 0)*n+j*2 + 1] +
	 y[(i*2 + 1)*n + j*2 + 0] + y[(i*2 + 1)*n+j*2 + 1] + 2) >> 2 :
	y[i * cstride + j];
      ysum  += ys;
      usum  += us;
      vsum  += vs;
      yysum += ys * ys;
      yusum += ys * us;
      yvsum += ys * vs;
      uusum += us * us;
      vvsum += vs * vs;
    }

  int64_t ssyy = yysum - ((int64_t)ysum*ysum >> lognc * 2);
  int64_t ssuu = uusum - ((int64_t)usum*usum >> lognc * 2);
  int64_t ssvv = vvsum - ((int64_t)vsum*vsum >> lognc * 2);
  int64_t ssyu = yusum - ((int64_t)ysum*usum >> lognc * 2);
  int64_t ssyv = yvsum - ((int64_t)ysum*vsum >> lognc * 2);

  // Require a correlation above a threshold
  if (ssyy) {
    if (ssyu * ssyu * 2 > ssyy * ssuu) {
      int64_t a64 = ((int64_t)ssyu << 16) / ssyy;
      int64_t b64 = (((int64_t)usum << 16) - a64 * ysum) >> lognc * 2;
      int32_t a = (int32_t)clip(a64, -1 << 23, 1 << 23);
      int32_t b = (int32_t)clip(b64 + (1 << 15), -1 << 31, (1U << 31) - 1);

      // Map reconstructed luma to new predicted chroma
      for (int i = 0; i < nc; i++)
        for (int j = 0; j < nc; j++) {
          u[i*(cstride >> sub) + j] = sub ?
            (clip255((a*ry[(i*2+0)*stride+j*2+0] + b) >> 16) +
             clip255((a*ry[(i*2+0)*stride+j*2+1] + b) >> 16) +
             clip255((a*ry[(i*2+1)*stride+j*2+0] + b) >> 16) +
             clip255((a*ry[(i*2+1)*stride+j*2+1] + b) >> 16) + 2) >> 2 :
            clip255((a*ry[i*stride+j] + b) >> 16);
        }
    }
    if (ssyv * ssyv * 2 > ssyy * ssvv) {
      int64_t a64 = ((int64_t)ssyv << 16) / ssyy;
      int64_t b64 = (((int64_t)vsum << 16) - a64 * ysum) >> lognc * 2;
      int32_t a = (int32_t)clip(a64, -1 << 23, 1 << 23);
      int32_t b = (int32_t)clip(b64 + (1 << 15), -1 << 31, (1U << 31) - 1);

      // Map reconstructed luma to new predicted chroma
      for (int i = 0; i < nc; i++)
        for (int j = 0; j < nc; j++) {
          v[i*(cstride >> sub) + j] = sub ?
            (clip255((a*ry[(i*2+0)*stride+j*2+0] + b) >> 16) +
             clip255((a*ry[(i*2+0)*stride+j*2+1] + b) >> 16) +
             clip255((a*ry[(i*2+1)*stride+j*2+0] + b) >> 16) +
             clip255((a*ry[(i*2+1)*stride+j*2+1] + b) >> 16) + 2) >> 2 :
            clip255((a*ry[i*stride+j] + b) >> 16);
        }
    }
  }
}
