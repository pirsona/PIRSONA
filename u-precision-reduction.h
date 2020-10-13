uint64_t U_profile_msbs[nusers][DIM];
uint64_t U_profile_msbs_this[nusers][DIM];
uint64_t U_profile_msbs_written[nusers][DIM];
uint64_t U_profile_msbs_read[nusers][DIM];
uint64_t U_profile_msbs_blinded[nusers][DIM];
uint64_t U_profile_msbs_blinded_read[nusers][DIM];
uint64_t U_profile_msb_alpha[nusers][DIM];
uint64_t U_profile_msb_blind[nusers][DIM];

uint64_t U_msb_prod[nusers][DIM];
uint64_t U_msb_prod_recv[nusers][DIM];

__m128i u_precision_seed;

void U_read_precision_reduction_bvrs(AES_KEY& prgkey, ssl_socket& sin)
{

  read(sin, boost::asio::buffer(&u_precision_seed, sizeof(__m128i))); 
  PRG(prgkey, u_precision_seed, (__m128i *)U_profile_msb_blind, (sizeof(uint64_t) * nusers * DIM) / sizeof(__m128i));

  std::cerr << "u_precision_seed = " << u_precision_seed[0] << " " << u_precision_seed[1] << std::endl;
  progress[step::u_precision_seed_in] = 1;  
 
  for(size_t j = 0; j < nusers; ++j)
  {
     read(sin, boost::asio::buffer(&U_profile_msb_alpha[j], DIM * sizeof(uint64_t)));

     progress[step::u_precision_beaver_in] = j + 1;
  }
}


void U_gen_msbs()
{

  while(progress[step::user_update] < nqueries || progress[step::item_update] < nqueries)
  {
    std::this_thread::yield();
  }

  for(size_t j = 0; j < nusers; ++j)
  {
     for(size_t dim = 0; dim < DIM; ++dim)
     {            
       U_profile_msbs[j][dim] = (uprofiles2[j].u64[dim] & msb) > 0; 
       arc4random_buf(&U_profile_msbs_this[j][dim], sizeof(uint64_t));
       U_profile_msbs_written[j][dim] = U_profile_msbs[j][dim] - U_profile_msbs_this[j][dim];    
     } 

     progress[step::u_msbs_gen] = j + 1;
  }
}


void write_U_profile_msbs(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{
     while(progress[step::u_msbs_gen] < j + 1)
     {
        std::this_thread::yield();
     }

 

     async_write(sout, boost::asio::buffer(U_profile_msbs_written[j], DIM * sizeof(uint64_t)),
          [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
          
          {       
              if(!ec)
              {
                
                 if(j + 1 < nusers)
                 {
                   write_U_profile_msbs( j + 1, io_context, sout);
                 }
              }          
              else
              {
                 write_U_profile_msbs(j , io_context, sout);
              }
          
          });  
}


void read_U_profile_msbs(tcp::socket& sin)
{

 
  for(size_t j = 0; j < nusers; ++j)
  { 

    while(progress[step::u_msbs_gen] < j + 1)
    {
      std::this_thread::yield();
    }
    read(sin, boost::asio::buffer(U_profile_msbs_read[j], DIM * sizeof(uint64_t)));

    progress[step::uprofile_msbs_in] = j + 1; 
  }
  

} 


void blind_U_profile_msbs()
{

  while(progress[step::u_precision_seed_in] < 1) 
  {
    std::this_thread::yield();
  }
 

 
  for(size_t j = 0; j < nusers; ++j)
  {
     while(progress[step::u_msbs_gen] < j + 1) 
     {
       std::this_thread::yield();
     }
   
    for(size_t dim = 0; dim < DIM; ++dim)
    { 
      U_profile_msbs_blinded[j][dim] = U_profile_msbs_this[j][dim] + U_profile_msb_blind[j][dim];
    }

    progress[step::u_profile_msbs_blinded] = j + 1;
  }
}

void write_blinded_U_profile_msbs(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{

    while(progress[step::u_profile_msbs_blinded] < j + 1)
    {
      std::this_thread::yield();
    }

     async_write(sout, boost::asio::buffer(U_profile_msbs_blinded[j], DIM * sizeof(uint64_t)),
          [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
          
          {       
              if(!ec)
              {
                
                 if(j + 1 < nusers)
                 {
                   write_blinded_U_profile_msbs( j + 1, io_context, sout);
                 }
              }          
              else
              {
                 write_blinded_U_profile_msbs(j , io_context, sout);
              }
          
          });  
}





void read_blinded_U_profile_msbs(tcp::socket& sin)
{
  for(size_t j = 0; j < nusers; ++j)
  {
    read(sin, boost::asio::buffer(U_profile_msbs_blinded_read[j], DIM * sizeof(uint64_t)));

    progress[step::blinded_u_msbs_in] = j + 1;
  }
}


void U_gen_msb_prod()
{
   for(size_t j = 0; j < nusers; ++j)
   {
    while(     progress[step::u_profile_msbs_blinded] < j + 1  || progress[step::blinded_u_msbs_in] < j + 1
            || progress[step::u_precision_beaver_in]  < j + 1  || progress[step::uprofile_msbs_in] < j + 1)
    {
      std::this_thread::yield();
    }

    for(size_t dim = 0; dim < DIM; ++dim)
    {

      #if(PARTY == 0)  
         U_msb_prod[j][dim] =  (U_profile_msbs_read[j][dim] * U_profile_msbs_written[j][dim]) + ((U_profile_msbs_written[j][dim] + U_profile_msbs_this[j][dim]) * U_profile_msbs_blinded_read[j][dim]) 
                                  - (U_profile_msb_blind[j][dim] * U_profile_msbs_read[j][dim]) + U_profile_msb_alpha[j][dim];
      #endif

      #if(PARTY == 1)  
       U_msb_prod[j][dim] =  (U_profile_msbs_written[j][dim] * U_profile_msbs_blinded_read[j][dim])  - (U_profile_msb_blind[j][dim] * U_profile_msbs_read[j][dim])
                            - (U_profile_msb_blind[j][dim] * U_profile_msbs_blinded_read[j][dim]);
      #endif   
    }

     progress[step::u_msb_prod_gen] = j + 1;
   }
}


void U_write_msb_prod(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{

  while(progress[step::u_msb_prod_gen] < j + 1)
  {
    std::this_thread::yield();
  }



 
   async_write(sout, boost::asio::buffer(U_msb_prod[j], DIM * sizeof(uint64_t)),
        [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
        
        {       
            if(!ec)
            {
              
               if(j + 1 < nusers)
               {
                 U_write_msb_prod( j + 1, io_context, sout);
               }
            }          
            else
            {
               U_write_msb_prod(j , io_context, sout);
            }
        
        });  
}

void U_read_msb_prod(tcp::socket& sin)
{
  for(size_t j = 0; j < nusers; ++j)
  {
     while(
            progress[step::uprofile_msbs_in] < j + 1 ||  progress[step::user_update] < nqueries  || progress[step::u_msb_prod_gen] < j + 1 
          )
     {
       std::this_thread::yield();
     }

    read(sin, boost::asio::buffer(&U_msb_prod_recv[j], DIM * sizeof(uint64_t)));

    for(size_t dim = 0; dim < DIM; ++dim) 
     {
        #ifdef VERBOSE
         std::cout << " msbsum-> " << (U_msb_prod_recv[j][dim] + U_msb_prod[j][dim]) << " = " << U_msb_prod_recv[j][dim] <<  " + " << U_msb_prod[j][dim] << std::endl;
        #endif
         
        if(U_msb_prod_recv[j][dim] + U_msb_prod[j][dim] != 0)
        {
          std::cerr << " UUmsbsum-> " << (U_msb_prod_recv[j][dim] + U_msb_prod[j][dim]) << " = " << U_msb_prod_recv[j][dim] <<  " + " << U_msb_prod[j][dim] << std::endl;
        }
     }

      progress[step::u_msb_prod_recv] = j + 1;
  }
  
 

}









void U_adjust_offset()
{



  for(size_t j = 0; j < nusers; ++j)
  {
    while(
          progress[step::uprofile_msbs_in] < j + 1 ||  progress[step::user_update] < nqueries  || progress[step::u_msb_prod_gen] < j + 1 
          )
    {
      std::this_thread::yield();
    }

   for(size_t dim = 0; dim < DIM; ++dim)
    {
     
    //  uprofiles_reduced[j].u64[dim] -= U_msb_prod[j][dim] * ( ~(~0ULL >> (3*precision - (3 * precision/2))) );
     
    }
    
    progress[step::offset_uprofiles] = j + 1;
 
  }
}