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

typedef std::bitset<64> bits;

typedef std::array<uint64_t, 64> bitarray; 

uint64_t odd_prime =  34359738363ULL; //
//uint64_t odd_prime = 18446744073709551557ULL;
auto start_of_program = std::chrono::system_clock::now();
// globals
AES_KEY aeskey;


 

 

 

 

 

int main(int argc, char * argv[])
{
  
  
 
  
  AES_set_encrypt_key(_mm_set_epi64x(597349, 121379), &aeskey);


  fixed_t<2 * precision> a;

  fixed_t<2 * precision> b;

  arc4random_buf(&a, sizeof(fixed_t<2 * precision>));
  arc4random_buf(&b, sizeof(fixed_t<2 * precision>));

  fixed_t<2 * precision> a_minus_b = a - b;
 

  try
  {
       
      const std::string host1 = (argc < 2) ? "127.0.0.1" : argv[1];
      const std::string host2 = (argc < 3) ? "127.0.0.1" : argv[2];
      const std::string host3 = (argc < 4) ? "127.0.0.1" : argv[3];

      //#include "connections.h"
          
 

 
    
  }
  catch (std::exception & e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

 

  return 0;
}