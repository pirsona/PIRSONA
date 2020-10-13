#ifndef DATATYPESNP_H__
#define DATATYPESNP_H__

#include <cstring>
#include <x86intrin.h>
#include <cmath>

#include "fixed-np.h"
#include "common.h"
//#define DIM 4

static constexpr size_t dim256 = DIM * sizeof(uint64_t) / sizeof(__m256i);
static constexpr size_t dim128 = DIM * sizeof(uint64_t) / sizeof(__m128i);


union profile
{

  double f64[DIM]; 
  profile() { }
  profile(double d[DIM]) { for (size_t i = 0; i < DIM; ++i) f64[i] = (double)d[i]; }
  inline profile & operator=(double d[DIM]) 
  { 
    for (size_t i = 0; i < DIM; ++i) f64[i] = (double)d[i]; return *this; 
  }

  // inline profile operator-()
  // {
  //   profile p;
  //   for (size_t i = 0; i < DIM; ++i) p.u64[i] = -u64[i];
  //   return p;
  // }
};


template <size_t precision>
union prof
{
  __m256i m256[dim256];
  __m128i m128[dim128];
  uint64_t u64[DIM];
  fixed_t<precision> f64[DIM];
  prof() { }
  prof(double d[DIM]) { for (size_t i = 0; i < DIM; ++i) f64[i] = d[i]; }
  inline prof<precision> & operator=(double d[DIM]) { for (size_t i = 0; i < DIM; ++i) f64[i] = d[i]; return *this; }
  inline prof<precision> operator-()
  {
    prof<precision> p;
    for (size_t i = 0; i < DIM; ++i) p.u64[i] = -u64[i];
    return p;
  }
  template <size_t precision2>
  inline operator prof<precision2>() 
  {
    prof<precision2> p;

    double scale1 = 1.0L * static_cast<double>(1ULL << precision);
    double scale2 = 1.0L * static_cast<double>(1ULL << precision2);
    for (size_t i = 0; i < DIM; ++i)
    {
      if(uint64_t(scale2/scale1) == 0){
      p.f64[i] = fixed_t<precision2>(f64[i]);
      }
      else{
      p.f64[i].val = (f64[i].val) * (uint64_t(scale2/scale1));  
      }
    } 
    return p;
  }
};


inline profile operator*(double c, const profile & p)
{
  profile cp;
  for (size_t i = 0; i < DIM; ++i) cp.f64[i] = c * p.f64[i];
  return cp;
}

inline profile operator+(const profile & p1, const profile & p2)
{
  profile p3;
  for (size_t i = 0; i < DIM; ++i) p3.f64[i] = p1.f64[i] + p2.f64[i];
  //for (size_t i = 0; i < DIM; ++i) p3.u64[i] = p1.u64[i] + p2.u64[i];
  return p3;
}

inline profile operator-(const profile & p1, const profile & p2)
{
  profile p3;
  for (size_t i = 0; i < DIM; ++i) p3.f64[i] = p1.f64[i] - p2.f64[i];
  //for (size_t i = 0; i < DIM; ++i) p3.u64[i] = p1.u64[i] + p2.u64[i];
  return p3;
}

template <size_t precision>
inline prof<3*precision> operator*(fixed_t<2*precision> c, const prof<precision> & p)
{
  prof<3*precision> cp;
  for (size_t i = 0; i < DIM; ++i) cp.f64[i] = p.f64[i] * c;
  return cp;
}



template <size_t precision>
inline prof<precision> operator-(const prof<precision> & p1, const prof<precision> & p2)
{
  prof<precision> p3;
  for (size_t i = 0; i < dim256; ++i) p3.m256[i] = _mm256_sub_epi64(p1.m256[i], p2.m256[i]);
 //for (size_t i = 0; i < DIM; ++i) p3.u64[i] = p1.u64[i] - p2.u64[i];
  return p3;
}

template <size_t precision>
inline prof<precision> & operator+=(prof<precision> & p1, const prof<precision> & p2)
{
  for (size_t i = 0; i < dim256; ++i) p1.m256[i] = _mm256_add_epi64(p1.m256[i], p2.m256[i]);
//  for (size_t i = 0; i < DIM; ++i) p1.u64[i] += p2.u64[i];
  return p1;
}

template <size_t precision>
inline bool operator==(prof<precision> & p1, const prof<precision> & p2)
{
  return memcmp(&p1, &p2, sizeof(prof<precision>)) == 0;
}

template <size_t precision>
inline bool operator!=(prof<precision> & p1, const prof<precision> & p2)
{
  return memcmp(&p1, &p2, sizeof(prof<precision>)) != 0;
}

template <size_t precision>
inline prof<precision> & operator-=(prof<precision> & p1, const prof<precision> & p2)
{
  for (size_t i = 0; i < dim256; ++i) p1.m256[i] = _mm256_sub_epi64(p1.m256[i], p2.m256[i]);
//  for (size_t i = 0; i < DIM; ++i) p1.u64[i] -= p2.u64[i];
  return p1;
}

inline double dot(const profile & p1, const profile& p2)
{
  double dp = 0.0;
  for (size_t i = 0; i < DIM; ++i) dp += p1.f64[i] * p2.f64[i];
  return dp;
}

#endif
