#include <thread>
#include <iostream>
#include <chrono>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ssl.hpp>
#include<vector>
 
#include<fstream>
#include "aes.h"
#include "network.h"
 

 #include "dpf.h"

 using namespace dpf;

typedef bool leaf_t;
typedef block<__m128i> node_t;
typedef AES_KEY prgkey_t;



using boost::asio::ip::tcp;
using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;
boost::asio::io_context io_context;
boost::asio::ssl::context ssl_context(boost::asio::ssl::context::tls); 
 prgkey_t prgkey;

   std::vector<dpf_key<leaf_t, node_t, prgkey_t>> keys_read;
   std::vector<dpf_key<leaf_t, node_t, prgkey_t>> keys_recv;
 //#include "isqrt.h"
 
 
 
  uint64_t key0 = 597349;
  uint64_t key1 = 121379;

 #include "compare.h"
 enum comparison_step
 {
  dpfs_in =0,
  hi_dpfs_in,
  lo_dpfs_in,
  lo_dpf_eval,
  hi_dpf_eval,
  eval,
  randomness_in,
  sq_zero_randomness_in,
  comparison_num_steps,
};
size_t comparison_progress[comparison_step::comparison_num_steps] = { 0 };
//#include "p_other.h"



uint64_t * query_b[nrecords];
uint64_t share0[nrecords];
uint64_t share1[nrecords];
double rand0[nrecords];
double rand1[nrecords];
uint64_t carry[nprefixes]; 

 
 void keep_polling(boost::asio::io_context& io_context)
 {
    while( 
               comparison_progress[comparison_step::dpfs_in]   < nprefixes/2 ||   
               comparison_progress[comparison_step::lo_dpfs_in] < nrecords/2 ||
               comparison_progress[comparison_step::hi_dpfs_in] < nprefixes/2 ||
               comparison_progress[comparison_step::hi_dpf_eval] < nprefixes/2 ||
               comparison_progress[comparison_step::lo_dpf_eval] < nrecords/2 
          )
    {
      io_context.reset();
      io_context.poll();
    }
 }




 uint64_t c0[nprefixes/2];
 uint64_t c1[nprefixes/2];
 uint64_t alpha0[nprefixes/2];
 uint64_t alpha1[nprefixes/2];
 

 int8_t sq_blinds0[2][nrecords];
 int8_t sq_blinds1[2][nrecords];
 int8_t sq_alpha0[nrecords];
 int8_t sq_alpha1[nrecords];

 uint64_t zero_blinds0[2][nrecords];
 uint64_t zero_blinds1[2][nrecords];
 uint64_t zero_alpha0[nrecords];
 uint64_t zero_alpha1[nrecords];


 __m128i seed_;

 void generate_randomness_for_squaring_and_zeroing()
 {
      int8_t  X0, X1; 
      int8_t  Y0, Y1;


      for(size_t j = 0; j < nrecords; ++j)
      {
        arc4random_buf(&sq_blinds0[0][j], sizeof(int8_t)); 
        arc4random_buf(&sq_blinds0[1][j], sizeof(int8_t));
        arc4random_buf(&sq_blinds1[0][j], sizeof(int8_t)); 
        arc4random_buf(&sq_blinds1[1][j], sizeof(int8_t));

        X0 = sq_blinds1[0][j];
        X1 = sq_blinds1[1][j];


        Y0 = sq_blinds0[0][j];
        Y1 = sq_blinds0[1][j];

        sq_alpha0[j] = X0 * Y1;
        sq_alpha1[j] = X1 * Y0;

        arc4random_buf(&zero_blinds0[0][j], sizeof(uint64_t)); 
        arc4random_buf(&zero_blinds0[1][j], sizeof(uint64_t));
        arc4random_buf(&zero_blinds1[0][j], sizeof(uint64_t)); 
        arc4random_buf(&zero_blinds1[1][j], sizeof(uint64_t));

        zero_alpha0[j] = zero_blinds1[0][j] * zero_blinds0[1][j];
        zero_alpha1[j] = zero_blinds1[1][j] * zero_blinds0[0][j];

        comparison_progress[comparison_step::sq_zero_randomness_in] = j + 1;
      }
 }

  void send_sq_blinds_to_P0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
  {
  
     while(comparison_progress[comparison_step::sq_zero_randomness_in] < j + 1) 
      {
        std::this_thread::yield(); 
      }
      
     async_write(sout, boost::asio::buffer(&sq_alpha0[j], sizeof(sq_alpha0[j])),
      [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
      
      {       
          if(!ec)
          {
            
             if(j + 1 < nrecords) 
             {
              send_sq_blinds_to_P0(j + 1, io_context, sout);
             }
    
          }          
          else
          {               
            send_sq_blinds_to_P0(j , io_context, sout);
          }
      
      }); 
   }

  void send_sq_blinds_to_P1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
  {
  
     while(comparison_progress[comparison_step::sq_zero_randomness_in] < j + 1) 
      {
        std::this_thread::yield(); 
      }
      
     async_write(sout, boost::asio::buffer(&sq_alpha1[j], sizeof(sq_alpha1[j])),
      [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
      
      {       
          if(!ec)
          {
            
             if(j + 1 < nrecords) 
             {
              send_sq_blinds_to_P1(j + 1, io_context, sout);
             }
    
          }          
          else
          {               
            send_sq_blinds_to_P1(j , io_context, sout);
          }
      
      });
    } 



  void send_zero_blinds_to_P0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
  {
  
     while(comparison_progress[comparison_step::sq_zero_randomness_in] < j + 1) 
      {
        std::this_thread::yield(); 
      }
      
     async_write(sout, boost::asio::buffer(&zero_alpha0[j], sizeof(zero_alpha0[j])),
      [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
      
      {       
          if(!ec)
          {
            
             if(j + 1 < nrecords) 
             {
              send_zero_blinds_to_P0(j + 1, io_context, sout);
             }
    
          }          
          else
          {               
            send_zero_blinds_to_P0(j , io_context, sout);
          }
      
      }); 
   }

  void send_zero_blinds_to_P1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
  {
  
     while(comparison_progress[comparison_step::sq_zero_randomness_in] < j + 1) 
      {
        std::this_thread::yield(); 
      }
      
     async_write(sout, boost::asio::buffer(&zero_alpha1[j], sizeof(zero_alpha1[j])),
      [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
      
      {       
          if(!ec)
          {
            
             if(j + 1 < nrecords) 
             {
              send_zero_blinds_to_P1(j + 1, io_context, sout);
             }
    
          }          
          else
          {               
            send_zero_blinds_to_P1(j , io_context, sout);
          }
      
      }); 
   }




 void generate_randomness_for_multiplication()
 {

      for(size_t j = 0; j < nprefixes/2; ++j)
      {
        arc4random_buf(&c0[j], sizeof(uint64_t));
        arc4random_buf(&c1[j], sizeof(uint64_t));
        uint64_t r;
        arc4random_buf(&r, sizeof(uint64_t));

        alpha0[j] = c0[j] * c1[j] - r;
        alpha1[j] = r;

      }

 }


   void generate_randomness()
   {
     for(size_t j = 0; j < nrecords; ++j)
     {
        double r0 = ((double) rand() / (RAND_MAX));
        rand0[j] = r0;

        double r1 = ((double) rand() / (RAND_MAX));
        rand1[j] = r1;
        
        // std::cout << "rand0[" << j << "] = " << rand0[j] << std::endl;
        // std::cout << "rand1[" << j << "] = " << rand1[j] << std::endl << std::endl;

        comparison_progress[comparison_step::randomness_in] = j + 1;
     }
   }


   void send_randomness_to_P0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
    {
  
           while(comparison_progress[comparison_step::randomness_in] < j + 1) 
            {
              std::this_thread::yield(); 
            }
            

 

           async_write(sout, boost::asio::buffer(&rand0[j], sizeof(rand0[j])),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {
                  
                   if(j + 1 < nrecords) 
                   {
                    send_randomness_to_P0(j + 1, io_context, sout);
                   }
          
                }          
                else
                {               
                  send_randomness_to_P0(j , io_context, sout);
                }
            
            }); 
 
   }

      void send_randomness_to_P1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
    {
  
           while(comparison_progress[comparison_step::randomness_in] < j + 1) 
            {
              std::this_thread::yield(); 
            }
            

 

           async_write(sout, boost::asio::buffer(&rand1[j], sizeof(rand0[j])),
            [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
            
            {       
                if(!ec)
                {
                  
                   if(j + 1 < nrecords) 
                   {
                    send_randomness_to_P1(j + 1, io_context, sout);
                   }
          
                }          
                else
                {               
                  send_randomness_to_P1(j , io_context, sout);
                }
            
            }); 
 
   }
  void do_carry_dpf_evalintervals()
  { 
 
    
      __m256i  *query;

      AES_KEY prgkey;


      
      for(size_t j = 0; j < nprefixes/2; ++j)
      {


          while(comparison_progress[comparison_step::dpfs_in] < j + 1) 
          {
             std::this_thread::yield();
          }

          size_t nitems_; 
          node_t root_;
          std::vector<node_t> cw_(carry_dpf_read[j].dpfdepth);
          std::array<node_t, nodes_per_leaf> finalizer_;
          
         /// std::cout << "j = " << j << std::endl;
         
          nitems_ = carry_dpf_read[j].dpfnitems;//carry_dpf_read[j].nitems; 
          root_ = carry_dpf_read[j].root; 
          
          const size_t to = nitems_-1;
          auto alloc_size = dpf_key<>::interval_bytes(0, to);
          query = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
       

          for(size_t d = 0; d < carry_dpf_read[j].dpfdepth; ++d) cw_[d] = carry_dpf_read[j].cw[d];
         
          finalizer_ = carry_dpf_read[j].finalizer;
         
          dpf_key<> dpfkey(std::move(nitems_),  std::move(root_), std::move(cw_), std::move(finalizer_), std::move(prgkey));
          dpfkey.evalinterval(0, to, (bool*)query, NULL, true);

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

      std::cout << "carry evalinterval done\n";

  }
   

  void do_hi_dpf_evalintervals()
  { 
 

    
     __m256i * query;
 

      AES_KEY prgkey;


      const size_t to = 4;
      auto alloc_size = dpf_key<>::interval_bytes(0, to);
      query = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
      
      for(size_t j = 0; j < nprefixes/2; ++j)
      {


        while(comparison_progress[comparison_step::hi_dpfs_in] < j + 1) 
        {
           std::this_thread::yield();
        }
        
        size_t nitems_; 
        node_t root_;
        std::vector<node_t> cw_(hi_dpf_read[j].dpfdepth);
        std::array<node_t, nodes_per_leaf> finalizer_;
        
     //   std::cout << "do_hi_dpf_evalintervals j = " << j << std::endl;
       
        nitems_ = hi_dpf_read[j].dpfnitems;//hi_dpf_read[j].nitems; 
        
        root_ = hi_dpf_read[j].root; 
        
        for(size_t d = 0; d < hi_dpf_read[j].dpfdepth; ++d) cw_[d] = hi_dpf_read[j].cw[d];
       
        finalizer_ = hi_dpf_read[j].finalizer;
       
        dpf_key<> dpfkey(std::move(nitems_),  std::move(root_), std::move(cw_), std::move(finalizer_), std::move(prgkey));
        
        // auto evalat = (a_hi[j] + carry[j]) & (nitems_-1);
        
        // bool b0 = dpfkey.eval(evalat);// ^ r0;  

        comparison_progress[comparison_step::hi_dpf_eval] = j + 1;
      }

      std::cout << "hi evalinterval done\n";

  }


  void do_lo_dpf_evalintervals()
  {


      __m256i * query;

     


      AES_KEY prgkey;



      const size_t to = 12;
      auto alloc_size = dpf_key<>::interval_bytes(0, to);
      query = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));
      
 
      for(size_t j = 0; j < nrecords/2; ++j)
      {

        while(comparison_progress[comparison_step::lo_dpfs_in] < j + 1) 
        {
         std::this_thread::yield();
        }
        
        size_t nitems_; 
        node_t root_;
        std::vector<node_t> cw_(lo_dpf_read[j].dpfdepth);
        std::array<node_t, nodes_per_leaf> finalizer_;
        

       
        nitems_ = lo_dpf_read[j].dpfnitems;//lo_dpf_read[j].nitems; 
        
        if(nitems_ == 0) continue;

        root_ = lo_dpf_read[j].root; 
        
        for(size_t d = 0; d < lo_dpf_read[j].dpfdepth; ++d) cw_[d] = lo_dpf_read[j].cw[d];
       
        finalizer_ = lo_dpf_read[j].finalizer;
       
        dpf_key<> dpfkey(std::move(nitems_),  std::move(root_), std::move(cw_), std::move(finalizer_), std::move(prgkey));
       
        
        dpfkey.evalinterval(0, to, (bool*)query, NULL, true);

        // compute the parities
        for (int i = 1; i < alloc_size / sizeof(__m256i); ++i)
        {
          query[0] ^= query[i];
        }
        if (alloc_size % sizeof(__m256i) > 0)
        {
          query[0][0] ^= query[alloc_size/sizeof(__m256i)][0] ^ query[alloc_size/sizeof(__m256i)][1];
        }

        comparison_progress[comparison_step::lo_dpf_eval] = j + 1;
      }

      std::cout << "lo evalinterval done\n";
  }


  void read_carry_dpfs_from_Pb(tcp::socket &sin)
  {

    std::cout << "read_carry_dpfs_from_Pb --> " << nprefixes << " -> " << nprefixes/2 << std::endl;

    for(size_t j = 0; j < nprefixes/2; ++j)
    {
 
      std::cout << "read_carry_dpfs_from_Pb " << j << std::endl;

      read(sin, boost::asio::buffer(&carry_dpf_read[j], sizeof(carry_dpf_read[j])));
     
      comparison_progress[comparison_step::dpfs_in] = j + 1;
    }
  
      std::cout << "reading carry dpfs complete \n";
  }



  void read_hi_dpfs_from_Pb(tcp::socket &sin)
  {
  

    for(size_t j = 0; j < nprefixes/2; ++j)
    {

      std::cout << "read_hi_dpfs_from_Pb --> " << j << std::endl;
      read(sin, boost::asio::buffer(&hi_dpf_read[j], sizeof(hi_dpf_read[j])));
     
      comparison_progress[comparison_step::hi_dpfs_in] = j + 1;
    }
     std::cout << "reading hi dpfs complete \n";
  
  }




  void read_lo_dpfs_from_Pb(tcp::socket &sin)
  {
    for(size_t j = 0; j < nrecords/2; ++j)
    {
      // std::cout << "low dpfs j = " << j << std::endl; 
      read(sin, boost::asio::buffer(&lo_dpf_read[j], sizeof(lo_dpf_read[j])));
     
      comparison_progress[comparison_step::lo_dpfs_in] = j + 1;
    }

    std::cout << "reading lo dpfs complete \n";
  
  }


int main(int argc, char* argv[])
{ 
  
  keys_read.reserve(nrecords);
  keys_recv.reserve(nrecords);
   //carry_dpf_read.reserve(nprefixes + 1);
  AES_KEY key;
  for(size_t j = 0; j < nrecords; ++j)
  {
    arc4random_buf(&share0[j], sizeof(uint64_t));
    arc4random_buf(&share1[j], sizeof(uint64_t));
  }
 
 

 
  tcp::acceptor acceptor1(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2));
  std::cout << " --- > " << std::endl;
  tcp::socket s1(acceptor1.accept());
  std::cerr << "Listenting on port 2: " << PORT_P1_P2 << std::endl;


  tcp::acceptor acceptor1_a(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_a));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_a(acceptor1_a.accept());
  std::cerr << "Listenting on port 2: " << PORT_P1_P2_a << std::endl;

  tcp::acceptor acceptor1_b(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_b));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_b(acceptor1_b.accept());
  std::cerr << "Listenting on port 3: " << PORT_P1_P2_b << std::endl;

    tcp::acceptor acceptor1_c(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_c));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_c(acceptor1_c.accept());
  std::cerr << "Listenting on port 3: " << PORT_P1_P2_c << std::endl;
 

    tcp::acceptor acceptor1_d(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_d));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_d(acceptor1_d.accept());
  std::cerr << "Listenting on port 3: " << PORT_P1_P2_d << std::endl;

    tcp::acceptor acceptor1_e(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_e));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_e(acceptor1_e.accept());
  std::cerr << "Listenting on port 3: " << PORT_P1_P2_e << std::endl;

    tcp::acceptor acceptor1_f(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P2_f));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_f(acceptor1_f.accept());
  std::cerr << "Listenting on port 3: " << PORT_P1_P2_f << std::endl;


  tcp::acceptor acceptor0(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2));
  std::cout << " --- > " << std::endl;
  tcp::socket s0(acceptor0.accept());
  std::cerr << "Listenting on port 5: " << PORT_P0_P2 << std::endl;

  tcp::acceptor acceptor0_a(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_a));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_a(acceptor0_a.accept());
  std::cerr << "Listenting on port 6: " << PORT_P0_P2_a << std::endl;

  tcp::acceptor acceptor0_b(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_b));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_b(acceptor0_b.accept());
  std::cerr << "Listenting on port 7: " << PORT_P0_P2_b << std::endl;

   tcp::acceptor acceptor0_c(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_c));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_c(acceptor0_c.accept());
  std::cerr << "Listenting on port 7: " << PORT_P0_P2_c << std::endl;

  tcp::acceptor acceptor0_d(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_d));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_d(acceptor0_d.accept());
  std::cerr << "Listenting on port 7: " << PORT_P0_P2_d << std::endl;
  

  tcp::acceptor acceptor0_e(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_e));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_e(acceptor0_e.accept());
  std::cerr << "Listenting on port 7: " << PORT_P0_P2_e << std::endl;

  tcp::acceptor acceptor0_f(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P2_f));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_f(acceptor0_f.accept());
  std::cerr << "Listenting on port 7: " << PORT_P0_P2_f << std::endl;

  tcp::acceptor acceptor3(io_context, tcp::endpoint(tcp::v4(), PORT_P3_P2));
  std::cout << " --- > " << std::endl;
  tcp::socket s3(acceptor3.accept());
  std::cerr << "Listenting on port: " << PORT_P3_P2 << std::endl;

  tcp::acceptor acceptor3_a(io_context, tcp::endpoint(tcp::v4(), PORT_P3_P2_a));
  std::cout << " --- > " << std::endl;
  tcp::socket s3_a(acceptor3_a.accept());
  std::cerr << "Listenting on port: " << PORT_P3_P2_a << std::endl;

  tcp::acceptor acceptor3_b(io_context, tcp::endpoint(tcp::v4(), PORT_P3_P2_b));
  std::cout << " --- > " << std::endl;
  tcp::socket s3_b(acceptor3_b.accept());
  std::cerr << "Listenting on port: " << PORT_P3_P2_b << std::endl;
 
  tcp::acceptor acceptor3_c(io_context, tcp::endpoint(tcp::v4(), PORT_P3_P2_c));
  std::cout << " --- > " << std::endl;
  tcp::socket s3_c(acceptor3_c.accept());
  std::cerr << "Listenting on port: " << PORT_P3_P2_c << std::endl;


  tcp::acceptor acceptor3_d(io_context, tcp::endpoint(tcp::v4(), PORT_P3_P2_d));
  std::cout << " --- > " << std::endl;
  tcp::socket s3_d(acceptor3_d.accept());
  std::cerr << "Listenting on port (connneection to P3): " << PORT_P3_P2_d << std::endl;

   generate_randomness_for_squaring_and_zeroing();
   arc4random_buf(&seed_, sizeof(__m128i));
   write(s0, boost::asio::buffer(&seed_, sizeof(__m128i)));
   write(s1, boost::asio::buffer(&seed_, sizeof(__m128i)));
   
   std::thread poller(keep_polling, std::ref(io_context)); 
 

   std::thread dpf_reader_from_Pb(read_carry_dpfs_from_Pb, std::ref(s0_b));
   std::thread hi_dpf_reader_from_Pb(read_hi_dpfs_from_Pb, std::ref(s0_c));
   std::thread lo_dpf_reader_from_Pb(read_lo_dpfs_from_Pb, std::ref(s0_a));

   std::thread  sq_blinds_sender_P0(send_sq_blinds_to_P0, 0, std::ref(io_context), std::ref(s0_d));  
   std::thread  sq_blinds_sender_P1(send_sq_blinds_to_P1, 0, std::ref(io_context), std::ref(s1_d));

   std::thread  zero_blinds_sender_P0(send_zero_blinds_to_P0, 0, std::ref(io_context),  std::ref(s0_e));  
   std::thread  zero_blinds_sender_P1(send_zero_blinds_to_P1, 0, std::ref(io_context),  std::ref(s1_e));

   std::thread carry_evaluater(do_carry_dpf_evalintervals); 
   
   std::thread hi_evaluater(do_hi_dpf_evalintervals);
   std::thread lo_evaluater(do_lo_dpf_evalintervals);
   
   poller.join();
 
   dpf_reader_from_Pb.join(); 
   hi_dpf_reader_from_Pb.join();
   lo_dpf_reader_from_Pb.join();

   carry_evaluater.join();
   hi_evaluater.join();
   lo_evaluater.join();

   sq_blinds_sender_P0.join();
   sq_blinds_sender_P1.join();
   zero_blinds_sender_P0.join();
   zero_blinds_sender_P1.join();


   return 0;
}

 