


uint64_t I_profile_msbs[total_bucket_elements][DIM];
uint64_t I_profile_msbs_this[total_bucket_elements][DIM];
uint64_t I_profile_msbs_written[total_bucket_elements][DIM];
uint64_t I_profile_msbs_read[total_bucket_elements][DIM];
uint64_t I_profile_msbs_blinded[total_bucket_elements][DIM];
uint64_t I_profile_msbs_blinded_read[total_bucket_elements][DIM];
uint64_t I_profile_msb_alpha[total_bucket_elements][DIM];
uint64_t I_profile_msb_blind[total_bucket_elements][DIM];




uint64_t msb_prod[total_bucket_elements][DIM];
uint64_t msb_prod_recv[total_bucket_elements][DIM];

__m128i I_precision_seed;


void write_I_profile_msbs(size_t j, boost::asio::io_context& io_context, tcp::socket & sout)
{
     while(progress[step::i_msb_in] < j + 1)
     {
        std::this_thread::yield();
     }

     if(j % bucket_size_ == 0 && precision == 16)
     {
       io_context.poll();
     } 

     async_write(sout, boost::asio::buffer(I_profile_msbs_written[j], DIM * sizeof(uint64_t)),
          [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
          
          {       
              if(!ec)
              {
                
                 if(j + 1 < total_bucket_elements)
                 {
                   write_I_profile_msbs( j + 1, io_context, sout);
                 }
              }          
              else
              {
                 write_I_profile_msbs(j , io_context, sout);
              }
          
          });  
}


void read_I_profile_msbs(tcp::socket & sin)
{
  
  std::cerr << "reading\n";

  for(size_t j = 0; j < total_bucket_elements; ++j)
  { 
     while(progress[step::i_msb_in] < j + 1)
     {
        std::this_thread::yield();
     }
    
    boost::system::error_code ec;
    read(sin, boost::asio::buffer(I_profile_msbs_read[j], DIM * sizeof(uint64_t)), ec);

    progress[step::iprofile_msbs_in] = j + 1; 
  }
} 


void gen_precision_blinds(AES_KEY& prgkey, ssl_socket& sin)
{
  read(sin, boost::asio::buffer(&I_precision_seed, sizeof(__m128i)));   
 
  PRG(prgkey, I_precision_seed, (__m128i *)I_profile_msb_blind, (sizeof(uint64_t) * total_bucket_elements * DIM) / sizeof(__m128i));

  std::cerr << "I_precision_seed = " << I_precision_seed[0] << " " << I_precision_seed[1] << std::endl;
  
  progress[step::precision_seed_in] = 1; 
}


void read_precision_reduction_bvrs(ssl_socket & sin)
{

  for(size_t j = 0; j < total_bucket_elements; ++j)
  {
     read(sin, boost::asio::buffer(&I_profile_msb_alpha[j], DIM * sizeof(uint64_t)));

     progress[step::precision_beaver_in] = j + 1;
  }
}



void blind_I_profile_msbs()
{
  while(progress[step::precision_seed_in] < 1) 
  {
    std::this_thread::yield();
  }
 
 
  for(size_t j = 0; j < total_bucket_elements; ++j)
  {
     while(progress[step::i_msb_in] < j + 1) 
     {
       std::this_thread::yield();
     }
   
    for(size_t dim = 0; dim < DIM; ++dim)
    { 
      I_profile_msbs_blinded[j][dim] = I_profile_msbs_this[j][dim] + I_profile_msb_blind[j][dim];
    }

    progress[step::i_profile_msbs_blinded] = j + 1;
  }
}

void write_blinded_I_profile_msbs(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{

    while(progress[step::i_profile_msbs_blinded] < j + 1)
    {
      std::this_thread::yield();
    }

     async_write(sout, boost::asio::buffer(I_profile_msbs_blinded[j], DIM * sizeof(uint64_t)),
          [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
          
          {       
              if(!ec)
              {
                
                 if(j + 1 < total_bucket_elements)
                 {
                   write_blinded_I_profile_msbs( j + 1, io_context, sout);
                 }
              }          
              else
              {
                 write_blinded_I_profile_msbs(j , io_context, sout);
              }
          
          });  
}


void read_blinded_I_profile_msbs(tcp::socket& sin)
{
  for(size_t j = 0; j < total_bucket_elements; ++j)
  {
    read(sin, boost::asio::buffer(I_profile_msbs_blinded_read[j], DIM * sizeof(uint64_t)));

    progress[step::blinded_i_msbs_in] = j + 1;
  }
}




void gen_msb_prod()
{
  for(size_t j = 0; j < total_bucket_elements; ++j)
  {
    while( progress[step::i_profile_msbs_blinded] < j + 1 || progress[step::blinded_i_msbs_in] < j + 1 
          || progress[step::iprofile_msbs_in] < j + 1 || progress[step::precision_beaver_in] < j + 1)
    {
      std::this_thread::yield();
    }

    for(size_t dim = 0; dim < DIM; ++dim)
    {

      #if(PARTY == 0)  
         msb_prod[j][dim] =  (I_profile_msbs_read[j][dim] * I_profile_msbs_written[j][dim]) + ((I_profile_msbs_written[j][dim] + I_profile_msbs_this[j][dim]) * I_profile_msbs_blinded_read[j][dim]) 
                                  - (I_profile_msb_blind[j][dim] * I_profile_msbs_read[j][dim]) + I_profile_msb_alpha[j][dim];
      #endif

      #if(PARTY == 1)  
       msb_prod[j][dim] =  (I_profile_msbs_written[j][dim] * I_profile_msbs_blinded_read[j][dim])  - (I_profile_msb_blind[j][dim] * I_profile_msbs_read[j][dim])
                            - (I_profile_msb_blind[j][dim] * I_profile_msbs_blinded_read[j][dim]);
      #endif

      assert( msb_prod[j][dim] != 0);
    
    }

    progress[step::msb_prod_gen] = j + 1;
  }
}


void write_msb_prod(size_t j, boost::asio::io_context& io_context, tcp::socket& sout)
{

  while(progress[step::msb_prod_gen] < j + 1)
  {
    std::this_thread::yield();
  }



 
   async_write(sout, boost::asio::buffer(msb_prod[j], DIM * sizeof(uint64_t)),
        [j, &io_context, &sout](boost::system::error_code ec, std::size_t) 
        
        {       
            if(!ec)
            {
              
               if(j + 1 < total_bucket_elements)
               {
                 write_msb_prod( j + 1, io_context, sout);
               }
            }          
            else
            {
               write_msb_prod(j , io_context, sout);
            }
        
        });  
}

void read_msb_prod(tcp::socket& sin)
{
  for(size_t j = 0; j < total_bucket_elements; ++j)
  {
     while(progress[step::msb_prod_gen] < j + 1)
     {
       std::this_thread::yield();
     }

    read(sin, boost::asio::buffer(&msb_prod_recv[j], DIM * sizeof(uint64_t)));

    for(size_t dim = 0; dim < DIM; ++dim) 
     {
        #ifdef VERBOSE
         std::cout << " msbsum-> " << (msb_prod_recv[j][dim] + msb_prod[j][dim]) << " = " << msb_prod_recv[j][dim] <<  " + " << msb_prod[j][dim] << std::endl;
        #endif
         
        if(msb_prod_recv[j][dim] + msb_prod[j][dim] != 0)
        {
          std::cerr << " msbsum-> " << (msb_prod_recv[j][dim] + msb_prod[j][dim]) << " = " << msb_prod_recv[j][dim] <<  " + " << msb_prod[j][dim] << std::endl;
        }
     }
  }
}







void adjust_offset()
{



  for(size_t j = 0; j < total_bucket_elements; ++j)
  {
    while(
      progress[step::iprofile_msbs_in] < j + 1 
          || progress[step::msb_prod_gen] < j + 1
          )
    {
      std::this_thread::yield();
    }

   for(size_t dim = 0; dim < DIM; ++dim)
    {
     
      iprofiles_reduced[(j % nitems)].u64[dim] -= msb_prod[j][dim] * ( ~(~0ULL >> (3*precision - (3 * precision/2))) );
     
      if(I_profile_msbs_read[j][dim] == 1 && I_profile_msbs[j][dim] == 1)
      {
        std::cerr << "Both One!\n\n " << (j % nitems) << " " << (j / bucket_size_) << std::endl; 
       //iprofiles_reduced[(j % nitems)].u64[dim] -= ~(~0ULL >> (3*precision - (3 * precision/2)));
      }
    }
    
    progress[step::offset_iprofiles] = j + 1;

    // if(items_read % nitems == 0  || items_read == total_bucket_elements)
    // { 
    //   std::cout << "progress[step::buckets_in] (before) =  " << progress[step::buckets_in] << std::endl;

    //   progress[step::buckets_in] = progress[step::buckets_in] + 1;
    //   std::cout << "progress[step::buckets_complete] = " << progress[step::buckets_complete] << std::endl;
    //   std::cout << "progress[step::buckets_in] (after) = " << progress[step::buckets_in] << std::endl;
    // }
  }
}