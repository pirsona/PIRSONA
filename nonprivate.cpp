#include <cstdlib>
#include <iostream>
#include <chrono>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "datatypesnp.h"
#include <fstream>
//#include "nonprivate/fixed.h"


using namespace std::chrono;
#define P_ZERO


int main(int argc, char* argv[])
{

  std::cerr << "DIM = " << DIM << std::endl;
  const char * ifile = "./iprofilesA";

  int ifd = open(ifile, O_RDWR);
  profile * iprofiles = (profile *) mmap(NULL,
  nitems * sizeof(profile), PROT_READ, MAP_PRIVATE, ifd, 0);

//#ifdef DEBUG
  int qfd_debug = open("./queries", O_RDONLY);
  uint64_t * queries_debug = (uint64_t *) mmap(NULL,
    2 * nqueries * sizeof(uint64_t), PROT_READ, MAP_PRIVATE, qfd_debug, 0);

  int ufd_debug = open("./uprofilesA", O_RDONLY);
  profile * uprofiles = (profile *) mmap(NULL,
  nusers * sizeof(profile), PROT_READ, MAP_PRIVATE, ufd_debug, 0);
//#endif

std::cout << "nitems = "   << nitems << std::endl;
std::cout << "nusers = "   << nusers << std::endl;
std::cout << "nqueries = " << nqueries << std::endl;

  try
  {
    profile * uprofiles2;
    profile * iprofiles2;


    if (posix_memalign((void**)&uprofiles2, sizeof(__m256i),
          nusers * sizeof(profile)) != 0 ||
        posix_memalign((void**)&iprofiles2, sizeof(__m256i),
          nitems * sizeof(profile)) != 0 )
    {
      throw std::runtime_error("Memory allocation failed");
    }

    memset(uprofiles2, 0, nusers * sizeof(profile));
    memset(iprofiles2, 0, nitems * sizeof(profile));

    
    profile * uprofiles_updated;
    profile * iprofiles_updated;


    if (posix_memalign((void**)&uprofiles_updated, sizeof(__m256i),
          nusers * sizeof(profile)) != 0 ||
        posix_memalign((void**)&iprofiles_updated, sizeof(__m256i),
          nitems * sizeof(profile)) != 0 )
    {
      throw std::runtime_error("Memory allocation failed");
    }

    memset(uprofiles_updated, 0, nusers * sizeof(profile));
    memset(iprofiles_updated, 0, nitems * sizeof(profile));
    
    
    auto start_nonprivate = high_resolution_clock::now();
    std::ofstream nonprivate_time;
    nonprivate_time.open("nonprivate_exp/nonprivate_time"+std::to_string(atoi(argv[1])));
    for(size_t j = 0; j < nqueries; j++)
    {

         int u = queries_debug[2*j];
         int v = queries_debug[2*j + 1];

         profile & usr = uprofiles[queries_debug[2*j]];
         profile itm  =  iprofiles[queries_debug[2*j + 1]];

         double d = dot(usr, itm);
         
         profile user_update = (1 - d) * itm;
         profile item_update = (1 - d) * usr;

    
         uprofiles2[ queries_debug[2*j] ]          = uprofiles2[ queries_debug[2*j] ]         + user_update;
        
         iprofiles2[ queries_debug[2 * j  + 1] ]   = iprofiles2[ queries_debug[2 * j  + 1] ]  + item_update;
    }
    
        for(size_t j = 0; j < nusers; ++j)
        {
            uprofiles_updated[j] = uprofiles[j] +  uprofiles2[j];
           
            std::cout << "uprofiles_updated[" << j << "] = " << ": " << uprofiles_updated[j].f64[0] << " "
                      <<  uprofiles_updated[j].f64[1] << " " << uprofiles_updated[j].f64[2] 
                      << " " << uprofiles_updated[j].f64[3] << std::endl; 
        }

    for(size_t j = 0; j < nitems; ++j)  iprofiles_updated[j] = iprofiles[j] +  iprofiles2[j];
       
       for(size_t j = 0; j < nitems; ++j)
        {
                std::cout << "iprofiles_updated[" << j << "] = " << iprofiles_updated[j].f64[0] << " "
                      <<  iprofiles_updated[j].f64[1] << " " << iprofiles_updated[j].f64[2] 
                      << " " << iprofiles_updated[j].f64[3] << std::endl; 
        }

    for(size_t j = 0; j < nusers; ++j)
    {
      double norm = 0.0;
      for(size_t i = 0; i < DIM; ++i) norm += (uprofiles_updated[j].f64[i] * uprofiles_updated[j].f64[i]);
      norm = sqrt(norm);
      std::cout << "norm = " << norm << std::endl;
      for(size_t i = 0; i < DIM; ++i) uprofiles_updated[j].f64[i] /= norm;
    }



   for(size_t j = 0; j < nitems; ++j)
    {
      double norm = 0.0;
      for(size_t i = 0; i < DIM; ++i) norm += (iprofiles_updated[j].f64[i] * iprofiles_updated[j].f64[i]);
      norm = sqrt(norm);
      std::cout << "norm = " << norm << std::endl;
      for(size_t i = 0; i < DIM; ++i) iprofiles_updated[j].f64[i] /= norm;
    }


    std::ofstream iprofiles_out;
    iprofiles_out.open("nonprivate_exp/iprofilesnorm_nonprivate"+std::to_string(atoi(argv[1])));
    

    for(size_t j = 0; j < nitems; ++j)
    { 

        for(size_t dim = 0; dim < DIM; ++dim) iprofiles_out << iprofiles_updated[j].f64[dim] << " ";
        iprofiles_out << std::endl;

    
    }

iprofiles_out.close();



    std::ofstream uprofiles_out;
    uprofiles_out.open("nonprivate_exp/uprofilesnorm_nonprivate"+std::to_string(atoi(argv[1])));
    for(size_t j = 0; j < nusers; ++j){
        
        for(size_t dim = 0; dim < DIM; ++dim) uprofiles_out << uprofiles_updated[j].f64[dim] << " ";
        uprofiles_out << std::endl;
      
 
      
    }
    uprofiles_out.close();
  auto stop_nonprivate = high_resolution_clock::now(); 
  auto duration_nonprivate = duration_cast<milliseconds>(stop_nonprivate - start_nonprivate);
  nonprivate_time << duration_nonprivate.count() << std::endl;

  }

  catch (std::exception & e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
