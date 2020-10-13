#ifndef DATATYPES_H__
#define DATATYPES_H__

#include <cstring>
#include <x86intrin.h>
#include <cmath>
typedef __int128_t uint128_t;
#include "fixed.h"
#include "common.h"

 

static constexpr size_t dim256 = DIM * sizeof(uint64_t) / sizeof(__m256i);
static constexpr size_t dim128 = DIM * sizeof(uint64_t) / sizeof(__m128i);

template <size_t precision> 
union profile128;

template <size_t precision>
union profile
{
  __m256i m256[dim256];
  __m128i m128[dim128];
  uint64_t u64[DIM];
  fixed_t<precision> f64[DIM];
  profile() { }
  profile(double d[DIM]) { for (size_t i = 0; i < DIM; ++i) f64[i] = d[i]; }
  inline profile<precision> & operator=(double d[DIM]) { for (size_t i = 0; i < DIM; ++i) f64[i] = d[i]; return *this; }
  inline profile<precision> operator-()
  {
    profile<precision> p;
    for (size_t i = 0; i < DIM; ++i) p.u64[i] = -u64[i];
    return p;
  }
  template <size_t precision2>
  inline operator profile<precision2>() 
  {
    profile<precision2> p;

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

  inline profile<precision> & operator=(profile128<precision> p128) 
  {
    profile<precision> p64;

    for (size_t i = 0; i < DIM; ++i)
    {
       p64.u64[i] = p128.u128[i];
    }

    return std::move(p64);
  }

};

 
template <size_t precision>
inline profile128<precision> operator+(const profile128<precision> & p1, const profile<precision> & p2)
{
  profile128<precision> p3;
  for (size_t i = 0; i < DIM; ++i) 
  {
      p3.u128[i] = p1.u128[i] + p2.u64[i];
  }
  //for (size_t i = 0; i < DIM; ++i) p3.u64[i] = p1.u64[i] + p2.u64[i];
  return p3;
}

template <size_t precision>
inline profile128<precision> operator-(const profile128<precision> & p1, const profile128<precision> & p2)
{
  profile128<precision> p3;
  for (size_t i = 0; i < DIM; ++i) 
  {
      p3.u128[i] = p1.u128[i] - p2.u128[i];
  }
  //for (size_t i = 0; i < DIM; ++i) p3.u64[i] = p1.u64[i] + p2.u64[i];
  return p3;
}

template <size_t precision>
inline profile128<precision> operator+(const profile128<precision> & p1, const profile128<precision> & p2)
{
  profile128<precision> p3;
  for (size_t i = 0; i < DIM; ++i) 
  {
      p3.u128[i] = p1.u128[i] + p2.u128[i];
  }
  //for (size_t i = 0; i < DIM; ++i) p3.u64[i] = p1.u64[i] + p2.u64[i];
  return p3;
}
 
template <size_t precision>
inline profile128<2*precision> operator*(fixed_t<precision, uint128_t> c, const profile128<precision> & p)
{
  profile128<2*precision> cp;
  for (size_t i = 0; i < DIM; ++i) cp.f128[i] = c * p.f128[i];
  return cp;
}

template <size_t precision>
inline profile128<precision> & operator+=(profile128<precision> & p1, const profile128<precision> & p2)
{
  for (size_t i = 0; i < DIM; ++i) 
    {
      p1.u128[i] += p2.u128[i];
    }
//  
  return p1;
}

template <size_t precision>
inline profile128<precision> & operator-=(profile128<precision> & p1, const profile128<precision> & p2)
{
  for (size_t i = 0; i < DIM; ++i) 
    {
      p1.u128[i] -= p2.u128[i];
    }
//  
  return p1;
}
struct p3_beavers
{
  profile<precision>   A_m; // Used for MUX
  uint64_t RRtCW; // Used for MUX
  uint64_t sanity_blinds;
 
  static constexpr size_t size = 1 * sizeof(profile<precision>) + 1 * sizeof(uint64_t)
  								 + sizeof(uint64_t);
};


struct beaver3
{

  profile<precision> u0_blind, u1_blind;  
 
  profile<precision> beta;
 
  profile<precision> sanity_blind;
 
  fixed_t<2*precision> alpha; // used to blind \langle \vec{u}_0, \vec{u}_1 \rangle


  static constexpr size_t size =  4 *  sizeof(profile<precision>) 

                                + 1 * sizeof(fixed_t<2*precision>);  

};


 

 

struct beaver1
{
 
  profile<precision> A_m;
  profile<precision> usr;
  profile<precision> itm;
  fixed_t<2*precision> dp;
 
  static constexpr size_t size =      sizeof(fixed_t<2*precision>) 
                                + 3 * sizeof(profile<precision>);
};


struct beaver2_sanity
{
  profile<precision> A_m;
  uint64_t RRtCW;
  uint64_t sanity_blinds;

  static constexpr size_t size = 1 * sizeof(profile<precision>) 
                                 + 2 * sizeof(uint64_t);
};

struct beaver2
{ 

  profile<3*precision> usr;

  profile<3*precision> itm;

  fixed_t<2*precision> dp;
  

  static constexpr size_t size =  2 * sizeof(profile<3*precision>) 
                                + 1 * sizeof(fixed_t<2*precision>);
 
};


 


struct blind_vecs
{
  profile<precision> usr;
  profile<precision> itm;

  static constexpr size_t size = 2 * sizeof(profile<precision>);
};

struct blind_vecs_constant
{
  profile<precision> vec;
  fixed_t<precision> sca;

  static constexpr size_t size =  sizeof(fixed_t<precision>) + sizeof(profile<precision>);
};

template <size_t precision>
inline profile<2*precision> operator*(fixed_t<precision> c, const profile<precision> & p)
{
  profile<2*precision> cp;
  for (size_t i = 0; i < DIM; ++i) cp.f64[i] = c * p.f64[i];
  return cp;
}

template <size_t precision>
inline profile<precision> operator * (const profile<precision> & p, double c)
{
  profile<precision> cp;
  for (size_t i = 0; i < DIM; ++i) cp.f64[i] = (double)p.f64[i] * c;
  return cp;
}

template <size_t precision>
inline profile<3*precision> operator*(fixed_t<2*precision> c, const profile<precision> & p)
{
  profile<3*precision> cp;
  for (size_t i = 0; i < DIM; ++i) cp.f64[i] = p.f64[i] * c;
  return cp;
}


template <size_t precision>
inline profile<3*precision> operator*(fixed_t<precision> c, const profile<2*precision> & p)
{
  profile<3*precision> cp;
  for (size_t i = 0; i < DIM; ++i) cp.f64[i] = c * p.f64[i];
  return cp;
}


 

template <size_t precision>
inline profile<precision> operator+(const profile<precision> & p1, const profile<precision> & p2)
{
  profile<precision> p3;
  for (size_t i = 0; i < dim256; ++i) p3.m256[i] = _mm256_add_epi64(p1.m256[i], p2.m256[i]);
  //for (size_t i = 0; i < DIM; ++i) p3.u64[i] = p1.u64[i] + p2.u64[i];
  return p3;
}

template <size_t precision>
inline profile<precision> operator-(const profile<precision> & p1, const profile<precision> & p2)
{
  profile<precision> p3;
  for (size_t i = 0; i < dim256; ++i) p3.m256[i] = _mm256_sub_epi64(p1.m256[i], p2.m256[i]);
//  for (size_t i = 0; i < DIM; ++i) p3.u64[i] = p1.u64[i] - p2.u64[i];
  return p3;
}

template <size_t precision>
inline profile<precision> & operator+=(profile<precision> & p1, const profile<precision> & p2)
{
  for (size_t i = 0; i < dim256; ++i) p1.m256[i] = _mm256_add_epi64(p1.m256[i], p2.m256[i]);
//  for (size_t i = 0; i < DIM; ++i) p1.u64[i] += p2.u64[i];
  return p1;
}

template <size_t precision>
inline bool operator==(profile<precision> & p1, const profile<precision> & p2)
{
  return memcmp(&p1, &p2, sizeof(profile<precision>)) == 0;
}

template <size_t precision>
inline bool operator!=(profile<precision> & p1, const profile<precision> & p2)
{
  return memcmp(&p1, &p2, sizeof(profile<precision>)) != 0;
}

template <size_t precision>
inline profile<precision> & operator-=(profile<precision> & p1, const profile<precision> & p2)
{
  for (size_t i = 0; i < dim256; ++i) p1.m256[i] = _mm256_sub_epi64(p1.m256[i], p2.m256[i]);
//  for (size_t i = 0; i < DIM; ++i) p1.u64[i] -= p2.u64[i];
  return p1;
}

template <size_t precision>
inline fixed_t<2*precision> dot(const profile<precision> & p1, const profile<precision> & p2)
{
  fixed_t<2*precision> dp = 0.0;
  for (size_t i = 0; i < DIM; ++i) dp += p1.f64[i] * p2.f64[i];
  return dp;
}

template <size_t precision>
inline fixed_t<precision> profile_dot3(profile<precision> * p1, const profile<precision> * p2, size_t n)
{
  fixed_t<precision> dp = 0.0;
  for (size_t i = 0; i < n; ++i)
  {

    for(size_t j = 0; j < DIM; ++j)
    {
      dp +=  p1[i].f64[j];
    }

  }

  return dp;
}

template <size_t precision>
std::ostream & operator<<(std::ostream & os, const profile<precision> & u)
{
  os << "[ ";
  for (size_t i = 0; i < DIM; ++i) os << u.f64[i] << " ";
  return os << "]";
}


template <size_t precision>
std::ostream & operator<<(std::ostream & os, const profile128<precision> & u)
{
  os << "[ ";
  for (size_t i = 0; i < DIM; ++i) os << u.f128[i] << " ";
  return os << "]";
}

template <size_t precision>
std::ostream & operator<<(std::ostream & os, const fixed_t<precision, uint64_t> & f)
{
  return os << (double)f;
}

template <size_t precision>
std::ostream & operator<<(std::ostream & os, const fixed_t<precision, uint128_t> & f)
{
  return os << (double)f;
}

#endif
