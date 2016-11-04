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

#include "global.h"

const int zigzag16[16] = {
    0, 1, 5, 6, 
    2, 4, 7, 12, 
    3, 8, 11, 13, 
    9, 10, 14, 15
};

const int zigzag64[64] = {
     0,  1,  5,  6, 14, 15, 27, 28,
     2,  4,  7, 13, 16, 26, 29, 42,
     3,  8, 12, 17, 25, 30, 41, 43,
     9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63
};

const int zigzag256[256] = {
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


const int chroma_qp[52] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29,
        30, 31, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38,
        39, 40, 41, 42, 43, 44, 45
};

const uint16_t gquant_table[6] = {26214,23302,20560,18396,16384,14564};
const uint16_t gdequant_table[6] = {40,45,51,57,64,72};
