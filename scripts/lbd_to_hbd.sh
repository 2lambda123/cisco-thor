#!/bin/sh

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <source> <destination>"
    exit
fi

src=$1
dst=$2

cp $1 $2

sed -i '
s/v128_low_u32/v256_low_v64/g
s/v128_low_v64/v256_low_v128/g
s/v128_high_v64/v256_high_v128/g
s/v128_from_64/v256_from_128/g
s/v128_from_v64/v256_from_v128/g
s/v128_from_32/v256_from_v64/g
s/v128_load_unaligned/v256_load_unaligned/g
s/v128_load_aligned/v256_load_aligned/g
s/v128_store_unaligned/v256_store_unaligned/g
s/v128_store_aligned/v256_store_aligned/g
s/v128_align/v256_align/g
s/v128_zero/v256_zero/g
s/v128_dup_8/v256_dup_16/g
s/v128_dup_16/v256_dup_32/g
s/v128_dup_32/v256_dup_64/g
s/sad128_internal/sad256_internal_u16/g
s/ssd128_internal/ssd256_internal_u16/g
s/v128 /v256 /g
s/v128_sad_u8_init/v256_sad_u16_init/g
s/v128_sad_u8/v256_sad_u16/g
s/v128_sad_u8_sum/v256_sad_u16_sum/g
s/v128_ssd_u8_init/v256_ssd_u16_init/g
s/v128_ssd_u8/v256_ssd_u16/g
s/v128_ssd_u8_sum/v256_ssd_u16_sum/g
s/v128_dotp_s16/v256_dotp_s32/g
s/v128_hadd_u8/v256_hadd_u16/g
s/v128_or/v256_or/g
s/v128_xor/v256_xor/g
s/v128_and/v256_and/g
s/v128_andn/v256_andn/g
s/v128_add_8/v256_add_16/g
s/v128_add_16/v256_add_32/g
s/v128_sadd_s16/v256_sadd_s32/g
s/v128_add_32/v256_add_64/g
s/v128_padd_s8/v256_padd_s16/g
s/v128_padd_s16/v256_padd_s32/g
s/v128_sub_8/v256_sub_16/g
s/v128_ssub_u8/v256_ssub_u16/g
s/v128_ssub_s8/v256_ssub_s16/g
s/v128_sub_16/v256_sub_32/g
s/v128_ssub_s16/v256_ssub_s32/g
s/v128_sub_32/v256_sub_64/g
s/v128_abs_s16/v256_abs_s32/g
s/v128_mul_s16/v256_mul_s32/g
s/v128_mullo_s16/v256_mullo_s32/g
s/v128_mulhi_s16/v256_mulhi_s32/g
s/v128_mullo_s32/v256_mullo_s64/g
s/v128_madd_s16/v256_madd_s32/g
s/v128_madd_us8/v256_madd_s16/g
s/v128_avg_u8/v256_avg_u16/g
s/v128_rdavg_u8/v256_rdavg_u16/g
s/v128_avg_u16/v256_avg_u32/g
s/v128_min_u8/v256_min_u16/g
s/v128_max_u8/v256_max_u16/g
s/v128_min_s8/v256_min_s16/g
s/v128_max_s8/v256_max_s16/g
s/v128_min_s16/v256_min_s32/g
s/v128_max_s16/v256_max_s32/g
s/v128_ziplo_8/v256_ziplo_16/g
s/v128_ziphi_8/v256_ziphi_16/g
s/v128_ziplo_16/v256_ziplo_32/g
s/v128_ziphi_16/v256_ziphi_32/g
s/v128_ziplo_32/v256_ziplo_64/g
s/v128_ziphi_32/v256_ziphi_64/g
s/v128_ziplo_64/v256_ziplo_128/g
s/v128_ziphi_64/v256_ziphi_128/g
s/v128_zip_8/v256_zip_16/g
s/v128_zip_16/v256_zip_32/g
s/v128_zip_32/v256_zip_64/g
s/v128_unziplo_8/v256_unziplo_16/g
s/v128_unziphi_8/v256_unziphi_16/g
s/v128_unziplo_16/v256_unziplo_32/g
s/v128_unziphi_16/v256_unziphi_32/g
s/v128_unziplo_32/v256_unziplo_64/g
s/v128_unziphi_32/v256_unziphi_64/g
s/v128_unpack_u8_s16/v256_unpack_u16_s32/g
s/v128_unpacklo_u8_s16/v256_unpacklo_u16_s32/g
s/v128_unpackhi_u8_s16/v256_unpackhi_u16_s32/g
s/v128_pack_s32_s16/v256_pack_s64_s32/g
s/v128_pack_s16_u8/v256_pack_s32_u16/g
s/v128_pack_s16_s8/v256_pack_s32_s16/g
s/v128_unpack_u16_s32/v256_unpack_u32_s64/g
s/v128_unpack_s16_s32/v256_unpack_s32_s64/g
s/v128_unpacklo_u16_s32/v256_unpacklo_u32_s64/g
s/v128_unpacklo_s16_s32/v256_unpacklo_s32_s64/g
s/v128_unpackhi_u16_s32/v256_unpackhi_u32_s64/g
s/v128_unpackhi_s16_s32/v256_unpackhi_s32_s64/g
s/v128_shuffle_8/v256_pshuffle_8/g
s/v128_cmpgt_s8/v256_cmpgt_s16/g
s/v128_cmplt_s8/v256_cmplt_s16/g
s/v128_cmpeq_8/v256_cmpeq_16/g
s/v128_cmpgt_s16/v256_cmpgt_s32/g
s/v128_cmplt_s16/v256_cmplt_s32/g
s/v128_cmpeq_16/v256_cmpeq_32/g
s/v128_shl_8/v256_shl_16/g
s/v128_shr_u8/v256_shr_u16/g
s/v128_shr_s8/v256_shr_s16/g
s/v128_shl_16/v256_shl_32/g
s/v128_shr_u16/v256_shr_u32/g
s/v128_shr_s16/v256_shr_s32/g
s/v128_shl_32/v256_shl_64/g
s/v128_shr_u32/v256_shr_u64/g
s/v128_shr_s32/v256_shr_s64/g
s/v128_shr_n_byte/v256_shr_n_word/g
s/v128_shl_n_byte/v256_shl_n_word/g
s/v128_shl_n_8/v256_shl_n_16/g
s/v128_shl_n_16/v256_shl_n_32/g
s/v128_shl_n_32/v256_shl_n_64/g
s/v128_shr_n_u8/v256_shr_n_u16/g
s/v128_shr_n_u16/v256_shr_n_u32/g
s/v128_shr_n_u32/v256_shr_n_u64/g
s/v128_shr_n_s8/v256_shr_n_s16/g
s/v128_shr_n_s16/v256_shr_n_s32/g
s/v128_shr_n_s32/v256_shr_n_s64/g
' $dst

sed -i '
s/sad64_internal/sad128_internal_u16/g
s/ssd64_internal/ssd128_internal_u16/g
s/v64 /v128 /g
s/v64_low_u32/v128_low_v64/g
s/v64_high_u32/v128_high_v64/g
s/v64_low_s32/v128_low_v64/g
s/v64_high_s32/v128_high_v64/g
s/v64_from_32/v128_from_v64/g
s/v64_from_64/v128_from_128/g
s/v64_u64/v128_u128/g
s/v64_from_16/v128_from_32/g
s/v64_load_unaligned/v128_load_unaligned/g
s/v64_load_aligned/v128_load_aligned/g
s/v64_store_unaligned/v128_store_unaligned/g
s/v64_store_aligned/v128_store_aligned/g
s/v64_align/v128_align/g
s/v64_zero/v128_zero/g
s/v64_dup_8/v128_dup_16/g
s/v64_dup_16/v128_dup_32/g
s/v64_dup_32/v128_dup_64/g
s/v64_add_8/v128_add_16/g
s/v64_add_16/v128_add_32/g
s/v64_sadd_s16/v128_sadd_s32/g
s/v64_add_32/v128_add_64/g
s/v64_sub_8/v128_sub_16/g
s/v64_ssub_u8/v128_ssub_u16/g
s/v64_ssub_s8/v128_ssub_s16/g
s/v64_sub_16/v128_sub_32/g
s/v64_ssub_s16/v128_ssub_s32/g
s/v64_sub_32/v128_sub_64/g
s/v64_abs_s16/v128_abs_s32/g
s/v64_ziplo_8/v128_ziplo_16/g
s/v64_ziphi_8/v128_ziphi_16/g
s/v64_ziplo_16/v128_ziplo_32/g
s/v64_ziphi_16/v128_ziphi_32/g
s/v64_ziplo_32/v128_ziplo_64/g
s/v64_ziphi_32/v128_ziphi_64/g
s/v64_unziplo_8/v128_unziplo_16/g
s/v64_unziphi_8/v128_unziphi_16/g
s/v64_unziplo_16/v128_unziplo_32/g
s/v64_unziphi_16/v128_unziphi_32/g
s/v64_unpacklo_u8_s16/v128_unpacklo_u16_s32/g
s/v64_unpackhi_u8_s16/v128_unpackhi_u16_s32/g
s/v64_pack_s32_s16/v128_pack_s64_s32/g
s/v64_pack_s16_u8/v128_pack_s32_u16/g
s/v64_pack_s16_s8/v128_pack_s32_s16/g
s/v64_unpacklo_u16_s32/v128_unpacklo_u32_s64/g
s/v64_unpacklo_s16_s32/v128_unpacklo_s32_s64/g
s/v64_unpackhi_u16_s32/v128_unpackhi_u32_s64/g
s/v64_unpackhi_s16_s32/v128_unpackhi_s32_s64/g
s/v64_shuffle_8/v128_shuffle_16/g
s/v64_sad_u8_init/v128_sad_u16_init/g
s/v64_sad_u8/v128_sad_u16/g
s/v64_sad_u8_sum/v128_sad_u16_sum/g
s/v64_ssd_u8_init/v128_ssd_u16_init/g
s/v64_ssd_u8/v128_ssd_u16/g
s/v64_ssd_u8_sum/v128_ssd_u16_sum/g
s/v64_dotp_su8/v128_dotp_su16/g
s/v64_dotp_s16/v128_dotp_s32/g
s/v64_hadd_u8/v128_hadd_u16/g
s/v64_hadd_s16/v128_hadd_s32/g
s/v64_or/v128_or/g
s/v64_xor/v128_xor/g
s/v64_and/v128_and/g
s/v64_andn/v128_andn/g
s/v64_mullo_s16/v128_mullo_s32/g
s/v64_mulhi_s16/v128_mulhi_s32/g
s/v64_mullo_s32/v128_mullo_s64/g
s/v64_madd_s16/v128_madd_s32/g
s/v64_madd_us8/v128_madd_us16/g
s/v64_avg_u8/v128_avg_u16/g
s/v64_rdavg_u8/v128_rdavg_u16/g
s/v64_avg_u16/v128_avg_u32/g
s/v64_min_u8/v128_min_u16/g
s/v64_max_u8/v128_max_u16/g
s/v64_min_s8/v128_min_s16/g
s/v64_max_s8/v128_max_s16/g
s/v64_min_s16/v128_min_s32/g
s/v64_max_s16/v128_max_s32/g
s/v64_cmpgt_s8/v128_cmpgt_s16/g
s/v64_cmplt_s8/v128_cmplt_s16/g
s/v64_cmpeq_8/v128_cmpeq_16/g
s/v64_cmpgt_s16/v128_cmpgt_s32/g
s/v64_cmplt_s16/v128_cmplt_s32/g
s/v64_cmpeq_16/v128_cmpeq_32/g
s/v64_shl_8/v128_shl_16/g
s/v64_shr_u8/v128_shr_u16/g
s/v64_shr_s8/v128_shr_s16/g
s/v64_shl_16/v128_shl_32/g
s/v64_shr_u16/v128_shr_u32/g
s/v64_shr_s16/v128_shr_s32/g
s/v64_shl_32/v128_shl_64/g
s/v64_shr_u32/v128_shr_u64/g
s/v64_shr_s32/v128_shr_s64/g
s/v64_shr_n_byte/v128_shr_n_word/g
s/v64_shl_n_byte/v128_shl_n_word/g
s/v64_shl_n_8/v128_shl_n_16/g
s/v64_shr_n_u8/v128_shr_n_u16/g
s/v64_shr_n_s8/v128_shr_n_s16/g
s/v64_shl_n_16/v128_shl_n_32/g
s/v64_shr_n_u16/v128_shr_n_u32/g
s/v64_shr_n_s16/v128_shr_n_s32/g
s/v64_shl_n_32/v128_shl_n_64/g
s/v64_shr_n_u32/v128_shr_n_u64/g
s/v64_shr_n_s32/v128_shr_n_s64/g
' $dst

sed -i '
s/u32_load_unaligned/v64_load_unaligned/g
s/u32_load_aligned/v64_load_aligned/g
s/u32_store_unaligned/v64_store_unaligned/g
s/u32_store_aligned/v64_store_aligned/g
s/u32_zero/v64_zero/g
s/uint32_t/v64/g
' $dst

sed -i '
s/uint16_t/uint32_t/g
s/int16_t/int32_t/g
' $dst

sed -i '
s/quote64_/v64_/g
s/quote128_/v128_/g
s/quote256_/v256_/g
' $dst

sed -i '
s/quote64 /v64 /g
s/quote128 /v128 /g
s/quote256 /v256 /g
' $dst
