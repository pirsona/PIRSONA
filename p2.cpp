#include <thread>
#include <iostream>
#include <chrono>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include<vector>
 
#include<fstream>
#include "aes.h"
#include "network.h"

#include "datatypes.h"
  
#include "prg.h"
#include "dpf.h"
#include "sanitycheck.h"

 enum norm_step
 {
  blinds_created = 0,
  norm_bvrs_in,
  dotprod_gen_usr,
  dotprod_gen_itm,
  dotprod_bvr0_out,
  dotprod_bvr1_out,
  X_in,
  reduced_in,
  norm_bvrs_PB_out,
  norm_bvrs_P_other_out,
  dpfs_in,
  sshare_in,
  blind_cnt_in, 
  blind_sshare_in,
  blind_cnt_out,
  s_compute,
  z_compute,
  z_out,
  norm_num_steps,
};
size_t norm_progress[norm_step::norm_num_steps] = { 0 };

template<size_t nitems> struct query { size_t user; dpf_key<nitems> dpfkey; };
query<nitems> * queries;
query<nitems> * queries_cw;
uint64_t * dp0;
uint64_t * dp1; 
int8_t sanity_vector_mux[nqueries] = {0};

using boost::asio::ip::tcp;

AES_KEY key;
 
boost::asio::io_context io_context;

enum p2_step
{
  beaver_generated = 0,
  beaver_out0,
  beaver_out1,
  sanity_beaver_out0,
  sanity_beaver_out1,
  i_precision_beavers_gen,
  u_precision_beavers_gen,
  dp0_in,
  dp1_in,
  verified,
  num_steps
};

size_t p2_progress[p2_step::num_steps] = { 0 };

  
  beaver1 * bvr0 = (beaver1 *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver1));
  beaver1 * bvr1 = (beaver1 *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver1));
  beaver2 * bvr2 = (beaver2 *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver2));
 
  beaver2 * bvrs0 = (beaver2 *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver2));
  beaver2 * bvrs1 = (beaver2 *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver2));

  beaver2_sanity * bvrs0_sanity = (beaver2_sanity *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver2_sanity));
  beaver2_sanity * bvrs1_sanity = (beaver2_sanity *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver2_sanity));


  __m128i seed_precision0;
  __m128i seed_precision1; 
  uint64_t I_profile_msb_alpha_0[total_bucket_elements][DIM];
  uint64_t I_profile_msb_alpha_1[total_bucket_elements][DIM];
  uint64_t I_profile_msb_blind_0[total_bucket_elements][DIM];
  uint64_t I_profile_msb_blind_1[total_bucket_elements][DIM];

  
  __m128i u_seed_precision0;
  __m128i u_seed_precision1; 
   
  
  uint64_t U_profile_msb_alpha_0[nusers][DIM];
  uint64_t U_profile_msb_alpha_1[nusers][DIM];
  uint64_t U_profile_msb_blind_0[nusers][DIM];
  uint64_t U_profile_msb_blind_1[nusers][DIM];

   
   beaver3 * bvr3 = (beaver3 *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver3));
 

  __m128i seed[2];
  __m128i seed_mux;    
  __m128i seeds_sanity[2];

 
void keep_polling(boost::asio::io_context& io_context)
{
  while(
         p2_progress[p2_step::verified] < nqueries || p2_progress[p2_step::beaver_out1] < nqueries || p2_progress[p2_step::beaver_out0] < nqueries ||
         p2_progress[p2_step::sanity_beaver_out0] < nqueries ||  p2_progress[p2_step::sanity_beaver_out1] < nqueries

         )
  {
    io_context.reset();
    io_context.poll();
  } 
}


 
void send_sanity_seed_to_P0(tcp::socket& sout)
{
 
  //io_context.reset();
  boost::asio::async_write(sout, boost::asio::buffer(seeds_sanity, 2 * sizeof(__m128i)),
            [](boost::system::error_code ec, std::size_t) 
  {

  });

  //io_context.poll();
}


void send_sanity_seed_to_P1(tcp::socket& sout)
{
   
  boost::asio::async_write(sout, boost::asio::buffer(seeds_sanity, 2 * sizeof(__m128i)),
            [](boost::system::error_code ec, std::size_t) 
  {

  });

}



void write_U_precision_beavers_to_P0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
    while( p2_progress[p2_step::beaver_generated] < 1) 
    { 
        std::this_thread::yield(); 
    }
   
 
      if(j == 0)
      {
        write(sout, boost::asio::buffer(&u_seed_precision0 , sizeof(__m128i)));
      }
      
      async_write(sout, boost::asio::buffer(&U_profile_msb_alpha_0[j], DIM * sizeof(uint64_t)),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                   
                   if(j + 1 < nusers) 
                   {
                    write_U_precision_beavers_to_P0(j + 1, io_context, sout);
                   }
                }          
                else
                {                
                  write_U_precision_beavers_to_P0(j , io_context,  sout);
                }
            
            }); 
}

void write_U_precision_beavers_to_P1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
 
      
     while( p2_progress[p2_step::beaver_generated] < 1) 
     { 
      std::this_thread::yield(); 
     }
 
     if(j == 0)
     {
      write(sout, boost::asio::buffer(&u_seed_precision1 , sizeof(__m128i)));
     }  
 
      async_write(sout, boost::asio::buffer(&U_profile_msb_alpha_1[j], DIM * sizeof(uint64_t)),
            [ j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                   
                   if(j + 1 < nusers) 
                   {
                    write_U_precision_beavers_to_P1( j + 1, io_context, sout);
                   }
                }          
                else
                {                
                  write_U_precision_beavers_to_P1( j , io_context,  sout);
                }
            
            }); 
}


void write_I_precision_seed_to_P0(tcp::socket& sout)
{
    while(p2_progress[p2_step::beaver_generated] < 1)
  {
    std::this_thread::yield();
  }
  write(sout, boost::asio::buffer(&seed_precision0 , sizeof(__m128i)));
}

void write_precision_beavers_to_P0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
    while( p2_progress[p2_step::beaver_generated] < 1) 
    { 
        std::this_thread::yield(); 
    }
   
 
      // if(j == 0)
      // {
      //   write(sout, boost::asio::buffer(&seed_precision0 , sizeof(__m128i)));
      // }
      
      async_write(sout, boost::asio::buffer(&I_profile_msb_alpha_0[j], DIM * sizeof(uint64_t)),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                   
                   if(j + 1 < total_bucket_elements) 
                   {
                    write_precision_beavers_to_P0(j + 1, io_context, sout);
                   }
                }          
                else
                {                
                  write_precision_beavers_to_P0(j , io_context,  sout);
                }
            
            }); 
}



/*
 sends beavers beavers to reduce I-profile's precision to P1 
*/

void write_I_precision_seed_to_P1(tcp::socket& sout)
{ 

  while(p2_progress[p2_step::beaver_generated] < 1)
  {
    std::this_thread::yield();
  }
  write(sout, boost::asio::buffer(&seed_precision1 , sizeof(__m128i)));
}

void write_precision_beavers_to_P1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
 
  while( p2_progress[p2_step::beaver_generated] < 1) 
  { 
    std::this_thread::yield(); 
  }
   
  // if(j == 0)
  // {
  //   write(sout, boost::asio::buffer(&seed_precision1 , sizeof(__m128i)));
  // }  
 
  async_write(sout, boost::asio::buffer(&I_profile_msb_alpha_1[j], DIM * sizeof(uint64_t)),
        [ j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
        
        {       
            if(!ec)
            {                   
               if(j + 1 < total_bucket_elements) 
               {
                write_precision_beavers_to_P1( j + 1, io_context, sout);
               }
            }          
            else
            {                
              write_precision_beavers_to_P1( j , io_context,  sout);
            }
        
        }); 
}



/*
  sending beavers for adaptation rules to P0
*/ 
void write_beaver_to_P0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout_seed, tcp::socket& sout)
{
  
    while( p2_progress[p2_step::beaver_generated] < j + 1) 
    { 
        std::this_thread::yield(); 
    }
   
    if(j == 0)
    {
      std::cout << "writing to P0: " << seed[0][0] << " " << seed[0][1] << std::endl;
      write(sout_seed, boost::asio::buffer(&seed[0], sizeof(__m128i)));
    }

    async_write(sout, boost::asio::buffer(&bvrs0[j], beaver2::size),
            [j, &io_context, &sout, &sout_seed](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                   
                   if(j + 1 < nqueries) 
                   {
                    write_beaver_to_P0(j + 1, io_context,sout_seed, sout);
                   }
                }          
                else
                {                
                  write_beaver_to_P0(j , io_context,sout_seed, sout);
                }
            
            }); 

    p2_progress[p2_step::beaver_out0] = j + 1;

} 

/*
  sending beavers for adaptation rules to P1
*/ 
void write_beaver_to_P1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout_seed,  tcp::socket& sout)
{

     while( p2_progress[p2_step::beaver_generated] < j + 1) 
     { 
        std::this_thread::yield(); 
     }
    
    if(j == 0)
    {
      std::cout << "writing to P0: " << seed[1][0] << " " << seed[1][1] << std::endl;
      write(sout_seed, boost::asio::buffer(&seed[1], sizeof(__m128i)));
    }
 
    async_write(sout, boost::asio::buffer(&bvrs1[j], beaver2::size),
            [j, &io_context, &sout, &sout_seed](boost::system::error_code ec, std::size_t) 
            {       
                if(!ec)
                {                   
                   if(j + 1 < nqueries) 
                   {
                    write_beaver_to_P1(j + 1, io_context, sout_seed, sout);
                   }
                }          
                else
                {                
                  write_beaver_to_P1(j , io_context, sout_seed, sout);
                }
            
            }); 

    p2_progress[p2_step::beaver_out1] = j + 1;
  
}


/*
  sending beavers for sanity checks to P0
*/ 

void write_sanity_beaver_to_P0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
  
    while( p2_progress[p2_step::beaver_generated] < j + 1) 
    { 
        std::this_thread::yield(); 
    }
 
    async_write(sout, boost::asio::buffer(&bvrs0_sanity[j], beaver2_sanity::size),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                   
                   if(j + 1 < nqueries) 
                   {
                    write_sanity_beaver_to_P0(j + 1, io_context, sout);
                   }
                }          
                else
                {                
                  write_sanity_beaver_to_P0(j , io_context, sout);
                }
            
            }); 

    p2_progress[p2_step::sanity_beaver_out0] = j + 1;

} 


/*
  sending beavers for sanity checks to P1
*/ 
void write_sanity_beaver_to_P1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
  
    while( p2_progress[p2_step::beaver_generated] < j + 1) 
    { 
        std::this_thread::yield(); 
    }
   
 
    async_write(sout, boost::asio::buffer(&bvrs1_sanity[j], beaver2_sanity::size),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                   
                   if(j + 1 < nqueries) 
                   {
                    write_sanity_beaver_to_P1(j + 1, io_context, sout);
                   }
                }          
                else
                {                
                  write_sanity_beaver_to_P1(j , io_context, sout);
                }
            
            }); 

    p2_progress[p2_step::sanity_beaver_out1] = j + 1;

} 
   
void read_dp_from_P0(tcp::socket& sin)
{ 
  dp0 = (uint64_t *) malloc(sizeof(uint64_t) * nqueries);
  for(size_t j = 0; j < nqueries; ++j)
  {

 

   boost::asio::read(sin, boost::asio::buffer(&dp0[j], sizeof(uint64_t)));
   //std::cout << "dp0 = " << dp0 << std::endl;

   p2_progress[p2_step::dp0_in] = j + 1;
  }
}


void read_dp_from_P1(tcp::socket& sin)
{
   dp1 = (uint64_t *) malloc(sizeof(uint64_t) * nqueries);
  for(size_t j = 0; j < nqueries; ++j)
  {
 
    boost::asio::read(sin, boost::asio::buffer(&dp1[j], sizeof(uint64_t)));
    //std::cout << "dp1 = " << dp1 << std::endl;
    p2_progress[p2_step::dp1_in] = j + 1;
  }
}

void verify_(const std::vector<uint64_t> & srt0, const std::vector<uint64_t> & inv1)
{
  size_t verified = 0;
  size_t not_verified = 0;
  for(size_t j = 0; j < nqueries; ++j)
  {
   while(p2_progress[p2_step::dp0_in] < j + 1 || p2_progress[p2_step::dp1_in] < j + 1)
   {
    std::this_thread::yield();
   } 
  
   if(verify(dp0[j], dp1[j], srt0, inv1))
   {
     ++verified;// std::cout << "true\n\n";
     sanity_vector_mux[j] = 1;
   }
  else
   {
     std::cout << j << " -> " << dp0[j] << " " << dp1[j] << std::endl;
     ++not_verified;
     sanity_vector_mux[j] = 0;
   } 

   p2_progress[p2_step::verified] = j + 1;
  }

  std::cout << "verified = " << verified << std::endl;
  std::cout << "not_verified = " << not_verified << std::endl;
}


void send_mux_verification_to_Pb(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
  while(p2_progress[p2_step::verified] < j + 1)
  {
    std::this_thread::yield();
  }

  async_write(sout, boost::asio::buffer(&sanity_vector_mux[j], sizeof(int8_t)),
        [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
        
        {       
            if(!ec)
            {                   
               if(j + 1 < nqueries) 
               {
                send_mux_verification_to_Pb(j + 1, io_context, sout);
               }
            }          
            else
            {                
              send_mux_verification_to_Pb(j , io_context, sout);
            }
        
        }); 

} 


 
void generate_I_precision_beavers(AES_KEY& key)
{

   arc4random_buf(&seed_precision0, sizeof(__m128i));
   PRG(key, seed_precision0, (__m128i *) I_profile_msb_blind_0, ( DIM * sizeof(uint64_t) * total_bucket_elements) / sizeof(__m128i));
  
   arc4random_buf(&seed_precision1, sizeof(__m128i));
   PRG(key, seed_precision1, (__m128i *)I_profile_msb_blind_1, ( DIM * sizeof(uint64_t) * total_bucket_elements) / sizeof(__m128i));
  
  for(size_t j = 0; j < total_bucket_elements; ++j)
  {

    for(size_t dim = 0; dim < DIM; ++dim)
    { 
      I_profile_msb_alpha_0[j][dim] = I_profile_msb_blind_0[j][dim] * I_profile_msb_blind_1[j][dim];
      I_profile_msb_alpha_1[j][dim] = I_profile_msb_blind_0[j][dim] * I_profile_msb_blind_1[j][dim];
    }

    p2_progress[p2_step::i_precision_beavers_gen] = j + 1;
  }
}

void generate_U_precision_beavers(AES_KEY& key)
{

   arc4random_buf(&u_seed_precision0, sizeof(__m128i));
   PRG(key, u_seed_precision0, (__m128i *) U_profile_msb_blind_0, ( DIM * sizeof(uint64_t) * nusers) / sizeof(__m128i));
  
   arc4random_buf(&u_seed_precision1, sizeof(__m128i));
   PRG(key, u_seed_precision1, (__m128i *)U_profile_msb_blind_1, ( DIM * sizeof(uint64_t) * nusers) / sizeof(__m128i));

   std::cerr << "seed_precision0 = " << seed_precision0[0] << " " << seed_precision0[1] << std::endl;
   std::cerr << "seed_precision1 = " << seed_precision1[0] << " " << seed_precision1[1] << std::endl;
  

   for(size_t j = 0; j < nusers; ++j)
   {
    for(size_t dim = 0; dim < DIM; ++dim)
    {
      U_profile_msb_alpha_0[j][dim] = U_profile_msb_blind_0[j][dim] * U_profile_msb_blind_1[j][dim];
      U_profile_msb_alpha_1[j][dim] = U_profile_msb_blind_0[j][dim] * U_profile_msb_blind_1[j][dim];
    }
     p2_progress[p2_step::u_precision_beavers_gen] = j + 1;
   }
}

void generate_beavers(AES_KEY & key)
{

  const size_t buflen = nitems * sizeof(profile<precision>);

  profile<precision> * A = (profile<precision> *) std::aligned_alloc(sizeof(__m256i), nitems * sizeof(profile<precision>));
 
  profile<precision> A_m;
  
  fixed_t<2 * precision> pm1_;
 
  __m128i * s = (__m128i *) std::aligned_alloc(sizeof(__m128i), nitems * sizeof(__m128i));
  
  uint8_t * t = (uint8_t*)malloc(nitems * sizeof(uint8_t));
  
  profile<3*precision> * tCW = (profile<3 * precision>*) std::aligned_alloc(sizeof(__m256i), nitems * sizeof(profile<precision>));
  profile<3*precision>  zero_profile;  
  for(size_t d = 0; d < DIM; ++d) zero_profile.u64[d] = 0;
  uint64_t RR[nitems][DIM];
  for(size_t i = 0; i < nitems; ++i)
  {
    for(size_t j = 0; j < DIM; ++j)
    {
      arc4random_buf(&RR[i][j], sizeof(uint64_t));
    }
  }

   arc4random_buf(&seed_precision0, sizeof(__m128i));
   PRG(key, seed_precision0, (__m128i *) I_profile_msb_blind_0, ( DIM * sizeof(uint64_t) * total_bucket_elements) / sizeof(__m128i));
  
   arc4random_buf(&seed_precision1, sizeof(__m128i));
   PRG(key, seed_precision1, (__m128i *)I_profile_msb_blind_1, ( DIM * sizeof(uint64_t) * total_bucket_elements) / sizeof(__m128i));
  

   arc4random_buf(&u_seed_precision0, sizeof(__m128i));
   PRG(key, u_seed_precision0, (__m128i *) U_profile_msb_blind_0, ( DIM * sizeof(uint64_t) * nusers) / sizeof(__m128i));
  
   arc4random_buf(&u_seed_precision1, sizeof(__m128i));
   PRG(key, u_seed_precision1, (__m128i *)U_profile_msb_blind_1, ( DIM * sizeof(uint64_t) * nusers) / sizeof(__m128i));

   std::cerr << "seed_precision0 = " << seed_precision0[0] << " " << seed_precision0[1] << std::endl;
   std::cerr << "seed_precision1 = " << seed_precision1[0] << " " << seed_precision1[1] << std::endl;
  

   for(size_t j = 0; j < nusers; ++j)
   {
    for(size_t dim = 0; dim < DIM; ++dim)
    {
      U_profile_msb_alpha_0[j][dim] = U_profile_msb_blind_0[j][dim] * U_profile_msb_blind_1[j][dim];
      U_profile_msb_alpha_1[j][dim] = U_profile_msb_blind_0[j][dim] * U_profile_msb_blind_1[j][dim];
    }
   }

  
  for(size_t j = 0; j < total_bucket_elements; ++j)
  {

    for(size_t dim = 0; dim < DIM; ++dim)
    { 
      I_profile_msb_alpha_0[j][dim] = I_profile_msb_blind_0[j][dim] * I_profile_msb_blind_1[j][dim];
      I_profile_msb_alpha_1[j][dim] = I_profile_msb_blind_0[j][dim] * I_profile_msb_blind_1[j][dim];
    }
  }


  PRG(key, seed[1], (__m128i *)A, buflen / sizeof(__m128i)); 

  for(size_t i = 0; i < nqueries; ++i)
  { 

    dpf_mux<nitems,1>(key, queries[i].dpfkey, A, A_m, s, t, pm1_);

    bvrs0[i].usr= bvr0[i].dp * bvr1[i].usr     - bvr2[i].usr;
    bvrs0[i].itm= bvr0[i].dp * bvr1[i].itm     + bvr2[i].itm;
    bvrs0[i].dp= dot(bvr0[i].usr, bvr1[i].itm) - bvr2[i].dp;
   
    //bvrs0[i].A_m = A_m;// + blind_r;
    bvrs0_sanity[i].A_m = A_m;

    bvrs1[i].usr = bvr1[i].dp * bvr0[i].usr      + bvr2[i].usr;
    bvrs1[i].itm = bvr1[i].dp * bvr0[i].itm      - bvr2[i].itm; 
    bvrs1[i].dp  = dot(bvr1[i].usr, bvr0[i].itm) + bvr2[i].dp;
   
 
    arc4random_buf(&bvrs1_sanity[i].A_m, sizeof(bvrs1_sanity[i].A_m));

 

    evalfull3(key, queries[i].dpfkey, s, t); 

    profile<3*precision> tmp; 

    for(size_t j = 0; j < nitems; ++j)
    {
      PRG(key, s[j], (__m128i *)&tmp, dim128);
     
      if(t[j])  tCW[j]  = queries_cw[i].dpfkey.leaf;
      if(!t[j]) tCW[j]  =  zero_profile;// 
     
      tCW[j] = tmp - tCW[j] - tCW[j];
    }

    for(size_t j = 0; j < nitems; ++j)
    {
      for(size_t d = 0; d < DIM; ++d)
      {
       // bvrs0[i].RRtCW += (tCW[j].u64[d]);
       bvrs0_sanity[i].RRtCW += (tCW[j].u64[d]);
      }
    }
    //bvrs0[i].RRtCW = profile_dot3(tCW, RR, nitems);

    bvrs0_sanity[i].RRtCW = bvrs0_sanity[i].RRtCW;// + bvr1[i].sanity_blinds;

    p2_progress[p2_step::beaver_generated] = i + 1;

  }
}
 


   



int main(int argc, char* argv[])
{ 
  
 AES_set_encrypt_key(_mm_set_epi64x(597349, 121379), &key);

  std::string qfile = std::string("./queries.0");

  int qfd = open(qfile.c_str(), O_RDWR);
  queries = (query<nitems> *) mmap(NULL, nqueries * sizeof(query<nitems>),
    PROT_READ, MAP_PRIVATE, qfd, 0);


  std::string qfile_cw = std::string("./queries.1");
    int qfd_cw = open(qfile_cw.c_str(), O_RDWR);
  queries_cw = (query<nitems> *) mmap(NULL, nqueries * sizeof(query<nitems>),
    PROT_READ, MAP_PRIVATE, qfd_cw, 0);
  //size_t nqueries = 1ULL << 16;
 
  

 
  arc4random_buf(seeds_sanity, 2 * sizeof(__m128i));
  arc4random_buf(seed, 2 * sizeof(__m128i));
  arc4random_buf(&seed_mux, 1 * sizeof(__m128i));
 
  auto [rnd0, rnd1] = gen_rands(key, seeds_sanity, nitems);
  
  auto srt0 = sort(rnd0);
  auto inv1 = inverse(rnd1);


  
  arc4random_buf(bvr2, (nqueries) * sizeof(beaver2));

  AES_KEY prgkey; 
  AES_set_encrypt_key(_mm_set_epi64x(597349, 121379), &prgkey);
  
  const size_t buflen = (nqueries) * sizeof(beaver1);
  PRG(prgkey, seed[0], (__m128i *)bvr0, buflen / sizeof(__m128i));
  PRG(prgkey, seed[1], (__m128i *)bvr1, buflen / sizeof(__m128i));

  tcp::acceptor acceptor1(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2));
  std::cout << " --- > " << std::endl;
  tcp::socket s1(acceptor1.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P2 << std::endl;

  tcp::acceptor acceptor1_a(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_a));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_a(acceptor1_a.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P2_a << std::endl;

  tcp::acceptor acceptor1_b(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_b));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_b(acceptor1_b.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P2_b << std::endl;

  tcp::acceptor acceptor1_c(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_c));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_c(acceptor1_c.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P2_c << std::endl;


  tcp::acceptor acceptor1_d(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_d));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_d(acceptor1_d.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P2_d << std::endl;

  tcp::acceptor acceptor1_e(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_e));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_e(acceptor1_e.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P2_e << std::endl;

  tcp::acceptor acceptor1_f(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_f));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_f(acceptor1_f.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P2_f << std::endl;

  tcp::acceptor acceptor1_g(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_g));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_g(acceptor1_g.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P2_g << std::endl;

    tcp::acceptor acceptor1_h(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_h));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_h(acceptor1_h.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P2_h << std::endl;

  tcp::acceptor acceptor0(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2));
  std::cout << " --- > " << std::endl;
  tcp::socket s0(acceptor0.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P2 << std::endl;

  tcp::acceptor acceptor0_a(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_a));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_a(acceptor0_a.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P2_a << std::endl;

  tcp::acceptor acceptor0_b(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_b));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_b(acceptor0_b.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P2_b << std::endl;

  tcp::acceptor acceptor0_c(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_c));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_c(acceptor0_c.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P2_c << std::endl;

  tcp::acceptor acceptor0_d(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_d));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_d(acceptor0_d.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P2_d << std::endl;

  tcp::acceptor acceptor0_e(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_e));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_e(acceptor0_e.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P2_e << std::endl;

  tcp::acceptor acceptor0_f(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_f));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_f(acceptor0_f.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P2_f << std::endl;

  tcp::acceptor acceptor0_g(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_g));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_g(acceptor0_g.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P2_g << std::endl;

    tcp::acceptor acceptor0_h(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_h));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_h(acceptor0_h.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P2_h << std::endl;
 

  tcp::acceptor acceptor3(io_context, tcp::endpoint(tcp::v4(), PORT_P3_P2));
  std::cout << " --- > " << std::endl;
  tcp::socket s3(acceptor3.accept());
  std::cerr << "Listenting on port: " << PORT_P3_P2 << std::endl;
 


  std::thread poller(keep_polling, std::ref(io_context));
  
  std::thread beaver_generator(generate_beavers, std::ref(prgkey));
  
  std::thread beaver_to_P0_writer(write_beaver_to_P0, 0, std::ref(io_context), std::ref(s0_e), std::ref(s0)); 
  std::thread beaver_to_P1_writer(write_beaver_to_P1, 0, std::ref(io_context), std::ref(s1_e), std::ref(s1));

  std::thread I_precision_seed_writer_0(write_I_precision_seed_to_P0, std::ref(s0_h));
  std::thread I_precision_seed_writer_1(write_I_precision_seed_to_P1, std::ref(s1_h));

  std::thread precision_beaver_to_P0_writer(write_precision_beavers_to_P0, 0, std::ref(io_context), std::ref(s0_f)); 
  std::thread precision_beaver_to_P1_writer(write_precision_beavers_to_P1, 0, std::ref(io_context), std::ref(s1_f));

  std::thread U_precision_beaver_to_P0_writer(write_U_precision_beavers_to_P0, 0, std::ref(io_context), std::ref(s0_g)); 
  std::thread U_precision_beaver_to_P1_writer(write_U_precision_beavers_to_P1, 0, std::ref(io_context), std::ref(s1_g));

  // write_precision_beavers_to_P1(AES_KEY& prgkey, size_t j, boost::asio::io_context& io_context, tcp::socket& sout)

  std::thread sanity_beaver_to_P0_writer(write_sanity_beaver_to_P0, 0, std::ref(io_context), std::ref(s0_c)); 
  std::thread sanity_beaver_to_P1_writer(write_sanity_beaver_to_P1, 0, std::ref(io_context), std::ref(s1_c));

  std::thread dp_from_P0_reader(read_dp_from_P0,std::ref(s0_b));
  std::thread dp_from_P1_reader(read_dp_from_P1,std::ref(s1_b));

  std::thread verifier(verify_, std::ref(srt0), std::ref(inv1));
  
  std::thread mux_verification_P0_sender(send_mux_verification_to_Pb, 0, std::ref(io_context), std::ref(s0_d));
  std::thread mux_verification_P1_sender(send_mux_verification_to_Pb, 0, std::ref(io_context), std::ref(s1_d));

  std::thread sanity_seed_to_P0_writer(send_sanity_seed_to_P0, std::ref(s0_a));
  std::thread sanity_seed_to_P1_writer(send_sanity_seed_to_P1, std::ref(s1_a));
  



  poller.join();
  
  beaver_generator.join();
  
  beaver_to_P0_writer.join(); 
  beaver_to_P1_writer.join();
  
  I_precision_seed_writer_0.join();
  I_precision_seed_writer_1.join();
 
  precision_beaver_to_P0_writer.join();
  precision_beaver_to_P1_writer.join();
  
  U_precision_beaver_to_P0_writer.join();
  U_precision_beaver_to_P1_writer.join();

  sanity_beaver_to_P0_writer.join();
  sanity_beaver_to_P1_writer.join();

  dp_from_P0_reader.join(); 
  dp_from_P1_reader.join();

  verifier.join();
 
  mux_verification_P0_sender.join();  
  mux_verification_P1_sender.join();

  sanity_seed_to_P0_writer.join();
  sanity_seed_to_P1_writer.join();

  return 0;
}

 