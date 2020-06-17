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
 
 #include "dpf.h"
 
#include "sanitycheck.h"
 
using boost::asio::ip::tcp;



AES_KEY prgkey;

 
template<size_t nitems> struct query { size_t user; dpf_key<nitems> dpfkey; };
query<nitems> * queries;
query<nitems> * queries_cw;


__m128i seed_sanity;
uint64_t * t_sanity_0;
uint64_t * t_sanity_1; 

int * found_vec;

__m128i ** s;
uint8_t ** t;

profile<precision> *A;

fixed_t<2*precision>  pm1;

  p3_beavers * bvrs0;
  p3_beavers * bvrs1;

enum p3_step
{
  beaver_generated = 0,
  beaver_out0,
  beaver_out1,
  shift0_out,
  shift1_out,
  num_steps
};

size_t p3_progress[p3_step::num_steps] = { 0 };

 void keep_polling(boost::asio::io_context& io_context)
{
  while( p3_progress[p3_step::beaver_out0] < nqueries || p3_progress[p3_step::beaver_out1] < nqueries
        || p3_progress[p3_step::shift0_out] < nqueries || p3_progress[p3_step::shift1_out] < nqueries
        )
  {
    io_context.reset();
    io_context.poll();
  } 
}

 void generate_beavers(AES_KEY & key)
 {
  arc4random_buf(&seed_sanity, sizeof(seed_sanity));
 

  std::cout << "seed_sanity: " << seed_sanity[0] << " " << seed_sanity[1] << std::endl;

  if(posix_memalign((void**)&A, sizeof(__m256i), nitems * sizeof(profile<precision>)) != 0)
  {
    throw std::runtime_error("Memory allocation failed");
  }
    
  size_t buflen = (nitems) * sizeof(profile<precision>);
    
  PRG(key, seed_sanity, (__m128i *)A, buflen / sizeof(__m128i)); 

  
  buflen = sizeof(uint64_t) * nitems;  

  arc4random_buf(bvrs0, (nqueries) * sizeof(p3_beavers));
  arc4random_buf(bvrs1, (nqueries) * sizeof(p3_beavers));

  s = (__m128i**)malloc(nqueries * sizeof(__m128i*));
  t = (uint8_t**)malloc(nqueries * sizeof(uint8_t*));
          
  profile<3*precision>  tCW;
  profile<3*precision>  zero_profile;  
  for(size_t i = 0; i < DIM; ++i) zero_profile.u64[i] = 0;
  

  for(size_t i = 0; i < nqueries; ++i)
  {

    t[i] = (uint8_t*)malloc(nitems * sizeof(uint8_t));

    if(posix_memalign((void**)&s[i], sizeof(__m128i), nitems * sizeof(__m128i)))
      {
        throw std::runtime_error("posix_memalign failed");
      }

    dpf_mux<nitems,0>(key, queries[i].dpfkey, A, bvrs1[i].A_m, s[i], t[i], pm1);
    
    profile<precision>  one_profile;

      for(size_t dim = 0; dim < DIM; ++dim) one_profile.u64[dim] = 1;
     
      uint64_t RRtCW = 0;

      for(size_t j = 0; j < nitems; ++j)
      {
        
        profile<3*precision> tmp; 
        
        PRG(key, s[i][j], (__m128i *)&tmp, dim128);
        
        if(t[i][j])  tCW  = queries_cw[i].dpfkey.leaf;
        if(!t[i][j]) tCW  = zero_profile; //

        #ifdef VERBOSE
        std::cout << j << " tCW = "  << tmp << " - 2 . " << tCW[j] << " = " << tmp - tCW[j] - tCW[j] << std::endl;  
        #endif

        tCW = tmp - tCW - tCW;

        for(size_t dim = 0; dim < DIM; ++dim) RRtCW += tCW.u64[dim];
      }
      
      free(s[i]); free(t[i]);

      bvrs1[i].RRtCW = RRtCW;// + bvrs0[i].sanity_blinds;
      
      #ifdef VERBOSE
      std::cout << "RRtCW[" << i << "] = " << RRtCW[i] << std::endl;
      #endif      

      p3_progress[p3_step::beaver_generated] = i + 1;
    }
 }

void write_beaver_to_P0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout_seed,  tcp::socket& sout)
{ 
    while( p3_progress[p3_step::beaver_generated] < j + 1) 
    { 
        std::this_thread::yield(); 
    }
    
    if(j == 0)
    {  
     std::cout << "writing to P0: " << seed_sanity[0] << " " << seed_sanity[1] << std::endl;
     write(sout_seed, boost::asio::buffer(&seed_sanity, sizeof(__m128i)));
    }
 
    async_write(sout, boost::asio::buffer(&bvrs0[j], p3_beavers::size),
            [j, &io_context, &sout, &sout_seed](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                   
                   if(j + 1 < nqueries) 
                   {
                    write_beaver_to_P0(j + 1, io_context, sout_seed,  sout);
                   }
                }          
                else
                {                
                  write_beaver_to_P0(j , io_context, sout_seed, sout);
                }
            
            }); 

    p3_progress[p3_step::beaver_out0] = j + 1;
 

} 


void write_beaver_to_P1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout_seed, tcp::socket& sout)
{

    while( p3_progress[p3_step::beaver_generated] < j + 1) 
    { 
        std::this_thread::yield(); 
    }
   
     if(j == 0)
    {  
     std::cout << "writing to P1: " << seed_sanity[0] << " " << seed_sanity[1] << std::endl;
     write(sout_seed, boost::asio::buffer(&seed_sanity, sizeof(__m128i)));
    }

    async_write(sout, boost::asio::buffer(&bvrs1[j], p3_beavers::size),
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

    p3_progress[p3_step::beaver_out1] = j + 1;
 
}

uint64_t shift[nqueries][2];
void write_shits_to_P0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
 
  async_write(sout, boost::asio::buffer(shift[j],  2* sizeof(uint64_t)),
    [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
    
    { 
        if(!ec)
        {
           if(j + 1 < nqueries){ 
            write_shits_to_P0(j + 1, io_context, sout);
          }
        }          
        else
        {
          write_shits_to_P0(j , io_context, sout);
        }
    
    }); 

 
    p3_progress[p3_step::shift0_out] = j + 1; 
}

void write_shits_to_P1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
 
  async_write(sout, boost::asio::buffer(shift[j],  2* sizeof(uint64_t)),
    [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
    
    { 
        if(!ec)
        {
           if(j + 1 < nqueries){ 
            write_shits_to_P1(j + 1, io_context, sout);
          }
        }          
        else
        {
          write_shits_to_P1(j , io_context, sout);
        }
    
    }); 

 
    p3_progress[p3_step::shift1_out] = j + 1; 
}

int main(int argc, char* argv[])
{ 
     size_t len0 = pad_to_multiple(std::ceil(std::sqrt(nitems)), sizeof(__m256i)/sizeof(uint64_t));
    size_t len1 = pad_to_multiple(std::ceil(double(nitems) / len0), sizeof(__m256i)/sizeof(uint64_t));
     for(size_t j = 0; j < nqueries; ++j)
    {
     arc4random_buf(shift[j], 2 * sizeof(uint64_t));
     shift[j][0] = shift[j][0] % len0;
     shift[j][1] = shift[j][1] % len1;
    }
  AES_KEY key;
  AES_set_encrypt_key(_mm_set_epi64x(597349, 121379), &key);

    std::string qfile = std::string("./queries.1");
  std::string qfile_cw = std::string("./queries.0");
  // memory map the query shares
  int qfd = open(qfile.c_str(), O_RDWR);
  queries = (query<nitems> *) mmap(NULL, nqueries * sizeof(query<nitems>),
    PROT_READ, MAP_PRIVATE, qfd, 0);

    int qfd_cw = open(qfile_cw.c_str(), O_RDWR);
  queries_cw = (query<nitems> *) mmap(NULL, nqueries * sizeof(query<nitems>),
    PROT_READ, MAP_PRIVATE, qfd_cw, 0);

  boost::asio::io_context io_context;
    
  tcp::resolver resolver(io_context);
    
  const std::string host3 = (argc < 2) ? "127.0.0.1" : argv[1];



  tcp::acceptor acceptor1(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P3));
  std::cout << " --- > " << std::endl;
  tcp::socket s1(acceptor1.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P3 << std::endl;

  tcp::acceptor acceptor1_a(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P3_a));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_a(acceptor1_a.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P3 << std::endl;

  tcp::acceptor acceptor1_b(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P3_b));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_b(acceptor1_b.accept());
  std::cerr << "Listenting on port: " << PORT_P1_P3_b << std::endl;

  tcp::acceptor acceptor0(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P3));
  std::cout << " --- > " << std::endl;
  tcp::socket s0(acceptor0.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P3 << std::endl;

  tcp::acceptor acceptor0_a(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P3_a));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_a(acceptor0_a.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P3_a << std::endl;

   tcp::acceptor acceptor0_b(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P3_b));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_b(acceptor0_b.accept());
  std::cerr << "Listenting on port: " << PORT_P0_P3_b << std::endl;

  tcp::socket s2(io_context);
  boost::asio::connect(s2, resolver.resolve({host3,
  std::to_string(PORT_P3_P2)}));
  std::cout << "P0: [Established connection P3]" << std::endl;
  usleep(20000);
 

  try
  {
  size_t buflen = (nqueries) * sizeof(p3_beavers);
  if(
       posix_memalign((void**)&bvrs0, sizeof(__m256i), buflen) != 0
    || posix_memalign((void**)&bvrs1, sizeof(__m256i), buflen)
    )
  {
    throw std::runtime_error("Memory allocation failed");
  }

  std::thread poller(keep_polling, std::ref(io_context));
  std::thread beaver_generator(generate_beavers, std::ref(key));
 
  std::thread beaver_to_P0_writer(write_beaver_to_P0, 0, std::ref(io_context), std::ref(s0_b),  std::ref(s0)); 
  std::thread beaver_to_P1_writer(write_beaver_to_P1, 0, std::ref(io_context),  std::ref(s1_b), std::ref(s1));
  
  std::thread shifts_to_P0_writer(write_shits_to_P0, 0, std::ref(io_context), std::ref(s0_a)); 
  std::thread shifts_to_P1_writer(write_shits_to_P1, 0, std::ref(io_context), std::ref(s1_a));

  poller.join();
  beaver_generator.join();
  beaver_to_P0_writer.join(); 
  beaver_to_P1_writer.join();
  shifts_to_P0_writer.join();
  shifts_to_P1_writer.join();
  
 
  }

  catch (std::exception & e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
