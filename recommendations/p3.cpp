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
 

 #include "dpf.h"

 using namespace dpf;

typedef bool leaf_t;
typedef block<__m128i> node_t;
typedef AES_KEY prgkey_t;



 using boost::asio::ip::tcp;



 AES_KEY prgkey;

 #include "compare.h"

 enum comparison_step
 {
  dpfs_in = 0,  
  profiles0_out,
  profiles1_out,
  hi_dpfs_in,
  carry_dpfs_in,
  lo_dpfs_in,
  eval,
  comparison_num_steps,
};

  size_t comparison_progress[comparison_step::comparison_num_steps] = { 0 };
 
 std::vector<dpf_key<leaf_t, node_t, prgkey_t>> keys_read;
 std::vector<dpf_key<leaf_t, node_t, prgkey_t>> keys_recv;
 uint64_t share0[nrecords];
 uint64_t share1[nrecords];
 
  uint64_t * query_b[nrecords];
 


 double itemprofiles[nrecords][DIM];
 double userprofiles[DIM];
 
 uint64_t items_[nrecords][DIM];
 uint64_t users_[DIM];

 uint64_t profiles0[nrecords+1][DIM];
 uint64_t profiles1[nrecords+1][DIM];
   
 uint64_t users0[DIM];
 uint64_t users1[DIM];

 __m128i seed_;

 void keep_polling(boost::asio::io_context& io_context)
 {
     while(
    //        comparison_progress[comparison_step::profiles0_out] <= nrecords ||
    //        comparison_progress[comparison_step::profiles1_out] <= nrecords ||
              comparison_progress[comparison_step::hi_dpfs_in] < nprefixes / 2 || 
              comparison_progress[comparison_step::lo_dpfs_in] < nrecords/2    ||
              comparison_progress[comparison_step::dpfs_in]       < nprefixes/2 ||
              comparison_progress[comparison_step::eval]          < nprefixes/2
          )
    {
      io_context.reset();
      io_context.poll();
    }
 }

  void generate_user_and_item_profiles()
  {


    for(size_t dim = 0; dim < DIM; ++dim)
    {
      userprofiles[dim] = ((double) rand() / (RAND_MAX));
      uint64_t u_ = userprofiles[dim] * scale2;
      
      arc4random_buf(&users0[dim], sizeof(uint64_t));
      users1[dim] = u_ - users0[dim];
   }

    for(size_t j = 0; j < nrecords; ++j)
    {
      for(size_t dim = 0; dim < DIM; ++dim)
      {
        itemprofiles[j][dim] = ((double) rand() / (RAND_MAX));

        arc4random_buf(&profiles0[j][dim], sizeof(uint64_t));

        uint64_t i_ =  itemprofiles[j][dim] * scale2;
   
        profiles1[j][dim] = i_ - profiles0[j][dim];
      }
    }

    for(size_t dim = 0; dim < DIM; ++dim)
    {
      profiles0[nrecords][dim] = users0[dim];
      profiles1[nrecords][dim] = users1[dim];
    }

  }


  void send_profiles_to_P0(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
  {


      // std::cout << "send_profiles_to_P0 ////\n";

      // std::cout << "----> " << nrecords<< std::endl;
      async_write(sout, boost::asio::buffer(profiles0[j], DIM * sizeof(uint64_t)),
        [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
        
        {       
            if(!ec)
            {                    
             if(j + 1 < nrecords + 1) 
             {
              send_profiles_to_P0(j + 1, io_context, sout);
             }
            }          
            else
            {               
              send_profiles_to_P0(j , io_context, sout);
            }
        
        }); 

       comparison_progress[comparison_step::profiles0_out] = j + 1;
  }

  void send_profiles_to_P1(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
  {

   // std::cout << "send_profiles_to_P1 ////\n";
      async_write(sout, boost::asio::buffer(profiles1[j], DIM * sizeof(uint64_t)),
        [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
        
        {       
            if(!ec)
            {                    
             if(j + 1 < nrecords + 1) 
             {
              send_profiles_to_P1(j + 1, io_context, sout);
             }
            }          
            else
            {               
              send_profiles_to_P1(j , io_context, sout);
            }
        
        }); 
      comparison_progress[comparison_step::profiles1_out] = j + 1;
  }
  void read_carry_dpfs_from_Pb(tcp::socket &sin)
  {
    

    std::cout << "read_carry_dpfs_from_Pb\n\n";
    std::cout << "nprefixes/2 = " << nprefixes/2 << std::endl;
    for(size_t j = 0; j < nprefixes/2; ++j)
    {
 
      read(sin, boost::asio::buffer(&carry_dpf_read_p3[j], sizeof(carry_dpf_read_p3[j])));
      std::cout << "read_carry_dpfs_from_Pb " << j << std::endl;
      comparison_progress[comparison_step::dpfs_in] = j + 1;
    }
  
    std::cout << "reading carry dpfs complete \n";
  }
 

  void do_evalintervals(size_t nitems)
  { 

  
    for(size_t j = 0; j < nprefixes/2; ++j)
    {
 
      while(comparison_progress[comparison_step::dpfs_in] < j + 1)
      {
        std::this_thread::yield();
      }

      

      auto diff0 = extract_bits(share1[j] - share0[j], 2*precision-fracpart, 2*precision+intpart);
      
      uint64_t from = nitems/2 + diff0;
      uint64_t to = nitems/2 - diff0;
      uint64_t alloc_size0 = dpf_key<>::interval_bytes(0, to);
      uint64_t alloc_size1 = dpf_key<>::interval_bytes(from, nitems-1);
      uint64_t alloc_size = alloc_size0 + alloc_size1;
      // auto query = reinterpret_cast<__m256i*>(std::aligned_alloc(alignof(__m256i), alloc_size));


     
      // keys_recv[j].evalinterval(0, to, (bool*)query, NULL, true);
      // keys_recv[j].evalinterval(from, nitems-1, ((bool*)query) + alloc_size0, NULL, true);

      // for (int i = 1; i < alloc_size / sizeof(__m256i); ++i)
      // {
      //   query[0] ^= query[i];    
      // }
 
      comparison_progress[comparison_step::eval]  = j + 1;
    }


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
    //   std::cout << "low dpfs j = " << j << std::endl; 
      read(sin, boost::asio::buffer(&lo_dpf_read[j], sizeof(lo_dpf_read[j])));
     
      comparison_progress[comparison_step::lo_dpfs_in] = j + 1;
    }

    std::cout << "reading lo dpfs complete \n";
  
  }


 

int main(int argc, char* argv[])
{ 
  
  keys_read.reserve(nrecords);
  keys_recv.reserve(nrecords);
 
  AES_KEY key;
 
  
  boost::asio::io_context io_context;
    
  tcp::resolver resolver(io_context);
  
  arc4random_buf(&seed_, sizeof(__m128i));

  for(size_t j = 0; j < nrecords; ++j)
  {
    arc4random_buf(&share0[j], sizeof(uint64_t));
    arc4random_buf(&share1[j], sizeof(uint64_t));
  }
 

  const std::string host3 = (argc < 2) ? "127.0.0.1" : argv[1];



  tcp::acceptor acceptor1(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P3));
  std::cout << " --- > " << std::endl;
  tcp::socket s1(acceptor1.accept());
  std::cerr << "Listenting on port 1: " << PORT_P1_P3 << std::endl;

  tcp::acceptor acceptor1_a(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P3_a));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_a(acceptor1_a.accept());
  std::cerr << "Listenting on port 2: " << PORT_P1_P3_a << std::endl;

  tcp::acceptor acceptor1_b(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P3_b));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_b(acceptor1_b.accept());
  std::cerr << "Listenting on port 3: " << PORT_P1_P3_b << std::endl;

  tcp::acceptor acceptor1_c(io_context, tcp::endpoint(tcp::v4(), PORT_P1_P3_c));
  std::cout << " --- > " << std::endl;
  tcp::socket s1_c(acceptor1_c.accept());
  std::cerr << "Listenting on port 3: " << PORT_P1_P3_c << std::endl;
 

  tcp::acceptor acceptor0(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P3));
  std::cout << " --- > " << std::endl;
  tcp::socket s0(acceptor0.accept());
  std::cerr << "Listenting on port 10: " << PORT_P0_P3 << std::endl;

  tcp::acceptor acceptor0_a(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P3_a));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_a(acceptor0_a.accept());
  std::cerr << "Listenting on port 11: " << PORT_P0_P3_a << std::endl;

  tcp::acceptor acceptor0_b(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P3_b));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_b(acceptor0_b.accept());
  std::cerr << "Listenting on port 12: " << PORT_P0_P3_b << std::endl;

  tcp::acceptor acceptor0_c(io_context, tcp::endpoint(tcp::v4(), PORT_P0_P3_c));
  std::cout << " --- > " << std::endl;
  tcp::socket s0_c(acceptor0_c.accept());
  std::cerr << "Listenting on port 12: " << PORT_P0_P3_c << std::endl;

  tcp::socket s2(io_context);
  boost::asio::connect(s2, resolver.resolve({host3,
  std::to_string(PORT_P3_P2)}));
  std::cout << "P0: [Established connection P3]" << std::endl;
  usleep(20000);

    tcp::socket s2_a(io_context);
  boost::asio::connect(s2_a, resolver.resolve({host3,
  std::to_string(PORT_P3_P2_a)}));
  std::cout << "P0: [Established connection P3]" << std::endl;
  usleep(20000);


  tcp::socket s2_b(io_context);
  boost::asio::connect(s2_b, resolver.resolve({host3,
  std::to_string(PORT_P3_P2_b)}));
  std::cout << "P0: [Established connection P3]" << std::endl;
  usleep(20000);

  tcp::socket s2_c(io_context);
  boost::asio::connect(s2_c, resolver.resolve({host3,
  std::to_string(PORT_P3_P2_c)}));
  std::cout << "P0: [Established connection P3]" << std::endl;
  usleep(20000);
 
  tcp::socket s2_d(io_context);
  boost::asio::connect(s2_d, resolver.resolve({host3,
  std::to_string(PORT_P3_P2_d)}));
  std::cout << "P0: [Established connection P3]" << std::endl;
  usleep(20000);
  try
  {
 

    write(s0_a, boost::asio::buffer(&seed_, sizeof(__m128i)));
    write(s1_a, boost::asio::buffer(&seed_, sizeof(__m128i)));

    generate_user_and_item_profiles();
    // send_profiles_to_P0(0, io_context, s0);
    // send_profiles_to_P1(0, io_context, s1);
   
     std::thread poller(keep_polling, std::ref(io_context));     
     std::thread carry_dpf_reader_from_Pb(read_carry_dpfs_from_Pb, std::ref(s1_b));
     std::thread hi_dpf_reader_from_Pb(read_hi_dpfs_from_Pb, std::ref(s1_c));
     std::thread lo_dpf_reader_from_Pb(read_lo_dpfs_from_Pb, std::ref(s1_a));

     std::thread evaluater(do_evalintervals, nitems);

     poller.join();
     carry_dpf_reader_from_Pb.join();
     hi_dpf_reader_from_Pb.join();
     lo_dpf_reader_from_Pb.join();
     evaluater.join();
  }

  catch (std::exception & e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
