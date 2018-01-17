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

#include <string.h>

#include "global.h"
#include "decode_block.h"
#include "common_block.h"
#include "common_frame.h"
#include "temporal_interp.h"
#include "wt_matrix.h"
#include "getvlc.h"
#include "inter_prediction.h"
#include "read_bits.h"

extern int chroma_qp[52];

#undef TEMPLATE
#define TEMPLATE(func) (decoder_info->bitdepth == 8 ? func ## _lbd : func ## _hbd)

static int clpf_true(int k, int l, const yuv_frame_t *r, const yuv_frame_t *o, const deblock_data_t *d, int s, int w, int h, void *stream, unsigned int strength, unsigned int fb_size_log2, unsigned int shift, unsigned int size, int qp) {
  return 1;
}

static int clpf_bit(int k, int l, const yuv_frame_t *r, const yuv_frame_t *o, const deblock_data_t *d, int s, int w, int h, void *stream, unsigned int strength, unsigned int fb_size_log2, unsigned int shift, unsigned int size, int qp) {
  return get_flc(1, (stream_t*)stream);
}

void decode_frame(decoder_info_t *decoder_info, yuv_frame_t* rec_buffer)
{
  int height = decoder_info->height;
  int width = decoder_info->width;
  int k,l,r;
  int sb_size = 1 << decoder_info->log2_sb_size;
  int num_sb_hor = (width + sb_size - 1) / sb_size;
  int num_sb_ver = (height + sb_size - 1) / sb_size;
  stream_t *stream = decoder_info->stream;

  int bit_start = stream->bitcnt;
  int rec_buffer_idx;

  decoder_info->frame_info.interp_ref = 0;
  read_frame_header(decoder_info, stream);
  decoder_info->bit_count.stat_frame_type = decoder_info->frame_info.frame_type;
  int qp = decoder_info->frame_info.qp;
  if (decoder_info->frame_info.frame_type != I_FRAME) {
    int r;
    for (r = 0; r < decoder_info->frame_info.num_ref; r++) {
      if (decoder_info->frame_info.ref_array[r] == -1)
        decoder_info->frame_info.interp_ref = decoder_info->interp_ref;
    }
  }
  else {
    memset(decoder_info->deblock_data, 0, ((height / MIN_PB_SIZE) * (width / MIN_PB_SIZE) * sizeof(deblock_data_t)));
    decoder_info->frame_info.num_ref = 0;
  }

  decoder_info->frame_info.phase = decoder_info->frame_info.display_frame_num % (decoder_info->num_reorder_pics + 1);
  for (r=0; r<decoder_info->frame_info.num_ref; ++r){
    if (decoder_info->frame_info.ref_array[r]!=-1) {
      if (decoder_info->ref[decoder_info->frame_info.ref_array[r]]->frame_num > decoder_info->frame_info.display_frame_num) {
        decoder_info->bit_count.stat_frame_type = B_FRAME;
      }
    }
  }

  rec_buffer_idx = decoder_info->frame_info.display_frame_num%MAX_REORDER_BUFFER;
  decoder_info->rec = &rec_buffer[rec_buffer_idx];
  decoder_info->tmp = &rec_buffer[MAX_REORDER_BUFFER];
  decoder_info->rec->frame_num = decoder_info->frame_info.display_frame_num;

  if (decoder_info->frame_info.num_ref>2 && decoder_info->frame_info.ref_array[0]==-1) {
    // interpolate from the other references
    yuv_frame_t* ref1=decoder_info->ref[decoder_info->frame_info.ref_array[1]];
    yuv_frame_t* ref2=decoder_info->ref[decoder_info->frame_info.ref_array[2]];
    int display_frame_num = decoder_info->frame_info.display_frame_num;
    int off1 = ref2->frame_num - display_frame_num;
    int off2 = display_frame_num - ref1->frame_num;
    if (off1 < 0 && off2 < 0) {
      off1 = -off1;
      off2 = -off2;
    }
    if (off1 == off2) {
      off1 = off2 = 1;
    }
    // FIXME: won't work for the 1-sided case
    TEMPLATE(interpolate_frames)(decoder_info->interp_frames[0], ref1, ref2, off1+off2 , off2);
    TEMPLATE(pad_yuv_frame)(decoder_info->interp_frames[0]);
    decoder_info->interp_frames[0]->frame_num = display_frame_num;
  }

  decoder_info->bit_count.frame_header[decoder_info->bit_count.stat_frame_type] += (stream->bitcnt - bit_start);
  decoder_info->bit_count.frame_type[decoder_info->bit_count.stat_frame_type] += 1;
  decoder_info->frame_info.qp = qp;
  decoder_info->frame_info.qpb = qp;

  //Generate new interpolated frame
  for (k=0;k<num_sb_ver;k++){
    for (l=0;l<num_sb_hor;l++){
      int sub = decoder_info->subsample == 400 ? 31 : decoder_info->subsample == 420;
      int xposY = l*sb_size;
      int yposY = k*sb_size;
      TEMPLATE(process_block_dec)(decoder_info, sb_size, yposY, xposY, sub);
    }
  }

  qp = decoder_info->frame_info.qp = decoder_info->frame_info.qpb;

  //Scale and store MVs in decode_frame()
  if (decoder_info->interp_ref>1) {
    int gop_size = decoder_info->num_reorder_pics + 1;
    int coded_phase = (decoder_info->frame_info.decode_order_frame_num + gop_size - 2) % gop_size + 1;
    int b_level = log2i(coded_phase);
    int frame_type = decoder_info->bit_count.stat_frame_type;
    int frame_num = decoder_info->frame_info.display_frame_num;
    TEMPLATE(store_mv)(width, height, b_level, frame_type, frame_num, gop_size, decoder_info->deblock_data);
  }

  if (decoder_info->deblocking){
    TEMPLATE(deblock_frame_y)(decoder_info->rec, decoder_info->deblock_data, width, height, qp, decoder_info->bitdepth);
    if (decoder_info->subsample != 400) {
      int qpc = decoder_info->subsample != 444 ? chroma_qp[qp] : qp;
      TEMPLATE(deblock_frame_uv)(decoder_info->rec, decoder_info->deblock_data, width, height, qpc, decoder_info->bitdepth);
    }
  }

#if CDEF
  if (decoder_info->cdef_enable) {
    int nhfb = (height + CDEF_BLOCKSIZE - 1) >> CDEF_BLOCKSIZE_LOG2;
    int nvfb = (width + CDEF_BLOCKSIZE - 1) >> CDEF_BLOCKSIZE_LOG2;
    int fb_size_log2 = CDEF_BLOCKSIZE_LOG2;

    for (int k = 0; k < nhfb; k++) {
      for (int l = 0; l < nvfb; l++) {
        int xpos = l << fb_size_log2;
        int ypos = k << fb_size_log2;
        int preset = 0;
        if (decoder_info->cdef_bits) {
          int allskip = cdef_allskip(xpos, ypos, width, height, decoder_info->deblock_data, fb_size_log2);
          if (!allskip) {
            preset = get_flc(decoder_info->cdef_bits, stream);
          }
        }
        for (int plane = 0; plane < 2; plane++) {
          cdef_strength *cdef = &decoder_info->cdef[k*nvfb+l].plane[plane != 0];
          cdef->level = decoder_info->cdef_presets[preset].pri_strength[plane] * 2 + decoder_info->cdef_presets[preset].skip_condition[plane];
          cdef->sec_strength = decoder_info->cdef_presets[preset].sec_strength[plane];
          cdef->pri_damping = decoder_info->cdef_damping[0];
          cdef->sec_damping = decoder_info->cdef_damping[1];
        }
      }
    }
    TEMPLATE(cdef_frame)(decoder_info->cdef, decoder_info->rec, 0, decoder_info->deblock_data, stream, 0, decoder_info->bitdepth, 0);
    TEMPLATE(cdef_frame)(decoder_info->cdef, decoder_info->rec, 0, decoder_info->deblock_data, stream, 0, decoder_info->bitdepth, 1);
    TEMPLATE(cdef_frame)(decoder_info->cdef, decoder_info->rec, 0, decoder_info->deblock_data, stream, 0, decoder_info->bitdepth, 2);
  }
#endif

  if (decoder_info->clpf) {
    int strength_y = get_flc(2, stream);
    int strength_u = get_flc(2, stream);
    int strength_v = get_flc(2, stream);
    if (strength_y) {
      int fb_size_log2 = get_flc(2, stream) + 4;
      int enable_fb_flag = fb_size_log2 != 4;
      if (fb_size_log2 == 4)
        fb_size_log2 = 7;
      TEMPLATE(clpf_frame)(decoder_info->rec, 0, decoder_info->deblock_data, stream, enable_fb_flag, strength_y + (strength_y == 3), fb_size_log2, decoder_info->bitdepth, PLANE_Y, qp, enable_fb_flag ? clpf_bit : clpf_true);
    }
    if (strength_u)
      TEMPLATE(clpf_frame)(decoder_info->rec, 0, decoder_info->deblock_data, stream, 0, strength_u + (strength_u == 3), 4, decoder_info->bitdepth, PLANE_U, qp, clpf_true);
    if (strength_v)
      TEMPLATE(clpf_frame)(decoder_info->rec, 0, decoder_info->deblock_data, stream, 0, strength_v + (strength_v == 3), 4, decoder_info->bitdepth, PLANE_V, qp, clpf_true);
  }

  /* Sliding window operation for reference frame buffer by circular buffer */

  /* Store pointer to reference frame that is shifted out of reference buffer */
  yuv_frame_t *tmp = decoder_info->ref[MAX_REF_FRAMES-1];

  /* Update remaining pointers to implement sliding window reference buffer operation */
  memmove(decoder_info->ref+1, decoder_info->ref, sizeof(yuv_frame_t*)*(MAX_REF_FRAMES-1));

  /* Set ref[0] to the memory slot where the new current reconstructed frame wil replace reference frame being shifted out */
  decoder_info->ref[0] = tmp;

  /* Pad the reconstructed frame and write into ref[0] */
  TEMPLATE(create_reference_frame)(decoder_info->ref[0],decoder_info->rec);
}


