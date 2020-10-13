#include <thread>
#include <iostream>
#include <chrono>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/ssl.hpp>
#include<vector>
#include <cmath>
#include<fstream>
#include <chrono>
#include "aes.h"
#include "network.h"
 

 #include "dpf.h"

 using namespace dpf;

typedef bool leaf_t;
typedef block<__m128i> node_t;
typedef AES_KEY prgkey_t;



using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;
using namespace std::chrono;


 
 std::vector<dpf_key<>> keys_recv;
 std::vector<dpf_key<>> keys_b;
 std::vector<dpf_key<>> keys_other;


 #include "compare.h"
 
 enum comparison_step
 {
  dpf_gen = 0,
  zero_blinded_in,
  sq_blinded_0_in,
  sq_blinded_1_in,
  carry_dpfs_gen,
  carry_dpfs_in,
  carry_dpf_other_out,
  carry_dpf_b_out,
  hi_dpf_out,
  lo_dpf_out,
  carry_evals,
  hi_evals,
  lo_evals,
  hi_dpfs_in,
  lo_dpfs_in,
  hi_dpf_gen,
  lo_dpf_gen,
  dpfs_in, 
  a_hi_and_lo_in,
  hi_and_lo_in,
  eval,
  partial_sum_in,
  comparison_num_steps,
};

size_t comparison_progress[comparison_step::comparison_num_steps] = { 0 };

 
  __m128i seed_;
  __m128i seed2;

  uint8_t sq_alpha[nrecords];
  uint64_t zero_alpha[nrecords];

  uint8_t sq_blinds[3][nrecords];
  uint64_t zero_blinds[nrecords];

  uint8_t squared[nrecords];

  uint8_t  blinded_sq_sent0[nrecords];
  uint8_t  blinded_sq_sent1[nrecords];
  uint64_t blinded_zero_sent0[nrecords];
  uint8_t  blinded_zero_sent1[nrecords];

  uint8_t  blinded_sq_recv0[nrecords];
  uint8_t  blinded_sq_recv1[nrecords];
  uint64_t blinded_zero_recv0[nrecords];
  uint8_t  blinded_zero_recv1[nrecords];

  uint64_t profiles[nrecords + 1][DIM];
  uint64_t blinded_profiles[nrecords + 1][DIM];
  uint64_t blinds[nrecords][DIM];

  uint64_t lo[nrecords]; // hi bits
  uint64_t hi[nrecords]; // low bits

  uint64_t a_lo[nprefixes/2], a_hi[nprefixes/2];

  uint64_t a;
  uint64_t b[nrecords];
  uint64_t blinded_b[nrecords];
  uint64_t blinded_b_recv[nrecords];
  uint64_t prefixbit1[nrecords] = {0}; 

  uint64_t * query_recv[nrecords]; 

  double randomnessP2[nrecords];
  uint64_t randomnessP264[nrecords];
   
  double r[nitems];
  double R_[nrecords]; // R is the partial sum
  uint64_t R64[nrecords];
  uint64_t carry[nprefixes];
   
  uint64_t c[nprefixes];

  uint64_t lo_diff[nrecords];
  uint64_t prefixbit[nrecords]; 

  bool prefix_rand[nprefixes];

  int64_t squared_evals[nrecords];
  uint64_t uv[nrecords];
  int8_t eval_vector[nrecords];

 #include "zero-out.h"

void keep_polling(boost::asio::io_context& io_context)
{

   while ( 

           comparison_progress[comparison_step::carry_dpf_other_out]    < nprefixes/2    ||
           comparison_progress[comparison_step::hi_dpf_out]             < nprefixes/2    ||   
           comparison_progress[comparison_step::carry_dpfs_gen]         < nprefixes/2    || 
           comparison_progress[comparison_step::sq_blinded_0_in]        < nrecords       ||
           comparison_progress[comparison_step::zero_blinded_in]        < nrecords       ||
           comparison_progress[comparison_step::carry_dpfs_in]          < nprefixes/2    || 
           comparison_progress[comparison_step::hi_dpf_gen]             < nprefixes/2    ||
           comparison_progress[comparison_step::hi_dpfs_in]             < nprefixes/2    ||
           comparison_progress[comparison_step::lo_dpfs_in]             < nrecords/2     ||
           comparison_progress[comparison_step::lo_dpf_gen]             < nrecords/2     ||   
           comparison_progress[comparison_step::carry_evals]            < nprefixes/2    ||  
           comparison_progress[comparison_step::hi_evals]               < nprefixes/2    ||
           comparison_progress[comparison_step::lo_evals]               < nrecords/2         
        )
  {
    io_context.reset();
    io_context.poll();
  }
}


 

 void get_profiles(tcp::socket &sin)
 {
    for(size_t j = 0; j < nrecords; ++j)
    {
      read(sin, boost::asio::buffer(&profiles[j], DIM * sizeof(uint64_t)));

      for(size_t dim = 0; dim < DIM; ++dim) 
      { 
        blinded_profiles[j][dim] = profiles[j][dim] + blinds[j][dim];
      }
    }
 }

  
 void blind_b()
 {
  for(size_t j = 0; j < nrecords; ++j)
  {
    blinded_b[j] = b[j] + blinds[j][0];
  }
 }


 void write_blinded_b(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
 {
           async_write(sout, boost::asio::buffer(&blinded_b[j], sizeof(uint64_t)),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {

                   if(j + 1 < nrecords) 
                   {
                    std::cout << "send_lo_dpfs_to_P_other - " << j << std::endl;                   
                    write_blinded_b(j + 1, io_context, sout);
                   }
                }          
                else
                {               
                  write_blinded_b(j , io_context, sout);
                }
            
            }); 
 }

 void recv_blinded_b(tcp::socket& sin)
 {

    for(size_t j = 0; j < nrecords; ++j)
    {
      read(sin, boost::asio::buffer(&blinded_b_recv[j], sizeof(uint64_t)));
    }
 }

 void multiply_b()
 {
  for(size_t j = 0; j < nrecords; ++j)
  {
    b[j] = b[j] * (blinded_b_recv[j]);
  }
 }

 void generate_partial_sum()
 {

  

   for(size_t j = 0; j < nrecords; ++j)
   {
     double r_ = ((double) rand() / (RAND_MAX));
     r[j] = r_;
   }
   


    for(size_t j = 0; j < nrecords; ++j)
    { 
      for(size_t i = 0; i < j; ++i)
      { 
        R_[j] += r[i]; 
      }
    }

    for(size_t j = 0; j < nrecords; ++j)
    {
      R64[j] = R_[j] * scale2; 

      comparison_progress[comparison_step::partial_sum_in] = j + 1;
    }
 }



 // Extract hi and lo bits of "a" and "b[nprefixes]"
 void extract_hi_and_lo_bits()
 {

    size_t t = 0;
    for(size_t prefixlen = 0; prefixlen < nprefixes; ++prefixlen)
    {

       #if(PARTY  == 0)
         if(prefixlen % 2 == 0) continue;
        #endif

        #if(PARTY  == 1)
         if(prefixlen % 2 == 1) continue;
        #endif
     
      a_hi[t] = extract_bits(a, 2*precision+intpart-prefixlen, 2*precision+intpart);
      a_lo[t] = extract_bits(a, 2*precision-fracpart, 2*precision+intpart-prefixlen);
   //   std::cout << "a = " << a << std::endl;
    //  std::cout << "a_lo[" << t << "] = " << a_lo[t] << std::endl;

      comparison_progress[comparison_step::a_hi_and_lo_in] = t + 1;

      ++t;
    }

   for(size_t j = 0; j < nrecords; ++j)
   {
    
     size_t prefixlen =  std::ceil(std::log2(j)); 
     hi[j] = extract_bits(b[j], 2*precision+intpart-prefixlen, 2*precision+intpart);
     lo[j] = extract_bits(b[j], 2*precision-fracpart, 2*precision+intpart-prefixlen);

     comparison_progress[comparison_step::hi_and_lo_in] = j + 1;
   }
 }

 

 void read_randomness_from_P2(tcp::socket& sin)
 {
    for(size_t j = 0; j < nrecords; ++j)
    {
        
        while(comparison_progress[comparison_step::partial_sum_in] < j + 1)
        {
          std::this_thread::yield();
        }
       read(sin, boost::asio::buffer(&randomnessP2[j], sizeof(randomnessP2[j])));
       randomnessP264[j] = randomnessP2[j] * (R64[j]); 
       //std::cout << "randomnessP2[" << j << "] = " << randomnessP2[j] << std::endl;
    }
 }
  

 
 


     // P1 generates DPF keys -- 
     void generate_carry_dpfs(AES_KEY& prgkey)
     {
        __m256i *query0, *query2;


        size_t t = 0;

        for(size_t prefixlen = 0; prefixlen < nprefixes; ++prefixlen)
        {
         
          #if(PARTY  == 0)
            if(prefixlen % 2 == 0) continue;
          #endif

          #if(PARTY  == 1)
            if(prefixlen % 2 == 1) continue;
          #endif

          size_t carrydpf_nitems = 1ULL << (intpart+fracpart-prefixlen+1);
     
          auto [carrydpf0, carrydpf2] = dpf_key<>::gen(prgkey, carrydpf_nitems, a_lo[prefixlen]);
          

          // carrydpf0 is the dpf sent to P0  
          carry_dpf_written_b[t].dpfnitems = carrydpf0.nitems;
          carry_dpf_written_b[t].dpfdepth = carrydpf0.depth();
          carry_dpf_written_b[t].root = carrydpf0.root;
          carry_dpf_written_b[t].finalizer = carrydpf0.finalizer;


          //carrydpf2 is the dpf sent to P2
          carry_dpf_written_other[t].dpfnitems = carrydpf2.nitems;
          carry_dpf_written_other[t].dpfdepth = carrydpf2.depth();
          carry_dpf_written_other[t].root = carrydpf2.root;
          carry_dpf_written_other[t].finalizer = carrydpf2.finalizer;
     
     
          for(size_t j = 0; j < carrydpf2.depth(); ++j)
          {
            carry_dpf_written_other[t].cw[j] = carrydpf2.cw[j];
          }

     
          comparison_progress[comparison_step::carry_dpfs_gen] = t + 1;

          ++t;
        }
     }



 
    void send_carry_dpfs_to_P_other(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
    {
  
           while(comparison_progress[comparison_step::carry_dpfs_gen] < j + 1) 
            {
              std::this_thread::yield(); 
            }
            


            std::cout << " ---> send_carry_dpfs_to_P_other \n\n";
 

           async_write(sout, boost::asio::buffer(&carry_dpf_written_other[j], sizeof(carry_dpf_written_other[j])),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {

                   if(j + 1 < nprefixes/2) 
                   {
                    std::cout << "send_lo_dpfs_to_P_other - " << j << " " << nprefixes/2 << std::endl;                   
                    send_carry_dpfs_to_P_other(j + 1, io_context, sout);
                   }
                }          
                else
                {               
                  send_carry_dpfs_to_P_other(j , io_context, sout);
                }
            
            }); 


            comparison_progress[comparison_step::carry_dpf_other_out] = j + 1;
            
 
    } 


 
    void send_carry_dpfs_to_P_b(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
    {
 
         while(comparison_progress[comparison_step::carry_dpfs_gen] < j + 1) 
         {
          std::this_thread::yield(); 
         }

 
         
           async_write(sout, boost::asio::buffer(&carry_dpf_written_b[j], sizeof(carry_dpf_written_b[j])),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                    
                 if(j + 1 < nprefixes/2) 
                 {
                  
                 // std::cout << "send_carry_dpfs_to_P_b: " << j << std::endl;

                  send_carry_dpfs_to_P_b(j + 1, io_context, sout);
                 }
                }          
                else
                {               
                  send_carry_dpfs_to_P_b(j , io_context, sout);
                }
            
            }); 

           comparison_progress[comparison_step::carry_dpf_b_out] = j + 1;
 
    } 
    
    void read_carry_dpfs_from_P_b(tcp::socket &sin)
    {
        for(size_t j = 0; j < nprefixes/2; ++j)
        {
          //std::cout << "read_carry_dpfs_from_P_b: " << j << std::endl;
          read(sin, boost::asio::buffer(&carry_dpf_read_b[j], sizeof(carry_dpf_read_b[j])));
          comparison_progress[comparison_step::carry_dpfs_in] = j + 1;
        }

        std::cout << "read carry_dpfs_from_Pb sender done\n";
     }
      

    void generate_hi_dpfs(AES_KEY& prgkey)
    {
      uint64_t alpha0, alpha2;
      arc4random_buf(&alpha0, sizeof(alpha0));
      arc4random_buf(&alpha2, sizeof(alpha2));
      

      size_t t = 0;
      for(size_t prefixlen = 0; prefixlen < nprefixes; ++prefixlen)
      {

          #if(PARTY  == 0)
           if(prefixlen % 2 == 0) continue;
          #endif

          #if(PARTY  == 1)
           if(prefixlen % 2 == 1) continue;
          #endif
          
          size_t hidpf_nitems = 1ULL << prefixlen;
          uint64_t target = -(a_hi[prefixlen] + alpha0 + alpha2) & (hidpf_nitems-1);
          auto [hidpf0, hidpf2] = dpf_key<>::gen(prgkey, hidpf_nitems, target);
          
          hi_dpf_written_b[t].dpfnitems = hidpf0.nitems;
          hi_dpf_written_b[t].dpfdepth = hidpf0.depth();
          hi_dpf_written_b[t].root = hidpf0.root;
          hi_dpf_written_b[t].finalizer = hidpf0.finalizer;

          hi_dpf_written_other[t].dpfnitems = hidpf2.nitems;
          hi_dpf_written_other[t].dpfdepth = hidpf2.depth();
          hi_dpf_written_other[t].root = hidpf2.root;
          hi_dpf_written_other[t].finalizer = hidpf2.finalizer;

          comparison_progress[comparison_step::hi_dpf_gen] = t + 1;

          ++t;
      }
  }

    void generate_lo_dpfs(AES_KEY& prgkey)
    {


      size_t t = 0;

      for(size_t j = 0; j < nrecords; ++j)
      {

        #if(PARTY  == 0)
         if(j % 2 == 0) continue;
        #endif

        #if(PARTY  == 1)
         if(j % 2 == 1) continue;
        #endif

        size_t prefixlen = std::ceil(std::log2(j)); 
        
        lo_diff[t] = extract_bits(a - b[t], 2*precision-fracpart, 2*precision+intpart-prefixlen+1); 
         
        size_t lodpf_nitems = (1ULL << fracpart+intpart-prefixlen+2);
     
        assert(lodpf_nitems != 0);

        auto target = lo_diff[t] | (prefixbit1[t] << (fracpart+intpart-prefixlen + 1));


        auto [lodpf0, lodpf2] = dpf_key<>::gen(prgkey, lodpf_nitems, target);

          // std::cout << " -> nitems = " << lo_dpf_written_other[prefixlen].dpfnitems << std::endl;
          // std::cout << " ->  lodpf2.depth() = " << lodpf2.depth() << std::endl;

          lo_dpf_written_b[t].dpfnitems = lodpf0.nitems;
          lo_dpf_written_b[t].dpfdepth = lodpf0.depth();
          lo_dpf_written_b[t].root = lodpf0.root;
          lo_dpf_written_b[t].finalizer = lodpf0.finalizer;

          lo_dpf_written_other[t].dpfnitems = lodpf2.nitems;
          lo_dpf_written_other[t].dpfdepth = lodpf2.depth();
          lo_dpf_written_other[t].root = lodpf2.root;
          lo_dpf_written_other[t].finalizer = lodpf2.finalizer;

          assert(lo_dpf_written_other[t].dpfnitems != 0);

         comparison_progress[comparison_step::lo_dpf_gen] = t + 1;

         ++t;
      }
    }


 


  void do_carrydpf_evalintervals()
    {
          
        __m256i  *query;

        AES_KEY prgkey;

 

        
        for(size_t j = 0; j < nprefixes/2; ++j)
        {



            while(comparison_progress[comparison_step::carry_dpfs_in]  < j + 1 || comparison_progress[comparison_step::a_hi_and_lo_in] < j + 1) 
            {
                 std::this_thread::yield();
            }
            

            std::cout << "do_carrydpf_evalintervals " << j << std::endl; 

            size_t nitems_; 
            node_t root_;
            std::vector<node_t> cw_(carry_dpf_read_b[j].dpfdepth);
            std::array<node_t, nodes_per_leaf> finalizer_;
            
           /// std::cout << "j = " << j << std::endl;
           
            nitems_ = carry_dpf_read_b[j].dpfnitems;//carry_dpf_read[j].nitems; 
            root_ = carry_dpf_read_b[j].root; 
            
            uint64_t from = nitems_   - a_lo[j];
            uint64_t to   = nitems_/2 - a_lo[j];

 
            auto carrydpf_nitems = nitems_;
            
            auto alloc_size = 0;
            
          
            for(size_t d = 0; d < carry_dpf_read_b[j].dpfdepth; ++d) cw_[d] = carry_dpf_read_b[j].cw[d];
           
            finalizer_ = carry_dpf_read_b[j].finalizer;

            dpf_key<> carrydpf(std::move(nitems_),  std::move(root_), std::move(cw_), std::move(finalizer_), std::move(prgkey));
            
            if (a_lo[j] < carrydpf_nitems/2)
            {
                uint64_t from = carrydpf_nitems   - a_lo[j];
                uint64_t to   = carrydpf_nitems/2 - a_lo[j];

                auto alloc_size0 = dpf_key<>::interval_bytes(0, to);
                auto alloc_size1 = dpf_key<>::interval_bytes(from, carrydpf_nitems-1);
                alloc_size = alloc_size0 + alloc_size1;

                query = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
                carrydpf.evalinterval(0, to, (bool*)query, NULL, true);
                
                if(from >= carrydpf_nitems-1)
                {
                  std::cerr << "if \n";
                  std::cerr << "a_lo[" << j << "] = " << a_lo[j] << std::endl;
                  std::cerr << "from = " << from << std::endl;
                  std::cerr << "carrydpf_nitems-1 = " << carrydpf_nitems-1 << std::endl;
                }
                
                assert(from < carrydpf_nitems-1);
                carrydpf.evalinterval(from, carrydpf_nitems-1, ((bool*)query) + alloc_size0, NULL, true);
            }       
            else
            {
                uint64_t from = carrydpf_nitems - a_lo[j];
                uint64_t to = from + carrydpf_nitems/2;

                alloc_size = dpf_key<>::interval_bytes(from, to);

                query = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));

                if(from >= to)
                {
                  std::cerr << "else\n";
                  std::cerr << j <<  " ---> from = " << from << " = " << carrydpf_nitems << " - " << a_lo[j] << std::endl;
                  std::cerr << "to = " << to << std::endl;
                }

                assert(from < to);
               
                carrydpf.evalinterval(from, to, (bool*)query, NULL, true);
            }


            for (size_t i = 0; i < alloc_size / sizeof(__m256i); ++i)
            {
                  c[j] += (__builtin_parityll(query[i][0]) + __builtin_parityll(query[i][1])  + __builtin_parityll(query[i][2])  + __builtin_parityll(query[i][3]));
            }
             
            if (alloc_size % sizeof(__m256i) > 0)
            {
                  c[j] += ( __builtin_parityll(query[alloc_size/sizeof(__m256i)][0]) + __builtin_parityll(query[alloc_size/sizeof(__m256i)][1]));
            }

              comparison_progress[comparison_step::carry_evals] = j + 1;

        }

        std::cout << "evalinterval done\n";
    }



    void send_hi_dpfs_to_P_other(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
    {
  
           while(comparison_progress[comparison_step::hi_dpf_gen] < j + 1) 
            {
              std::this_thread::yield(); 
            }
            

            async_write(sout, boost::asio::buffer(&hi_dpf_written_other[j], sizeof(hi_dpf_written_other[j])),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {   

               std::cout << "---> send_carry_dpfs_to_P_other " << j << std::endl;    
                if(!ec)
                {
                  
                   if(j + 1 < nprefixes/2) 
                   {
                    send_hi_dpfs_to_P_other(j + 1, io_context, sout);
                   }
          
                }          
                else
                {               
                  send_hi_dpfs_to_P_other(j , io_context, sout);
                }
            
            }); 

  
           comparison_progress[comparison_step::hi_dpf_out] = j + 1;
 
    } 


    void send_hi_dpfs_to_P_b(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
    {
 
         while(comparison_progress[comparison_step::hi_dpf_gen] < j + 1) 
         {
          std::this_thread::yield(); 
         }

 
         
           async_write(sout, boost::asio::buffer(&hi_dpf_written_b[j], sizeof(hi_dpf_written_b[j])),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                    
                 if(j + 1 < nprefixes/2) 
                 {
                  send_hi_dpfs_to_P_b(j + 1, io_context, sout);
                 }
                }          
                else
                {               
                  send_hi_dpfs_to_P_b(j , io_context, sout);
                }
            
            }); 
    } 
    
    void read_hi_dpfs_from_P_b(tcp::socket &sin)
    {
        for(size_t j = 0; j < nprefixes/2; ++j)
        {
          read(sin, boost::asio::buffer(&hi_dpf_read_b[j], sizeof(hi_dpf_read_b[j])));

          comparison_progress[comparison_step::hi_dpfs_in] = j + 1;
        }

        std::cout << "hi dpfs read ... \n";
    }

    void do_hidpf_evalintervals()
    {
          
        __m256i  *query;

        AES_KEY prgkey;
   
        for(size_t j = 0; j < nprefixes/2; ++j)
        {

            while(comparison_progress[comparison_step::hi_dpfs_in]  < j + 1 || comparison_progress[comparison_step::hi_and_lo_in] < j + 1) 
            {
                 std::this_thread::yield();
            }
            

            size_t nitems_; 
            node_t root_;
            std::vector<node_t> cw_(hi_dpf_written_b[j].dpfdepth);
            std::array<node_t, nodes_per_leaf> finalizer_;
            
           /// std::cout << "j = " << j << std::endl;
           
            nitems_ = hi_dpf_written_b[j].dpfnitems;//carry_dpf_read[j].nitems; 
          
            
            root_ = hi_dpf_written_b[j].root; 
            
            const size_t to = nitems_-1;
            auto alloc_size = dpf_key<>::interval_bytes(0, to);
            query = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
         

            for(size_t d = 0; d < hi_dpf_written_b[j].dpfdepth; ++d) cw_[d] = hi_dpf_written_b[j].cw[d];
           
            finalizer_ = hi_dpf_written_b[j].finalizer;
           
            dpf_key<> dpfkey(std::move(nitems_),  std::move(root_), std::move(cw_), std::move(finalizer_), std::move(prgkey));     
              

            auto evalat = (a_hi[j] + carry[j]) & (nitems_-1);

            bool b0 = dpfkey.eval(evalat);// ^ r0;  

            comparison_progress[comparison_step::hi_evals] = j + 1;         
 
        } 

        // std::cout << "carry evalinterval done\n";
    }





  void send_lo_dpfs_to_P_other(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
  {

         while(comparison_progress[comparison_step::lo_dpf_gen] < j + 1) 
          {
            std::this_thread::yield(); 
          }
          

          async_write(sout, boost::asio::buffer(&lo_dpf_written_other[j], sizeof(lo_dpf_written_other[j])),
          [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
          
          {       
              if(!ec)
              {
                
                 if(j + 1 < nrecords/2) 
                 {
                   send_lo_dpfs_to_P_other(j + 1, io_context, sout);

                 }
        
              }          
              else
              {               
                send_lo_dpfs_to_P_other(j , io_context, sout);
              }
          
          }); 

          comparison_progress[comparison_step::lo_dpf_out] = j + 1;
  } 



    void send_lo_dpfs_to_P_b(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
    {
 
         while(comparison_progress[comparison_step::lo_dpf_gen] < j + 1) 
         {
          std::this_thread::yield(); 
         }

 
         
           async_write(sout, boost::asio::buffer(&lo_dpf_written_b[j], sizeof(lo_dpf_written_b[j])),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {                    
                 if(j + 1 < nrecords/2) 
                 {
                  send_lo_dpfs_to_P_b(j + 1, io_context, sout);
                 }
                }          
                else
                {               
                  send_lo_dpfs_to_P_b(j , io_context, sout);
                }
            
            }); 
    } 
    
void read_lo_dpfs_from_P_b(tcp::socket &sin)
{


    for(size_t j = 0; j < nrecords/2; ++j)
    {
      std::cout << "read_randomness_from_P2 " << j << std::endl;
      read(sin, boost::asio::buffer(&lo_dpf_read_b[j], sizeof(lo_dpf_read_b[j])));

      comparison_progress[comparison_step::lo_dpfs_in] = j + 1;
    }

    std::cout << "reading lowdpfs done\n";
 }


  




    void do_lodpf_evalintervals()
    {
          
 

        AES_KEY prgkey;

 

        
        for(size_t j = 0; j < nrecords/2; ++j)
        {

            while(comparison_progress[comparison_step::lo_dpf_gen]  < j + 1 || comparison_progress[comparison_step::hi_and_lo_in] < j + 1) 
            {
                 std::this_thread::yield();
            }
            

            size_t nitems_; 
            node_t root_;
            std::vector<node_t> cw_(lo_dpf_written_other[j].dpfdepth);
            std::array<node_t, nodes_per_leaf> finalizer_;
            
           
            nitems_ = lo_dpf_written_other[j].dpfnitems;//carry_dpf_read[j].nitems; 
         
            root_ = lo_dpf_written_other[j].root; 
            
            const size_t to = nitems_-1;
            auto alloc_size = dpf_key<>::interval_bytes(0, to);
   
         

            for(size_t d = 0; d < lo_dpf_written_other[j].dpfdepth; ++d) cw_[d] = lo_dpf_written_other[j].cw[d];
           
            finalizer_ = lo_dpf_written_other[j].finalizer;
           
            dpf_key<> dpfkey(std::move(nitems_),  std::move(root_), std::move(cw_), std::move(finalizer_), std::move(prgkey));
            
            uint64_t offset = 0;

            size_t lodpf_nitems = nitems_;

          //  if(prefixbit[j] == 1) offset = lodpf_nitems/2;
            
            __m256i  * query;

            if(lo_diff[j] < lodpf_nitems/4)
            {
 

                uint64_t from = lodpf_nitems/2 - lo_diff[j];
                uint64_t to = lodpf_nitems/4 - lo_diff[j];
         
                uint64_t alloc_size0 = dpf_key<>::interval_bytes(0+offset, to+offset);
                uint64_t alloc_size1 = dpf_key<>::interval_bytes(from+offset, lodpf_nitems/2-1+offset);

                alloc_size = alloc_size0 + alloc_size1;

                //  // do evalintervals
                query = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
         
                dpfkey.evalinterval(0 + offset, to + offset, (bool*)query, NULL, true);
 
                if(from < to)  dpfkey.evalinterval(from + offset, lodpf_nitems/2-1 + offset, ((bool*)query) + alloc_size0, NULL, true);
         
                // compute the parities
                for (int i = 1; i < alloc_size / sizeof(__m256i); ++i)
                {
                  query[0] ^= query[i];        
                }
                if (alloc_size % sizeof(__m256i) > 0)
                {
                  query[0][0] ^= query[alloc_size/sizeof(__m256i)][0] ^ query[alloc_size/sizeof(__m256i)][1];   
                }
            }            
            else
            {
                uint64_t from = lodpf_nitems/2 - lo_diff[j];
                uint64_t to = from + lodpf_nitems/4;


                uint64_t alloc_size = dpf_key<>::interval_bytes(from, to);

                // do evalintervals
                query = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
                dpfkey.evalinterval(from + offset, to + offset, (bool*)query, NULL, true);

                // compute the parities
                for (int i = 1; i < alloc_size / sizeof(__m256i); ++i)
                {
                  query[0] ^= query[i]; 
                }

                if (alloc_size % sizeof(__m256i) > 0)
                {
                  query[0][0] ^= query[alloc_size/sizeof(__m256i)][0] ^ query[alloc_size/sizeof(__m256i)][1];
                }
             }

             comparison_progress[comparison_step::lo_evals] = j + 1;

        }

        // std::cout << "carry evalinterval done\n";
    }


 


int main(int argc, char * argv[])
{  

 
    for(size_t j = 0; j < nrecords; ++j)
    {
      uint8_t x;  
      arc4random_buf(&x, sizeof(uint8_t));
      x = x % 2;
      eval_vector[j] = 1;
      if(x == 0) eval_vector[j] = 0;
       
     }

  uint64_t a0, a1, a_ = 222.4 * scale2;
  arc4random_buf(&a0, sizeof(a0));
  a1 = a_ - a0;
  assert(a_ == a0+a1);

  
  #if(PARTY == 0)
    a = a0;
  #endif

  #if(PARTY == 1)
    a = a1;
  #endif


  double r0[nrecords];
  double r1[nrecords];

  double R0[nrecords];
  double R1[nrecords];

  uint64_t R64_0[nrecords];
  uint64_t R64_1[nrecords];

  for(size_t j = 0; j < nprefixes; ++j)
  {
    prefix_rand[j] = arc4random_uniform(2);
  }

  for(size_t j = 0; j < nrecords; ++j)
  {
    R0[j] = 0.0;
    R1[j] = 0.0;
    
    double r0_ = ((double) rand() / (RAND_MAX));
    r0[j] = r0_;

    double r1_ = ((double) rand() / (RAND_MAX));
    r1[j] = r1_;  
  }

  for(size_t j = 0; j < nrecords; ++j)
  { 
    for(size_t i = 0; i < j; ++i)
    { 
      R0[j] += r0[i];         
      R1[j] += r1[i];
    }
  }

  for(size_t j = 0; j < nrecords; ++j)
  {
    R64_0[j] = R0[j] * scale2;
    R64_1[j] = R1[j] * scale2;
  }


  #if(PARTY == 0)
    for(size_t j = 0; j < nrecords; ++j) b[j] = R64_0[j];
  #endif

  #if(PARTY == 1)
    for(size_t j = 0; j < nrecords; ++j) b[j] = R64_1[j];
  #endif 




  std::cerr << "a = " << a << std::endl;

  size_t prefixlen = std::ceil(std::log2(nrecords)); 

 

  std::cout << "nrecords = " << nrecords << std::endl;
  std::cout << "prefixlen = " << prefixlen << std::endl;

  const std::string host1 = (argc < 2) ? "127.0.0.1" : argv[1];
  const std::string host2 = (argc < 3) ? "127.0.0.1" : argv[2];
  const std::string host3 = (argc < 4) ? "127.0.0.1" : argv[3];
  

  AES_KEY prgkey;
 

   #include "connections.h"


   read(s3_a, boost::asio::buffer(&seed_, sizeof(__m128i)));
   
   read(s2, boost::asio::buffer(&seed2, sizeof(__m128i)));

   PRG(prgkey, seed_, (__m128i *)blinds, ( (nrecords + 1) * DIM * sizeof(uint64_t)) / sizeof(__m128i));
   
   PRG(prgkey, seed2, (__m128i *) sq_blinds ,  (3 * (nrecords) * sizeof(uint8_t)) / sizeof(__m128i));
   PRG(prgkey, seed2, (__m128i *) zero_blinds, ((nrecords) * sizeof(uint64_t)) / sizeof(__m128i));
   
   blinds_for_squaring_and_zeroing();
   
   std::thread zero_blinds_writer0(write_zero_blinds0, 0, std::ref(io_context), std::ref(sb_a));
    std::thread zero_blinds_writer1(write_zero_blinds1, 0, std::ref(io_context), std::ref(sb_d));
   std::thread sq_blinds_writer0(write_sq_blinds0, 0, std::ref(io_context), std::ref(sb_b));
   std::thread sq_blinds_writer1(write_sq_blinds1, 0, std::ref(io_context), std::ref(sb_c));

   std::thread zero_blinds_reader0(read_zero_blinds0, std::ref(sb_a));
    std::thread zero_blinds_reader1(read_zero_blinds1, std::ref(sb_d));
   std::thread sq_blinds_reader0(read_sq_blinds0, std::ref(sb_b));
    std::thread sq_blinds_reader1(read_sq_blinds1, std::ref(sb_c));



   //get_profiles(s3);

   // std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now(); 
   // std::cout << "time measurement starts ......\n";

   // blind_b();

   // std::thread blinded_b_writer(write_blinded_b, 0, std::ref(io_context), std::ref(sb_c));
   // std::thread blinded_b_reader(recv_blinded_b, std::ref(sb_c));
   // std::thread b_multiplier(multiply_b);

   std::thread poller(keep_polling, std::ref(io_context));
   std::thread hi_and_lo_bit_extractor(extract_hi_and_lo_bits);
   // std::thread partial_sum_generator(generate_partial_sum);
   
   std::thread carry_dpfs_generator(generate_carry_dpfs, std::ref(prgkey));
   std::thread hi_dpf_generator(generate_hi_dpfs, std::ref(prgkey));
   std::thread lo_dpf_generator(generate_lo_dpfs, std::ref(prgkey));
   
   std::thread sq_blinds_from_P2_reader(read_sq_blinds_from_P2, std::ref(s2_d));
   std::thread zero_blinds_from_P2_reader(read_zero_blinds_from_P2, std::ref(s2_e));

   #if(PARTY == 0)
   std::thread carry_dpfs_to_P_other_sender(send_carry_dpfs_to_P_other, 0, std::ref(io_context), std::ref(s2_b));
   std::thread hi_dpfs_to_P_other_sender(send_hi_dpfs_to_P_other, 0, std::ref(io_context), std::ref(s2_c));
   std::thread lo_dpfs_to_P_other_sender(send_lo_dpfs_to_P_other, 0, std::ref(io_context), std::ref(s2_a));


   #endif
   
   #if(PARTY == 1)
   std::thread carry_dpfs_to_P_other_sender(send_carry_dpfs_to_P_other, 0, std::ref(io_context), std::ref(s3_b));
   std::thread hi_dpfs_to_P_other_sender(send_hi_dpfs_to_P_other, 0, std::ref(io_context), std::ref(s3_c));
   std::thread lo_dpfs_to_P_other_sender(send_lo_dpfs_to_P_other, 0, std::ref(io_context), std::ref(s3_a));
   #endif



  

   std::thread carry_dpfs_to_Pb_sender(send_carry_dpfs_to_P_b, 0, std::ref(io_context), std::ref(sb_e)); 
   std::thread carry_dpfs_to_Pb_reader(read_carry_dpfs_from_P_b, std::ref(sb_e));
   
   std::thread lo_dpfs_to_Pb_sender(send_lo_dpfs_to_P_b, 0, std::ref(io_context), std::ref(sb_f)); 
   std::thread lo_dpfs_to_Pb_reader(read_lo_dpfs_from_P_b, std::ref(sb_f));


   std::thread hi_dpfs_to_Pb_sender(send_hi_dpfs_to_P_b, 0, std::ref(io_context), std::ref(sb_g)); 
   std::thread hi_dpfs_to_Pb_reader(read_hi_dpfs_from_P_b, std::ref(sb_g));


   std::thread carry_dpf_evaluator(do_carrydpf_evalintervals);
   std::thread hi_dpf_evaluator(do_hidpf_evalintervals);
   std::thread lo_dpf_evaluator(do_lodpf_evalintervals);

 
   poller.join();
  
   zero_blinds_writer0.join();
   zero_blinds_writer1.join();
   sq_blinds_writer0.join();
   sq_blinds_writer1.join();
   zero_blinds_reader0.join();
   zero_blinds_reader1.join();
   sq_blinds_reader0.join();
   sq_blinds_reader1.join();
   
   hi_and_lo_bit_extractor.join();
   // partial_sum_generator.join();
   // blinded_b_writer.join();
   // blinded_b_reader.join();
   // b_multiplier.join();

    carry_dpfs_generator.join();
    hi_dpf_generator.join();
    lo_dpf_generator.join();

    sq_blinds_from_P2_reader.join();
    zero_blinds_from_P2_reader.join();

    carry_dpfs_to_P_other_sender.join();
    hi_dpfs_to_P_other_sender.join();
    lo_dpfs_to_P_other_sender.join();

   carry_dpfs_to_Pb_sender.join();
   carry_dpfs_to_Pb_reader.join();


   hi_dpfs_to_Pb_sender.join();
   hi_dpfs_to_Pb_reader.join();
      
   lo_dpfs_to_Pb_sender.join();
   lo_dpfs_to_Pb_reader.join();
   
   carry_dpf_evaluator.join();
   hi_dpf_evaluator.join();
   lo_dpf_evaluator.join();

   
   // std::cout << "evaluation is done ... \n"; 
   // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
   // std::cerr << "connections took "
   //            <<  double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count())/1000000
   //            << "seconds.\n";
 

  return 0;
}

 