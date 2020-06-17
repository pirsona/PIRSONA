#include <bsd/stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include "common.h"
using namespace std;

int main(int argc, char ** argv)
{
    size_t * queries = new size_t[2 * nqueries];
    
    //size_t users[nqueries];
    //size_t items[nqueries];

    size_t t = 0;
    string usr, itm, rating, timestamp, dummy;
   
   // ifstream out("ratings.dat");
    //ifstream out("ratings.txt");
  


    ifstream out("ratings.txt");
    std::cout << "nusers == " << nusers << std::endl;
    std::cout << "nitems == " << nitems << std::endl;
     while(out.good() && t < nqueries)
    {

      std::cout << "t = " << t << std::endl;
 
     // getline(out,usr,','); 
     // getline(out,itm,',');
     // getline(out,rating,','); 
     // getline(out,timestamp,'\n');

    // getline(out,usr,':');
    // getline(out, dummy, ':');
    // getline(out,itm,':');
    // getline(out, dummy, ':');
    // getline(out,rating,':');
    // getline(out, dummy, ':');
    // getline(out,timestamp,'\n');

    getline(out,usr,'\t'); 
    getline(out,itm,'\t');
    getline(out,rating,'\t'); 
    getline(out,timestamp,'\n');

    
    if(stoi(usr) < nusers){
    queries[2*t]   = stoi(usr);
    }
    else{
    srand(0);  
    queries[2*t] = rand() % nusers;//  arc4random_uniform(nusers);;     
    } 


    if(stoi(itm) < nitems){
    queries[2*t+1] = stoi(itm);
    }
    else{
    srand(1); 
    queries[2*t+1] = rand() % nitems; // arc4random_uniform(nitems);;     
    }   
     //users[t] = stoi(usr);
     //items[t] = stoi(itm);
    
    std::cout << "(usr, itm): " << t << ": (" <<  queries[2*t] << " , " <<queries[2*t+1]  << ") " << '\n';
    std::cout << "-------------------" << '\n';

    ++t;

   } 




  // for (size_t i = 0; i < nqueries; ++i)
  // {

  //   srand(i);
  //   queries[2*i]   =  arc4random_uniform(nusers);  //rand() % nusers; 
  //   srand(i+1);
  //   queries[2*i+1] =  arc4random_uniform(nitems);  //rand() % nitems;   
  //   std::cout << queries[2*i] << " " <<  queries[2*i+1] << std::endl;
  // }

  ssize_t bytes;
  int fd = open("./queries", O_RDWR, O_CREAT);
  //bytes = write(fd, &nqueries, sizeof(size_t));  
  bytes = write(fd, queries, 2 * nqueries * sizeof(uint64_t));
  close(fd);

  delete [] queries;

  return 0;
}
