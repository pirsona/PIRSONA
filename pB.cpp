#include <thread>
#include <iostream>
#include <chrono>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <boost/asio.hpp>
#include <mutex>
#include <boost/lexical_cast.hpp>
 
#include "datatypes.h"
#include "network.h"
 

#include "prg.h"
#include "dpf.h"
#include "pB.h"




using boost::asio::ip::tcp;
using namespace std::chrono;

auto start_of_program = std::chrono::system_clock::now();
// globals
AES_KEY aeskey;

int8_t sanity_vector_mux[nqueries]   = {2};
int8_t sanity_vector_demux[nqueries] = {2};



p3_beavers * p3_bvrs;

query<nitems> * queries;

profile<precision>   * itmBB;
profile<precision> * A = (profile<precision> *) std::aligned_alloc(sizeof(__m256i), nitems * sizeof(profile<precision>));
profile<precision> * uprofiles;
blind_vecs * blinds;
profile<precision> * iprofiles;
blind_vecs * blinded_profile;
fixed_t<2*precision> * blinded_norm;
profile<3*precision> * final_cw;
profile<3*precision> * final_cw2;

beaver1 * bvr;
beaver2 * bvr2;
beaver2_sanity * bvr2_sanity;
 

__m128i ** S;
uint8_t ** T;

__m128i seed3;
__m128i sanity_seeds[2]; 
profile<precision> * itm;

fixed_t<2*precision> * pm1;
fixed_t<2*precision> * norm;
fixed_t<2*precision> * blinded_norm_out;

profile<3*precision> ** expanded_leaf;
 
profile<3*precision> * iprofiles2;
profile<3*precision> * uprofiles2;
 
profile<3 * (precision/2)> * uprofiles_reconstructed;
profile<3 * (precision/2)> * iprofiles_reconstructed;

std::vector<__m256i> rnd0_[4];
std::vector<__m256i> rnd1_[4];

uint64_t * dotproducts;
uint64_t dp_sanity[nqueries];      // DEMUX
uint64_t dp_sanity_recv[nqueries]; // DEMUX

uint64_t shift[nqueries][2];







profile<3 * (precision/2)> * iprofiles_reduced = (profile<3 * (precision/2)> *) std::aligned_alloc(sizeof(__m256i), nitems * sizeof(profile<3 * (precision/2)>));
profile<3 * (precision/2)> * uprofiles_reduced; 




constexpr uint64_t msb = (1ULL << 63);
 

auto startMPC = std::chrono::system_clock::now(); 

 
 


#include "4P-mux.h"
#include "sanitycheck.h"
#include "mux-sanitycheck.h"
#include "demux-sanitycheck.h"
#include "i-precision-reduction.h"
#include "u-precision-reduction.h"

void keep_polling(boost::asio::io_context& io_context)
{
  while(
         progress[step::user_reconstructed] < nusers || progress[step::item_reconstructed] < nitems || 
         progress[step::uprofile_out]       < nusers || progress[step::iprofile_out] < nitems  || 
         progress[step::iprofile_msbs_in] < total_bucket_elements || progress[step::precision_beaver_in] < total_bucket_elements

         )
  {
    io_context.reset();
    io_context.poll_one();
  } 
}



void read_sanity_beavers(tcp::socket & sin)
{
  bvr2_sanity = (beaver2_sanity *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver2_sanity));

  for(size_t i = 0; i < nqueries; ++i)
  {
    boost::asio::read(sin, boost::asio::buffer(&bvr2_sanity[i], beaver2_sanity::size));
    progress[step::bvr2_sanity_in] = i + 1;
  }
}

void read_beavers(AES_KEY &key, boost::asio::io_context & io_context, tcp::socket& seed_sin, tcp::socket & sin)
{

  bvr  = (beaver1 *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver1));

  bvr2 = (beaver2 *) std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(beaver2));
 
  __m128i seed;

  read(seed_sin, boost::asio::buffer(&seed, sizeof(__m128i)));

  
  #if(PARTY == 1)
      const size_t buflen = (nitems) * sizeof(profile<precision>);
      PRG(key, seed, (__m128i *)A, buflen / sizeof(__m128i));
      progress[step::mux_seed] = 1;
  #endif


  PRG(aeskey, seed, (__m128i *)bvr, nqueries * sizeof(beaver1) / sizeof(__m128i));

  for(size_t i = 0; i < nqueries; ++i)
  {
 
    boost::asio::read(sin, boost::asio::buffer(&bvr2[i], beaver2::size));
   
    progress[step::bvr2_in] = i + 1;
  }
}

 
 
void do_evalfull_parallel(size_t core)
{
 for(size_t i = core; i < nqueries; i += n_cores)
 {
   T[i] = (uint8_t*) malloc(nitems * sizeof(uint8_t));
   S[i] = (__m128i*) std::aligned_alloc(sizeof(__m128i), nitems * sizeof(__m128i));
   expanded_leaf[i] = (profile<3*precision> *) std::aligned_alloc(sizeof(__m256i), nitems * sizeof(profile<3*precision>));

  evalfull3(aeskey, queries[i].dpfkey, S[i], T[i]);
  evalfull_done[core] = evalfull_done[core] + 1;
  for(size_t k = 0; k < nitems; ++k)
  {
    PRG(aeskey, S[i][k], (__m128i *)&expanded_leaf[i][k], dim128);
  }
  
   leaf_expanded[core] = leaf_expanded[core] + 1;
  }
}

void update_progress1()
{
  int bound = 1, next_core = 0;
  while (progress[step::evalfull_done_] < nqueries)
  {
    while (evalfull_done[next_core] < bound) std::this_thread::yield();
    progress[step::evalfull_done_] = progress[step::evalfull_done_] + 1;
    bound = 1 + progress[step::evalfull_done_] / n_cores;
    next_core = progress[step::evalfull_done_] % n_cores;
  }
}

void update_progress2()
{
  int bound = 1, next_core = 0;
  while (progress[step::leaf_expanded_] < nqueries)
  {
    while (leaf_expanded[next_core] < bound) std::this_thread::yield();
    progress[step::leaf_expanded_] = progress[step::leaf_expanded_] + 1;
    bound = 1 + progress[step::leaf_expanded_] / n_cores;
    next_core = progress[step::leaf_expanded_] % n_cores;
  }
} 

void read_profiles(tcp::socket & sin)
{
  
  blinded_profile = (blind_vecs *) std::aligned_alloc(sizeof(__m256i),  nqueries * sizeof(blind_vecs));

  for (size_t i = 0; i < nqueries; ++i)
  {     
     read(sin, boost::asio::buffer(&blinded_profile[i], blind_vecs::size));
 
     progress[step::profiles_in] = i + 1;
  }
}

void blind_profiles()
{ 
  blinds = (blind_vecs *)std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(blind_vecs));

  for (size_t i = 0; i < nqueries; ++i)
  {
    while (
           progress[step::bvr2_in] < i + 1     ||
           progress[step::itm_mux_out] < i + 1  
           ) 
      { 
        std::this_thread::yield(); 
      }

 

    // blind user and item profile shares
    blinds[i] = {
                 uprofiles[queries[i].user] + bvr[i].usr,
                 itm[i] + bvr[i].itm
                };
    // send the blinded profile shares
 
    progress[step::profiles_gen] = i + 1;
    
  }
 
}





 void write_blinded_profiles(const size_t i, boost::asio::io_context& io_context, tcp::socket& sout)
 {

      while(progress[step::profiles_gen] < i + 1)
      { 
        std::this_thread::yield();
      }

     async_write(sout, boost::asio::buffer(&blinds[i], blind_vecs::size),
            [i, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                   
                   if(i + 1 < nqueries) 
                   {
                    write_blinded_profiles(i + 1, io_context, sout);
                   }
                }          
                else
                {                
                  write_blinded_profiles(i , io_context, sout);
                }
            
            }); 
 
 }


void read_norms(boost::asio::io_context & io_context, tcp::socket & sin)
{

  blinded_norm = (fixed_t<2*precision>*)std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(fixed_t<2*precision>));
 
  for (size_t i = 0; i < nqueries; ++i)
  {   
    
    read(sin, boost::asio::buffer(&blinded_norm[i], sizeof(fixed_t<2*precision>)));
    
    progress[step::norms_in] = i + 1;

  }

}

 void write_blinded_norms(const size_t i, boost::asio::io_context& io_context, tcp::socket& sout)
 {

    while(progress[step::norms_gen] < i + 1)
    { 
      std::this_thread::yield();
    }       

    async_write(sout, boost::asio::buffer(&blinded_norm_out[i], sizeof(fixed_t<2*precision>)),
           [i, &io_context, &sout](boost::system::error_code ec, std::size_t)             
    {       
        if(!ec)
        { 
           if(i + 1 < nqueries){
            write_blinded_norms(i + 1, io_context, sout);
           }
        }          
        else
        {
          write_blinded_norms(i , io_context, sout);
        }
    
    }); 
 
 }

void generate_norms()
{
  norm = (fixed_t<2*precision>*)std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(fixed_t<2*precision>));

  blinded_norm_out = (fixed_t<2*precision>*)std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(fixed_t<2*precision>));
 
  for (size_t i = 0; i < nqueries; ++i)
  {
    std::cout << i << "--> gen_norms" << std::endl;
    while (progress[step::profiles_in] < i + 1 || progress[step::profiles_gen] < i + 1) 
    { 
      //std::cout << "9\n"; 
      std::this_thread::yield(); 
    }
    
    // compute share of \pm(1-norm)   
     if(sanity_vector_demux[i] != 0 && sanity_vector_mux[i] != 0)
    {
    norm[i] = pm1[i] - bvr2[i].dp + dot(blinded_profile[i].usr, bvr[i].itm)
      - dot(uprofiles[queries[i].user], (itm[i] + blinded_profile[i].itm));
    }
 

    // blind \pm(1-norm) share
    blinded_norm_out[i] = norm[i] + bvr[i].dp; 
    // send blinded \pm(1-norm) share    
    progress[step::norms_gen] = i + 1;
 
  }
 
}




void read_cws(boost::asio::io_context & io_context, tcp::socket & sin)
{


  final_cw = (profile<3 * precision> *)std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(profile<3 * precision>));

 
  for (size_t i = 0; i < nqueries; ++i)
  {
    read(sin, boost::asio::buffer(&final_cw[i], sizeof(profile<3*precision>)));

    progress[step::cws_in] = i + 1;
  }
 
}

void generate_cws()
{ 

 final_cw2 = (profile<3 * precision> *)std::aligned_alloc(sizeof(__m256i), nqueries * sizeof(profile<3 * precision>));
 

  for (size_t i = 0; i < nqueries; ++i)
  {
    while (
            progress[step::norms_in]  < i + 1    || 
            progress[step::norms_gen] < i + 1    || 
            progress[step::dp_gen]    < i + 1    ||
            progress[step::demux_sanity_done] < i + 1
           ) 
    {
      //std::cout << "10\n"; 

 

     std::this_thread::yield(); 
    }

    // compute share of final_cw
   
    if( sanity_vector_mux[i] != 0 && sanity_vector_demux[i] != 0)
    {
     final_cw2[i] = queries[i].dpfkey.leaf
      + (norm[i] * (uprofiles[queries[i].user] + blinded_profile[i].usr)
      - (blinded_norm[i] * bvr[i].usr) + bvr2[i].usr);
    }
  

    progress[step::cws_gen] = i + 1; 
  }

  // //std::cout << "write_cws end" << std::endl;
}


 void write_cws(const size_t i, boost::asio::io_context& io_context, tcp::socket& sout)
 {

      while(progress[step::cws_gen] < i + 1)
      { 
        //std::cout << "11\n"; 
 
        std::this_thread::yield();
      }

       

     async_write(sout, boost::asio::buffer(&final_cw2[i], sizeof(profile<3*precision>)),
            [i, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {
                  if(i + 1 < nqueries) write_cws(i + 1, io_context, sout);
                }          
                else
                { 
                  write_cws(i , io_context, sout);
                }
            
            }); 

     progress[step::cws_out] = i + 1;
 }


void update_uprofiles()
{

  uprofiles_reduced = (profile<3 * (precision/2)> *) std::aligned_alloc(sizeof(__m256i), nusers * sizeof(profile<3 * (precision/2)>));
  memset(uprofiles_reduced, 0, nusers * sizeof(profile<3 * (precision/2)>));
  
  uprofiles2 = (profile<3*precision> * ) std::aligned_alloc(sizeof(__m256i), nusers * sizeof(profile<3 * precision>)); 
  memset(uprofiles2, 0, nusers * sizeof(profile<3*precision>));
  
  for (size_t i = 0; i < nusers; ++i)
  {
    uprofiles_reduced[i] = (profile<3 * (precision/2)>)uprofiles[i];
  }

  for (size_t i = 0; i < nqueries; ++i)
  {
    while (
           progress[step::norms_in]     < i + 1 ||
           progress[step::norms_gen]    < i + 1 || 
           progress[step::dp_gen] < i + 1       ||
           progress[step::mux_sanity_done] < i + 1 ||
           progress[step::demux_sanity_done] < i + 1
          ) 

      {
        //std::cout << "12\n"; 
 
        std::this_thread::yield(); 
      }
 

    uprofiles2[queries[i].user] +=  (norm[i] * (itm[i] + blinded_profile[i].itm)  - (blinded_norm[i] * bvr[i].itm) + bvr2[i].itm);
    
    if(sanity_vector_mux[i] == 1 && sanity_vector_demux[i] == 1)
    {
     uprofiles_reduced[queries[i].user] +=  profile<3 * (precision/2)>(uprofiles2[queries[i].user]);//    profile<3 * (precision/2)>(norm[i] * (itm[i] + blinded_profile[i].itm)  - (blinded_norm[i] * bvr[i].itm) + bvr2[i].itm);
    }
    else
    {
     std::cerr << "uprofile update skipped: " << i << " -> " << sanity_vector_mux[i] << " " << sanity_vector_demux[i] << std::endl;
    }
    
    progress[step::user_update] = 0;
  }
  
  progress[step::user_update] = nqueries;
  
    auto stopMPC = std::chrono::system_clock::now();
    auto durationMPC = duration_cast<milliseconds>(stopMPC - startMPC);
    std::cerr << "MPC time (uprofiles) = " << durationMPC.count() << std::endl; 

   

    #if(PARTY == 0)
    int fd_uprofile = open("./updated_uprofiles.0", O_WRONLY, O_CREAT);
    ssize_t bytes = write(fd_uprofile, uprofiles_reduced, nusers * sizeof(profile<3 * (precision/2)>));
    #endif

    #if(PARTY == 1)
    int fd_uprofile = open("./updated_uprofiles.1", O_WRONLY, O_CREAT);
    ssize_t bytes = write(fd_uprofile, uprofiles_reduced, nusers * sizeof(profile<3 * (precision/2)>));
    #endif

    close(fd_uprofile);


}


void write_iprofiles(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
  while(progress[step::item_update] < nqueries || progress[step::offset_iprofiles] < total_bucket_elements)
  {
    //std::cout << "13\n"; 
    std::this_thread::yield();
  } 

 
   async_write(sout, boost::asio::buffer(&iprofiles_reduced[j], sizeof(profile<3 * (precision/2)>)),
        [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
        
        {       
            if(!ec)
            {
              
               if(j + 1 < nitems)
               {
                write_iprofiles(j + 1, io_context, sout);
               }
            }          
            else
            {

              write_iprofiles(j , io_context, sout);
            }
        
        }); 

   progress[step::iprofile_out] = j + 1;

}

void reconstruct_iprofiles(tcp::socket& sin)
{

 
  iprofiles_reconstructed = (profile<3 * (precision/2)> *)std::aligned_alloc(sizeof(__m256i), nitems * sizeof(profile<3 * (precision/2)>));
  
  for(size_t i = 0; i < nitems; ++i)
  { 

    while(progress[step::item_update] < nqueries || progress[step::iprofile_msbs_in] < total_bucket_elements)
    {
      std::this_thread::yield();
    }
   
    read(sin, boost::asio::buffer(&iprofiles_reconstructed[i], sizeof(profile<3 * (precision/2)>)));
    
    iprofiles_reconstructed[i] += iprofiles_reduced[i];
    
    std::cout << "iprofiles_reconstructed[" << i << "] = " << iprofiles_reconstructed[i] << std::endl; // << " , norm = " << norm << std::endl;
 
    progress[step::item_reconstructed] = i + 1;
  }

  progress[step::item_reconstructed] = nitems;
}

void write_uprofiles(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
    while(progress[step::user_update] < nqueries || progress[step::offset_uprofiles] < nusers)
    {
      //std::cout << "14\n"; 
      std::this_thread::yield();
    }
   //  std::cerr << "uprofiles_reduced[" << j << "] = " << uprofiles_reduced[j] << std::endl;
     async_write(sout, boost::asio::buffer(&uprofiles_reduced[j], sizeof(profile<3 * (precision/2)>)),
          [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
          
          {       
              if(!ec)
              {
                
                 if(j + 1 < nusers)
                 {
                  write_uprofiles(j + 1, io_context, sout);
                 }
              }          
              else
              {

                write_uprofiles(j , io_context, sout);
              }
          
          }); 

   progress[step::uprofile_out] = j + 1;
}

void reconstruct_uprofiles(tcp::socket& sin)
{

  uprofiles_reconstructed = (profile<3 * (precision/2)> *)std::aligned_alloc(sizeof(__m256i), nusers * sizeof(profile<3 * (precision/2)>));
 
  for(size_t i = 0; i < nusers; ++i)
  {
    while(progress[step::user_update] < nqueries || progress[step::offset_uprofiles] < nusers)
    {
     // std::cout << "15\n"; 
      std::this_thread::yield();
    }
 
    read(sin, boost::asio::buffer(&uprofiles_reconstructed[i], sizeof(profile<3 * (precision/2)>)));
    uprofiles_reconstructed[i] += uprofiles_reduced[i];

  
    double norm = 0.0;

    for(size_t dim = 0; dim < DIM; ++dim)
    {
     norm += double(uprofiles_reconstructed[i].f64[dim]) * double(uprofiles_reconstructed[i].f64[dim]);  
    }
    std::cout << "uprofiles_reconstructed[" << i << "] = " << uprofiles_reconstructed[i] << " , norm = " << norm << std::endl;
    progress[step::user_reconstructed] = i + 1;
  }


  free(uprofiles_reconstructed);

}


 
 
void update_iprofiles(char* argv[])
{
  std::ofstream gradient_time; 
 
  gradient_time.open("gradient_time", std::fstream::app);
 
  memset(iprofiles_reduced, 0, nitems * sizeof(profile<3 * (precision/2)>));
   
  for(size_t i = 0; i < nitems; ++i) iprofiles_reduced[i] = profile<3 * (precision/2)>(iprofiles[i]);

  std::cerr << "update_iprofiles\n\n";
 
  iprofiles2 = (profile<3*precision> * ) std::aligned_alloc(sizeof(__m256i), nitems * sizeof(profile<3 * precision>));
 
  memset(iprofiles2, 0, nitems * sizeof(profile<3*precision>));

  size_t updated    = 0;
  size_t test = 0; 
  for(size_t j = 0; j < nqueries; ++j)
  {
       while( 
              progress[step::cws_in]  < j + 1 || progress[step::cws_gen]  < j + 1 ||  progress[step::evalfull_done_]  < j + 1 ||
              progress[step::leaf_expanded_] < j + 1 ||  progress[step::dp_gen] < j + 1  || progress[step::mux_sanity_done] < j + 1 || 
              progress[step::demux_sanity_done] < j + 1   
            ) 
       { 
         std::this_thread::yield(); 
       }
 

      std::cout  << "queries = " << j << std::endl;
       
     if(sanity_vector_mux[j] == 1 && sanity_vector_demux[j] == 1)
     {  
       dpf_demux<nitems,PARTY>(queries[j].dpfkey, expanded_leaf[j], final_cw[j] + final_cw2[j], iprofiles2, S[j], T[j]);
     }

    updated = updated + 1;

    if(updated % bucket_size_ == 0 || updated == nqueries)
    {
      size_t cur_bucket = progress[step::buckets_complete];

      for(size_t k = 0; k < nitems; ++k)
      { 
        iprofiles_reduced[k] += profile<3 * (precision/2)>(iprofiles2[k]); // precision reduction

        size_t indx = (cur_bucket * nitems) + k;
        assert(indx == test);

        for(size_t dim = 0; dim < DIM; ++dim)
         {            
           I_profile_msbs[indx][dim] = (iprofiles2[k].u64[dim] & msb) > 0; 
           arc4random_buf(&I_profile_msbs_this[indx][dim], sizeof(uint64_t));
           I_profile_msbs_written[indx][dim] = I_profile_msbs[indx][dim] - I_profile_msbs_this[indx][dim];    
         } 
        
        progress[step::i_msb_in] = test + 1; 

        test = test + 1;
   
        std::cout << "progress[step::i_msb_in] = " << progress[step::i_msb_in] << std::endl;
      }

 

      std::cout << "update done: " << j << std::endl;

      memset(iprofiles2, 0, nitems * sizeof(profile<3*precision>));

      progress[step::buckets_complete] =  progress[step::buckets_complete] + 1;
 
      std::cout << "progress[step::buckets_complete] = " << progress[step::buckets_complete] << std::endl;
    }
   
    free(S[j]);
    free(T[j]);
    free(expanded_leaf[j]);
    progress[step::item_update] = j + 1;
  
  }
  
 
  
    std::cout << "item_update done\n\n";
    #if(PARTY == 0)
    int fd_iprofile = open("./updated_iprofiles.0", O_WRONLY, O_CREAT);
    //bytes = write(fd0, &nusers, sizeof(size_t));
    ssize_t bytes = write(fd_iprofile, iprofiles_reduced, nitems * sizeof(profile<3 * (precision/2)>));
    #endif

   #if(PARTY == 1)
    int fd_iprofile = open("./updated_iprofiles.1", O_WRONLY, O_CREAT);
    //bytes = write(fd0, &nusers, sizeof(size_t));
    ssize_t bytes = write(fd_iprofile, iprofiles_reduced, nitems * sizeof(profile<3 * (precision/2)>));
    #endif


  progress[step::item_update] = nqueries;
  
 
 
  auto stopMPC = std::chrono::system_clock::now();
  auto durationMPC = duration_cast<milliseconds>(stopMPC - startMPC);
  
  gradient_time << durationMPC.count() << std::endl;

   std::cerr << "MPC time (iprofiles) = " << durationMPC.count() << std::endl; 

  free(iprofiles2);
  free(final_cw);
  free(final_cw2);
}

 
 




int main(int argc, char * argv[])
{
   std::cerr << "n_cores        = " << n_cores << std::endl;
   std::cerr << "bucket_size_   = " << bucket_size_ << std::endl;
   std::cerr << "n_buckets      = " << n_buckets << std::endl;
   std::cerr << "last_buck_size = " << last_buck_size << std::endl;
   for(size_t j = 0; j < nqueries; ++j)
   {
    sanity_vector_demux[j] = 2;
    sanity_vector_mux[j] = 2;
   }   

 
  
  S = (__m128i**)malloc(nqueries * sizeof(__m128i*));
  T = (uint8_t**)malloc(nqueries * sizeof(uint8_t*));
  expanded_leaf = (profile<3*precision> **) malloc(nqueries * sizeof(profile<3*precision> *));
  
 
  
  AES_set_encrypt_key(_mm_set_epi64x(597349, 121379), &aeskey);

  std::string qfile = std::string("./queries.") + std::to_string(PARTY);
  std::string ufile = std::string("./uprofiles.") + std::to_string(PARTY);
  std::string ifile = std::string("./iprofiles.") + std::to_string(PARTY);
 

  // memory map the query shares
  int qfd = open(qfile.c_str(), O_RDWR);
  queries = (query<nitems> *) mmap(NULL, nqueries * sizeof(query<nitems>),
    PROT_READ, MAP_PRIVATE, qfd, 0);

  // memory map the user profile shares
  int ufd = open(ufile.c_str(), O_RDWR);
  uprofiles = (profile<precision> *) mmap(NULL,
    nusers * sizeof(profile<precision>), PROT_READ, MAP_PRIVATE, ufd, 0);

  // memory map the item profiles [SHARES]
  int ifd = open(ifile.c_str(), O_RDWR);
  iprofiles = (profile<precision> *) mmap(NULL,
    nitems * sizeof(profile<precision>), PROT_READ, MAP_PRIVATE, ifd, 0);





  try
  {
   
  const std::string host1 = (argc < 2) ? "127.0.0.1" : argv[1];
  const std::string host2 = (argc < 3) ? "127.0.0.1" : argv[2];
  const std::string host3 = (argc < 4) ? "127.0.0.1" : argv[3];

  #include "connections.h"
      
      std::thread poller(keep_polling, std::ref(io_context));
      
      std::thread progress_updater1(update_progress1);  
      std::thread progress_updater2(update_progress2); 
 
      std::vector<std::thread> evaluater(n_cores);
      std::mutex iomutex;
      for(size_t j = 0; j < n_cores; ++j)
      {
        std::cout << "core number = " << j << std::endl;
 
        evaluater[j] =  std::thread(do_evalfull_parallel, j);
 
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(j, &cpuset);
        int rc = pthread_setaffinity_np(evaluater[j].native_handle(),  sizeof(cpu_set_t), &cpuset);
       
          if (rc != 0) 
          {
            std::cerr << "Error calling pthread_setaffinity_np: " << j << " --> " << rc << "\n";
          }
      }



      std::thread sanity_seed_reader(read_sanity_seeds, std::ref(aeskey), std::ref(s2_a));
    
      std::thread shift_reader(read_shifts, std::ref(s3_a));
    
      std::thread dp_generator(generate_dotproducts);

      std::thread dp_to_P2_writer(send_dp_to_P2, 0,  std::ref(io_context), std::ref(s2_b));
      

      std::thread precision_beaver_generator(gen_precision_blinds,  std::ref(aeskey), std::ref(s2_h));

      std::thread precision_reduction_beaver_reader(read_precision_reduction_bvrs, std::ref(s2_f));
      
      std::thread U_precision_reduction_beaver_reader(U_read_precision_reduction_bvrs, std::ref(aeskey), std::ref(s2_g));

   
      std::thread I_profile_msbs_blinder(blind_I_profile_msbs);
      std::thread blinded_I_msb_writer(write_blinded_I_profile_msbs, 0, std::ref(io_context), std::ref(sb_k));
      std::thread blinded_I_msb_reader(read_blinded_I_profile_msbs, std::ref(sb_k));  
   
      std::thread U_profile_msbs_blinder(blind_U_profile_msbs);
      std::thread blinded_U_msb_writer(write_blinded_U_profile_msbs, 0, std::ref(io_context), std::ref(sb_n));
      std::thread blinded_U_msb_reader(read_blinded_U_profile_msbs, std::ref(sb_n));  


      std::thread msb_prod_generator(gen_msb_prod);
      
      std::thread U_msb_prod_generator(U_gen_msb_prod); 
      
      std::thread U_offset_adjuster(U_adjust_offset);

      std::thread msb_prod_writer(write_msb_prod, 0, std::ref(io_context), std::ref(sb_l));
      std::thread msb_prod_reader(read_msb_prod, std::ref(sb_l));

      std::thread U_msb_prod_writer(U_write_msb_prod, 0, std::ref(io_context), std::ref(sb_o));
      std::thread U_msb_prod_reader(U_read_msb_prod, std::ref(sb_o));


      std::thread offset_adjuster(adjust_offset);
      
      std::thread mux_verification_reader(read_mux_verification, std::ref(s2_d));

      std::thread demux_sanity_generator(demux_sanity_check, std::ref(aeskey));
 

      std::thread demux_sanity_dp_writer(write_demux_sanity_dp, 0, std::ref(io_context), std::ref(sb_h));
      std::thread demux_sanity_dp_reader(read_demux_sanity_dp, std::ref(sb_h)); 
      std::thread demux_sanity_verifier(demux_verify);

      std::thread beaver_reader(read_beavers, std::ref(aeskey), std::ref(io_context), std::ref(s2_e), std::ref(s2));
      std::thread sanity_beaver_reader(read_sanity_beavers, std::ref(s2_c));
     
      std::thread mux_bvr_reader(read_mux_bvr, std::ref(aeskey),  std::ref(io_context), std::ref(s3_b), std::ref(s3));
      
      std::thread blinded_items_reader(read_blinded_items,  std::ref(io_context), std::ref(sb));
      std::thread blinded_items_generator(generate_blinded_items);    
      std::thread blinded_items_writer(write_blinded_items, 0, std::ref(io_context), std::ref(sb)); 
      std::thread mux_norm_writer(write_mux_norm, 0, std::ref(io_context), std::ref(sb_a));
  

      std::thread blinded_profiles_reader(read_profiles, std::ref(sb_c));
      std::thread profiles_blinder(blind_profiles);
      std::thread blinded_profiles_writer(write_blinded_profiles, 0, std::ref(io_context),  std::ref(sb_c));
    
      // exchange blinded \pm(1-norm) shares
      std::thread blinded_norms_reader(read_norms , std::ref(io_context), std::ref(sb_d));
      std::thread norms_blinder(generate_norms);
      std::thread blinded_norms_writer(write_blinded_norms, 0, std::ref(io_context), std::ref(sb_d));
   
      std::thread uprofile_updater(update_uprofiles);
 

      std::thread uprofile_writer(write_uprofiles, 0, std::ref(io_context), std::ref(sb_f));
      std::thread uprofile_reconstructor(reconstruct_uprofiles, std::ref(sb_f));

      // exchange dpf final cws
      std::thread cws_generator(generate_cws);
      std::thread cws_reader(read_cws , std::ref(io_context), std::ref(sb_e));
      std::thread cws_writer(write_cws, 0, std::ref(io_context), std::ref(sb_e));
      std::thread iprofile_updater(update_iprofiles, argv);
      
 
      std::thread U_profile_msbs_generator(U_gen_msbs); 

      std::thread I_profile_msbs_writer(write_I_profile_msbs, 0, std::ref(io_context), std::ref(sb_j));
      std::thread I_profile_msbs_reader(read_I_profile_msbs,  std::ref(sb_j));
    
      std::thread U_profile_msbs_writer(write_U_profile_msbs, 0, std::ref(io_context), std::ref(sb_m));
      std::thread U_profile_msbs_reader(read_U_profile_msbs,  std::ref(sb_m ));
       

      std::thread iprofile_writer(write_iprofiles, 0, std::ref(io_context), std::ref(sb_g));
      std::thread iprofile_reconstructor(reconstruct_iprofiles, std::ref(sb_g));
 

      poller.join();
      
      progress_updater1.join();  // updating the progress variables for parallel evalfull
      progress_updater2.join();  // updating the progress variables for parallel evalfull
 
     
      for(size_t j = 0; j < n_cores; ++j)
      {
        evaluater[j].join();   // parallel evalfull
      }

      sanity_seed_reader.join();
      shift_reader.join();
      dp_generator.join();

      precision_beaver_generator.join();
      precision_reduction_beaver_reader.join();
      U_precision_reduction_beaver_reader.join();
      
      I_profile_msbs_blinder.join();
      blinded_I_msb_writer.join();
      blinded_I_msb_reader.join();

      U_profile_msbs_blinder.join();
      blinded_U_msb_writer.join();
      blinded_U_msb_reader.join();

      msb_prod_generator.join();
      
      msb_prod_writer.join();
      msb_prod_reader.join();
      
      U_msb_prod_writer.join();
      U_msb_prod_reader.join();

      offset_adjuster.join();
      dp_to_P2_writer.join();

      mux_verification_reader.join();
      demux_sanity_generator.join();
      demux_sanity_dp_writer.join();
      demux_sanity_dp_reader.join();
      demux_sanity_verifier.join();

      beaver_reader.join(); 
      sanity_beaver_reader.join();
 
      mux_bvr_reader.join();

      blinded_items_reader.join();
      blinded_items_generator.join();
      blinded_items_writer.join();
      mux_norm_writer.join();

      blinded_profiles_reader.join();
      profiles_blinder.join();
      blinded_profiles_writer.join();
  
      norms_blinder.join();
      blinded_norms_writer.join();
      blinded_norms_reader.join();
   
      uprofile_updater.join(); 

      uprofile_writer.join();      
      uprofile_reconstructor.join();
      
      cws_generator.join();
      cws_reader.join();    
      cws_writer.join();
      iprofile_updater.join();
     
      U_profile_msbs_generator.join();
      I_profile_msbs_writer.join();
      I_profile_msbs_reader.join();
      
      U_profile_msbs_writer.join();
      U_profile_msbs_reader.join();
      U_msb_prod_generator.join();
      U_offset_adjuster.join();

      iprofile_writer.join();
      iprofile_reconstructor.join();

    
  }
  catch (std::exception & e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  munmap(uprofiles, nusers * sizeof(profile<precision>));
  close(ufd);
  // munmap(iprofiles_R, nitems * sizeof(profile<precision>));
  // close(ifd_R);
  munmap(iprofiles, nitems * sizeof(profile<precision>));
  close(ifd);
  munmap(queries, nqueries * sizeof(query<nitems>));
  close(qfd);

  return 0;
}