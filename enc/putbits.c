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
#include <memory.h>
#include "global.h"
#include "putbits.h"

static void flush_bytebuf(stream_t *str, FILE *outfile)
{
  if (outfile)
  {
    if (fwrite(str->bitstream, sizeof(unsigned char), str->bytepos, outfile) != str->bytepos)
    {
      fatalerror("Problem writing bitstream to file.");
    }
  }
  str->bytepos = 0;
}

void flush_all_bits(stream_t *str, FILE *outfile)
{
  uint32_t frame_bytes;
  int i;
  int bytes = 4 - str->bitrest/8;
  frame_bytes = str->bytepos + bytes;
  if (outfile)
  {
    uint8_t frame_bytes_buf[4];
    for (i = 0; i < 4; i++)
    {
      frame_bytes_buf[i] = (uint8_t)(frame_bytes >> (24 - i*8));
    }
    if (fwrite(frame_bytes_buf, sizeof(frame_bytes_buf), 1, outfile) != 1)
    {
      fatalerror("Problem writing bitstream to file.");
    }
  }

  if ((str->bytepos+bytes) > str->bytesize)
  {
    flush_bytebuf(str,outfile);
  }
  for (i = 0; i < bytes; i++)
  {
    str->bitstream[str->bytepos++] = (str->bitbuf >> (24-i*8)) & 0xff;
  }
  str->bitbuf = 0;
  str->bitrest = 32;

  if (outfile)
  {
    if (fwrite(str->bitstream,sizeof(unsigned char),str->bytepos,outfile) != str->bytepos)
    {
      fatalerror("Problem writing bitstream to file.");
    }
  }
  str->bytepos = 0;
}    
                    
int get_bit_pos(stream_t *str){
  int bitpos = 8*str->bytepos + (32 - str->bitrest);
  return bitpos; 
}

static inline unsigned int mask(unsigned int n)
{
  return (1 << n) - 1;
}

static inline void flush_bitbuf(stream_t *str, int bytes)
{
  int i;
  if ((str->bytepos+bytes) > str->bytesize)
  {
    fatalerror("Run out of bits in stream buffer.");
  }
  for (i=3; i>=4-bytes; --i) {
    str->bitstream[str->bytepos++] = (str->bitbuf >> (8*i)) & 0xff;
  }
  str->bitbuf = 0;
  str->bitrest = 32;
}

static unsigned int putbits(unsigned int n, unsigned int val, stream_t *str)
{
  unsigned int rest;

  if (n <= str->bitrest)
  {
    str->bitbuf |= ((val & mask(n)) << (str->bitrest-n));
    str->bitrest -= n;
  }
  else
  {
    rest = n-str->bitrest;
    str->bitbuf |= (val >> rest) & mask(n-rest);
    flush_bitbuf(str,4);
    str->bitbuf |= (val & mask(rest)) << (32-rest);
    str->bitrest -= rest;
  }

  return n;
}

void write_stream_pos(stream_t *stream, stream_pos_t *stream_pos){
  // Flush bitrest to memory if we move forward
  if (stream_pos->bytepos > stream->bytepos) {
    uint32_t tmp = 0;
    for (int i = 0; i < 4; i++)
      tmp |= stream->bitstream[stream->bytepos + i] << ((3 - i) * 8);
    tmp &= mask(stream->bitrest);
    putbits(stream->bitrest, tmp, stream);
    flush_bitbuf(stream, 4);
  }

  stream->bitrest = stream_pos->bitrest;
  stream->bytepos = stream_pos->bytepos;
  stream->bitbuf = stream_pos->bitbuf;
}

void read_stream_pos(stream_pos_t *stream_pos, stream_t *stream){
  stream_pos->bitrest = stream->bitrest;
  stream_pos->bytepos = stream->bytepos;
  stream_pos->bitbuf = stream->bitbuf;
}
