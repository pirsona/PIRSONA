#ifndef PRG_H__
#define PRG_H__

#include "aes.h"

inline void PRG(const AES_KEY & prgkey, const __m128i & seed, void * outbuf, const uint32_t len, const uint32_t from = 0)
{
  __m128i * outbuf128 = reinterpret_cast<__m128i *>(outbuf);
  for (size_t i = 0; i < len; ++i)
  {
    outbuf128[i] = _mm_xor_si128(seed, _mm_set_epi64x(0, from+i));
  }
  AES_ecb_encrypt_blks(outbuf128, static_cast<unsigned int>(len), &prgkey);
  for (size_t i = 0; i < len; ++i) 
  {
    outbuf128[i] = _mm_xor_si128(outbuf128[i], _mm_set_epi64x(0, from+i));
    outbuf128[i] = _mm_xor_si128(outbuf128[i], seed);
  }
}


inline void PRG3(const AES_KEY & key, const __m128i seed, __m128i * outbuf, const size_t len, size_t t)
{
  for (size_t i = 0; i < len; ++i)
  {
    outbuf[i] = _mm_xor_si128(seed, _mm_set_epi64x(t, i + t));
  }
  AES_ecb_encrypt_blks(outbuf, len, &key);
  for (size_t i = 0; i < len; ++i)
  {
    outbuf[i] = _mm_xor_si128(outbuf[i], _mm_set_epi64x(t, i + t));
    outbuf[i] = _mm_xor_si128(outbuf[i], seed);
  }
}


inline void PRG2(const AES_KEY & key, const __m128i seed, __m128i * outbuf, const size_t len, size_t & ii)
{
  for (size_t i = 0; i < len; ++i)
  {
    //__m128i i_ = _mm_set_epi64x(0, i);
    outbuf[i] = _mm_xor_si128(seed, _mm_set_epi64x(0, i));
    AES_ecb_encrypt_blks(outbuf, 1, &key);
    outbuf[i] = _mm_xor_si128(outbuf[i], _mm_set_epi64x(0, i));
    outbuf[i] = _mm_xor_si128(outbuf[i], seed);
  }
}

#endif